// Copyright (c) 2016 Till Kolditz
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*
 * File:   Euclidean.hpp
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 6. Dezember 2016, 00:55
 */

#ifndef EUCLIDEAN_HPP
#define EUCLIDEAN_HPP

#include <iostream>
#include <vector>
#include <cinttypes>

#include <boost/multiprecision/cpp_int.hpp>

using boost::multiprecision::uint128_t;

namespace Private {
    template<typename T, typename S>
    struct extractor {
    };

    template<typename T>
    struct extractor<T, uint128_t> {
        static T doIt(
                const uint128_t source) {
            const constexpr unsigned nBitsLimb = sizeof(boost::multiprecision::limb_type) * CHAR_BIT; // size of limb in bits
            boost::multiprecision::limb_type target = 0;
            const unsigned nLimbs = source.backend().size(); // number of limbs
            auto pLimbs = source.backend().limbs();
            for (unsigned i = 0; i < nLimbs && ((i * nBitsLimb) < (sizeof(T) * CHAR_BIT)); ++i) {
                target |= (pLimbs[i]) << (i * nBitsLimb);
            }
            return static_cast<T>(target);
        }
    };
}

/*
 * This algorithm actually computes the modulo inverse of the given first argument
 * b0 in the residual class ring modulo <codewidth> (the second argument). One
 * requirement of this algorithm is, that template type <T> is large enough to
 * store <codewidth>+1 bits!
 */
template<typename T>
T ext_euclidean(
        T b0,
        size_t codewidth) {
    uint128_t a0(1);
    a0 <<= codewidth;
    // uint128_t a[32], b[32], q[32], r[32], s[32], t[32];
    std::vector<uint128_t> a(32), b(32), q(32), r(32), s(32), t(32);
    int aI = 1, bI = 1, qI = 0, rI = 0, sI = 1, tI = 1;
    a[0] = a0;
    b[0] = b0;
    s[0] = t[0] = T(0);
    ssize_t i = 0;
    do {
        q[qI++] = a[i] / b[i];
        r[rI++] = a[i] % b[i];
        a[aI++] = b[i];
        b[bI++] = r[i];
        s[sI++] = 0;
        t[tI++] = 0;
    } while (b[++i] > 0);
    s[i] = 1;
    t[i] = 0;

    for (ssize_t j = i; j > 0; --j) {
        s[j - 1] = t[j];
        t[j - 1] = s[j] - q[j - 1] * t[j];
    }
    uint128_t result = ((b0 * t[0]) % a0);
    result += result < 0 ? a0 : 0;
    if (result == 1) {
        return Private::extractor<T, uint128_t>::doIt(t[0]);
    } else {
        return 0;
    }
}

#endif /* EUCLIDEAN_HPP */
