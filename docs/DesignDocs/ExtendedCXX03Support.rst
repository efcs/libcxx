=======================
Extended C++03 Support
=======================

.. contents::
   :local:

Overview
========

libc++ was designed from the ground up to target the C++11. As such, libc++
provides many C++11 components in C++03 mode.

C++11 Compiler Extensions
=========================

Clang provides a large subset of C++11 in C++03 as an extension. The features
libc++ expects Clang  to provide are:

* Variadic Templates.
* R-Value References.
* Alias Templates
* `= default` and `= delete`
* Reference qualified functions

There are also features that Clang *does not* provide as an extension in C++03
mode. These include:

* `constexpr` and `noexcept`
*  Trailing return types.
* `>>` without a space.


Macros
======

**_LIBCPP_CXX03_LANG**
  Defined when libc++ is targeting C++03.

**_LIBCPP_EXTENDED_CXX03_LANG**
  Defined when libc++ is targeting C++03 with a compiler that provides
  the required C++11 language features as extensions.

**_LIBCPP_MINIMAL_CXX11_LANG**
  Defined when `!defined(_LIBCPP_CXX03_LANG) || defined(_LIBCPP_EXTENDED_CXX03_LANG)`.
