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

TEST_CASE(test_refresh_method) {
  using namespace fs;
  {
    directory_entry e;
    static_assert(noexcept(e.refresh()) == false,
                  "operation cannot be noexcept");
    static_assert(std::is_same<decltype(e.refresh()), void>::value,
                  "operation must return void");
  }
  {
    directory_entry e;
    e.refresh();
    TEST_CHECK(!e.exists());
  }
}

TEST_CASE(test_refresh_ec_method) {
  using namespace fs;
  {
    directory_entry e;
    std::error_code ec;
    static_assert(noexcept(e.refresh(ec)), "operation should be noexcept");
    static_assert(std::is_same<decltype(e.refresh(ec)), void>::value,
                  "operation must return void");
  }
  {
    directory_entry e;
    std::error_code ec = GetTestEC();
    e.refresh(ec);
    TEST_CHECK(ErrorIs(ec, std::errc::no_such_file_or_directory));
  }
}

TEST_CASE(refresh_on_file_dne) {
  using namespace fs;
  scoped_test_env env;
  const path dir = env.create_dir("dir");
  const path file = env.create_file("dir/file", 42);

  const perms old_perms = fs::status(dir).permissions();

  // test file doesn't exist
  {
    fs::directory_entry ent(file);
    fs::remove(file);
    TEST_CHECK(ent.exists());

    ent.refresh();

    fs::permissions(dir, perms::none);
    TEST_CHECK(!ent.exists());
  }
  fs::permissions(dir, old_perms);
  env.create_file("dir/file", 101);
  {
    fs::directory_entry ent(file);
    fs::remove(file);
    TEST_CHECK(ent.exists());

    std::error_code ec = GetTestEC();
    ent.refresh(ec);
    TEST_CHECK(ErrorIs(ec, std::errc::no_such_file_or_directory));

    fs::permissions(dir, perms::none);
    TEST_CHECK(!ent.exists());
  }
}

TEST_CASE(refresh_on_bad_symlink) {
  using namespace fs;
  scoped_test_env env;
  const path dir = env.create_dir("dir");
  const path file = env.create_file("dir/file", 42);
  const path sym = env.create_symlink("dir/file", "sym");

  const perms old_perms = fs::status(dir).permissions();

  // test file doesn't exist
  {
    fs::directory_entry ent(sym);
    fs::remove(file);
    TEST_CHECK(ent.is_symlink());
    TEST_CHECK(ent.is_regular_file());
    TEST_CHECK(ent.exists());

    ent.refresh();

    fs::permissions(dir, perms::none);
    TEST_CHECK(ent.is_symlink());
    TEST_CHECK(!ent.is_regular_file());
    TEST_CHECK(!ent.exists());
  }
  fs::permissions(dir, old_perms);
  env.create_file("dir/file", 101);
  {
    fs::directory_entry ent(sym);
    fs::remove(file);
    TEST_CHECK(ent.is_symlink());
    TEST_CHECK(ent.is_regular_file());
    TEST_CHECK(ent.exists());

    std::error_code ec = GetTestEC();
    ent.refresh(ec);
    TEST_CHECK(ErrorIs(ec, std::errc::no_such_file_or_directory));

    fs::permissions(dir, perms::none);
    TEST_CHECK(!ent.exists());
  }
}

TEST_CASE(refresh_cannot_resolve) {
  using namespace fs;
  scoped_test_env env;
  const path dir = env.create_dir("dir");
  const path file = env.create_file("dir/file", 42);
  const path sym = env.create_symlink("dir/file", "sym");

  perms old_perms = fs::status(dir).permissions();

  {
    fs::directory_entry ent(file);
    permissions(dir, perms::none);

    TEST_CHECK(ent.is_regular_file());

    std::error_code ec = GetTestEC();
    ent.refresh(ec);

    TEST_CHECK(ErrorIs(ec, std::errc::permission_denied));
    TEST_CHECK(ent.path() == file);
  }
  fs::permissions(dir, old_perms);
  {
    fs::directory_entry ent(sym);
    permissions(dir, perms::none);
    TEST_CHECK(ent.is_symlink());

    std::error_code ec = GetTestEC();
    ent.refresh(ec);

    TEST_CHECK(ErrorIs(ec, std::errc::permission_denied));
    TEST_CHECK(ent.path() == sym);
  }
  fs::permissions(dir, old_perms);
#ifndef TEST_HAS_NO_EXCEPTIONS
  {
    fs::directory_entry ent_file(file);
    fs::directory_entry ent_sym(sym);
    fs::permissions(dir, perms::none);

    TEST_CHECK_THROW(fs::filesystem_error, ent_file.refresh());
    TEST_CHECK_THROW(fs::filesystem_error, ent_sym.refresh());
  }
#endif
}

