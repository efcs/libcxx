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

TEST_CASE(signatures)
{
  using namespace fs;
  {
    const fs::directory_entry e("foo");
    std::error_code ec;
    static_assert(std::is_same<decltype(e.status()), fs::file_status>::value, "");
    static_assert(std::is_same<decltype(e.status(ec)), fs::file_status>::value, "");
    static_assert(noexcept(e.status()) == false, "");
    static_assert(noexcept(e.status(ec)) == true, "");
  }
}

TEST_CASE(basic) {
  using namespace fs;
  using fs::path;
  using fs::directory_entry;
  using fs::file_status;
  auto TestFn = [](path const& p) {
    const directory_entry e(p);
    std::error_code pec, eec;
    file_status ps = fs::status(p, pec);
    file_status es = e.status(eec);
    return ps.type() == es.type() &&
           ps.permissions() == es.permissions() &&
           pec == eec;
  };
  {
    TEST_CHECK(TestFn(StaticEnv::File));
    TEST_CHECK(TestFn(StaticEnv::Dir));
    TEST_CHECK(TestFn(StaticEnv::SymlinkToFile));
    TEST_CHECK(TestFn(StaticEnv::DNE));
  }
}


TEST_SUITE_END()
