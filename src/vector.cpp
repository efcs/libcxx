//===------------------------- vector.cpp ---------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "vector"

_LIBCPP_BEGIN_NAMESPACE_STD

template class _LIBCPP_CLASS_TEMPLATE_INSTANTIATION_VIS __vector_base_common<true>;
_LIBCPP_INSTANTIATE_VECTORS( )

template class _LIBCPP_EXTERN_TEMPLATE_TYPE_VIS vector<bool>;


_LIBCPP_END_NAMESPACE_STD
