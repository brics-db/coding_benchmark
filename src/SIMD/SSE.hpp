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

#define LIB_COLUMN_OPERATORS_SIMD_SSE_HPP_

#include <cstdint>
#include <cstdlib>
#include <functional>

#include <Util/Intrinsics.hpp>
#include <Util/Functors.hpp>

#include <SIMD/SIMD.hpp>
#include <SIMD/SSE_base.tcc>
#include <SIMD/SSE_uint08.tcc>
#include <SIMD/SSE_uint16.tcc>
#include <SIMD/SSE_uint32.tcc>
#include <SIMD/SSE_uint64.tcc>

namespace coding_benchmark {
    namespace simd {

        template<typename T>
        struct mm<__m128i, T> :
                public sse::mm128<T> {

            typedef sse::mm128<T> BASE;

            using BASE::mask_t;
            using BASE::popcnt_t;

            using BASE::set;
            using BASE::set1;
            using BASE::set_inc;
            using BASE::min;
            using BASE::max;
            using BASE::add;
            using BASE::mullo;
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

    }
}
