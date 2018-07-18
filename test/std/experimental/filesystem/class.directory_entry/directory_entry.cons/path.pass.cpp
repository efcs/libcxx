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

//          directory_entry() noexcept = default;
//          directory_entry(const directory_entry&) = default;
//          directory_entry(directory_entry&&) noexcept = default;
// explicit directory_entry(const path);

#include "filesystem_include.hpp"
#include <type_traits>
#include <cassert>

#include "test_macros.h"
#include "rapid-cxx-test.hpp"
#include "filesystem_test_helper.hpp"
#include "test_convertible.hpp"

TEST_SUITE(directory_entry_path_ctor_suite)

TEST_CASE(path_ctor) {
  using namespace fs;
  {
    static_assert(std::is_constructible<directory_entry, const path&>::value,
                  "directory_entry must be constructible from path");
    static_assert(
        !std::is_nothrow_constructible<directory_entry, const path&>::value,
        "directory_entry constructor should not be noexcept");
    static_assert(!std::is_convertible<path const&, directory_entry>::value,
                  "directory_entry constructor should be explicit");
  }
  {
    const path p("foo/bar/baz");
    const directory_entry e(p);
    TEST_CHECK(e.path() == p);
  }
}

TEST_CASE(path_ec_ctor) {
  using namespace fs;
  {
    static_assert(
        std::is_constructible<directory_entry, const path&,
                              std::error_code&>::value,
        "directory_entry must be constructible from path and error_code");
    static_assert(!std::is_nothrow_constructible<directory_entry, const path&,
                                                 std::error_code&>::value,
                  "directory_entry constructor should not be noexcept");
    static_assert(
        test_convertible<directory_entry, const path&, std::error_code&>(),
        "directory_entry constructor should not be explicit");
  }
  {
    std::error_code ec = GetTestEC();
    const directory_entry e(StaticEnv::File, ec);
    TEST_CHECK(e.path() == StaticEnv::File);
    TEST_CHECK(!ec);
  }
  {
    const path p("foo/bar/baz");
    std::error_code ec = GetTestEC();
    const directory_entry e(p, ec);
    TEST_CHECK(e.path() == p);
    TEST_CHECK(ec);
    TEST_CHECK(ec != GetTestEC());
    TEST_CHECK(ec ==
               std::make_error_code(std::errc::no_such_file_or_directory));
  }
}

TEST_CASE(path_ctor_calls_refresh) {
  using namespace fs;
  scoped_test_env env;
  const path dir = env.create_dir("dir");
  const path file = env.create_file("dir/file", 42);
  const path sym = env.create_symlink("dir/file", "sym");

  {
    fs::directory_entry ent(file);
    std::error_code ec = GetTestEC();
    fs::directory_entry ent_ec(file, ec);
    TEST_CHECK(!ec);

    fs::remove(file);

    TEST_CHECK(ent.exists());
    TEST_CHECK(ent_ec.exists());

    TEST_CHECK(ent.file_size() == 42);
    TEST_CHECK(ent_ec.file_size() == 42);
  }

  env.create_file("dir/file", 101);

  {
    fs::directory_entry ent(sym);
    std::error_code ec = GetTestEC();
    fs::directory_entry ent_ec(sym, ec);
    TEST_CHECK(!ec);

    fs::remove(file);
    fs::remove(sym);

    TEST_CHECK(ent.is_symlink());
    TEST_CHECK(ent_ec.is_symlink());

    TEST_CHECK(ent.is_regular_file());
    TEST_CHECK(ent_ec.is_regular_file());

    TEST_CHECK(ent.file_size() == 101);
    TEST_CHECK(ent_ec.file_size() == 101);
  }
}

TEST_CASE(path_ctor_dne) {
  using namespace fs;

  {
    std::error_code ec = GetTestEC();
    fs::directory_entry ent(StaticEnv::DNE, ec);
    TEST_CHECK(ec);
    TEST_CHECK(ec != GetTestEC());
    TEST_CHECK(ec ==
               std::make_error_code(std::errc::no_such_file_or_directory));
    TEST_CHECK(ent.path() == StaticEnv::DNE);
  }
  {
    std::error_code ec = GetTestEC();
    fs::directory_entry ent(StaticEnv::BadSymlink, ec);
    TEST_CHECK(ec);
    TEST_CHECK(ec != GetTestEC());
    TEST_CHECK(ec ==
               std::make_error_code(std::errc::no_such_file_or_directory));
    TEST_CHECK(ent.path() == StaticEnv::BadSymlink);
  }
  // DNE does not cause the constructor to throw
  {
    directory_entry ent(StaticEnv::DNE);
    TEST_CHECK(ent.path() == StaticEnv::DNE);

    directory_entry ent_two(StaticEnv::BadSymlink);
    TEST_CHECK(ent_two.path() == StaticEnv::BadSymlink);
  }
}

TEST_CASE(path_ctor_cannot_resolve) {
  using namespace fs;
  scoped_test_env env;
  const path dir = env.create_dir("dir");
  const path file = env.create_file("dir/file", 42);
  const path sym = env.create_symlink("dir/file", "sym");
  permissions(dir, perms::none);

  {
    std::error_code ec = GetTestEC();
    fs::directory_entry ent(file, ec);
    TEST_CHECK(ec);
    TEST_CHECK(ec != GetTestEC());
    TEST_CHECK(ec == std::make_error_code(std::errc::permission_denied));
    TEST_CHECK(ent.path() == path{});
  }
  {
    std::error_code ec = GetTestEC();
    fs::directory_entry ent(sym, ec);
    TEST_CHECK(ec);
    TEST_CHECK(ec != GetTestEC());
    TEST_CHECK(ec == std::make_error_code(std::errc::permission_denied));
    TEST_CHECK(ent.path() == path{});
  }
#ifndef TEST_HAS_NO_EXCEPTIONS
  {
    TEST_CHECK_THROW(fs::filesystem_error, fs::directory_entry(file));
    TEST_CHECK_THROW(fs::filesystem_error, fs::directory_entry(sym));
  }
#endif
}

TEST_SUITE_END()