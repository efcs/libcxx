//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef TEST_SUPPORT_DEBUG_MODE_HELPER_H
#define TEST_SUPPORT_DEBUG_MODE_HELPER_H

#ifndef _LIBCPP_DEBUG
#error _LIBCPP_DEBUG must be defined before including this header
#endif

#include <ciso646>
#ifndef _LIBCPP_VERSION
#error This header may only be used for libc++ tests"
#endif

#include <__debug>
#include <utility>
#include <cstddef>
#include <cstdlib>
#include <cassert>
#include <string>
#include <sstream>
#include <iostream>

#include <unistd.h>
#include <sys/wait.h>
#include "test_macros.h"
#include "assert_checkpoint.h"
#include "test_allocator.h"

struct DebugInfoMatcher {
  static const int any_line = -1;
  static constexpr const char* any_file = "*";
  static constexpr const char* any_msg = "*";

  constexpr DebugInfoMatcher() : msg(nullptr), file(any_file), line(any_line) { }
  constexpr DebugInfoMatcher(const char* msg, const char* file = any_file, int line = any_line)
    : msg(msg), file(file), line(line) {}

  bool Matches(std::__libcpp_debug_info const& got) const {
    assert(msg && "empty matcher");
    bool IsMatch = [&]() {
      if (line != any_line && line != got.__line_)
        return false;
      if (file != std::string(any_file) && std::string(got.__file_).find(file) == std::string::npos)
        return false;
      if (msg != std::string(any_msg) && std::string(got.__msg_).find(msg) == std::string::npos)
        return false;
      return true;
    }();
    if (!IsMatch) {
      std::cout << "Failed to match debug info!\n"
                << ToString() << "\n"
                << "VS\n"
                << got.what() << "\n";
    }
    return IsMatch;
  }

  std::string ToString() const {
    std::stringstream ss;
    ss << "msg = \"" << msg << "\"\n"
       << "line = " << (line == any_line ? "'*'" : std::to_string(line)) << "\n"
       << "file = " << (file == any_file ? "'*'" : any_file) << "";
    return ss.str();
  }

  bool empty() const { return msg == nullptr; }

private:
  const char* msg;
  const char* file;
  int line;
};

static constexpr DebugInfoMatcher AnyMatcher(DebugInfoMatcher::any_msg);

inline DebugInfoMatcher& GlobalMatcher() {
  static DebugInfoMatcher GMatch;
  return GMatch;
}

struct DeathTest {
  enum ResultKind {
    RK_MatchFound, RK_MatchFailure, RK_DidNotDie, RK_Unknown
  };

  TEST_NORETURN static void DeathTestDebugHandler(std::__libcpp_debug_info const& info) {
    assert(!GlobalMatcher().empty());
    if (GlobalMatcher().Matches(info)) {
      std::exit(RK_MatchFound);
    }
    std::exit(RK_MatchFailure);
  }


  DeathTest(DebugInfoMatcher const& Matcher) : matcher_(Matcher) {}

  template <class Func>
  ResultKind Run(Func&& f) {
    int pipe_res = pipe(pipe_fd);
    assert(pipe_res != -1);

    pid_t child_pid = fork();
    assert(child_pid != -1);
    child_pid_ = child_pid;
    if (child_pid_ == 0) {
      RunForChild(std::forward<Func>(f));
      assert(false && "unreachable");
    }
    return RunForParent();
  }

  std::string const& getError() const { return error_msg_; }

private:
  template <class Func>
  TEST_NORETURN void RunForChild(Func&& f) {
    close(pipe_fd[0]);
    while ((dup2(pipe_fd[1], STDOUT_FILENO) == -1) && (errno == EINTR)) {}
    GlobalMatcher() = matcher_;
    std::__libcpp_set_debug_function(&DeathTestDebugHandler);
    f();
    std::exit(RK_DidNotDie);
  }

  void CaptureIO() {
    close(pipe_fd[1]);
    int read_fd = pipe_fd[0];
    char buffer[256];
    int num_read;
    do {
      while ((num_read = read(read_fd, buffer, 255)) > 0) {
        buffer[num_read] = '\0';
        error_msg_ += buffer;
      }
    } while (num_read == -1 && errno == EINTR);
    close(read_fd);
  }

  ResultKind RunForParent() {
    CaptureIO();

    int status_value;
    pid_t result = waitpid(child_pid_, &status_value, 0);
    assert(result != 1);
    status_ = status_value;

    if (WIFEXITED(status_value)) {
      int exit_status = WEXITSTATUS(status_value);
      assert(exit_status == RK_MatchFound || exit_status == RK_MatchFailure ||
            exit_status == RK_DidNotDie);
      return static_cast<ResultKind>(exit_status);
    }
    return RK_Unknown;
  }

  DeathTest(DeathTest const&) = delete;
  DeathTest& operator=(DeathTest const&) = delete;

private:
  DebugInfoMatcher matcher_;
  pid_t child_pid_ = -1;
  int status_ = -1;
  int pipe_fd[2];
  std::string error_msg_;
};

template <class Func>
inline bool ExpectDeath(const char* stmt, Func&& func, DebugInfoMatcher Matcher) {
  DeathTest DT(Matcher);
  DeathTest::ResultKind RK = DT.Run(func);
  auto OnFailure = [&](const char* msg) {
    std::cerr << "EXPECT_DEATH( " << stmt << " ) failed! (" << msg << ")\n\n";
    if (!DT.getError().empty())
      std::cerr << DT.getError() << "\n";
    return false;
  };
  switch (RK) {
  case DeathTest::RK_MatchFound:
    return true;
  case DeathTest::RK_Unknown:
      return OnFailure("reason unknown");
  case DeathTest::RK_DidNotDie:
      return OnFailure("child did not die");
  case DeathTest::RK_MatchFailure:
      return OnFailure("matcher failed");
  }
}

template <class Func>
inline bool ExpectDeath(const char* stmt, Func&& func) {
  return ExpectDeath(stmt, func, AnyMatcher);
}

/// Assert that the specified expression throws a libc++ debug exception.
#define EXPECT_DEATH(...) assert((ExpectDeath(#__VA_ARGS__, [&]() { __VA_ARGS__; } )))

#define EXPECT_DEATH_MATCHES(Matcher, ...) assert((ExpectDeath(#__VA_ARGS__, [&]() { __VA_ARGS__; }, Matcher)))

#endif // TEST_SUPPORT_DEBUG_MODE_HELPER_H
