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

// uintmax_t file_size() const;
// uintmax_t file_size(error_code const&) const noexcept;

#include "filesystem_include.hpp"
#include <type_traits>
#include <cassert>

#include "filesystem_test_helper.hpp"
#include "rapid-cxx-test.hpp"

TEST_SUITE(directory_entry_obs_testsuite)

TEST_CASE(signatures) {
  using namespace fs;
  {
    const fs::directory_entry e("foo");
    std::error_code ec;
    static_assert(std::is_same<decltype(e.file_size()), uintmax_t>::value, "");
    static_assert(std::is_same<decltype(e.file_size(ec)), uintmax_t>::value,
                  "");
    static_assert(noexcept(e.file_size()) == false, "");
    static_assert(noexcept(e.file_size(ec)) == true, "");
  }
}

TEST_CASE(basic) {
  using namespace fs;

  scoped_test_env env;
  const path file = env.create_file("file", 42);
  const path file2 = env.create_file("file2", 101);
  const path sym = env.create_symlink("file", "sym");

  {
    directory_entry ent(file);
    remove(file);
    std::error_code ec = GetTestEC();
    TEST_CHECK(ent.file_size(ec) == 42);
    TEST_CHECK(!ec);
  }
  {
    directory_entry ent(file2);
    remove(file2);
    std::error_code ec = GetTestEC();
    TEST_CHECK(ent.file_size(ec) == 101);
    TEST_CHECK(!ec);
  }
  env.create_file("file", 99);
  {
    directory_entry ent(sym);
    std::error_code ec = GetTestEC();
    TEST_CHECK(ent.file_size(ec) == 99);
    TEST_CHECK(!ec);
  }
}

TEST_CASE(not_regular_file) {
  using namespace fs;

  scoped_test_env env;
  const path dir = env.create_dir("dir");
  const path fifo = env.create_fifo("fifo");
  const path sym_to_dir = env.create_symlink("dir", "sym");

  {
    directory_entry ent(dir);
    std::error_code ec = GetTestEC();
    TEST_CHECK(ent.file_size(ec) == uintmax_t(-1));
    TEST_CHECK(ErrorIs(ec, std::errc::not_supported));
  }
  {
    directory_entry ent(fifo);
    std::error_code ec = GetTestEC();
    TEST_CHECK(ent.file_size(ec) == uintmax_t(-1));
    TEST_CHECK(ErrorIs(ec, std::errc::not_supported));
  }
  {
    directory_entry ent(sym_to_dir);
    std::error_code ec = GetTestEC();
    TEST_CHECK(ent.file_size(ec) == uintmax_t(-1));
    TEST_CHECK(ErrorIs(ec, std::errc::not_supported));
  }
}

TEST_SUITE_END()
