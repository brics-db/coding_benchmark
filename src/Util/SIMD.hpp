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

template<typename T>
struct Larger;

template<>
struct Larger<uint8_t> {
    typedef uint16_t larger_t;
};

template<>
struct Larger<int8_t> {
    typedef int16_t larger_t;
};

template<>
struct Larger<uint16_t> {
    typedef uint32_t larger_t;
};

template<>
struct Larger<int16_t> {
    typedef int32_t larger_t;
};

template<>
struct Larger<uint32_t> {
    typedef uint64_t larger_t;
};

template<>
struct Larger<int32_t> {
    typedef int64_t larger_t;
};

template<typename T>
struct Smaller;

template<>
struct Smaller<uint8_t> {
    typedef uint8_t smaller_t;
};

template<>
struct Smaller<int8_t> {
    typedef int16_t smaller_t;
};

template<>
struct Smaller<uint16_t> {
    typedef uint8_t smaller_t;
};

template<>
struct Smaller<int16_t> {
    typedef int8_t smaller_t;
};

template<>
struct Smaller<uint32_t> {
    typedef uint16_t smaller_t;
};

template<>
struct Smaller<int32_t> {
    typedef int16_t smaller_t;
};

template<typename V, typename T>
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
    static __m128i popcount3(
            __m128i a);
    static __m128i cvt_larger_lo(
            __m128i a);
    static __m128i cvt_larger_hi(
            __m128i a);
    static uint8_t sum(
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
    static uint64_t popcount3(
            __m128i a);
    static __m128i cvt_larger_lo(
            __m128i a);
    static __m128i cvt_larger_hi(
            __m128i a);
    static uint16_t sum(
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
    static uint32_t popcount3(
            __m128i a);
    static __m128i cvt_larger_lo(
            __m128i a);
    static __m128i cvt_larger_hi(
            __m128i a);
    static uint32_t sum(
            __m128i a);
};

template<>
struct SIMD<__m128i, uint64_t> {
    static __m128i set1(
            uint32_t a);
    static __m128i add(
            __m128i a,
            __m128i b);
    static uint16_t popcount(
            __m128i a);
    static uint16_t popcount2(
            __m128i a);
    static uint16_t popcount3(
            __m128i a);
    static uint64_t sum(
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
    static __m256i cvt_larger_lo(
            __m256i a);
    static __m256i cvt_larger_hi(
            __m256i a);
    static uint8_t sum(
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
    static __m256i cvt_larger_lo(
            __m256i a);
    static __m256i cvt_larger_hi(
            __m256i a);
    static uint16_t sum(
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
    static __m256i cvt_larger_lo(
            __m256i a);
    static __m256i cvt_larger_hi(
            __m256i a);
    static uint32_t sum(
            __m256i a);
};

template<>
struct SIMD<__m256i, uint64_t> {
    static __m256i set1(
            uint32_t a);
    static __m256i add(
            __m256i a,
            __m256i b);
    static uint32_t popcount(
            __m256i a);
    static uint32_t popcount2(
            __m256i a);
    static uint64_t sum(
            __m256i a);
};
#endif /* __AVX2__ */
