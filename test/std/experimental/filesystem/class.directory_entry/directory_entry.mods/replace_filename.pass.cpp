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

// directory_entry& operator=(directory_entry const&) = default;
// directory_entry& operator=(directory_entry&&) noexcept = default;
// void assign(path const&);
// void replace_filename(path const&);

#include "filesystem_include.hpp"
#include <type_traits>
#include <cassert>

#include "test_macros.h"
#include "rapid-cxx-test.hpp"
#include "filesystem_test_helper.hpp"

TEST_SUITE(directory_entry_mods_suite)

TEST_CASE(test_replace_filename_method) {
  using namespace fs;
  const path p("/path/to/foo.exe");
  const path replace("bar.out");
  const path expect("/path/to/bar.out");
  directory_entry e(p);
  {
    static_assert(noexcept(e.replace_filename(replace)) == false,
                  "operation cannot be noexcept");
    static_assert(
        std::is_same<decltype(e.replace_filename(replace)), void>::value,
        "operation must return void");
  }
  {
    assert(e.path() == p);
    e.replace_filename(replace);
    assert(e.path() == expect);
  }
}

TEST_SUITE_END()
