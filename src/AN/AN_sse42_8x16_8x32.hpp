// Copyright (c) 2016 Till Kolditz, Stefan de Bruijn
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
 * File:   AN_sse42_8x16_8x32.hpp
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 13. Dezember 2016, 01:01
 */

#pragma once

#include <limits>
#include <type_traits>

#include "ANTest.hpp"

template<typename DATAIN, typename DATAOUT, size_t UNROLL>
struct AN_sse42_8x16_8x32 :
        public ANTest<DATAIN, DATAOUT, UNROLL>,
        public SSE42Test {

    AN_sse42_8x16_8x32(
            const char* const name,
            AlignedBlock & in,
            AlignedBlock & out,
            DATAOUT A,
            DATAOUT AInv)
            : ANTest<DATAIN, DATAOUT, UNROLL>(name, in, out, A, AInv) {
    }

    virtual ~AN_sse42_8x16_8x32() {
    }

    void RunEnc(
            const size_t numIterations) override {
        for (size_t iteration = 0; iteration < numIterations; ++iteration) {
            auto *mm_Data = this->in.template begin<__m128i >();
            auto * const mm_DataEnd = this->in.template end<__m128i >();
            auto *mm_Out = this->out.template begin<__m128i >();
            auto mm_A = _mm_set1_epi32(this->A);

            constexpr const bool isSigned = std::is_signed<DATAIN>::value;
            auto mmShuffleS = _mm_set_epi32(0xFFFFFFFF, 0xFFFFFFFF, 0x0F0E0D0C, 0x0B0A0908);
            auto mmShuffleU = _mm_set_epi32(0xFFFF0F0E, 0xFFFF0D0C, 0xFFFF0B0A, 0xFFFF0908);

            while (mm_Data <= (mm_DataEnd - UNROLL)) {
                // let the compiler unroll the loop
                for (size_t unroll = 0; unroll < UNROLL; ++unroll) {
                    auto mmIn = _mm_lddqu_si128(mm_Data++);
                    if (isSigned) {
                        _mm_storeu_si128(mm_Out++, _mm_mullo_epi32(_mm_cvtepi16_epi32(mmIn), mm_A));
                        _mm_storeu_si128(mm_Out++, _mm_mullo_epi32(_mm_cvtepi16_epi32(_mm_shuffle_epi8(mmIn, mmShuffleS)), mm_A));
                    } else {
                        _mm_storeu_si128(mm_Out++, _mm_mullo_epi32(_mm_cvtepu16_epi32(mmIn), mm_A));
                        _mm_storeu_si128(mm_Out++, _mm_mullo_epi32(_mm_shuffle_epi8(mmIn, mmShuffleU), mm_A));
                    }
                }
            }
            // remaining numbers
            while (mm_Data <= (mm_DataEnd - 1)) {
                auto mmIn = _mm_lddqu_si128(mm_Data++);
                if (isSigned) {
                    _mm_storeu_si128(mm_Out++, _mm_mullo_epi32(_mm_cvtepi16_epi32(mmIn), mm_A));
                    _mm_storeu_si128(mm_Out++, _mm_mullo_epi32(_mm_cvtepi16_epi32(_mm_shuffle_epi8(mmIn, mmShuffleS)), mm_A));
                } else {
                    _mm_storeu_si128(mm_Out++, _mm_mullo_epi32(_mm_cvtepu16_epi32(mmIn), mm_A));
                    _mm_storeu_si128(mm_Out++, _mm_mullo_epi32(_mm_shuffle_epi8(mmIn, mmShuffleU), mm_A));
                }
            }
            if (mm_Data < mm_DataEnd) {
                auto data16End = reinterpret_cast<DATAIN*>(mm_DataEnd);
                auto out32 = reinterpret_cast<DATAOUT*>(mm_Out);
                for (auto data16 = reinterpret_cast<DATAIN*>(mm_Data); data16 < data16End; ++data16, ++out32)
                    *out32 = *data16 * this->A;
            }
        }
    }
};
