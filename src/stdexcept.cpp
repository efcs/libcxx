//===------------------------ stdexcept.cpp -------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "stdexcept"
#ifndef _LIBCPP_DEFER_STDEXCEPT_TO_VCRUNTIME
#include "new"
#include "string"
#include "system_error"
#include "include/refstring.h"

/* For _LIBCPPABI_VERSION */
#if !defined(_LIBCPP_BUILDING_HAS_NO_ABI_LIBRARY) && \
    (defined(LIBCXX_BUILDING_LIBCXXABI) || defined(__APPLE__) || defined(LIBCXXRT))
#include <cxxabi.h>
#endif

static_assert(sizeof(std::__libcpp_refstring) == sizeof(const char *), "");


namespace std  // purposefully not using versioning namespace
{

logic_error::logic_error(const string& msg) : __imp_(msg.c_str())
{
}

logic_error::logic_error(const char* msg) : __imp_(msg)
{
}

logic_error::logic_error(const logic_error& le) _NOEXCEPT : __imp_(le.__imp_)
{
}

logic_error&
logic_error::operator=(const logic_error& le) _NOEXCEPT
{
    __imp_ = le.__imp_;
    return *this;
}

runtime_error::runtime_error(const string& msg) : __imp_(msg.c_str())
{
}

runtime_error::runtime_error(const char* msg) : __imp_(msg)
{
}

runtime_error::runtime_error(const runtime_error& le) _NOEXCEPT
  : __imp_(le.__imp_)
{
}

runtime_error&
runtime_error::operator=(const runtime_error& le) _NOEXCEPT
{
    __imp_ = le.__imp_;
    return *this;
}

#if !defined(_LIBCPPABI_VERSION) && !defined(LIBSTDCXX)

const char*
logic_error::what() const _NOEXCEPT
{
    return __imp_.c_str();
}

const char*
runtime_error::what() const _NOEXCEPT
{
    return __imp_.c_str();
}

#if !defined(_LIBCPP_ABI_MICROSOFT) || defined(_LIBCPP_NO_VCRUNTIME)

logic_error::~logic_error() _NOEXCEPT {}
domain_error::~domain_error() _NOEXCEPT {}
invalid_argument::~invalid_argument() _NOEXCEPT {}
length_error::~length_error() _NOEXCEPT {}
out_of_range::~out_of_range() _NOEXCEPT {}

runtime_error::~runtime_error() _NOEXCEPT {}
range_error::~range_error() _NOEXCEPT {}
overflow_error::~overflow_error() _NOEXCEPT {}
underflow_error::~underflow_error() _NOEXCEPT {}

#endif
#endif

}  // std


#endif // !_LIBCPP_DEFER_STDEXCEPT_TO_VCRUNTIME
