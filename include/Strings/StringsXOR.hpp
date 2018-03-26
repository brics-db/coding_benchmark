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
 * StringsXOR.hpp
 *
 *  Created on: 25.03.2018
 *      Author: Till Kolditz - Till.Kolditz@gmail.com
 */

#pragma once

#include <stdexcept>

#include <Strings/StringBase.hpp>

template<typename T>
T strxor(
        const T* t) {
    T res = 0;
    while (*t != 0) {
        res ^= *t++;
    }
    return res;
}

extern template
uint8_t strxor(
        const uint8_t* t);

extern template
int8_t strxor(
        const int8_t* t);

extern template
uint16_t strxor(
        const uint16_t* t);

extern template
int16_t strxor(
        const int16_t* t);

extern template
uint32_t strxor(
        const uint32_t* t);

extern template
int32_t strxor(
        const int32_t* t);

template<typename T>
__m128i _mm_strxor(
        const __m128i * mm,
        const size_t NUM) {
    __m128i mmXOR = _mm_setzero_si128();
    const __m128i * const mmEnd = mm + (NUM / (sizeof(__m128i) / sizeof(T)));
    while (mm <= (mmEnd - 1)) {
        mmXOR = _mm_xor_si128(mmXOR, _mm_lddqu_si128(mm++));
    }
    return mmXOR;
}

extern template __m128i _mm_strxor<uint8_t>(
        const __m128i * mm,
        const size_t NUM);

extern template __m128i _mm_strxor<int8_t>(
        const __m128i * mm,
        const size_t NUM);

extern template __m128i _mm_strxor<uint16_t>(
        const __m128i * mm,
        const size_t NUM);

extern template __m128i _mm_strxor<int16_t>(
        const __m128i * mm,
        const size_t NUM);

extern template __m128i _mm_strxor<uint32_t>(
        const __m128i * mm,
        const size_t NUM);

extern template __m128i _mm_strxor<int32_t>(
        const __m128i * mm,
        const size_t NUM);

template<typename T>
int strcmp2_xor(
        const T *s1,
        const T *s2,
        const T old_xor1,
        const T old_xor2,
        const size_t NUM) {
    T new_xor1 = T(0);
    T new_xor2 = T(0);
    const T * ps1 = s1;
    const T * ps2 = s2;
    while (1) {
        new_xor1 ^= *ps1;
        new_xor2 ^= *ps2;
        int res = ((*ps1 == 0) || (*ps1 != *ps2));
        if (__builtin_expect((res), 0)) {
            break;
        }
        ++ps1;
        ++ps2;
    }
    const T * const s1end = s1 + NUM;
    while (ps1 < s1end) {
        new_xor1 ^= *ps1++;
    }
    const T * const s2end = s2 + NUM;
    while (ps2 < s2end) {
        new_xor2 ^= *ps2++;
    }
    if (new_xor1 != old_xor1) {
        throw std::runtime_error("old_xor1 != new_xor1");
    }
    if (new_xor2 != old_xor2) {
        throw std::runtime_error("old_xor2 != new_xor2");
    }
#ifndef NDEBUG
    if (static_cast<size_t>(ps1 - s1) != NUM) {
        std::cerr << "\tpos1=" << (ps1 - s1) << " != " << NUM << std::endl;
    }
    if (static_cast<size_t>(ps2 - s2) != NUM) {
        std::cerr << "\tpos2=" << (ps2 - s2) << " != " << NUM << std::endl;
    }
#endif
    return (*ps1 - *ps2);
}

extern template
int strcmp2_xor(
        const uint8_t *s1,
        const uint8_t *s2,
        const uint8_t old_xor1,
        const uint8_t old_xor2,
        const size_t NUM);

extern template
int strcmp2_xor(
        const int8_t *s1,
        const int8_t *s2,
        const int8_t old_xor1,
        const int8_t old_xor2,
        const size_t NUM);

extern template
int strcmp2_xor(
        const uint16_t *s1,
        const uint16_t *s2,
        const uint16_t old_xor1,
        const uint16_t old_xor2,
        const size_t NUM);

extern template
int strcmp2_xor(
        const int16_t *s1,
        const int16_t *s2,
        const int16_t old_xor1,
        const int16_t old_xor2,
        const size_t NUM);

extern template
int strcmp2_xor(
        const uint32_t *s1,
        const uint32_t *s2,
        const uint32_t old_xor1,
        const uint32_t old_xor2,
        const size_t NUM);

extern template
int strcmp2_xor(
        const int32_t *s1,
        const int32_t *s2,
        const int32_t old_xor1,
        const int32_t old_xor2,
        const size_t NUM);

int _mm_strcmp_xor(
        const char* cs,
        const char* ct,
        __m128i old_xor1,
        __m128i old_xor2,
        const size_t NUM);

int _mm_strcmp_xor(
        const unsigned short* cs,
        const unsigned short* ct,
        __m128i old_xor1,
        __m128i old_xor2,
        const size_t NUM);
