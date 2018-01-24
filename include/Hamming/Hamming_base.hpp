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

#if not defined(HAMMING_SCALAR) and not defined(HAMMING_SIMD)
#error "Clients must not include this file directly, but e.g. file <Hamming/Hamming_scalar.hpp> or <Hamming/Hamming_simd.hpp> or <Hamming/Hamming_compute.hpp>!"
#endif

#include <cstdint>
#include <Util/Intrinsics.hpp>

namespace coding_benchmark {

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
    struct hamming_typehelper_t<uint64_t, uint64_t> {
        typedef uint8_t code_t;
    };

#ifdef __SSE4_2__
    template<>
    struct hamming_typehelper_t<uint16_t, __m128i> {
    typedef uint64_t code_t;
};

    template<>
    struct hamming_typehelper_t<uint32_t, __m128i> {
    typedef uint32_t code_t;
};

    template<>
    struct hamming_typehelper_t<uint64_t, __m128i> {
    typedef uint16_t code_t;
};
#endif

#ifdef __AVX2__
    template<>
    struct hamming_typehelper_t<uint16_t, __m256i> {
        typedef __m128i code_t;
    };

    template<>
    struct hamming_typehelper_t<uint32_t, __m256i> {
        typedef uint64_t code_t;
    };

    template<>
    struct hamming_typehelper_t<uint64_t, __m256i> {
        typedef uint32_t code_t;
    };
#endif

#ifdef __AVX512F__
    template<>
    struct hamming_typehelper_t<uint16_t, __m512i> {
        typedef __m256i code_t;
    };

    template<>
    struct hamming_typehelper_t<uint32_t, __m512i> {
        typedef __m128i code_t;
    };

    template<>
    struct hamming_typehelper_t<uint64_t, __m512i> {
        typedef uint64_t code_t;
    };
#endif

    template<typename data_t, typename granularity_t>
    struct hamming_t {
        typedef typename hamming_typehelper_t<data_t, granularity_t>::code_t code_t;
        granularity_t data;
        code_t code;

        void store(
                granularity_t data);

        void store2(
                granularity_t data);

        void store3(
                granularity_t data);

        bool isValid();

        static code_t computeHamming(
                granularity_t data);

        static code_t computeHamming2(
                granularity_t data);

        static code_t computeHamming3(
                granularity_t data);

        static bool code_cmp_eq(
                code_t c1,
                code_t c2);
    };

}
