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

#include <cstdint>
#include <limits>

#include "ANTest.hpp"

template<size_t UNROLL>
struct AN_sse42_8x16_8x32 : public ANTest<uint16_t, uint32_t, UNROLL>, public SSE42Test {

    AN_sse42_8x16_8x32 (const char* const name, AlignedBlock & in, AlignedBlock & out, uint32_t A, uint32_t Ainv) :
            ANTest<uint16_t, uint32_t, UNROLL>(name, in, out, A, Ainv) {
    }

    virtual
    ~AN_sse42_8x16_8x32 () {
    }

    void
    RunEnc (const size_t numIterations) override {
        for (size_t iteration = 0; iteration < numIterations; ++iteration) {
            __m128i *dataIn = this->in.template begin<__m128i>();
            __m128i * const dataInEnd = this->in.template end<__m128i>();
            __m128i *dataOut = this->out.template begin<__m128i>();
            __m128i mm_A = _mm_set1_epi32(this->A);
            __m128i mmShuffle1 = _mm_set_epi32(0xFFFF0706, 0xFFFF0504, 0xFFFF0302, 0xFFFF0100);
            __m128i mmShuffle2 = _mm_set_epi32(0xFFFF0F0E, 0xFFFF0D0C, 0xFFFF0B0A, 0xFFFF0908);
            while (dataIn <= (dataInEnd - UNROLL)) {
                // let the compiler unroll the loop
                for (size_t unroll = 0; unroll < UNROLL; ++unroll) {
                    auto mmIn = _mm_lddqu_si128(dataIn++);
                    _mm_storeu_si128(dataOut++, _mm_mullo_epi32(_mm_shuffle_epi8(mmIn, mmShuffle1), mm_A));
                    _mm_storeu_si128(dataOut++, _mm_mullo_epi32(_mm_shuffle_epi8(mmIn, mmShuffle2), mm_A));
                }
            }
            // remaining numbers
            while (dataIn <= (dataInEnd - 1)) {
                auto mmIn = _mm_lddqu_si128(dataIn++);
                _mm_storeu_si128(dataOut++, _mm_mullo_epi32(_mm_shuffle_epi8(mmIn, mmShuffle1), mm_A));
                _mm_storeu_si128(dataOut++, _mm_mullo_epi32(_mm_shuffle_epi8(mmIn, mmShuffle2), mm_A));
            }
            if (dataIn < dataInEnd) {
                auto data16End = reinterpret_cast<uint16_t*>(dataInEnd);
                auto out32 = reinterpret_cast<uint32_t*>(dataOut);
                for (auto data16 = reinterpret_cast<uint16_t*>(dataIn); data16 < data16End; ++data16, ++out32)
                    *out32 = *data16 * this->A;
            }
        }
    }
};
