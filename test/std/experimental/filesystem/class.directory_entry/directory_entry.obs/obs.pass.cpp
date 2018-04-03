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

TEST_SUITE(directory_entry_obs_testsuite)


TEST_CASE(basic) {
  using namespace fs;
  using fs::path;
  using fs::directory_entry;
  using fs::file_status;

  scoped_test_env env;
  path f = env.create_file("foo", 42);
  path d = env.create_dir("dir");
  path fifo = env.create_fifo("fifo");
  path hl = env.create_hardlink("foo", "hl");
  for (auto p : {hl, f, d, fifo}) {
    directory_entry e(p);
    file_status st = fs::status(p);
    std::error_code size_ec;
    uintmax_t size = fs::file_size(p, size_ec);
    uintmax_t nlink = fs::hard_link_count(p);
    fs::file_time_type write_time = fs::last_write_time(p);
    fs::remove(p);
    TEST_REQUIRE(e.exists());
    TEST_REQUIRE(!exists(p));
    TEST_CHECK(e.status().type() == st.type());
    TEST_CHECK(e.status().permissions() == st.permissions());
    TEST_CHECK(e.symlink_status().type() == st.type());
    TEST_CHECK(e.symlink_status().permissions() == st.permissions());
    TEST_CHECK(e.exists() == exists(st));
    TEST_CHECK(e.is_block_file() == is_block_file(st));
    TEST_CHECK(e.is_character_file() == is_character_file(st));
    TEST_CHECK(e.is_directory() == is_directory(st));
    TEST_CHECK(e.is_fifo() == is_fifo(st));
    TEST_CHECK(e.is_other() == is_other(st));
    TEST_CHECK(e.is_regular_file() == is_regular_file(st));
    TEST_CHECK(e.is_socket() == is_socket(st));
    TEST_CHECK(e.is_symlink() == is_symlink(st));
    TEST_CHECK(e.hard_link_count() == nlink);
    TEST_CHECK(e.last_write_time() == write_time);
    if (!size_ec) {
      TEST_CHECK(e.file_size() == size);
    }
  }
}


TEST_SUITE_END()
