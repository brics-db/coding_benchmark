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

#ifndef AN_AVX2
#error "Clients must not include this file directly, but file <AN/AN_avx2.hpp>!"
#endif

#include <AN/AN_avx2_16x16_16x32.hpp>

namespace coding_benchmark {

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
                __m256i mmDMin = _mm256_set1_epi32(dMin); // we assume 16-bit input data
                __m256i mmDMax = _mm256_set1_epi32(dMax); // we assume 16-bit input data
                __m256i mmAinv = _mm256_set1_epi32(this->A_INV);
                while (mm_Data <= (mm_DataEnd - UNROLL)) {
                    // let the compiler unroll the loop
                    for (size_t k = 0; k < UNROLL; ++k) {
                        auto mmIn = _mm256_mullo_epi32(_mm256_lddqu_si256(mm_Data), mmAinv);
                        if (_mm256_movemask_epi8(_mm256_cmpgt_epi32(mmIn, mmDMax)) | _mm256_movemask_epi8(_mm256_cmpgt_epi32(mmDMin, mmIn))) {
                            throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<int32_t*>(mm_Data) - this->bufEncoded.template begin<int32_t>(), iteration);
                        }
                        ++mm_Data;
                    }
                }
                // here follows the non-unrolled remainder
                while (mm_Data <= (mm_DataEnd - 1)) {
                    auto mmIn = _mm256_mullo_epi32(_mm256_lddqu_si256(mm_Data), mmAinv);
                    if (_mm256_movemask_epi8(_mm256_cmpgt_epi32(mmIn, mmDMax)) | _mm256_movemask_epi8(_mm256_cmpgt_epi32(mmDMin, mmIn))) {
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

        bool DoArithmeticChecked(
                const ArithmeticConfiguration & config) override {
            return std::visit(ArithmeticSelector(), config.mode);
        }

        struct ArithmetorChecked {
            AN_avx2_16x16_16x32_s_inv & test;
            const ArithmeticConfiguration & config;
            const size_t iteration;
            ArithmetorChecked(
                    AN_avx2_16x16_16x32_s_inv & test,
                    const ArithmeticConfiguration & config,
                    const size_t iteration)
                    : test(test),
                      config(config),
                      iteration(iteration) {
            }
            void operator()(
                    ArithmeticConfiguration::Add) {
                int32_t dMin = std::numeric_limits<int16_t>::min();
                int32_t dMax = std::numeric_limits<int16_t>::max();
                __m256i mmDMin = _mm256_set1_epi32(dMin); // we assume 16-bit input data
                __m256i mmDMax = _mm256_set1_epi32(dMax); // we assume 16-bit input data
                __m256i mmAinv = _mm256_set1_epi32(test.A_INV);
                auto mmData = test.bufEncoded.template begin<__m256i >();
                const auto mmDataEnd = test.bufEncoded.template end<__m256i >();
                auto mmOut = test.bufResult.template begin<__m256i >();
                int32_t operandEnc = config.operand * test.A;
                auto mmOperandEnc = _mm256_set1_epi32(operandEnc);
                while (mmData <= (mmDataEnd - UNROLL)) {
                    // let the compiler unroll the loop
                    for (size_t unroll = 0; unroll < UNROLL; ++unroll) {
                        auto mmIn = _mm256_lddqu_si256(mmData++);
                        auto mmIn2 = _mm256_mullo_epi32(mmIn, mmAinv);
                        if (_mm256_movemask_epi8(_mm256_cmpgt_epi32(mmIn2, mmDMax)) | _mm256_movemask_epi8(_mm256_cmpgt_epi32(mmDMin, mmIn2))) {
                            throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<int32_t*>(mmData) - test.bufEncoded.template begin<int32_t>(), iteration);
                        }
                        _mm256_storeu_si256(mmOut++, _mm256_add_epi32(mmIn, mmOperandEnc));
                    }
                }
                // remaining numbers
                while (mmData <= (mmDataEnd - 1)) {
                    auto mmIn = _mm256_lddqu_si256(mmData++);
                    auto mmIn2 = _mm256_mullo_epi32(mmIn, mmAinv);
                    if (_mm256_movemask_epi8(_mm256_cmpgt_epi32(mmIn2, mmDMax)) | _mm256_movemask_epi8(_mm256_cmpgt_epi32(mmDMin, mmIn2))) {
                        throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<int32_t*>(mmData) - test.bufEncoded.template begin<int32_t>(), iteration);
                    }
                    _mm256_storeu_si256(mmOut++, _mm256_add_epi32(mmIn, mmOperandEnc));
                }
                if (mmData < mmDataEnd) {
                    auto data32End = reinterpret_cast<int32_t*>(mmDataEnd);
                    auto out32 = reinterpret_cast<int32_t*>(mmOut);
                    for (auto data32 = reinterpret_cast<int32_t*>(mmData); data32 < data32End; ++data32, ++out32) {
                        auto tmp = *data32 * test.A_INV;
                        if ((tmp < dMin) | (tmp > dMax)) {
                            throw ErrorInfo(__FILE__, __LINE__, data32 - test.bufEncoded.template begin<int32_t>(), iteration);
                        }
                        *out32 = *data32 + operandEnc;
                    }
                }
            }
            void operator()(
                    ArithmeticConfiguration::Sub) {
            }
            void operator()(
                    ArithmeticConfiguration::Mul) {
            }
            void operator()(
                    ArithmeticConfiguration::Div) {
            }
        };

        void RunArithmeticChecked(
                const ArithmeticConfiguration & config) override {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                _ReadWriteBarrier();
                std::visit(ArithmetorChecked(*this, config, iteration), config.mode);
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

}
