// Copyright (c) 2017 Till Kolditz
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
 * File:   SIMD.hpp
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 21-08-2017 12:22
 */

#pragma once

#include <cstdint>

#include <Util/Intrinsics.hpp>

template<typename vector_t, typename type_t>
struct SIMD;

template<>
struct SIMD<__m128i, uint8_t> {
    static __m128i set1(
            uint8_t a);
    static __m128i add(
            __m128i a,
            __m128i b);
    static __m128i popcount(
            __m128i a);
    static __m128i popcount2(
            __m128i a);
};

template<>
struct SIMD<__m128i, uint16_t> {
    static __m128i set1(
            uint16_t a);
    static __m128i add(
            __m128i a,
            __m128i b);
    static uint64_t popcount(
            __m128i a);
    static uint64_t popcount2(
            __m128i a);
};

template<>
struct SIMD<__m128i, uint32_t> {
    static __m128i set1(
            uint32_t a);
    static __m128i add(
            __m128i a,
            __m128i b);
    static uint32_t popcount(
            __m128i a);
    static uint32_t popcount2(
            __m128i a);
};

#ifdef __AVX2__
template<>
struct SIMD<__m256i, uint8_t> {
    static __m256i set1(
            uint8_t a);
    static __m256i add(
            __m256i a,
            __m256i b);
    static __m256i popcount(
            __m256i a);
    static __m256i popcount2(
            __m256i a);
};

template<>
struct SIMD<__m256i, uint16_t> {
    static __m256i set1(
            uint16_t a);
    static __m256i add(
            __m256i a,
            __m256i b);
    static __m128i popcount(
            __m256i a);
    static __m128i popcount2(
            __m256i a);
};

template<>
struct SIMD<__m256i, uint32_t> {
    static __m256i set1(
            uint32_t a);
    static __m256i add(
            __m256i a,
            __m256i b);
    static uint64_t popcount(
            __m256i a);
    static uint64_t popcount2(
            __m256i a);
};
#endif /* __AVX2__ */
