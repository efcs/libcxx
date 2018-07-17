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

// file_status status() const;
// file_status status(error_code const&) const noexcept;

#include "filesystem_include.hpp"
#include <type_traits>
#include <cassert>

#include "filesystem_test_helper.hpp"
#include "rapid-cxx-test.hpp"

TEST_SUITE(directory_entry_status_testsuite)

TEST_CASE(example_not_mutually_exclusive) {
  using namespace fs;
  using fs::path;
  using fs::directory_entry;
  using fs::directory_iterator;
  using fs::file_status;

  scoped_test_env env;
  path d = env.create_dir("dir");
  path f = env.create_file("file", 42);
  path s = env.create_symlink(f, d / "sym");

  directory_entry ent(s);
  TEST_CHECK(ent.is_symlink());
  TEST_CHECK(ent.is_regular_file());
}

TEST_CASE(example_one) {
  using namespace fs;
  using fs::path;
  using fs::directory_entry;
  using fs::directory_iterator;
  using fs::file_status;

  scoped_test_env env;
  path d = env.create_dir("dir");
  path s = env.create_symlink("dne", d / "sym");
  {
    // We get the directory entry for the symlink via a directory_iterator.
    directory_iterator it(d);
    directory_entry ent = *it;
    TEST_REQUIRE(ent.path() == s);
    // Somebody else writes over the symlink with a real file.
    // TOCTOU violation.
    fs::remove(s);
    // Sometime later, somebody looks at the directory_entry caches.
    TEST_CHECK(ent.is_symlink() == true); // Passes, the value is cached.
    TEST_CHECK(ent.exists() == false); // Passes, value not cached.
  }
}

TEST_CASE(sample) {
  using namespace fs;
  using fs::path;
  using fs::directory_entry;
  using fs::directory_iterator;
  using fs::file_status;

  scoped_test_env env;
  path d = env.create_dir("dir");
  path f = env.create_file("file", 42);
  env.create_symlink(f, d / "sym_one");
  env.create_symlink(f, d / "sym_two");


  for (auto ent : directory_iterator(d)) {
    if (ent.is_symlink())
      fs::remove(ent);
  }
}


TEST_SUITE_END()
