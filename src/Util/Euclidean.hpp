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

#include <vector>

/*
 * This algorithm actually computes the modulo inverse of the given first argument
 * b0 in the residual class ring modulo <codewidth> (the second argument). One
 * requirement of this algorithm is, that template type <T> is large enough to
 * store <codewidth>+1 bits!
 */
template<typename T>
T
ext_euclidean (T b0, size_t codewidth) {
    T a0(1);
    a0 <<= codewidth;
    std::vector<T> a, b, q, r, s, t;
    a.push_back(a0), b.push_back(b0), s.push_back(T(0)), t.push_back(T(0));
    size_t i = 0;
    do {
        q.push_back(a[i] / b[i]);
        r.push_back(a[i] % b[i]);
        a.push_back(b[i]);
        b.push_back(r[i]);
        s.push_back(0);
        t.push_back(0);
    } while (b[++i] > 0);
    s[i] = 1;
    t[i] = 0;

    for (size_t j = i; j > 0; --j) {
        s[j - 1] = t[j];
        t[j - 1] = s[j] - q[j - 1] * t[j];
    }

    T result = ((b0 * t.front()) % a0);
    result += result < 0 ? a0 : 0;
    if (result == 1) {
        return t.front();
    } else {
        return 0;
    }
}

#endif /* EUCLIDEAN_HPP */
