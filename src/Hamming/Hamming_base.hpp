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
 * File:   Hamming_base.hpp
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 21-08-2017 12:36
 */

#pragma once

#include <cstdint>
#include <Util/Intrinsics.hpp>

template<typename data_t, typename granularity_t>
struct hamming_typehelper_t;

template<>
struct hamming_typehelper_t<uint16_t, uint16_t> {
    typedef uint8_t code_t;
};

template<>
struct hamming_typehelper_t<uint32_t, uint32_t> {
    typedef uint8_t code_t;
};

template<>
struct hamming_typehelper_t<uint8_t, __m128i> {
    typedef __m128i code_t;
};

template<>
struct hamming_typehelper_t<uint16_t, __m128i> {
    typedef uint64_t code_t;
};

template<>
struct hamming_typehelper_t<uint32_t, __m128i> {
    typedef uint32_t code_t;
};

template<typename data_t, typename granularity_t>
struct hamming_t {
    typedef typename hamming_typehelper_t<data_t, granularity_t>::code_t code_t;
    data_t data;
    code_t code;

    static code_t computeHamming(
            granularity_t data);

    static code_t computeHamming2(
            granularity_t data);

    static bool code_cmp_eq(
            code_t c1,
            code_t c2);
};

// Scalar
extern template uint8_t hamming_t<uint16_t, uint16_t>::computeHamming(
        uint16_t);
extern template uint8_t hamming_t<uint32_t, uint32_t>::computeHamming(
        uint32_t);
// SSE4.2
extern template uint64_t hamming_t<uint16_t, __m128i>::computeHamming(
        __m128i);
extern template uint64_t hamming_t<uint16_t, __m128i>::computeHamming2(
        __m128i);
extern template uint32_t hamming_t<uint32_t, __m128i>::computeHamming(
        __m128i);
extern template uint32_t hamming_t<uint32_t, __m128i>::computeHamming2(
        __m128i);
// AVX2
#ifdef __AVX2__
extern template __m128i hamming_t<uint16_t, __m256i>::computeHamming(
        __m128i);
extern template __m128i hamming_t<uint16_t, __m256i>::computeHamming2(
        __m128i);
extern template uint64_t hamming_t<uint32_t, __m256i>::computeHamming(
        __m128i);
extern template uint64_t hamming_t<uint32_t, __m256i>::computeHamming2(
        __m128i);
#endif
