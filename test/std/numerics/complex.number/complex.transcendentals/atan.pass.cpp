//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// <complex>

// template<class T>
//   complex<T>
//   atan(const complex<T>& x);

#include <complex>
#include <cassert>

#include "truncate_fp.h"

#include "../cases.h"

template <class T>
void
test(const std::complex<T>& c, std::complex<T> x)
{
    assert(atan(c) == x);
}

template <class T>
void
test()
{
    test(std::complex<T>(0, 0), std::complex<T>(0, 0));
}

void test_edges()
{
    const unsigned N = sizeof(testcases) / sizeof(testcases[0]);
    for (unsigned i = 0; i < N; ++i)
    {
        std::complex<double> r = std::atan(testcases[i]);
        std::complex<double> t1(-imag(testcases[i]), real(testcases[i]));
        std::complex<double> t2 = atanh(t1);
        std::complex<double> z(truncate_fp(imag(t2)), truncate_fp(-real(t2)));
        if (std::isnan(real(r)))
            assert(std::isnan(real(z)));
        else
        {
            assert(real(r) == real(z));
            assert(std::signbit(real(r)) == std::signbit(real(z)));
        }

        double imag_r = truncate_fp(imag(r));
        double imag_z = truncate_fp(imag(z));
        if (std::isnan(imag_r))
            assert(std::isnan(imag_z));
        else
        {
            assert(imag_r == imag_z);
            assert(std::signbit(imag_r) == std::signbit(imag_z));
        }
    }
}

int main(int, char**)
{
    test<float>();
    test<double>();
    test<long double>();
    test_edges();

  return 0;
}
