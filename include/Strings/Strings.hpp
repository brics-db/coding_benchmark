// Copyright 2017 Till Kolditz
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
 * Strings.hpp
 *
 *  Created on: 25.03.2018
 *      Author: Till Kolditz - Till.Kolditz@gmail.com
 */

#pragma once

#include <Strings/StringBase.hpp>

/* https://www.strchr.com/strcmp_and_strlen_using_sse_4.2 */
int _mm_strcmp(
        const char* cs,
        const char* ct,
        const size_t __attribute__((unused)) NUM);

int _mm_strcmp(
        const unsigned char* cs,
        const unsigned char* ct,
        const size_t __attribute__((unused)) NUM);

int _mm_strcmp(
        const short* cs,
        const short* ct,
        const size_t __attribute__((unused)) NUM);

int _mm_strcmp(
        const unsigned short* cs,
        const unsigned short* ct,
        const size_t __attribute__((unused)) NUM);

template<typename T>
int strcmp2(
        const T *s1,
        const T *s2,
        const size_t __attribute__((unused)) NUM) {
    const T * ps1 = s1;
    const T * ps2 = s2;
    while (1) {
        int res = ((*ps1 == 0) || (*ps1 != *ps2));
        if (__builtin_expect((res), 0)) {
            break;
        }
        ++ps1;
        ++ps2;
    }
#ifndef NDEBUG
    if (static_cast<size_t>(ps1 - s1) != (NUM - 1)) { // the loop does LATE increment
        std::cerr << "\tpos1=" << (ps1 - s1) << " != " << NUM << std::endl;
    }
    if (static_cast<size_t>(ps2 - s2) != (NUM - 1)) { // the loop does LATE increment
        std::cerr << "\tpos2=" << (ps2 - s2) << " != " << NUM << std::endl;
    }
#endif
    return (*s1 - *s2);
}

extern template int strcmp2(
        const unsigned char * s1,
        const unsigned char * s2,
        const size_t NUM);

extern template int strcmp2(
        const signed char * s1,
        const signed char * s2,
        const size_t NUM);

extern template int strcmp2(
        const unsigned short * s1,
        const unsigned short * s2,
        const size_t NUM);

extern template int strcmp2(
        const signed short * s1,
        const signed short * s2,
        const size_t NUM);
