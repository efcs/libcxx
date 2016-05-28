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

// uintmax_t remove_all(const path& p);
// uintmax_t remove_all(const path& p, error_code& ec) noexcept;

#include <experimental/filesystem>
#include <type_traits>
#include <chrono>
#include <thread>
#include <cassert>

#include "test_macros.h"
#include "rapid-cxx-test.hpp"
#include "filesystem_test_helper.hpp"

using namespace std::experimental::filesystem;
namespace fs = std::experimental::filesystem;

TEST_SUITE(filesystem_remove_all_test_suite)

TEST_CASE(test_signatures)
{
    const path p; ((void)p);
    std::error_code ec; ((void)ec);
    ASSERT_SAME_TYPE(decltype(fs::remove_all(p)), std::uintmax_t);
    ASSERT_SAME_TYPE(decltype(fs::remove_all(p, ec)), std::uintmax_t);

    ASSERT_NOT_NOEXCEPT(fs::remove_all(p));
    ASSERT_NOEXCEPT(fs::remove_all(p, ec));
}

TEST_CASE(test_error_reporting)
{
    auto checkThrow = [](path const& f, const std::error_code& ec)
    {
        try {
            fs::remove_all(f);
            return false;
        } catch (filesystem_error const& err) {
            return err.path1() == f
                && err.path2() == ""
                && err.code() == ec;
        }
    };
    scoped_test_env env;
    const path non_empty_dir = env.create_dir("dir");
    env.create_file(non_empty_dir / "file1", 42);
    const path bad_perms_dir = env.create_dir("bad_dir");
    const path file_in_bad_dir = env.create_file(bad_perms_dir / "file", 42);
    permissions(bad_perms_dir, perms::none);
    const path bad_perms_file = env.create_file("file2", 42);
    permissions(bad_perms_file, perms::none);

    const path testCases[] = {
        "",
        env.make_env_path("dne"),
        file_in_bad_dir,
        bad_perms_file,
    };
    const std::uintmax_t BadRet = -1;
    for (auto& p : testCases) {
        std::error_code ec;
        TEST_CHECK(fs::remove_all(p, ec) == BadRet);
        TEST_CHECK(ec);
        TEST_CHECK(checkThrow(p, ec));
    }
}

TEST_CASE(basic_remove_all_test)
{
    scoped_test_env env;
    const path dne = env.make_env_path("dne");
    const path link = env.create_symlink(dne, "link");
    const path nested_link = env.make_env_path("nested_link");
    create_symlink(link, nested_link);
    const path testCases[] = {
        env.create_file("file", 42),
        env.create_dir("empty_dir"),
        nested_link,
        link
    };
    for (auto& p : testCases) {
        std::error_code ec = std::make_error_code(std::errc::address_in_use);
        TEST_CHECK(remove(p, ec));
        TEST_CHECK(!ec);
        TEST_CHECK(!exists(symlink_status(p)));
    }
}

TEST_CASE(symlink_to_dir)
{
    scoped_test_env env;
    const path dir = env.create_dir("dir");
    const path file = env.create_file(dir / "file", 42);
    const path link = env.create_symlink(dir, "sym");

    {
        std::error_code ec = std::make_error_code(std::errc::address_in_use);
        TEST_CHECK(remove_all(link, ec) == 1);
        TEST_CHECK(!ec);
        TEST_CHECK(!exists(symlink_status(link)));
        TEST_CHECK(exists(dir));
        TEST_CHECK(exists(file));
    }
}

TEST_SUITE_END()