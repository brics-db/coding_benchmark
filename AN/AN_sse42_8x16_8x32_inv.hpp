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
struct AN_sse42_8x16_8x32_inv : public AN_sse42_8x16_8x32<UNROLL> {

    AN_sse42_8x16_8x32_inv (const char* const name, AlignedBlock & in, AlignedBlock & out, uint32_t A = 63'877ul, uint32_t Ainv = 3'510'769'485ul) :
            AN_sse42_8x16_8x32<UNROLL>(name, in, out, A, Ainv) {
    }

    virtual
    ~AN_sse42_8x16_8x32_inv () {
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

    virtual bool
    DoCheck () override {
        return true;
    }

    virtual void
    RunCheck (const size_t numIterations) override {
        for (size_t iteration = 0; iteration < numIterations; ++iteration) {
            auto data = this->out.template begin<__m128i>();
            auto dataEnd = this->out.template end<__m128i>();
            uint32_t unencMax = std::numeric_limits<uint16_t>::max();
            __m128i mm_unencmax = _mm_set1_epi32(unencMax); // we assume 16-bit input data
            __m128i mm_ainv = _mm_set1_epi32(this->A_INV);
            while (data <= (dataEnd - UNROLL)) {
                // let the compiler unroll the loop
                for (size_t k = 0; k < UNROLL; ++k) {
                    auto mmIn = _mm_mullo_epi32(_mm_lddqu_si128(data), mm_ainv);
                    if (0xFFFF != _mm_movemask_epi8(_mm_cmpeq_epi32(_mm_min_epu32(mmIn, mm_unencmax), mmIn))) { // we need to do this "hack" because comparison is only on signed integers!
                        throw ErrorInfo(reinterpret_cast<uint32_t*>(data) - this->out.template begin<uint32_t>(), iteration);
                    }
                    ++data;
                }
            }
            // here follows the non-unrolled remainder
            while (data <= (dataEnd - 1)) {
                auto mmIn = _mm_mullo_epi32(_mm_lddqu_si128(data), mm_ainv);
                if (0xFFFF != _mm_movemask_epi8(_mm_cmpeq_epi32(_mm_min_epu32(mmIn, mm_unencmax), mmIn))) { // we need to do this "hack" because comparison is only on signed integers!
                    throw ErrorInfo(reinterpret_cast<uint32_t*>(data) - this->out.template begin<uint32_t>(), iteration);
                }
                ++data;
            }
            if (data < dataEnd) {
                auto dataEnd2 = reinterpret_cast<uint32_t*>(dataEnd);
                for (auto data2 = reinterpret_cast<uint32_t*>(data); data2 < dataEnd2; ++data2) {
                    if ((*data2 * this->A_INV) > unencMax) {
                        throw ErrorInfo(data2 - this->out.template begin<uint32_t>(), iteration);
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
            const size_t VALUES_PER_SIMDREG = sizeof (__m128i) / sizeof (uint32_t);
            const size_t VALUES_PER_UNROLL = UNROLL * VALUES_PER_SIMDREG;
            size_t numValues = this->in.template end<int16_t>() - this->in.template begin<int16_t>();
            size_t i = 0;
            auto dataIn = this->out.template begin<__m128i>();
            auto dataOut = this->in.template begin<uint64_t>();
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
                auto out16 = reinterpret_cast<uint16_t*>(dataOut);
                auto in32 = reinterpret_cast<uint32_t*>(dataIn);
                for (; i < numValues; ++i, ++in32, ++out16) {
                    *out16 = *in32 * this->A_INV;
                }
            }
        }
    }
};