TEST_CASE(refresh_doesnt_throw_on_dne_but_reports_it) {
  using namespace fs;
  scoped_test_env env;

  const path file = env.create_file("file1", 42);
  const path sym = env.create_symlink("file1", "sym");

  {
    directory_entry ent(file);
    TEST_CHECK(ent.file_size() == 42);

    remove(file);

    std::error_code ec = GetTestEC();
    ent.refresh(ec);
    TEST_CHECK(ErrorIs(ec, std::errc::no_such_file_or_directory));

    ec = GetTestEC();
    TEST_CHECK(ent.file_size(ec) == uintmax_t(-1));
    TEST_CHECK(ErrorIs(ec, std::errc::no_such_file_or_directory));

    // doesn't throw!
    TEST_CHECK(ent.file_size() == uintmax_t(-1));
  }
  env.create_file("file1", 99);
  {
    directory_entry ent(sym);
    TEST_CHECK(ent.is_symlink());
    TEST_CHECK(ent.is_regular_file());
    TEST_CHECK(ent.file_size() == 99);

    remove(file);

    std::error_code ec = GetTestEC();
    ent.refresh(ec);
    TEST_CHECK(ErrorIs(ec, std::errc::no_such_file_or_directory));

    ec = GetTestEC();
    TEST_CHECK(ent.file_size(ec) == uintmax_t(-1));
    TEST_CHECK(ErrorIs(ec, std::errc::no_such_file_or_directory));

    // doesn't throw!
    TEST_CHECK(ent.file_size() == uintmax_t(-1));
  }
}

TEST_CASE(access_cache_after_refresh_fails) {
  using namespace fs;
  scoped_test_env env;
  const path dir = env.create_dir("dir");
  const path file = env.create_file("dir/file", 42);
  const path file_out_of_dir = env.create_file("file1", 101);
  const path sym = env.create_symlink("dir/file", "sym");
  const path sym_in_dir = env.create_symlink("dir/file", "dir/sym2");

  const perms old_perms = fs::status(dir).permissions();

#define CHECK_ACCESS(func, expect)                                             \
  ec = GetTestEC();                                                            \
  TEST_CHECK(ent.func(ec) == expect);                                          \
  TEST_CHECK(ErrorIs(ec, std::errc::permission_denied))

  // test file doesn't exist
  {
    fs::directory_entry ent(file);

    TEST_CHECK(!ent.is_symlink());
    TEST_CHECK(ent.is_regular_file());
    TEST_CHECK(ent.exists());

    fs::permissions(dir, perms::none);
    std::error_code ec = GetTestEC();
    ent.refresh(ec);
    TEST_CHECK(ErrorIs(ec, std::errc::permission_denied));

    CHECK_ACCESS(exists, false);
    CHECK_ACCESS(is_symlink, false);
    CHECK_ACCESS(last_write_time, file_time_type::min());
    CHECK_ACCESS(hard_link_count, uintmax_t(-1));
  }
  fs::permissions(dir, old_perms);
  {
    fs::directory_entry ent(sym_in_dir);
    TEST_CHECK(ent.is_symlink());
    TEST_CHECK(ent.is_regular_file());
    TEST_CHECK(ent.exists());

    fs::permissions(dir, perms::none);
    std::error_code ec = GetTestEC();
    ent.refresh(ec);
    TEST_CHECK(ErrorIs(ec, std::errc::permission_denied));

    CHECK_ACCESS(exists, false);
    CHECK_ACCESS(is_symlink, false);
    CHECK_ACCESS(last_write_time, file_time_type::min());
    CHECK_ACCESS(hard_link_count, uintmax_t(-1));
  }
  fs::permissions(dir, old_perms);
  {
    fs::directory_entry ent(sym);
    TEST_CHECK(ent.is_symlink());
    TEST_CHECK(ent.is_regular_file());
    TEST_CHECK(ent.exists());

    fs::permissions(dir, perms::none);
    std::error_code ec = GetTestEC();
    ent.refresh(ec);
    TEST_CHECK(ErrorIs(ec, std::errc::permission_denied));

    TEST_CHECK(ent.is_symlink());

    CHECK_ACCESS(exists, false);
    CHECK_ACCESS(is_regular_file, false);
    CHECK_ACCESS(last_write_time, file_time_type::min());
    CHECK_ACCESS(hard_link_count, uintmax_t(-1));
  }
#undef CHECK_ACCESS
}

TEST_SUITE_END()
