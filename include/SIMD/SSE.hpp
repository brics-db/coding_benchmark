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
 * File:   SSE.hpp
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 23. Februar 2017, 22:43
 */

#pragma once

#ifdef __SSE4_2__

#define LIB_COLUMN_OPERATORS_SIMD_SSE_HPP_

#include <cstdint>
#include <cstdlib>
#include <functional>
#include <type_traits>

#include <Util/Intrinsics.hpp>
#include <Util/Functors.hpp>

#include <SIMD/SIMD.hpp>
#include <SIMD/SSE_base.tcc>
#include <SIMD/SSE_int08.tcc>
#include <SIMD/SSE_int16.tcc>
#include <SIMD/SSE_int32.tcc>
#include <SIMD/SSE_int64.tcc>

namespace coding_benchmark {
    namespace simd {

        template<typename T>
        struct mm<__m128i, T> :
                public sse::mm128<T> {

            typedef sse::mm128<T> BASE;

            using BASE::mask_t;
            using BASE::popcnt_t;
            using BASE::FULL_MASK;

            using BASE::set;
            using BASE::set1;
            using BASE::set_inc;
            using BASE::min;
            using BASE::max;
            using BASE::sum;
            using BASE::pack_right;
            using BASE::pack_right2;
            using BASE::popcount;
            using BASE::popcount2;
            using BASE::popcount3;
            using BASE::cvt_larger_lo;
            using BASE::cvt_larger_hi;

            static inline __m128i loadu(
                    __m128i * src) {
                return _mm_lddqu_si128(src);
            }

            static inline void storeu(
                    __m128i * dst,
                    __m128i src) {
                _mm_storeu_si128(dst, src);
            }
        };

        template<typename T>
        struct mm_op<__m128i, T, std::greater_equal> :
                public sse::mm128op<T, std::greater_equal> {
            typedef sse::mm128op<T, std::greater_equal> BASE;
            using BASE::mask_t;
            using BASE::cmp;
            using BASE::cmp_mask;
        };

        template<typename T>
        struct mm_op<__m128i, T, std::greater> :
                public sse::mm128op<T, std::greater> {
            typedef sse::mm128op<T, std::greater> BASE;
            using BASE::mask_t;
            using BASE::cmp;
            using BASE::cmp_mask;
        };

        template<typename T>
        struct mm_op<__m128i, T, std::less_equal> :
                public sse::mm128op<T, std::less_equal> {
            typedef sse::mm128op<T, std::less_equal> BASE;
            using BASE::mask_t;
            using BASE::cmp;
            using BASE::cmp_mask;
        };

        template<typename T>
        struct mm_op<__m128i, T, std::less> :
                public sse::mm128op<T, std::less> {
            typedef sse::mm128op<T, std::less> BASE;
            using BASE::mask_t;
            using BASE::cmp;
            using BASE::cmp_mask;
        };

        template<typename T>
        struct mm_op<__m128i, T, std::equal_to> :
                public sse::mm128op<T, std::equal_to> {
            typedef sse::mm128op<T, std::equal_to> BASE;
            using BASE::mask_t;
            using BASE::cmp;
            using BASE::cmp_mask;
        };

        template<typename T>
        struct mm_op<__m128i, T, std::not_equal_to> :
                public sse::mm128op<T, std::not_equal_to> {
            typedef sse::mm128op<T, std::not_equal_to> BASE;
            using BASE::mask_t;
            using BASE::cmp;
            using BASE::cmp_mask;
        };

        template<typename T>
        struct mm_op<__m128i, T, coding_benchmark::and_is> :
                public sse::mm128op<T, coding_benchmark::and_is> {
            typedef sse::mm128op<T, coding_benchmark::and_is> BASE;
            using BASE::mask_t;
            using BASE::cmp;
            using BASE::cmp_mask;
        };

        template<typename T>
        struct mm_op<__m128i, T, coding_benchmark::or_is> :
                public sse::mm128op<T, coding_benchmark::or_is> {
            typedef sse::mm128op<T, coding_benchmark::or_is> BASE;
            using BASE::mask_t;
            using BASE::cmp;
            using BASE::cmp_mask;
        };

        template<typename T>
        struct mm_op<__m128i, T, coding_benchmark::xor_is> :
                public sse::mm128op<T, coding_benchmark::xor_is> {
            typedef sse::mm128op<T, coding_benchmark::xor_is> BASE;
            using BASE::mask_t;
            using BASE::cmp;
            using BASE::cmp_mask;
        };

        template<typename T>
        struct mm_op<__m128i, T, coding_benchmark::is_not> :
                public sse::mm128op<T, coding_benchmark::is_not> {
            typedef sse::mm128op<T, coding_benchmark::is_not> BASE;
            using BASE::mask_t;
            using BASE::cmp;
            using BASE::cmp_mask;
        };

        template<typename T>
        struct mm_op<__m128i, T, coding_benchmark::add> :
                public sse::mm128op<T, coding_benchmark::add> {
            typedef sse::mm128op<T, coding_benchmark::add> BASE;
            using BASE::add;
            using BASE::compute;
        };

        template<typename T>
        struct mm_op<__m128i, T, coding_benchmark::sub> :
                public sse::mm128op<T, coding_benchmark::sub> {
            typedef sse::mm128op<T, coding_benchmark::sub> BASE;
            using BASE::sub;
            using BASE::compute;
        };

        template<typename T>
        struct mm_op<__m128i, T, coding_benchmark::mul> :
                public sse::mm128op<T, coding_benchmark::mul> {
            typedef sse::mm128op<T, coding_benchmark::mul> BASE;
            using BASE::mullo;
            using BASE::compute;
        };

        template<typename T>
        struct mm_op<__m128i, T, coding_benchmark::div> :
                public sse::mm128op<T, coding_benchmark::div> {
            typedef sse::mm128op<T, coding_benchmark::div> BASE;
            using BASE::div;
            using BASE::compute;
        };

    }
}

#endif
