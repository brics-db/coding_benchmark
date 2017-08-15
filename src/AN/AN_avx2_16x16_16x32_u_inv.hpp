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

#include <AN/AN_avx2_16x16_16x32.hpp>

template<size_t UNROLL>
struct AN_avx2_16x16_16x32_u_inv :
        public AN_avx2_16x16_16x32<uint16_t, uint32_t, UNROLL> {

    using AN_avx2_16x16_16x32<uint16_t, uint32_t, UNROLL>::AN_avx2_16x16_16x32;

    virtual ~AN_avx2_16x16_16x32_u_inv() {
    }

    virtual bool DoCheck() override {
        return true;
    }

    virtual void RunCheck(
            const CheckConfiguration & config) override {
        for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
            _ReadWriteBarrier();
            auto mm_Data = this->bufEncoded.template begin<__m256i >();
            auto mm_DataEnd = this->bufEncoded.template end<__m256i >();
            uint32_t dMax = std::numeric_limits<uint16_t>::max();
            __m256i mm_dMax = _mm256_set1_epi32(dMax); // we assume 16-bit input data
            __m256i mm_AInv = _mm256_set1_epi32(this->A_INV);
            while (mm_Data <= (mm_DataEnd - UNROLL)) {
                // let the compiler unroll the loop
                for (size_t k = 0; k < UNROLL; ++k) {
                    auto mmIn = _mm256_mullo_epi32(_mm256_lddqu_si256(mm_Data), mm_AInv);
                    if (!_mm256_movemask_epi8(_mm256_cmpeq_epi32(_mm256_min_epu32(mmIn, mm_dMax), mmIn))) { // we need to do this "hack" because comparison is only on signed integers!
                        throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<uint32_t*>(mm_Data) - this->bufEncoded.template begin<uint32_t>(), iteration);
                    }
                    ++mm_Data;
                }
            }
            // here follows the non-unrolled remainder
            while (mm_Data <= (mm_DataEnd - 1)) {
                auto mmIn = _mm256_mullo_epi32(_mm256_lddqu_si256(mm_Data), mm_AInv);
                if (!_mm256_movemask_epi8(_mm256_cmpeq_epi32(_mm256_min_epu32(mmIn, mm_dMax), mmIn))) { // we need to do this "hack" because comparison is only on signed integers!
                    throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<uint32_t*>(mm_Data) - this->bufEncoded.template begin<uint32_t>(), iteration);
                }
                ++mm_Data;
            }
            if (mm_Data < mm_DataEnd) {
                auto dataEnd2 = reinterpret_cast<uint32_t*>(mm_DataEnd);
                for (auto data2 = reinterpret_cast<uint32_t*>(mm_Data); data2 < dataEnd2; ++data2) {
                    if ((*data2 * this->A_INV) > dMax) {
                        throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<uint32_t*>(data2) - this->bufEncoded.template begin<uint32_t>(), iteration);
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
            _ReadWriteBarrier();
            const ssize_t VALUES_PER_SIMDREG = sizeof(__m256i) / sizeof (uint32_t);
            const ssize_t VALUES_PER_UNROLL = UNROLL * VALUES_PER_SIMDREG;
            ssize_t numValues = this->bufRaw.template end<int16_t>() - this->bufRaw.template begin<int16_t>();
            ssize_t i = 0;
            auto mm_In = this->bufEncoded.template begin<__m256i >();
            auto mm_Out = this->bufResult.template begin<__m128i >();
            auto mm_AInv = _mm256_set1_epi32(this->A_INV);
            auto mm_Shuffle = _mm256_set_epi64x(0xFFFFFFFFFFFFFFFFll, 0xFFFFFFFFFFFFFFFFll, 0x1D1C191815141110ll, 0x0D0C090805040100ll);
            for (; i <= (numValues - VALUES_PER_UNROLL); i += VALUES_PER_UNROLL) {
                // let the compiler unroll the loop
                for (size_t unroll = 0; unroll < UNROLL; ++unroll) {
                    auto tmp = _mm256_lddqu_si256(mm_In++);
                    tmp = _mm256_mullo_epi32(tmp, mm_AInv);
                    tmp = _mm256_shuffle_epi8(tmp, mm_Shuffle);
                    _mm_storeu_si128(mm_Out++, _mm256_extracti128_si256(tmp, 0));
                }
            }
            // remaining numbers
            for (; i <= (numValues - VALUES_PER_SIMDREG); i += VALUES_PER_SIMDREG) {
                auto tmp = _mm256_lddqu_si256(mm_In++);
                tmp = _mm256_mullo_epi32(tmp, mm_AInv);
                tmp = _mm256_shuffle_epi8(tmp, mm_Shuffle);
                _mm_storeu_si128(mm_Out++, _mm256_extracti128_si256(tmp, 0));
            }
            if (i < numValues) {
                auto out16 = reinterpret_cast<uint16_t*>(mm_Out);
                auto in32 = reinterpret_cast<uint32_t*>(mm_In);
                for (; i < numValues; ++i, ++in32, ++out16) {
                    *out16 = *in32 * this->A_INV;
                }
            }
        }
    }
};

