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

// file_time_type last_write_time() const;
// file_time_type last_write_time(error_code const&) const noexcept;

#include "filesystem_include.hpp"
#include <type_traits>
#include <cassert>

#include "filesystem_test_helper.hpp"
#include "rapid-cxx-test.hpp"

TEST_SUITE(directory_entry_obs_testsuite)

TEST_CASE(signatures) {
  using namespace fs;
  {
    const fs::directory_entry e;
    std::error_code ec;
    static_assert(std::is_same<decltype(e.last_write_time()), file_time_type>::value,
                  "");
    static_assert(std::is_same<decltype(e.last_write_time(ec)), file_time_type>::value,
                  "");
    static_assert(noexcept(e.last_write_time()) == false, "");
    static_assert(noexcept(e.last_write_time(ec)) == true, "");
  }
}

TEST_SUITE_END()
