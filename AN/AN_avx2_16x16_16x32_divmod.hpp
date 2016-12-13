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

#include "AN_avx2_16x16_16x32.hpp"

template<size_t UNROLL>
struct AN_avx2_16x16_16x32_divmod : public AN_avx2_16x16_16x32<UNROLL> {

    AN_avx2_16x16_16x32_divmod (const char* const name, AlignedBlock & in, AlignedBlock & out, uint32_t A = 63'877ul, uint32_t Ainv = 3'510'769'485ul) :
            AN_avx2_16x16_16x32<UNROLL>(name, in, out, A, Ainv) {
    }

    virtual
    ~AN_avx2_16x16_16x32_divmod () {
    }

    virtual bool
    DoCheck () override {
        return false;
    }

    virtual void
    RunCheck (const size_t numIterations) override {
        for (size_t iteration = 0; iteration < numIterations; ++iteration) {
            auto data = this->out.template begin<__m256i>();
            auto dataEnd = this->out.template end<__m256i>();
            while (data <= (dataEnd - UNROLL)) {
                // let the compiler unroll the loop
                for (size_t k = 0; k < UNROLL; ++k) {
                    auto mmIn = _mm256_lddqu_si256(data);
                    if ((_mm256_extract_epi32(mmIn, 0) % A != 0) || (_mm256_extract_epi32(mmIn, 1) % A != 0) || (_mm256_extract_epi32(mmIn, 2) % A != 0) || (_mm256_extract_epi32(mmIn, 3) % A != 0) || (_mm256_extract_epi32(mmIn, 4) % A != 0) || (_mm256_extract_epi32(mmIn, 5) % A != 0) || (_mm256_extract_epi32(mmIn, 6) % A != 0) || (_mm256_extract_epi32(mmIn, 7) % A != 0)) { // we need to do this "hack" because comparison is only on signed integers!
                        throw ErrorInfo(reinterpret_cast<uint32_t*>(data) - this->out.template begin<uint32_t>(), iteration);
                    }
                    ++data;
                }
            }
            // here follows the non-unrolled remainder
            while (data <= (dataEnd - 1)) {
                auto mmIn = _mm256_lddqu_si256(data);
                if ((_mm256_extract_epi32(mmIn, 0) % A != 0) || (_mm256_extract_epi32(mmIn, 1) % A != 0) || (_mm256_extract_epi32(mmIn, 2) % A != 0) || (_mm256_extract_epi32(mmIn, 3) % A != 0) || (_mm256_extract_epi32(mmIn, 4) % A != 0) || (_mm256_extract_epi32(mmIn, 5) % A != 0) || (_mm256_extract_epi32(mmIn, 6) % A != 0) || (_mm256_extract_epi32(mmIn, 7) % A != 0)) { // we need to do this "hack" because comparison is only on signed integers!
                    throw ErrorInfo(reinterpret_cast<uint32_t*>(data) - this->out.template begin<uint32_t>(), iteration);
                }
                ++data;
            }
            if (data < dataEnd) {
                auto dataEnd2 = reinterpret_cast<uint32_t*>(dataEnd);
                for (auto data2 = reinterpret_cast<uint32_t*>(data); data2 < dataEnd2; ++data2) {
                    if ((*data2 % this->A) != 0) {
                        throw ErrorInfo(reinterpret_cast<uint32_t*>(data2) - this->out.template begin<uint32_t>(), iteration);
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
            const size_t VALUES_PER_SIMDREG = sizeof (__m256i) / sizeof (uint32_t);
            const size_t VALUES_PER_UNROLL = UNROLL * VALUES_PER_SIMDREG;
            size_t numValues = this->in.template end<int16_t>() - this->in.template begin<int16_t>();
            size_t i = 0;
            auto dataIn = this->out.template begin<__m256i>();
            auto dataOut = this->in.template begin<__int64>();
            auto mmAinv = _mm256_set1_epi32(this->A_INV);
            auto mmPermute = _mm256_set_epi32(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000007, 0x00000006, 0x00000005, 0x00000004);
            auto mmShuffle = _mm_set_epi32(0xFFFFFFFF, 0xFFFFFFFF, 0x0D0C0908, 0x05040100);
            for (; i <= (numValues - VALUES_PER_UNROLL); i += VALUES_PER_UNROLL) {
                // let the compiler unroll the loop
                for (size_t unroll = 0; unroll < UNROLL; ++unroll) {
                    auto tmp = _mm256_lddqu_si256(dataIn++);
                    auto tmp1 = _mm256_cvtepi32_pd(tmp);
                    auto tmp2 = _mm256_cvtepi32_pd(_mm256_permutevar8x32_epi32(tmp, mmPermute));
                    tmp1 = _mm256_div_pd(tmp1, mmAinv);
                    tmp2 = _mm256_div_pd(tmp2, mmAinv);
                    tmp1 = _mm256_cvtpd_epi32(tmp1);
                    tmp2 = _mm256_cvtpd_epi32(tmp2);
                    auto tmp3 = _mm256_extracti128_si256(tmp1, 0);
                    auto tmp4 = _mm256_extracti128_si256(tmp2, 0);
                    tmp3 = _mm_shuffle_epi8(tmp3, mmShuffle);
                    tmp4 = _mm_shuffle_epi8(tmp4, mmShuffle);
                    *dataOut++ = _mm_extract_epi64(tmp3, 0);
                    *dataOut++ = _mm_extract_epi64(tmp4, 0);
                }
            }
            // remaining numbers
            for (; i <= (numValues - VALUES_PER_SIMDREG); i += VALUES_PER_SIMDREG) {
                auto tmp = _mm256_lddqu_si256(dataIn++);
                auto tmp1 = _mm256_cvtepi32_pd(tmp);
                auto tmp2 = _mm256_cvtepi32_pd(_mm256_permutevar8x32_epi32(tmp, mmPermute));
                tmp1 = _mm256_div_pd(tmp1, mmAinv);
                tmp2 = _mm256_div_pd(tmp2, mmAinv);
                tmp1 = _mm256_cvtpd_epi32(tmp1);
                tmp2 = _mm256_cvtpd_epi32(tmp2);
                auto tmp3 = _mm256_extracti128_si256(tmp1, 0);
                auto tmp4 = _mm256_extracti128_si256(tmp2, 0);
                tmp3 = _mm_shuffle_epi8(tmp3, mmShuffle);
                tmp4 = _mm_shuffle_epi8(tmp4, mmShuffle);
                *dataOut++ = _mm_extract_epi64(tmp3, 0);
                *dataOut++ = _mm_extract_epi64(tmp4, 0);
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
