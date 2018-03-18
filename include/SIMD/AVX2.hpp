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
 * File:   AVX2.hpp
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 23. Februar 2017, 22:43
 */

#pragma once

#ifdef __AVX2__

#define LIB_COLUMN_OPERATORS_SIMD_AVX2_HPP_

#include <cstdint>
#include <cstdlib>
#include <functional>

#include <Util/Intrinsics.hpp>
#include <Util/Functors.hpp>

#include <SIMD/SSE.hpp>
#include <SIMD/AVX2_base.tcc>
#include <SIMD/AVX2_int08.tcc>
#include <SIMD/AVX2_int16.tcc>
#include <SIMD/AVX2_int32.tcc>
#include <SIMD/AVX2_int64.tcc>

namespace coding_benchmark {
    namespace simd {

        template<typename T>
        struct mm<__m256i, T> :
                public avx2::mm256<T> {

            typedef avx2::mm256<T> BASE;

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

            static inline __m256i loadu(
                    __m256i * src) {
                return _mm256_lddqu_si256(src);
            }

            static inline void storeu(
                    __m256i * dst,
                    __m256i src) {
                _mm256_storeu_si256(dst, src);
            }
        };

        template<typename T>
        struct mm_op<__m256i, T, std::greater_equal> :
                public avx2::mm256op<T, std::greater_equal> {
            typedef avx2::mm256op<T, std::greater_equal> BASE;
            using BASE::mask_t;
            using BASE::cmp;
            using BASE::cmp_mask;
        };

        template<typename T>
        struct mm_op<__m256i, T, std::greater> :
                public avx2::mm256op<T, std::greater> {
            typedef avx2::mm256op<T, std::greater> BASE;
            using BASE::mask_t;
            using BASE::cmp;
            using BASE::cmp_mask;
        };

        template<typename T>
        struct mm_op<__m256i, T, std::less_equal> :
                public avx2::mm256op<T, std::less_equal> {
            typedef avx2::mm256op<T, std::less_equal> BASE;
            using BASE::mask_t;
            using BASE::cmp;
            using BASE::cmp_mask;
        };

        template<typename T>
        struct mm_op<__m256i, T, std::less> :
                public avx2::mm256op<T, std::less> {
            typedef avx2::mm256op<T, std::less> BASE;
            using BASE::mask_t;
            using BASE::cmp;
            using BASE::cmp_mask;
        };

        template<typename T>
        struct mm_op<__m256i, T, std::equal_to> :
                public avx2::mm256op<T, std::less> {
            typedef avx2::mm256op<T, std::less> BASE;
            using BASE::mask_t;
            using BASE::cmp;
            using BASE::cmp_mask;
        };

        template<typename T>
        struct mm_op<__m256i, T, std::not_equal_to> :
                public avx2::mm256op<T, std::less> {
            typedef avx2::mm256op<T, std::less> BASE;
            using BASE::mask_t;
            using BASE::cmp;
            using BASE::cmp_mask;
        };

        template<typename T>
        struct mm_op<__m256i, T, coding_benchmark::and_is> :
                public avx2::mm256op<T, coding_benchmark::and_is> {
            typedef avx2::mm256op<T, coding_benchmark::and_is> BASE;
            using BASE::mask_t;
            using BASE::cmp;
            using BASE::cmp_mask;
        };

        template<typename T>
        struct mm_op<__m256i, T, coding_benchmark::or_is> :
                public avx2::mm256op<T, coding_benchmark::or_is> {
            typedef avx2::mm256op<T, coding_benchmark::or_is> BASE;
            using BASE::mask_t;
            using BASE::cmp;
            using BASE::cmp_mask;
        };

        template<typename T>
        struct mm_op<__m256i, T, coding_benchmark::xor_is> :
                public avx2::mm256op<T, coding_benchmark::xor_is> {
            typedef avx2::mm256op<T, coding_benchmark::xor_is> BASE;
            using BASE::mask_t;
            using BASE::cmp;
            using BASE::cmp_mask;
        };

        template<typename T>
        struct mm_op<__m256i, T, coding_benchmark::is_not> :
                public avx2::mm256op<T, coding_benchmark::is_not> {
            typedef avx2::mm256op<T, coding_benchmark::is_not> BASE;
            using BASE::mask_t;
            using BASE::cmp;
            using BASE::cmp_mask;
        };

        template<typename T>
        struct mm_op<__m256i, T, coding_benchmark::add> :
                public avx2::mm256op<T, coding_benchmark::add> {
            typedef avx2::mm256op<T, coding_benchmark::add> BASE;
            using BASE::add;
            using BASE::compute;
        };

        template<typename T>
        struct mm_op<__m256i, T, coding_benchmark::sub> :
                public avx2::mm256op<T, coding_benchmark::sub> {
            typedef avx2::mm256op<T, coding_benchmark::sub> BASE;
            using BASE::sub;
            using BASE::compute;
        };

        template<typename T>
        struct mm_op<__m256i, T, coding_benchmark::mul> :
                public avx2::mm256op<T, coding_benchmark::mul> {
            typedef avx2::mm256op<T, coding_benchmark::mul> BASE;
            using BASE::mullo;
            using BASE::compute;
        };

        template<typename T>
        struct mm_op<__m256i, T, coding_benchmark::div> :
                public avx2::mm256op<T, coding_benchmark::div> {
            typedef avx2::mm256op<T, coding_benchmark::div> BASE;
            using BASE::div;
            using BASE::compute;
        };

    }
}

#endif
