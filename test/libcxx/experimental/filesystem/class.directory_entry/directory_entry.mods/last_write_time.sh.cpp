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

// RUN: %build -I%libcxx_src_root/src/experimental/filesystem
// RUN: %run

#include "filesystem_include.hpp"
#include <type_traits>
#include <cassert>

#include "test_macros.h"
#include "rapid-cxx-test.hpp"
#include "filesystem_test_helper.hpp"

#include "filesystem_common.h"

using namespace fs::detail;

TEST_SUITE(directory_entry_mods_suite)

TEST_CASE(last_write_time_not_representable_error) {
  using namespace fs;
  using namespace std::chrono;
  scoped_test_env env;
  const path dir = env.create_dir("dir");
  const path file = env.create_file("dir/file", 42);

  TimeSpec ToTime;
  ToTime.tv_sec = std::numeric_limits<decltype(ToTime.tv_sec)>::max();
  ToTime.tv_nsec = duration_cast<nanoseconds>(seconds(1)).count() - 1;

  TimeStructArray TS;
  SetTimeStructTo(TS[0], ToTime);
  SetTimeStructTo(TS[1], ToTime);

  file_time_type old_time = last_write_time(file);
  directory_entry ent(file);

  file_time_type start_time = file_time_type::clock::now() - hours(1);
  last_write_time(file, start_time);

  TEST_CHECK(ent.last_write_time() == old_time);

  bool IsRepresentable = true;
  file_time_type rep_value;
  {
    std::error_code ec;
    if (SetFileTimes(file, TS, ec)) {
      TEST_REQUIRE(false && "unsupported");
    }
    ec.clear();
    rep_value = last_write_time(file, ec);
    IsRepresentable = !bool(ec);
  }

  if (!IsRepresentable) {
    std::error_code ec = GetTestEC();
    ent.refresh(ec);
    TEST_CHECK(ec);
    TEST_CHECK(ec != GetTestEC());

#ifndef TEST_HAS_NO_EXCEPTIONS
    { TEST_CHECK_THROW(fs::filesystem_error, ent.refresh()); }
#endif
  } else {
    std::error_code ec = GetTestEC();
    ent.refresh(ec);
    TEST_CHECK(!ec);
    TEST_CHECK(ent.last_write_time() == rep_value);
  }
}

TEST_SUITE_END()
