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


#include "filesystem_include.hpp"
#include <type_traits>
#include <cassert>

#include "test_macros.h"
#include "rapid-cxx-test.hpp"
#include "filesystem_test_helper.hpp"

#ifdef _LIBCPP_ENABLE_FILESYSTEM_DIRECTORY_ENTRY_STRICT_ERROR_REPORTING
#error macro should not be defined by default
#endif

TEST_SUITE(directory_entry_refresh_suite)

TEST_CASE(ctor_throws_dne) {
  using namespace fs;

  const path p = StaticEnv::DNE;
  std::error_code ec = GetTestEC();
  directory_entry e(p, ec);
  TEST_CHECK(ErrorIs(ec, std::errc::no_such_file_or_directory));

  TEST_CHECK_NO_THROW(directory_entry(p));
}

TEST_CASE(assign_throws_dne) {
  using namespace fs;
  const path p = StaticEnv::DNE;
  directory_entry e;

  std::error_code ec = GetTestEC();
  e.assign(p, ec);
  TEST_CHECK(ErrorIs(ec, std::errc::no_such_file_or_directory));

  e = directory_entry{};
  TEST_CHECK_NO_THROW(e.assign(p));
}

TEST_CASE(replace_filename_throws_dne) {
  using namespace fs;
  const path good_file = StaticEnv::File;
  TEST_REQUIRE(exists(good_file));

  const path full_path = StaticEnv::DNE;
  TEST_REQUIRE(full_path.parent_path() == good_file.parent_path());
  const path fname = full_path.filename();

  directory_entry e(good_file);

  std::error_code ec = GetTestEC();
  e.replace_filename(fname, ec);
  TEST_CHECK(ErrorIs(ec, std::errc::no_such_file_or_directory));

  e = directory_entry{good_file};
  TEST_CHECK_NO_THROW(e.replace_filename(fname));
}

TEST_CASE(refresh_throws_dne) {
  using namespace fs;

  const path p = StaticEnv::DNE;
  directory_entry e;
  std::error_code dummy_ec;
  e.assign(p, dummy_ec);
  TEST_REQUIRE(dummy_ec);
  TEST_REQUIRE(e.path() == p);

  std::error_code ec = GetTestEC();
  e.refresh(ec);
  TEST_CHECK(ErrorIs(ec, std::errc::no_such_file_or_directory));

  TEST_CHECK_NO_THROW(e.refresh());
}

TEST_SUITE_END()
