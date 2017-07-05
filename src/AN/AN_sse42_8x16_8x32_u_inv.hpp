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

#pragma once

#include "AN_sse42_8x16_8x32.hpp"

template<size_t UNROLL>
struct AN_sse42_8x16_8x32_u_inv :
        public AN_sse42_8x16_8x32<uint16_t, uint32_t, UNROLL> {

    AN_sse42_8x16_8x32_u_inv(
            const char* const name,
            AlignedBlock & in,
            AlignedBlock & out,
            uint32_t A,
            uint32_t Ainv)
            : AN_sse42_8x16_8x32<uint16_t, uint32_t, UNROLL>(name, in, out, A, Ainv) {
    }

    virtual ~AN_sse42_8x16_8x32_u_inv() {
    }

    virtual bool DoCheck() override {
        return true;
    }

    virtual void RunCheck(
            const size_t numIterations) override {
        for (size_t iteration = 0; iteration < numIterations; ++iteration) {
            auto mm_Data = this->out.template begin<__m128i >();
            auto mm_DataEnd = this->out.template end<__m128i >();
            uint32_t dMax = std::numeric_limits<uint16_t>::max();
            __m128i mm_dMax = _mm_set1_epi32(dMax); // we assume 16-bit input data
            __m128i mm_ainv = _mm_set1_epi32(this->A_INV);
            while (mm_Data <= (mm_DataEnd - UNROLL)) {
                // let the compiler unroll the loop
                for (size_t k = 0; k < UNROLL; ++k) {
                    auto mmIn = _mm_mullo_epi32(_mm_lddqu_si128(mm_Data), mm_ainv);
                    if (0xFFFF != _mm_movemask_epi8(_mm_cmpeq_epi32(_mm_min_epu32(mmIn, mm_dMax), mmIn))) { // we need to do this "hack" because comparison is only on signed integers!
                        throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<uint32_t*>(mm_Data) - this->out.template begin<uint32_t>(), iteration);
                    }
                    ++mm_Data;
                }
            }
            // here follows the non-unrolled remainder
            while (mm_Data <= (mm_DataEnd - 1)) {
                auto mmIn = _mm_mullo_epi32(_mm_lddqu_si128(mm_Data), mm_ainv);
                if (0xFFFF != _mm_movemask_epi8(_mm_cmpeq_epi32(_mm_min_epu32(mmIn, mm_dMax), mmIn))) { // we need to do this "hack" because comparison is only on signed integers!
                    throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<uint32_t*>(mm_Data) - this->out.template begin<uint32_t>(), iteration);
                }
                ++mm_Data;
            }
            if (mm_Data < mm_DataEnd) {
                auto dataEnd2 = reinterpret_cast<uint32_t*>(mm_DataEnd);
                for (auto data2 = reinterpret_cast<uint32_t*>(mm_Data); data2 < dataEnd2; ++data2) {
                    if ((*data2 * this->A_INV) > dMax) {
                        throw ErrorInfo(__FILE__, __LINE__, data2 - this->out.template begin<uint32_t>(), iteration);
                    }
                }
            }
        }
    }

    bool DoDec() override {
        return true;
    }

    void RunDec(
            const size_t numIterations) override {
        for (size_t iteration = 0; iteration < numIterations; ++iteration) {
            const size_t VALUES_PER_SIMDREG = sizeof(__m128i) / sizeof (uint32_t);
            const size_t VALUES_PER_UNROLL = UNROLL * VALUES_PER_SIMDREG;
            size_t numValues = this->in.template end<uint16_t>() - this->in.template begin<uint16_t>();
            size_t i = 0;
            auto mm_Data = this->out.template begin<__m128i>();
            auto mm_Out = this->in.template begin<uint64_t>();
            auto mm_Ainv = _mm_set1_epi32(this->A_INV);
            auto mmShuffle = _mm_set_epi32(0xFFFFFFFF, 0xFFFFFFFF, 0x0D0C0908, 0x05040100);
            for (; i <= (numValues - VALUES_PER_UNROLL); i += VALUES_PER_UNROLL) {
                // let the compiler unroll the loop
                for (size_t unroll = 0; unroll < UNROLL; ++unroll) {
                    auto tmp = _mm_lddqu_si128(mm_Data++);
                    tmp = _mm_mullo_epi32(tmp, mm_Ainv);
                    tmp = _mm_shuffle_epi8(tmp, mmShuffle);
                    *mm_Out++ = _mm_extract_epi64(tmp, 0);
                }
            }
            // remaining numbers
            for (; i <= (numValues - VALUES_PER_SIMDREG); i += VALUES_PER_SIMDREG) {
                auto tmp = _mm_lddqu_si128(mm_Data++);
                tmp = _mm_mullo_epi32(tmp, mm_Ainv);
                tmp = _mm_shuffle_epi8(tmp, mmShuffle);
                *mm_Out++ = _mm_extract_epi64(tmp, 0);
            }
            if (i < numValues) {
                auto out16 = reinterpret_cast<uint16_t*>(mm_Out);
                auto in32 = reinterpret_cast<uint32_t*>(mm_Data);
                for (; i < numValues; ++i, ++in32, ++out16) {
                    *out16 = *in32 * this->A_INV;
                }
            }
        }
    }
};
