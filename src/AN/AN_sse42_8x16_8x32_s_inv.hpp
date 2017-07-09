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
struct AN_sse42_8x16_8x32_s_inv :
        public AN_sse42_8x16_8x32<int16_t, int32_t, UNROLL> {

    using AN_sse42_8x16_8x32<int16_t, int32_t, UNROLL>::AN_sse42_8x16_8x32;

    virtual ~AN_sse42_8x16_8x32_s_inv() {
    }

    virtual bool DoCheck() override {
        return true;
    }

    virtual void RunCheck(
            const CheckConfiguration & config) override {
        for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
            auto data = this->out.template begin<__m128i>();
            auto dataEnd = this->out.template end<__m128i>();
            int32_t dMin = std::numeric_limits<int16_t>::min();
            int32_t dMax = std::numeric_limits<int16_t>::max();
            __m128i mm_dMin = _mm_set1_epi32(dMin); // we assume 16-bit input data
            __m128i mm_dMax = _mm_set1_epi32(dMax); // we assume 16-bit input data
            __m128i mm_ainv = _mm_set1_epi32(this->A_INV);
            while (data <= (dataEnd - UNROLL)) {
                // let the compiler unroll the loop
                for (size_t k = 0; k < UNROLL; ++k) {
                    auto mmIn = _mm_mullo_epi32(_mm_lddqu_si128(data), mm_ainv);
                    if (_mm_movemask_epi8(_mm_cmplt_epi32(mmIn, mm_dMin)) | _mm_movemask_epi8(_mm_cmpgt_epi32(mmIn, mm_dMax))) {
                        throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<int32_t*>(data) - this->out.template begin<int32_t>(), iteration);
                    }
                    ++data;
                }
            }
            // here follows the non-unrolled remainder
            while (data <= (dataEnd - 1)) {
                auto mmIn = _mm_mullo_epi32(_mm_lddqu_si128(data), mm_ainv);
                if (_mm_movemask_epi8(_mm_cmplt_epi32(mmIn, mm_dMin)) | _mm_movemask_epi8(_mm_cmpgt_epi32(mmIn, mm_dMax))) {
                    throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<int32_t*>(data) - this->out.template begin<int32_t>(), iteration);
                }
                ++data;
            }
            if (data < dataEnd) {
                auto dataEnd2 = reinterpret_cast<int32_t*>(dataEnd);
                for (auto data2 = reinterpret_cast<int32_t*>(data); data2 < dataEnd2; ++data2) {
                    auto data = *data2 * this->A_INV;
                    if (data < dMin || data > dMax) {
                        throw ErrorInfo(__FILE__, __LINE__, data2 - this->out.template begin<int32_t>(), iteration);
                    }
                }
            }
        }
    }

    bool DoDecode() override {
        return true;
    }

    void RunDecode(
            const DecodeConfiguration & config) override {
        for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
            const size_t VALUES_PER_SIMDREG = sizeof(__m128i) / sizeof (int32_t);
            const size_t VALUES_PER_UNROLL = UNROLL * VALUES_PER_SIMDREG;
            size_t numValues = this->in.template end<int16_t>() - this->in.template begin<int16_t>();
            size_t i = 0;
            auto dataIn = this->out.template begin<__m128i>();
            auto dataOut = this->in.template begin<int64_t>();
            auto mm_Ainv = _mm_set1_epi32(this->A_INV);
            auto mmShuffle = _mm_set_epi32(0xFFFFFFFF, 0xFFFFFFFF, 0x0D0C0908, 0x05040100);
            for (; i <= (numValues - VALUES_PER_UNROLL); i += VALUES_PER_UNROLL) {
                // let the compiler unroll the loop
                for (size_t unroll = 0; unroll < UNROLL; ++unroll) {
                    auto tmp = _mm_lddqu_si128(dataIn++);
                    tmp = _mm_mullo_epi32(tmp, mm_Ainv);
                    tmp = _mm_shuffle_epi8(tmp, mmShuffle);
                    *dataOut++ = _mm_extract_epi64(tmp, 0);
                }
            }
            // remaining numbers
            for (; i <= (numValues - VALUES_PER_SIMDREG); i += VALUES_PER_SIMDREG) {
                auto tmp = _mm_lddqu_si128(dataIn++);
                tmp = _mm_mullo_epi32(tmp, mm_Ainv);
                tmp = _mm_shuffle_epi8(tmp, mmShuffle);
                *dataOut++ = _mm_extract_epi64(tmp, 0);
            }
            if (i < numValues) {
                auto out16 = reinterpret_cast<int16_t*>(dataOut);
                auto in32 = reinterpret_cast<int32_t*>(dataIn);
                for (; i < numValues; ++i, ++in32, ++out16) {
                    *out16 = *in32 * this->A_INV;
                }
            }
        }
    }
};
