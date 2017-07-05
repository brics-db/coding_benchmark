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
    T a0(1);
    a0 <<= codewidth;
    // std::vector<T> a, b, q, r, s, t;
    T a[16], b[16], q[16], r[16], s[16], t[16];
    // a.push_back(a0), b.push_back(b0), s.push_back(T(0)), t.push_back(T(0));
    uint8_t aI = 1, bI = 1, qI = 0, rI = 0, sI = 1, tI = 1;
    a[0] = a0;
    b[0] = b0;
    s[0] = t[0] = T(0);
    ssize_t i = 0;
    do {
        // q.push_back(a[i] / b[i]);
        q[qI++] = a[i] / b[i];
        // r.push_back(a[i] % b[i]);
        r[rI++] = a[i] % b[i];
        // a.push_back(b[i]);
        a[aI++] = b[i];
        // b.push_back(r[i]);
        b[bI++] = r[i];
        // s.push_back(0);
        s[sI++] = 0;
        // t.push_back(0);
        t[tI++] = 0;
    } while (b[++i] > 0);
    s[i] = 1;
    t[i] = 0;

    for (ssize_t j = i; j > 0; --j) {
        s[j - 1] = t[j];
        t[j - 1] = s[j] - q[j - 1] * t[j];
    }
    // std::cerr << "[ext_eclidean] " << b0 << ": " << std::max(q.size(), std::max(r.size(), std::max(a.size(), std::max(b.size(), std::max(s.size(), t.size()))))) << std::endl;
    // T result = ((b0 * t.front()) % a0);
    T result = ((b0 * t[0]) % a0);
    result += result < 0 ? a0 : 0;
    if (result == 1) {
        return t[0];
    } else {
        return 0;
    }
}

#endif /* EUCLIDEAN_HPP */
