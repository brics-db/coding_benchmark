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
 * File:   AVX512.hpp
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 09. August 2017, 14:19
 */

#pragma once

#ifdef __AVX512F__

#define LIB_COLUMN_OPERATORS_SIMD_AVX512_HPP_

#include <cstdint>
#include <cstdlib>
#include <functional>

#include <Util/Intrinsics.hpp>
#include <Util/Functors.hpp>

#include <SIMD/SSE.hpp>
#include <SIMD/AVX2.hpp>
#include <SIMD/AVX512_base.tcc>
#include <SIMD/AVX512_int08.tcc>
#include <SIMD/AVX512_int16.tcc>
#include <SIMD/AVX512_int32.tcc>
#include <SIMD/AVX512_int64.tcc>

namespace coding_benchmark {
    namespace simd {

        template<typename T>
        struct mm<__m512i, T> :
                public avx512::mm512<T> {

            typedef avx512::mm512<T> BASE;

            using BASE::mask_t;
            using BASE::popcnt_t;

            using BASE::set;
            using BASE::set1;
            using BASE::set_inc;
            using BASE::min;
            using BASE::max;
            using BASE::sum;
            using BASE::pack_right;
            using BASE::pack_right2;
            using BASE::pack_right3;
            using BASE::popcount;
            using BASE::popcount2;
            using BASE::popcount3;

            static inline __m512i loadu(
                    __m512i * src) {
                return _mm512_loadu_si512(src);
            }

            static inline void storeu(
                    void * dst,
                    __m512i src) {
                _mm512_storeu_si512(dst, src);
            }
        };

        template<typename T>
        struct mm_op<__m512i, T, std::greater_equal> :
                public avx512::mm512op<T, std::greater_equal> {
            typedef avx512::mm512op<T, std::greater_equal> BASE;
            using BASE::mask_t;
            using BASE::cmp;
            using BASE::cmp_mask;
        };

        template<typename T>
        struct mm_op<__m512i, T, std::greater> :
                public avx512::mm512op<T, std::greater> {
            typedef avx512::mm512op<T, std::greater> BASE;
            using BASE::mask_t;
            using BASE::cmp;
            using BASE::cmp_mask;
        };

        template<typename T>
        struct mm_op<__m512i, T, std::less_equal> :
                public avx512::mm512op<T, std::less_equal> {
            typedef avx512::mm512op<T, std::less_equal> BASE;
            using BASE::mask_t;
            using BASE::cmp;
            using BASE::cmp_mask;
        };

        template<typename T>
        struct mm_op<__m512i, T, std::less> :
                public avx512::mm512op<T, std::less> {
            typedef avx512::mm512op<T, std::less> BASE;
            using BASE::mask_t;
            using BASE::cmp;
            using BASE::cmp_mask;
        };

        template<typename T>
        struct mm_op<__m512i, T, std::equal_to> :
                public avx512::mm512op<T, std::equal_to> {
            typedef avx512::mm512op<T, std::equal_to> BASE;
            using BASE::mask_t;
            using BASE::cmp;
            using BASE::cmp_mask;
        };

        template<typename T>
        struct mm_op<__m512i, T, std::not_equal_to> :
                public avx512::mm512op<T, std::not_equal_to> {
            typedef avx512::mm512op<T, std::not_equal_to> BASE;
            using BASE::mask_t;
            using BASE::cmp;
            using BASE::cmp_mask;
        };

        template<typename T>
        struct mm_op<__m512i, T, coding_benchmark::and_is> :
                public avx512::mm512op<T, coding_benchmark::and_is> {
            typedef avx512::mm512op<T, coding_benchmark::and_is> BASE;
            using BASE::mask_t;
            using BASE::cmp;
            using BASE::cmp_mask;
        };

        template<typename T>
        struct mm_op<__m512i, T, coding_benchmark::or_is> :
                public avx512::mm512op<T, coding_benchmark::or_is> {
            typedef avx512::mm512op<T, coding_benchmark::or_is> BASE;
            using BASE::mask_t;
            using BASE::cmp;
            using BASE::cmp_mask;
        };

        template<typename T>
        struct mm_op<__m512i, T, coding_benchmark::xor_is> :
                public avx512::mm512op<T, coding_benchmark::xor_is> {
            typedef avx512::mm512op<T, coding_benchmark::xor_is> BASE;
            using BASE::mask_t;
            using BASE::cmp;
            using BASE::cmp_mask;
        };

        template<typename T>
        struct mm_op<__m512i, T, coding_benchmark::is_not> :
                public avx512::mm512op<T, coding_benchmark::is_not> {
            typedef avx512::mm512op<T, coding_benchmark::is_not> BASE;
            using BASE::mask_t;
            using BASE::cmp;
            using BASE::cmp_mask;
        };

        template<typename T>
        struct mm_op<__m512i, T, coding_benchmark::add> :
                public avx512::mm512op<T, coding_benchmark::add> {
            typedef avx512::mm512op<T, coding_benchmark::add> BASE;
            using BASE::add;
            using BASE::compute;
        };

        template<typename T>
        struct mm_op<__m512i, T, coding_benchmark::sub> :
                public avx512::mm512op<T, coding_benchmark::sub> {
            typedef avx512::mm512op<T, coding_benchmark::sub> BASE;
            using BASE::sub;
            using BASE::compute;
        };

        template<typename T>
        struct mm_op<__m512i, T, coding_benchmark::mul> :
                public avx512::mm512op<T, coding_benchmark::mul> {
            typedef avx512::mm512op<T, coding_benchmark::mul> BASE;
            using BASE::mullo;
            using BASE::compute;
        };

        template<typename T>
        struct mm_op<__m512i, T, coding_benchmark::div> :
                public avx512::mm512op<T, coding_benchmark::div> {
            typedef avx512::mm512op<T, coding_benchmark::div> BASE;
            using BASE::div;
            using BASE::compute;
        };

    }
}

#endif
