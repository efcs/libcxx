//===---------------------- system_error.cpp ------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "__config"

_LIBCPP_BEGIN_NAMESPACE_STD

#if defined(_LIBCPP_DEPRECATED_ABI_LEGACY_LIBRARY_DEFINITIONS_FOR_INLINE_FUNCTIONS)
class error_condition;
class error_code;

class _LIBCPP_TYPE_VIS error_category
{
public:
    virtual ~error_category() _NOEXCEPT;

    error_category() _NOEXCEPT;

private:
    error_category(const error_category&);// = delete;
    error_category& operator=(const error_category&);// = delete;

public:
    virtual const char* name() const _NOEXCEPT = 0;
    virtual error_condition default_error_condition(int __ev) const _NOEXCEPT;
    virtual bool equivalent(int __code, const error_condition& __condition) const _NOEXCEPT;
    virtual bool equivalent(const error_code& __code, int __condition) const _NOEXCEPT;
    virtual string message(int __ev) const = 0;

};

error_category::error_category() _NOEXCEPT
{
}
#endif // _LIBCPP_DEPRECATED_ABI_LEGACY_LIBRARY_DEFINITIONS_FOR_INLINE_FUNCTIONS


_LIBCPP_END_NAMESPACE_STD
