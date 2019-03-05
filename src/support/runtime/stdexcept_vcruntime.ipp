// -*- C++ -*-
//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef _LIBCPP_ABI_VCRUNTIME
#error this header can only be used when defering to vcruntime
#endif

namespace std {
  logic_error::logic_error(const _VSTD::string& __s) : exception(__s.c_str()) {}
  runtime_error::runtime_error(const _VSTD::string& __s) : exception(__s.c_str()) {}
}
