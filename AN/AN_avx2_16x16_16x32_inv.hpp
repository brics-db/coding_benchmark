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
#include <iostream>
#include "AN_avx2_16x16_16x32.hpp"

template<size_t UNROLL>
struct AN_avx2_16x16_16x32_inv : public AN_avx2_16x16_16x32<UNROLL> {

    AN_avx2_16x16_16x32_inv (const char* const name, AlignedBlock & in, AlignedBlock & out, uint32_t A = 63'877ul, uint32_t Ainv = 3'510'769'485ul) :
            AN_avx2_16x16_16x32<UNROLL>(name, in, out, A, Ainv) {
    }

    virtual
    ~AN_avx2_16x16_16x32_inv () {
    }

    virtual bool
    DoCheck () override {
        return true;
    }

    virtual void
    RunCheck (const size_t numIterations) override {
        for (size_t iteration = 0; iteration < numIterations; ++iteration) {
            auto data = this->out.template begin<__m256i>();
            auto dataEnd = this->out.template end<__m256i>();
            uint32_t unencMax = std::numeric_limits<uint16_t>::max();
            __m256i mmUnencMax = _mm256_set1_epi32(unencMax); // we assume 16-bit input data
            __m256i mmAinv = _mm256_set1_epi32(this->A_INV);
            while (data <= (dataEnd - UNROLL)) {
                // let the compiler unroll the loop
                for (size_t k = 0; k < UNROLL; ++k) {
                    auto mmIn = _mm256_mullo_epi32(_mm256_lddqu_si256(data), mmAinv);
                    if (!_mm256_movemask_epi8(_mm256_cmpeq_epi32(_mm256_min_epu32(mmIn, mmUnencMax), mmIn))) { // we need to do this "hack" because comparison is only on signed integers!
                        throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<uint32_t*>(data) - this->out.template begin<uint32_t>(), iteration);
                    }
                    ++data;
                }
            }
            // here follows the non-unrolled remainder
            while (data <= (dataEnd - 1)) {
                auto mmIn = _mm256_mullo_epi32(_mm256_lddqu_si256(data), mmAinv);
                if (!_mm256_movemask_epi8(_mm256_cmpeq_epi32(_mm256_min_epu32(mmIn, mmUnencMax), mmIn))) { // we need to do this "hack" because comparison is only on signed integers!
                    throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<uint32_t*>(data) - this->out.template begin<uint32_t>(), iteration);
                }
                ++data;
            }
            if (data < dataEnd) {
                auto dataEnd2 = reinterpret_cast<uint32_t*>(dataEnd);
                for (auto data2 = reinterpret_cast<uint32_t*>(data); data2 < dataEnd2; ++data2) {
                    if ((*data2 * this->A_INV) > unencMax) {
                        throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<uint32_t*>(data2) - this->out.template begin<uint32_t>(), iteration);
                    }
                }
            }
        }
    }

    bool
    DoDec () override {
        return true;
    }

    void
    RunDec (const size_t numIterations) override {
        for (size_t iteration = 0; iteration < numIterations; ++iteration) {
            const ssize_t VALUES_PER_SIMDREG = sizeof (__m256i) / sizeof (uint32_t);
            const ssize_t VALUES_PER_UNROLL = UNROLL * VALUES_PER_SIMDREG;
            ssize_t numValues = this->in.template end<int16_t>() - this->in.template begin<int16_t>();
            ssize_t i = 0;
            auto dataIn = this->out.template begin<__m256i>();
            auto dataOut = this->in.template begin<__m128i>();
            auto mmAinv = _mm256_set1_epi32(this->A_INV);
            auto mmShuffle = _mm256_set_epi64x(0xFFFFFFFFFFFFFFFFll, 0xFFFFFFFFFFFFFFFFll, 0x1D1C191815141110ll, 0x0D0C090805040100ll);
            for (; i <= (numValues - VALUES_PER_UNROLL); i += VALUES_PER_UNROLL) {
                // let the compiler unroll the loop
                for (size_t unroll = 0; unroll < UNROLL; ++unroll) {
                    auto tmp = _mm256_lddqu_si256(dataIn++);
                    tmp = _mm256_mullo_epi32(tmp, mmAinv);
                    tmp = _mm256_shuffle_epi8(tmp, mmShuffle);
                    _mm_storeu_si128(dataOut++, _mm256_extracti128_si256(tmp, 0));
                }
            }
            // remaining numbers
            for (; i <= (numValues - VALUES_PER_SIMDREG); i += VALUES_PER_SIMDREG) {
                auto tmp = _mm256_lddqu_si256(dataIn++);
                tmp = _mm256_mullo_epi32(tmp, mmAinv);
                tmp = _mm256_shuffle_epi8(tmp, mmShuffle);
                _mm_storeu_si128(dataOut++, _mm256_extracti128_si256(tmp, 0));
            }
            if (i < numValues) {
                auto out16 = reinterpret_cast<uint16_t*>(dataOut);
                auto in32 = reinterpret_cast<uint32_t*>(dataIn);
                for (; i < numValues; ++i, ++in32, ++out16) {
                    *out16 = *in32 * this->A_INV;
                }
            }
        }
    }
};

