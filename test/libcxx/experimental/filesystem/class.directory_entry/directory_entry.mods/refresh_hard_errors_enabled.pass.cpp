//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// UNSUPPORTED: c++98, c++03

// <experimental/filesystem>

// class directory_entry

// MODULES_DEFINES: _LIBCPP_ENABLE_FILESYSTEM_DIRECTORY_ENTRY_STRICT_ERROR_REPORTING
#define _LIBCPP_ENABLE_FILESYSTEM_DIRECTORY_ENTRY_STRICT_ERROR_REPORTING

#include "filesystem_include.hpp"
#include <type_traits>
#include <cassert>

#include "test_macros.h"
#include "rapid-cxx-test.hpp"
#include "filesystem_test_helper.hpp"

TEST_SUITE(directory_entry_refresh_suite)

struct Checker {
  std::errc expected_err;
  fs::path expected_path;

  explicit Checker(fs::path p, std::errc expected_err = std::errc::no_such_file_or_directory)
    : expected_err(expected_err), expected_path(p) {}

  void operator()(fs::filesystem_error const& Err) const {
    TEST_CHECK(ErrorIs(Err.code(), expected_err));
    TEST_CHECK(Err.path1() == expected_path);
  }
};

TEST_CASE(ctor_throws_dne) {
  using namespace fs;

  for (path p : {StaticEnv::DNE, StaticEnv::BadSymlink}) {
    std::error_code ec = GetTestEC();
    directory_entry e(p, ec);
    TEST_CHECK(ErrorIs(ec, std::errc::no_such_file_or_directory));

    Checker CheckExcept(p);
    TEST_CHECK_THROW_RESULT(filesystem_error, CheckExcept, directory_entry(p));
  }
}

TEST_CASE(assign_throws_dne) {
  using namespace fs;
  for (path p : {StaticEnv::DNE, StaticEnv::BadSymlink}) {
    directory_entry e;

    std::error_code ec = GetTestEC();
    e.assign(p, ec);
    TEST_CHECK(ErrorIs(ec, std::errc::no_such_file_or_directory));

    e = directory_entry{};
    Checker CheckExcept(p);
    TEST_CHECK_THROW_RESULT(filesystem_error, CheckExcept, e.assign(p));
  }
}


TEST_CASE(replace_filename_throws_dne) {
  using namespace fs;
  const path good_file = StaticEnv::File;
  TEST_REQUIRE(exists(good_file));

  for (path full_path : {StaticEnv::DNE, StaticEnv::BadSymlink}) {
    TEST_REQUIRE(full_path.parent_path() == good_file.parent_path());
    const path fname = full_path.filename();

    directory_entry e(good_file);

    std::error_code ec = GetTestEC();
    e.replace_filename(fname, ec);
    TEST_CHECK(ErrorIs(ec, std::errc::no_such_file_or_directory));

    e = directory_entry{good_file};
    Checker CheckExcept(full_path);
    TEST_CHECK_THROW_RESULT(filesystem_error, CheckExcept, e.replace_filename(fname));
  }
}


TEST_CASE(refresh_throws_dne) {
  using namespace fs;

  for (path p : {StaticEnv::DNE, StaticEnv::BadSymlink}) {
    directory_entry e;
    std::error_code dummy_ec;
    e.assign(p, dummy_ec);
    TEST_REQUIRE(dummy_ec);
    TEST_REQUIRE(e.path() == p);

    std::error_code ec = GetTestEC();
    e.refresh(ec);
    TEST_CHECK(ErrorIs(ec, std::errc::no_such_file_or_directory));

    Checker CheckExcept(p);
    TEST_CHECK_THROW_RESULT(filesystem_error, CheckExcept, e.refresh());
  }
}

TEST_SUITE_END()
