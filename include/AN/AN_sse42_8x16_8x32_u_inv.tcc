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

#include <AN/AN_sse42_8x16_8x32.tcc>

namespace coding_benchmark {

    template<size_t UNROLL>
    struct AN_sse42_8x16_8x32_u_inv :
            public AN_sse42_8x16_8x32<uint16_t, uint32_t, UNROLL> {

        using AN_sse42_8x16_8x32<uint16_t, uint32_t, UNROLL>::AN_sse42_8x16_8x32;

        virtual ~AN_sse42_8x16_8x32_u_inv() {
        }

        virtual bool DoCheck() override {
            return true;
        }

        virtual void RunCheck(
                const CheckConfiguration & config) override {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                auto mm_Data = this->bufEncoded.template begin<__m128i >();
                auto mm_DataEnd = this->bufEncoded.template end<__m128i >();
                uint32_t dMax = std::numeric_limits<uint16_t>::max();
                __m128i mmDMax = _mm_set1_epi32(dMax); // we assume 16-bit input data
                __m128i mmAInv = _mm_set1_epi32(this->A_INV);
                while (mm_Data <= (mm_DataEnd - UNROLL)) {
                    // let the compiler unroll the loop
                    for (size_t k = 0; k < UNROLL; ++k) {
                        auto mmIn = _mm_mullo_epi32(_mm_lddqu_si128(mm_Data), mmAInv);
                        if (0xFFFF != _mm_movemask_epi8(_mm_cmpeq_epi32(_mm_min_epu32(mmIn, mmDMax), mmIn))) { // we need to do this "hack" because comparison is only on signed integers!
                            throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<uint32_t*>(mm_Data) - this->bufEncoded.template begin<uint32_t>(), iteration);
                        }
                        ++mm_Data;
                    }
                }
                // here follows the non-unrolled remainder
                while (mm_Data <= (mm_DataEnd - 1)) {
                    auto mmIn = _mm_mullo_epi32(_mm_lddqu_si128(mm_Data), mmAInv);
                    if (0xFFFF != _mm_movemask_epi8(_mm_cmpeq_epi32(_mm_min_epu32(mmIn, mmDMax), mmIn))) { // we need to do this "hack" because comparison is only on signed integers!
                        throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<uint32_t*>(mm_Data) - this->bufEncoded.template begin<uint32_t>(), iteration);
                    }
                    ++mm_Data;
                }
                if (mm_Data < mm_DataEnd) {
                    auto dataEnd2 = reinterpret_cast<uint32_t*>(mm_DataEnd);
                    for (auto data2 = reinterpret_cast<uint32_t*>(mm_Data); data2 < dataEnd2; ++data2) {
                        if ((*data2 * this->A_INV) > dMax) {
                            throw ErrorInfo(__FILE__, __LINE__, data2 - this->bufEncoded.template begin<uint32_t>(), iteration);
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
            AN_sse42_8x16_8x32_u_inv & test;
            const ArithmeticConfiguration & config;
            const size_t iteration;
            ArithmetorChecked(
                    AN_sse42_8x16_8x32_u_inv & test,
                    const ArithmeticConfiguration & config,
                    const size_t iteration)
                    : test(test),
                      config(config),
                      iteration(iteration) {
            }
            void operator()(
                    ArithmeticConfiguration::Add) {
                uint32_t dMax = std::numeric_limits<uint16_t>::max();
                __m128i mmDMax = _mm_set1_epi32(dMax); // we assume 16-bit input data
                __m128i mmAInv = _mm_set1_epi32(test.A_INV);
                auto mmData = test.bufEncoded.template begin<__m128i >();
                const auto mmDataEnd = test.bufEncoded.template end<__m128i >();
                auto mmOut = test.bufResult.template begin<__m128i >();
                uint32_t operandEnc = config.operand * test.A;
                auto mmOperandEnc = _mm_set1_epi32(operandEnc);
                while (mmData <= (mmDataEnd - UNROLL)) {
                    // let the compiler unroll the loop
                    for (size_t unroll = 0; unroll < UNROLL; ++unroll) {
                        auto mmIn = _mm_lddqu_si128(mmData++);
                        auto mmIn2 = _mm_mullo_epi32(mmIn, mmAInv);
                        if (0xFFFF == _mm_movemask_epi8(_mm_cmpeq_epi32(_mm_min_epu32(mmIn2, mmDMax), mmIn2))) { // we need to do this "hack" because comparison is only on signed integers!
                            _mm_storeu_si128(mmOut++, _mm_add_epi32(mmIn, mmOperandEnc));
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<int32_t*>(mmData) - test.bufEncoded.template begin<int32_t>(), iteration);
                        }
                    }
                }
                // remaining numbers
                while (mmData <= (mmDataEnd - 1)) {
                    auto mmIn = _mm_lddqu_si128(mmData++);
                    auto mmIn2 = _mm_mullo_epi32(mmIn, mmAInv);
                    if (0xFFFF == _mm_movemask_epi8(_mm_cmpeq_epi32(_mm_min_epu32(mmIn2, mmDMax), mmIn2))) { // we need to do this "hack" because comparison is only on signed integers!
                        _mm_storeu_si128(mmOut++, _mm_add_epi32(mmIn, mmOperandEnc));
                    } else {
                        throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<uint32_t*>(mmData) - test.bufEncoded.template begin<uint32_t>(), iteration);
                    }
                }
                if (mmData < mmDataEnd) {
                    auto data32End = reinterpret_cast<uint32_t*>(mmDataEnd);
                    auto out32 = reinterpret_cast<uint32_t*>(mmOut);
                    for (auto data32 = reinterpret_cast<uint32_t*>(mmData); data32 < data32End; ++data32, ++out32) {
                        auto tmp = *data32 * test.A_INV;
                        if (tmp <= dMax) {
                            *out32 = *data32 + operandEnc;
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, data32 - test.bufEncoded.template begin<uint32_t>(), iteration);
                        }
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

        bool DoDecode() override {
            return true;
        }

        void RunDecode(
                const DecodeConfiguration & config) override {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                const size_t VALUES_PER_SIMDREG = sizeof(__m128i) /sizeof (uint32_t);
            const size_t VALUES_PER_UNROLL = UNROLL * VALUES_PER_SIMDREG;
            size_t numValues = this->bufRaw.template end<uint16_t>() - this->bufRaw.template begin<uint16_t>();
            size_t i = 0;
            auto mm_Data = this->bufEncoded.template begin<__m128i>();
            auto mm_Out = this->bufResult.template begin<uint64_t>();
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

}
