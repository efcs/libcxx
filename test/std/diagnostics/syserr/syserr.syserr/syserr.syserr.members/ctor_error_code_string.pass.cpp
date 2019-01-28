//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// <system_error>

// class system_error

// system_error(error_code ec, const string& what_arg);

// Test is slightly non-portable

#include <system_error>
#include <string>
#include <cassert>

#include "test_macros.h"

std::string sanitize_null_like_libcxx(std::string s) {
    size_t pos;
    while ((pos = s.find('\0')) != std::string::npos)
        s.replace(pos, 1, "\\0");
    return s;
}

void do_test(std::string const& what_arg) {
    std::system_error se(make_error_code(std::errc::not_a_directory), what_arg);
    assert(se.code() == std::make_error_code(std::errc::not_a_directory));
    std::string what_message(se.what());
    assert(what_message.find(what_arg.c_str()) != std::string::npos);
    assert(what_message.find("Not a directory") != std::string::npos);

    // Test libc++'s behavior in case of null. libc++ replaces each null in the
    // 'what' argument with "\\0" instead.
    LIBCPP_ASSERT(what_message.find(sanitize_null_like_libcxx(what_arg)) != std::string::npos);
}

int main()
{
    {
        std::string what_arg("my message");
        do_test(what_arg);
    }
    { // test with embedded null
        const char what_msg[] = "test message\0message after null";
        const size_t what_size = sizeof(what_msg) / sizeof(what_msg[0]);
        std::string what_arg(what_msg, what_size-1);
        assert(what_arg.back() == 'l');
        do_test(what_arg);
    }
}
