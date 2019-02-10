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

// These test make use of 'if constexpr'.
#if TEST_STD_VER <= 14
#error This header may only be used in C++17 and greater
#endif
#ifdef TEST_HAS_NO_EXCEPTIONS
#error These tests require exceptions
#endif

#ifndef __cpp_if_constexpr
#error These tests require if constexpr
#endif

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
      std::cerr << "Failed to match debug info!\n"
                << "Expected:\n" << ToString() << "\n"
                << "Got:\n" << got.what() << "\n";
    }
    return IsMatch;
  }

  std::string ToString() const {
    std::stringstream ss;
    ss << "DebugInfoMatcher: \n"
       << "  " << "msg = \"" << msg << "\"\n"
       << "  " << "line = " << (line == any_line ? "'*'" : std::to_string(line)) << "\n"
       << "  " << "file = " << (file == any_file ? "'*'" : any_file) << "\n";
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

inline void DeathTestDebugHandler(std::__libcpp_debug_info const& info) {
  assert(!GlobalMatcher().empty());
  if (GlobalMatcher().Matches(info)) {
    std::exit(0);
  }
  std::exit(1);
}

struct DeathTest {
  enum ResultKind {
    RK_Died, RK_MatchFailure, RK_Lived, RK_Unknown
  };

  DeathTest(DebugInfoMatcher const& Matcher) : matcher_(Matcher) {}

  template <class Func>
  ResultKind Run(Func&& f) {
    int pipe_fd[2];
    assert(pipe(pipe_fd) != -1);

    pid_t child_pid = fork();
    assert(child_pid != -1);
    child_pid_ = child_pid;
    if (child_pid_ == 0) {
      close(pipe_fd[0]);
      while ((dup2(pipe_fd[1], STDOUT_FILENO) == -1) && (errno == EINTR)) {}
      GlobalMatcher() = matcher_;
      std::__libcpp_set_debug_function(&DeathTestDebugHandler);
      f();
      std::exit(2);
    } else {
      close(pipe_fd[1]);
      int status_value;
      pid_t result = waitpid(child_pid_, &status_value, 0);
      assert(result != 1);
      status_ = status_value;
      if (WIFEXITED(status_value)) {
        if (WEXITSTATUS(status_value) == 0)
          return RK_Died;
        else if (WEXITSTATUS(status_value) == 1)
          return RK_MatchFailure;
        else if (WEXITSTATUS(status_value) == 2)
          return RK_Lived;
      }
      return RK_Unknown;
    }
    assert(false);
  }

private:
  DeathTest(DeathTest const&) = delete;
  DeathTest& operator=(DeathTest const&) = delete;

private:
  DebugInfoMatcher matcher_;
  pid_t child_pid_ = -1;
  int status_ = -1;
};

template <class Func>
inline bool ExpectDeath(const char* stmt, Func&& func, DebugInfoMatcher Matcher) {
  DeathTest DT(Matcher);
  DeathTest::ResultKind RK = DT.Run(func);
  auto OnFailure = [&](const char* msg) {
    std::cerr << "EXPECT_DEATH( " << stmt << " ) failed! (" << msg << ")\n\n";
    return false;
  };
  switch (RK) {
  case DeathTest::RK_Died:
    return true;
  case DeathTest::RK_Unknown:
      return OnFailure("reason unknown");
  case DeathTest::RK_Lived:
      return OnFailure("child lived");
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


namespace IteratorDebugChecks {

enum ContainerType {
  CT_None,
  CT_String,
  CT_Vector,
  CT_VectorBool,
  CT_List,
  CT_Deque,
  CT_ForwardList,
  CT_Map,
  CT_Set,
  CT_MultiMap,
  CT_MultiSet,
  CT_UnorderedMap,
  CT_UnorderedSet,
  CT_UnorderedMultiMap,
  CT_UnorderedMultiSet
};

constexpr bool isSequential(ContainerType CT) {
  return CT_Vector >= CT && CT_ForwardList <= CT;
}

constexpr bool isAssociative(ContainerType CT) {
  return CT_Map >= CT && CT_MultiSet <= CT;
}

constexpr bool isUnordered(ContainerType CT) {
  return CT_UnorderedMap >= CT && CT_UnorderedMultiSet <= CT;
}

constexpr bool isSet(ContainerType CT) {
  return CT == CT_Set
      || CT == CT_MultiSet
      || CT == CT_UnorderedSet
      || CT == CT_UnorderedMultiSet;
}

constexpr bool isMap(ContainerType CT) {
  return CT == CT_Map
      || CT == CT_MultiMap
      || CT == CT_UnorderedMap
      || CT == CT_UnorderedMultiMap;
}

constexpr bool isMulti(ContainerType CT) {
  return CT == CT_MultiMap
      || CT == CT_MultiSet
      || CT == CT_UnorderedMultiMap
      || CT == CT_UnorderedMultiSet;
}

template <class Container, class ValueType = typename Container::value_type>
struct ContainerDebugHelper {
  static_assert(std::is_constructible<ValueType, int>::value,
                "must be constructible from int");

  static ValueType makeValueType(int val = 0, int = 0) {
    return ValueType(val);
  }
};

template <class Container>
struct ContainerDebugHelper<Container, char> {
  static char makeValueType(int = 0, int = 0) {
    return 'A';
  }
};

template <class Container, class Key, class Value>
struct ContainerDebugHelper<Container, std::pair<const Key, Value> > {
  using ValueType = std::pair<const Key, Value>;
  static_assert(std::is_constructible<Key, int>::value,
                "must be constructible from int");
  static_assert(std::is_constructible<Value, int>::value,
                "must be constructible from int");

  static ValueType makeValueType(int key = 0, int val = 0) {
    return ValueType(key, val);
  }
};

template <class Container, ContainerType CT,
    class Helper = ContainerDebugHelper<Container> >
struct BasicContainerChecks {
  using value_type = typename Container::value_type;
  using iterator = typename Container::iterator;
  using const_iterator = typename Container::const_iterator;
  using allocator_type = typename Container::allocator_type;
  using traits = std::iterator_traits<iterator>;
  using category = typename traits::iterator_category;

  static_assert(std::is_same<test_allocator<value_type>, allocator_type>::value,
                "the container must use a test allocator");

  static constexpr bool IsBiDir =
      std::is_convertible<category, std::bidirectional_iterator_tag>::value;

public:
  static void run() {
    run_iterator_tests();
    run_container_tests();
    run_allocator_aware_tests();
  }

  static void run_iterator_tests() {
    try {
      TestNullIterators<iterator>();
      TestNullIterators<const_iterator>();
      if constexpr (IsBiDir) { DecrementBegin(); }
      IncrementEnd();
      DerefEndIterator();
    } catch (...) {
      assert(false && "uncaught debug exception");
    }
  }

  static void run_container_tests() {
    try {
      CopyInvalidatesIterators();
      MoveInvalidatesIterators();
      if constexpr (CT != CT_ForwardList) {
          EraseIter();
          EraseIterIter();
      }
    } catch (...) {
      assert(false && "uncaught debug exception");
    }
  }

  static void run_allocator_aware_tests() {
    try {
      SwapNonEqualAllocators();
      if constexpr (CT != CT_ForwardList ) {
          // FIXME: This should work for both forward_list and string
          SwapInvalidatesIterators();
      }
    } catch (...) {
      assert(false && "uncaught debug exception");
    }
  }

  static Container makeContainer(int size, allocator_type A = allocator_type()) {
    Container C(A);
    if constexpr (CT == CT_ForwardList) {
      for (int i = 0; i < size; ++i)
        C.insert_after(C.before_begin(), Helper::makeValueType(i));
    } else {
      for (int i = 0; i < size; ++i)
        C.insert(C.end(), Helper::makeValueType(i));
      assert(C.size() == static_cast<std::size_t>(size));
    }
    return C;
  }

  static value_type makeValueType(int value) {
    return Helper::makeValueType(value);
  }

private:
  // Iterator tests
  template <class Iter>
  static void TestNullIterators() {
    CHECKPOINT("testing null iterator");
    Iter it;
    EXPECT_DEATH( ++it );
    EXPECT_DEATH( it++ );
    EXPECT_DEATH( *it );
    if constexpr (CT != CT_VectorBool) {
      EXPECT_DEATH( it.operator->() );
    }
    if constexpr (IsBiDir) {
      EXPECT_DEATH( --it );
      EXPECT_DEATH( it-- );
    }
  }

  static void DecrementBegin() {
    CHECKPOINT("testing decrement on begin");
    Container C = makeContainer(1);
    iterator i = C.end();
    const_iterator ci = C.cend();
    --i;
    --ci;
    assert(i == C.begin());
    EXPECT_DEATH( --i );
    EXPECT_DEATH( i-- );
    EXPECT_DEATH( --ci );
    EXPECT_DEATH( ci-- );
  }

  static void IncrementEnd() {
    CHECKPOINT("testing increment on end");
    Container C = makeContainer(1);
    iterator i = C.begin();
    const_iterator ci = C.begin();
    ++i;
    ++ci;
    assert(i == C.end());
    EXPECT_DEATH( ++i );
    EXPECT_DEATH( i++ );
    EXPECT_DEATH( ++ci );
    EXPECT_DEATH( ci++ );
  }

  static void DerefEndIterator() {
    CHECKPOINT("testing deref end iterator");
    Container C = makeContainer(1);
    iterator i = C.begin();
    const_iterator ci = C.cbegin();
    (void)*i; (void)*ci;
    if constexpr (CT != CT_VectorBool) {
      i.operator->();
      ci.operator->();
    }
    ++i; ++ci;
    assert(i == C.end());
    EXPECT_DEATH( *i );
    EXPECT_DEATH( *ci );
    if constexpr (CT != CT_VectorBool) {
      EXPECT_DEATH( i.operator->() );
      EXPECT_DEATH( ci.operator->() );
    }
  }

  // Container tests
  static void CopyInvalidatesIterators() {
    CHECKPOINT("copy invalidates iterators");
    Container C1 = makeContainer(3);
    iterator i = C1.begin();
    Container C2 = C1;
    if constexpr (CT == CT_ForwardList) {
      iterator i_next = i;
      ++i_next;
      (void)*i_next;
      EXPECT_DEATH( C2.erase_after(i) );
      C1.erase_after(i);
      EXPECT_DEATH( *i_next );
    } else {
      EXPECT_DEATH( C2.erase(i) );
      (void)*i;
      C1.erase(i);
      EXPECT_DEATH( *i );
    }
  }

  static void MoveInvalidatesIterators() {
    CHECKPOINT("copy move invalidates iterators");
    Container C1 = makeContainer(3);
    iterator i = C1.begin();
    Container C2 = std::move(C1);
    (void) *i;
    if constexpr (CT == CT_ForwardList) {
      EXPECT_DEATH( C1.erase_after(i) );
      C2.erase_after(i);
    } else {
      EXPECT_DEATH( C1.erase(i) );
      C2.erase(i);
      EXPECT_DEATH(*i);
    }
  }

  static void EraseIter() {
    CHECKPOINT("testing erase invalidation");
    Container C1 = makeContainer(2);
    iterator it1 = C1.begin();
    iterator it1_next = it1;
    ++it1_next;
    Container C2 = C1;
    EXPECT_DEATH( C2.erase(it1) ); // wrong container
    EXPECT_DEATH( C2.erase(C2.end()) ); // erase with end
    C1.erase(it1_next);
    EXPECT_DEATH( C1.erase(it1_next) ); // invalidated iterator
    C1.erase(it1);
    EXPECT_DEATH( C1.erase(it1) ); // invalidated iterator
  }

  static void EraseIterIter() {
    CHECKPOINT("testing erase iter iter invalidation");
    Container C1 = makeContainer(2);
    iterator it1 = C1.begin();
    iterator it1_next = it1;
    ++it1_next;
    Container C2 = C1;
    iterator it2 = C2.begin();
    iterator it2_next = it2;
    ++it2_next;
    EXPECT_DEATH( C2.erase(it1, it1_next) ); // begin from wrong container
    EXPECT_DEATH( C2.erase(it1, it2_next) ); // end   from wrong container
    EXPECT_DEATH( C2.erase(it2, it1_next) ); // both  from wrong container
    C2.erase(it2, it2_next);
  }

  // Allocator aware tests
  static void SwapInvalidatesIterators() {
    CHECKPOINT("testing swap invalidates iterators");
    Container C1 = makeContainer(3);
    Container C2 = makeContainer(3);
    iterator it1 = C1.begin();
    iterator it2 = C2.begin();
    swap(C1, C2);
    EXPECT_DEATH( C1.erase(it1) );
    if (CT == CT_String) {
      EXPECT_DEATH(C1.erase(it2));
    } else
      C1.erase(it2);
    //C2.erase(it1);
    EXPECT_DEATH( C1.erase(it1) );
  }

  static void SwapNonEqualAllocators() {
    CHECKPOINT("testing swap with non-equal allocators");
    Container C1 = makeContainer(3, allocator_type(1));
    Container C2 = makeContainer(1, allocator_type(2));
    Container C3 = makeContainer(2, allocator_type(2));
    swap(C2, C3);
    EXPECT_DEATH( swap(C1, C2) );
  }

private:
  BasicContainerChecks() = delete;
};

} // namespace IteratorDebugChecks

#endif // TEST_SUPPORT_DEBUG_MODE_HELPER_H
