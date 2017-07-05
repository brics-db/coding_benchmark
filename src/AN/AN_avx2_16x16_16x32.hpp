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
 * File:   AN_avx2_16x16_16x32.hpp
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 13. Dezember 2016, 00:11
 */

#pragma once

#include "ANTest.hpp"

template<typename DATAIN, typename DATAOUT, size_t UNROLL>
struct AN_avx2_16x16_16x32 :
        public ANTest<DATAIN, DATAOUT, UNROLL>,
        public AVX2Test {

    AN_avx2_16x16_16x32(
            const char* const name,
            AlignedBlock & in,
            AlignedBlock & out,
            DATAOUT A,
            DATAOUT AInv)
            : ANTest<DATAIN, DATAOUT, UNROLL>(name, in, out, A, AInv) {
    }

    virtual ~AN_avx2_16x16_16x32() {
    }

    void RunEnc(
            const size_t numIterations) override {
        for (size_t iter = 0; iter < numIterations; ++iter) {
            auto *dataIn = this->in.template begin<__m128i >();
            auto *dataInEnd = this->in.template end<__m128i >();
            auto *dataOut = this->out.template begin<__m256i >();
            auto mmA = _mm256_set1_epi32(this->A);

            constexpr const bool isSigned = std::is_signed<DATAIN>::value;
            while (dataIn <= (dataInEnd - UNROLL)) {
                // let the compiler unroll the loop
                for (size_t unroll = 0; unroll < UNROLL; ++unroll) {
                    auto mm128 = _mm_lddqu_si128(dataIn++);
                    auto mm256 = isSigned ? _mm256_cvtepi16_epi32(mm128) : _mm256_cvtepu16_epi32(mm128);
                    _mm256_storeu_si256(dataOut++, _mm256_mullo_epi32(mm256, mmA));
                }
            }

            while (dataIn <= (dataInEnd - 1)) {
                auto mm128 = _mm_lddqu_si128(dataIn++);
                auto mm256 = isSigned ? _mm256_cvtepi16_epi32(mm128) : _mm256_cvtepu16_epi32(mm128);
                _mm256_storeu_si256(dataOut++, _mm256_mullo_epi32(mm256, mmA));
            }

            // multiply remaining numbers sequentially
            if (dataIn < dataInEnd) {
                auto data16 = reinterpret_cast<DATAIN*>(dataIn);
                auto data16End = reinterpret_cast<DATAIN*>(dataInEnd);
                auto out32 = reinterpret_cast<DATAOUT*>(dataOut);
                do {
                    *out32++ = static_cast<DATAOUT>(*data16++) * this->A;
                } while (data16 < data16End);
            }
        }
    }
};
