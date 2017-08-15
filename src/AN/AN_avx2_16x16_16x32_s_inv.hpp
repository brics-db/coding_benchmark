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
struct AN_avx2_16x16_16x32_s_inv :
        public AN_avx2_16x16_16x32<int16_t, int32_t, UNROLL> {

    using AN_avx2_16x16_16x32<int16_t, int32_t, UNROLL>::AN_avx2_16x16_16x32;

    virtual ~AN_avx2_16x16_16x32_s_inv() {
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
            int32_t dMin = std::numeric_limits<int16_t>::min();
            int32_t dMax = std::numeric_limits<int16_t>::max();
            __m256i mm_dMin = _mm256_set1_epi32(dMin); // we assume 16-bit input data
            __m256i mm_dMax = _mm256_set1_epi32(dMax); // we assume 16-bit input data
            __m256i mmAinv = _mm256_set1_epi32(this->A_INV);
            while (mm_Data <= (mm_DataEnd - UNROLL)) {
                // let the compiler unroll the loop
                for (size_t k = 0; k < UNROLL; ++k) {
                    auto mmIn = _mm256_mullo_epi32(_mm256_lddqu_si256(mm_Data), mmAinv);
                    if (_mm256_movemask_epi8(_mm256_cmpgt_epi32(mmIn, mm_dMax)) | _mm256_movemask_epi8(_mm256_cmpgt_epi32(mm_dMin, mmIn))) {
                        throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<int32_t*>(mm_Data) - this->bufEncoded.template begin<int32_t>(), iteration);
                    }
                    ++mm_Data;
                }
            }
            // here follows the non-unrolled remainder
            while (mm_Data <= (mm_DataEnd - 1)) {
                auto mmIn = _mm256_mullo_epi32(_mm256_lddqu_si256(mm_Data), mmAinv);
                if (_mm256_movemask_epi8(_mm256_cmpgt_epi32(mmIn, mm_dMax)) | _mm256_movemask_epi8(_mm256_cmpgt_epi32(mm_dMin, mmIn))) {
                    throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<int32_t*>(mm_Data) - this->bufEncoded.template begin<int32_t>(), iteration);
                }
                ++mm_Data;
            }
            if (mm_Data < mm_DataEnd) {
                auto dataEnd2 = reinterpret_cast<int32_t*>(mm_DataEnd);
                for (auto data2 = reinterpret_cast<int32_t*>(mm_Data); data2 < dataEnd2; ++data2) {
                    auto data = *data2 * this->A_INV;
                    if (data < dMin || data > dMax) {
                        throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<int32_t*>(data2) - this->bufEncoded.template begin<int32_t>(), iteration);
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
            const ssize_t VALUES_PER_SIMDREG = sizeof(__m256i) / sizeof (int32_t);
            const ssize_t VALUES_PER_UNROLL = UNROLL * VALUES_PER_SIMDREG;
            ssize_t numValues = this->bufRaw.template end<int16_t>() - this->bufRaw.template begin<int16_t>();
            ssize_t i = 0;
            auto mm_In = this->bufEncoded.template begin<__m256i >();
            auto mm_Out = this->bufResult.template begin<__m128i >();
            auto mmAinv = _mm256_set1_epi32(this->A_INV);
            auto mmShuffle = _mm256_set_epi64x(0xFFFFFFFFFFFFFFFFll, 0xFFFFFFFFFFFFFFFFll, 0x1D1C191815141110ll, 0x0D0C090805040100ll);
            for (; i <= (numValues - VALUES_PER_UNROLL); i += VALUES_PER_UNROLL) {
                // let the compiler unroll the loop
                for (size_t unroll = 0; unroll < UNROLL; ++unroll) {
                    auto tmp = _mm256_lddqu_si256(mm_In++);
                    tmp = _mm256_mullo_epi32(tmp, mmAinv);
                    tmp = _mm256_shuffle_epi8(tmp, mmShuffle);
                    _mm_storeu_si128(mm_Out++, _mm256_extracti128_si256(tmp, 0));
                }
            }
            // remaining numbers
            for (; i <= (numValues - VALUES_PER_SIMDREG); i += VALUES_PER_SIMDREG) {
                auto tmp = _mm256_lddqu_si256(mm_In++);
                tmp = _mm256_mullo_epi32(tmp, mmAinv);
                tmp = _mm256_shuffle_epi8(tmp, mmShuffle);
                _mm_storeu_si128(mm_Out++, _mm256_extracti128_si256(tmp, 0));
            }
            if (i < numValues) {
                auto out16 = reinterpret_cast<int16_t*>(mm_Out);
                auto in32 = reinterpret_cast<int32_t*>(mm_In);
                for (; i < numValues; ++i, ++in32, ++out16) {
                    *out16 = *in32 * this->A_INV;
                }
            }
        }
    }
};

