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
 * AN_avx2_16x16_16x32_inv.tcc
 *
 *  Created on: 09.01.2018
 *      Author: Till Kolditz - Till.Kolditz@gmail.com
 */

#pragma once

#ifndef AN_AVX2
#error "Clients must not include this file directly, but file <AN/AN_avx2.hpp>!"
#endif

#include <AN/AN_avx2_16x16_16x32.hpp>

namespace coding_benchmark {

    template<typename DATARAW, typename DATAENC, size_t UNROLL>
    struct AN_avx2_16x16_16x32_inv :
            public AN_avx2_16x16_16x32<DATARAW, DATAENC, UNROLL> {

        typedef AN_avx2_16x16_16x32<DATARAW, DATAENC, UNROLL> BASE;

        using BASE::NUM_VALUES_PER_SIMDREG;
        using BASE::NUM_VALUES_PER_UNROLL;

        using BASE::AN_avx2_16x16_16x32;

        virtual ~AN_avx2_16x16_16x32_inv() {
        }

        bool DoDecode() override {
            return true;
        }

        void RunDecode(
                const DecodeConfiguration & config) override {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                _ReadWriteBarrier();
                ssize_t numValues = this->bufRaw.template end<DATARAW>() - this->bufRaw.template begin<DATARAW>();
                ssize_t i = 0;
                auto mm_In = this->bufEncoded.template begin<__m128i >();
                auto mm_Out = this->bufResult.template begin<int64_t>();
                auto mmAinv = _mm256_set1_pd(static_cast<double>(this->A_INV));
                auto mmShuffle = _mm_set_epi32(static_cast<DATAENC>(0xFFFFFFFF), static_cast<DATAENC>(0xFFFFFFFF), static_cast<DATAENC>(0x0D0C0908), static_cast<DATAENC>(0x05040100));
                for (; i <= (numValues - NUM_VALUES_PER_UNROLL); i += NUM_VALUES_PER_UNROLL) {
                    // let the compiler unroll the loop
                    for (size_t unroll = 0; unroll < UNROLL; ++unroll) {
                        auto tmp1 = _mm256_cvtepi32_pd(_mm_lddqu_si128(mm_In++));
                        auto tmp2 = _mm256_cvtepi32_pd(_mm_lddqu_si128(mm_In++));
                        tmp1 = _mm256_div_pd(tmp1, mmAinv);
                        tmp2 = _mm256_div_pd(tmp2, mmAinv);
                        auto tmp3 = _mm_shuffle_epi8(_mm256_cvtpd_epi32(tmp1), mmShuffle);
                        auto tmp4 = _mm_shuffle_epi8(_mm256_cvtpd_epi32(tmp2), mmShuffle);
                        *mm_Out++ = _mm_extract_epi64(tmp3, 0);
                        *mm_Out++ = _mm_extract_epi64(tmp4, 0);
                    }
                }
                // remaining numbers
                for (; i <= (numValues - NUM_VALUES_PER_SIMDREG); i += NUM_VALUES_PER_SIMDREG) {
                    auto tmp1 = _mm256_cvtepi32_pd(_mm_lddqu_si128(mm_In++));
                    auto tmp2 = _mm256_cvtepi32_pd(_mm_lddqu_si128(mm_In++));
                    tmp1 = _mm256_div_pd(tmp1, mmAinv);
                    tmp2 = _mm256_div_pd(tmp2, mmAinv);
                    auto tmp3 = _mm_shuffle_epi8(_mm256_cvtpd_epi32(tmp1), mmShuffle);
                    auto tmp4 = _mm_shuffle_epi8(_mm256_cvtpd_epi32(tmp2), mmShuffle);
                    *mm_Out++ = _mm_extract_epi64(tmp3, 0);
                    *mm_Out++ = _mm_extract_epi64(tmp4, 0);
                }
                if (i < numValues) {
                    auto out16 = reinterpret_cast<DATARAW*>(mm_Out);
                    auto in32 = reinterpret_cast<DATAENC*>(mm_In);
                    for (; i < numValues; ++i, ++in32, ++out16) {
                        *out16 = *in32 * this->A_INV;
                    }
                }
            }
        }
    };

}
