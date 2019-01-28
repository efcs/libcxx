//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++98, c++03

// <filesystem>

// class filesystem_error

// filesystem_error(const string& what_arg, error_code ec);
// filesystem_error(const string& what_arg, const path& p1, error_code ec);
// filesystem_error(const string& what_arg, const path& p1, const path& p2, error_code ec);
// const std::error_code& code() const;
// const char* what() const noexcept;
// const path& path1() const noexcept;
// const path& path2() const noexcept;

#include "filesystem_include.hpp"
#include <type_traits>
#include <cassert>

#include "test_macros.h"

std::string sanitize_null_like_libcxx(std::string s) {
  size_t pos;
  while ((pos = s.find('\0')) != std::string::npos)
    s.replace(pos, 1, "\\0");
  return s;
}

std::string string_with_null(std::string LHS, std::string RHS) {
  std::string result = std::move(LHS);
  auto old_size = result.size();
  result += '\0';
  assert(result.size() == old_size + 1);
  result += RHS;
  return result;
}

void test_constructors() {
  using namespace fs;

  const std::string what_arg = "Hello World";
  std::error_code ec = std::make_error_code(std::errc::file_exists);
  const path p1("foo");
  const path p2("bar");

  auto CheckWhat = [&](filesystem_error const& e) {
    std::system_error se(e.code(), what_arg);
    std::string what = e.what();
    assert(what.find(what_arg.c_str()) != std::string::npos);
    assert(what.find(se.what()) != std::string::npos);
  };

  // filesystem_error(const string& what_arg, error_code ec);
  {
    ASSERT_NOT_NOEXCEPT(filesystem_error(what_arg, ec));
    filesystem_error e(what_arg, ec);
    CheckWhat(e);
    assert(e.code() == ec);
    assert(e.path1().empty() && e.path2().empty());
  }
  // filesystem_error(const string& what_arg, const path&, error_code ec);
  {
    ASSERT_NOT_NOEXCEPT(filesystem_error(what_arg, p1, ec));
    filesystem_error e(what_arg, p1, ec);
    CheckWhat(e);
    assert(e.code() == ec);
    assert(e.path1() == p1);
    assert(e.path2().empty());
  }
  // filesystem_error(const string& what_arg, const path&, const path&, error_code ec);
  {
    ASSERT_NOT_NOEXCEPT(filesystem_error(what_arg, p1, p2, ec));
    filesystem_error e(what_arg, p1, p2, ec);
    CheckWhat(e);
    assert(e.code() == ec);
    assert(e.path1() == p1);
    assert(e.path2() == p2);
  }
}


void test_constructors_with_null() {
  using namespace fs;

  const std::string what_arg = string_with_null("Hello World", "After Null!");
  std::error_code ec = std::make_error_code(std::errc::file_exists);
  const path p1(string_with_null("foo", "after_null"));
  const path p2(string_with_null("bar", "after_null"));

  const std::string what_arg_sanitized = "Hello World\\0After Null!";
  const std::string p1_sanitized = "foo\\0after_null";
  const std::string p2_sanitized = "bar\\0after_null";
  ((void)what_arg_sanitized); // Only used by libc++
  ((void)p1_sanitized);
  ((void)p2_sanitized);

  auto CheckWhat = [&](filesystem_error const& e, int num_paths) {
    ((void)num_paths);
    std::system_error se(e.code(), what_arg);
    std::string what = e.what();
    assert(what.find(what_arg.c_str()) != std::string::npos);
    assert(what.find(se.what()) != std::string::npos);
    LIBCPP_ASSERT(what.find("After Null!") != std::string::npos);
    LIBCPP_ASSERT(what.find(what_arg_sanitized) != std::string::npos);
    if (num_paths == 1 || num_paths == 2) {
      LIBCPP_ASSERT(what.find(p1_sanitized) != std::string::npos);
    }
    if (num_paths == 2) {
      LIBCPP_ASSERT(what.find(p2_sanitized) != std::string::npos);
    }
  };

  // filesystem_error(const string& what_arg, error_code ec);
  {
    filesystem_error e(what_arg, ec);
    CheckWhat(e, 0);
    assert(e.code() == ec);
    assert(e.path1().empty() && e.path2().empty());
  }
  // filesystem_error(const string& what_arg, const path&, error_code ec);
  {
    ASSERT_NOT_NOEXCEPT(filesystem_error(what_arg, p1, ec));
    filesystem_error e(what_arg, p1, ec);
    CheckWhat(e, 1);
    assert(e.code() == ec);
    assert(e.path1() == p1);
    assert(e.path2().empty());
  }
  // filesystem_error(const string& what_arg, const path&, const path&, error_code ec);
  {
    filesystem_error e(what_arg, p1, p2, ec);
    CheckWhat(e, 2);
    assert(e.code() == ec);
    assert(e.path1() == p1);
    assert(e.path2() == p2);
  }
}

void test_signatures()
{
  using namespace fs;
  const path p;
  std::error_code ec;
  const filesystem_error e("lala", ec);
  // const path& path1() const noexcept;
  {
    ASSERT_SAME_TYPE(path const&, decltype(e.path1()));
    ASSERT_NOEXCEPT(e.path1());
  }
  // const path& path2() const noexcept
  {
    ASSERT_SAME_TYPE(path const&, decltype(e.path2()));
    ASSERT_NOEXCEPT(e.path2());
  }
  // const char* what() const noexcept
  {
    ASSERT_SAME_TYPE(const char*, decltype(e.what()));
    ASSERT_NOEXCEPT(e.what());
  }
}

int main() {
  static_assert(std::is_base_of<std::system_error, fs::filesystem_error>::value, "");
  test_constructors();
  test_constructors_with_null();
  test_signatures();
}
