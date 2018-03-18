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
 * AN_sse42_8x16_8x32_inv.tcc
 *
 *  Created on: 08.01.2018
 *      Author: Till Kolditz - Till.Kolditz@gmail.com
 */

#pragma once

#ifndef AN_SSE42
#error "Clients must not include this file directly, but file <AN/AN_sse42.hpp>!"
#endif

#include <AN/AN_sse42_8x16_8x32.tcc>

namespace coding_benchmark {

    template<typename DATARAW, typename DATAENC, size_t UNROLL>
    struct AN_sse42_8x16_8x32_inv :
            public AN_sse42_8x16_8x32<DATARAW, DATAENC, UNROLL> {

        typedef AN_sse42_8x16_8x32<DATARAW, DATAENC, UNROLL> BASE;

        using BASE::AN_sse42_8x16_8x32;

        virtual ~AN_sse42_8x16_8x32_inv() {
        }

        bool DoDecode() override {
            return true;
        }

        void RunDecode(
                const DecodeConfiguration & config) override {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                auto in128 = config.source.template begin<__m128i >();
                const auto in128end = this->template ComputeEnd<DATAENC>(in128, config);
                auto dataOut = config.target.template begin<int64_t>();
                auto mm_Ainv = _mm_set1_epi32(this->A_INV);
                auto mmShuffle = _mm_set_epi64x(0xFFFFFFFFFFFFFFFF, 0x0D0C090805040100);
                while (in128 <= (in128end - UNROLL)) {
                    // let the compiler unroll the loop
                    for (size_t unroll = 0; unroll < UNROLL; ++unroll) {
                        auto tmp = _mm_lddqu_si128(in128++);
                        tmp = _mm_mullo_epi32(tmp, mm_Ainv);
                        tmp = _mm_shuffle_epi8(tmp, mmShuffle);
                        *dataOut++ = _mm_extract_epi64(tmp, 0);
                    }
                }
                // remaining numbers
                while (in128 <= (in128end - 1)) {
                    auto tmp = _mm_lddqu_si128(in128++);
                    tmp = _mm_mullo_epi32(tmp, mm_Ainv);
                    tmp = _mm_shuffle_epi8(tmp, mmShuffle);
                    *dataOut++ = _mm_extract_epi64(tmp, 0);
                }
                if (in128 < in128end) {
                    auto in32 = reinterpret_cast<DATAENC*>(in128);
                    const auto in32end = reinterpret_cast<DATAENC * const >(in128end);
                    auto out16 = reinterpret_cast<DATARAW*>(dataOut);
                    while (in32 < in32end) {
                        *out16++ = *in32++ * this->A_INV;
                    }
                }
            }
        }

    };

}
