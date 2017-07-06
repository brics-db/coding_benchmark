// Copyright 2016 Till Kolditz, Stefan de Bruijn
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
 * File:   Hamming_sse42.hpp
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 07. December 2016, 13:12
 */

#pragma once

#include <cstdint>

#include "../Test.hpp"
#include "../Util/Intrinsics.hpp"
#include "Hamming_seq.hpp"

struct hamming_sse42_16_t {

    __m128i data;
    uint64_t code;
};

struct hamming_sse42_32_t {

    __m128i data;
    uint32_t code;
};

template<typename OrigType>
struct TypeMapSSE42;

/*
 * For the following algorithms, see
 * https://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel
 * Credit also goes to the book "Hacker's Delight" 2nd Edition, by Henry S. Warren, Jr., published at Addison-Wesley
 */

template<>
struct TypeMapSSE42<uint16_t> {

    typedef hamming_sse42_16_t hamming_sse42_t;

    static uint64_t hammingWeight16(
            __m128i && data) {
        static auto pattern1 = _mm_set1_epi16(0x5555);
        static auto pattern2 = _mm_set1_epi16(0x3333);
        static auto pattern3 = _mm_set1_epi16(0x0F0F);
        static auto pattern4 = _mm_set1_epi16(0x0101);
        static auto shuffle = _mm_set_epi64x(0xFFFFFFFFFFFFFFFF, 0x0E0C0A0806040200);
        auto temp = _mm_sub_epi16(data, _mm_and_si128(_mm_srli_epi16(data, 1), pattern1));
        temp = _mm_add_epi16(_mm_and_si128(temp, pattern2), _mm_and_si128(_mm_srli_epi16(temp, 2), pattern2));
        temp = _mm_and_si128(_mm_add_epi16(temp, _mm_srli_epi16(temp, 4)), pattern3);
        temp = _mm_srli_epi16(_mm_mullo_epi16(temp, pattern4), 8);
        return static_cast<uint64_t>(_mm_extract_epi64(_mm_shuffle_epi8(temp, shuffle), 0));
    }

    static uint64_t computeHamming(
            __m128i && data) {
        static auto pattern1 = _mm_set1_epi16(static_cast<int16_t>(0xAD5B));
        static auto pattern2 = _mm_set1_epi16(static_cast<int16_t>(0x366D));
        static auto pattern3 = _mm_set1_epi16(static_cast<int16_t>(0xC78E));
        static auto pattern4 = _mm_set1_epi16(static_cast<int16_t>(0x07F0));
        static auto pattern5 = _mm_set1_epi16(static_cast<int16_t>(0xF800));
        uint64_t hamming = 0;
        uint64_t tmp1(0), tmp2(0);
        tmp2 = hammingWeight16(_mm_and_si128(data, pattern1)) & 0x0101010101010101;
        hamming |= tmp2 << 1;
        tmp1 = hammingWeight16(_mm_and_si128(data, pattern2)) & 0x0101010101010101;
        tmp2 ^= tmp1;
        hamming |= tmp1 << 2;
        tmp1 = hammingWeight16(_mm_and_si128(data, pattern3)) & 0x0101010101010101;
        tmp2 ^= tmp1;
        hamming |= tmp1 << 3;
        tmp1 = hammingWeight16(_mm_and_si128(data, pattern4)) & 0x0101010101010101;
        tmp2 ^= tmp1;
        hamming |= tmp1 << 4;
        tmp1 = hammingWeight16(_mm_and_si128(data, pattern5)) & 0x0101010101010101;
        tmp2 ^= tmp1;
        hamming |= tmp1 << 5;
        hamming |= (hammingWeight16(std::move(data)) + tmp2) & 0x0101010101010101;
        return hamming;
    }
};

template<>
struct TypeMapSSE42<uint32_t> {

    typedef hamming_sse42_32_t hamming_sse42_t;

    static uint32_t hammingWeight32(
            __m128i && data) {
        static auto pattern1 = _mm_set1_epi32(0x55555555);
        static auto pattern2 = _mm_set1_epi32(0x33333333);
        static auto pattern3 = _mm_set1_epi32(0x0F0F0F0F);
        static auto pattern4 = _mm_set1_epi32(0x01010101);
        static auto shuffle = _mm_set_epi64x(0xFFFFFFFFFFFFFFFF, 0xFFFFFFFF0C080400);
        auto temp = _mm_sub_epi32(data, _mm_and_si128(_mm_srli_epi32(data, 1), pattern1));
        temp = _mm_add_epi32(_mm_and_si128(temp, pattern2), _mm_and_si128(_mm_srli_epi32(temp, 2), pattern2));
        temp = _mm_and_si128(_mm_add_epi32(temp, _mm_srli_epi32(temp, 4)), pattern3);
        temp = _mm_srli_epi32(_mm_mullo_epi32(temp, pattern4), 24);
        return static_cast<uint32_t>(_mm_extract_epi32(_mm_shuffle_epi8(temp, shuffle), 0));
    }

    static uint32_t computeHamming(
            __m128i && data) {
        static auto pattern1 = _mm_set1_epi32(0x56AAAD5B);
        static auto pattern2 = _mm_set1_epi32(0x9B33366D);
        static auto pattern3 = _mm_set1_epi32(0xE3C3C78E);
        static auto pattern4 = _mm_set1_epi32(0x03FC07F0);
        static auto pattern5 = _mm_set1_epi32(0x03FFF800);
        static auto pattern6 = _mm_set1_epi32(0xFC000000);
        uint32_t hamming = 0;
        uint32_t tmp1(0), tmp2(0);
        tmp2 = hammingWeight32(_mm_and_si128(data, pattern1)) & 0x01010101;
        hamming |= tmp2 << 1;
        tmp1 = hammingWeight32(_mm_and_si128(data, pattern2)) & 0x01010101;
        tmp2 ^= tmp1;
        hamming |= tmp1 << 2;
        tmp1 = hammingWeight32(_mm_and_si128(data, pattern3)) & 0x01010101;
        tmp2 ^= tmp1;
        hamming |= tmp1 << 3;
        tmp1 = hammingWeight32(_mm_and_si128(data, pattern4)) & 0x01010101;
        tmp2 ^= tmp1;
        hamming |= tmp1 << 4;
        tmp1 = hammingWeight32(_mm_and_si128(data, pattern5)) & 0x01010101;
        tmp2 ^= tmp1;
        hamming |= tmp1 << 5;
        tmp1 = hammingWeight32(_mm_and_si128(data, pattern6)) & 0x01010101;
        tmp2 ^= tmp1;
        hamming |= tmp1 << 6;
        hamming |= (hammingWeight32(std::move(data)) + tmp2) & 0x01010101;
        return hamming;
    }
};

template<typename DATAIN, size_t UNROLL>
struct Hamming_sse42 :
        public Test<DATAIN, typename TypeMapSSE42<DATAIN>::hamming_sse42_t>,
        public SSE42Test {

    typedef typename TypeMapSSE42<DATAIN>::hamming_sse42_t hamming_sse42_t;
    typedef typename TypeMapSeq<DATAIN>::hamming_seq_t hamming_seq_t;

    Hamming_sse42(
            const char* const name,
            AlignedBlock & in,
            AlignedBlock & out)
            : Test<DATAIN, hamming_sse42_t>(name, in, out) {
    }

    virtual ~Hamming_sse42() {
    }

    void RunEnc(
            const EncodeConfiguration & config) override {
        for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
            auto data = this->in.template begin<__m128i >();
            auto dataEnd = this->in.template end<__m128i >();
            auto dataOut = this->out.template begin<hamming_sse42_t>();
            while (data <= (dataEnd - UNROLL)) {
                for (size_t k = 0; k < UNROLL; ++k, ++data, ++dataOut) {
                    dataOut->data = *data;
                    dataOut->code = TypeMapSSE42<DATAIN>::computeHamming(std::move(*data));
                }
            }
            for (; data <= (dataEnd - 1); ++data, ++dataEnd) {
                dataOut->data = *data;
                dataOut->code = TypeMapSSE42<DATAIN>::computeHamming(std::move(*data));
            }
            if (data < dataEnd) {
                auto data2 = reinterpret_cast<DATAIN*>(data);
                auto dataEnd2 = reinterpret_cast<DATAIN*>(dataEnd);
                auto dataOut2 = reinterpret_cast<hamming_seq_t*>(dataOut);
                for (; data2 < dataEnd2; ++data2, ++dataOut2) {
                    dataOut2->data = *data2;
                    dataOut2->code = TypeMapSeq<DATAIN>::computeHamming(std::move(*data2));
                }
            }
        }
    }

    bool DoCheck() override {
        return true;
    }

    void RunCheck(
            const CheckConfiguration & config) override {
        for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
            size_t numValues = this->in.template end<DATAIN>() - this->in.template begin<DATAIN>();
            size_t i = 0;
            auto data = this->out.template begin<hamming_sse42_t>();
            while (i <= (numValues - UNROLL)) {
                for (size_t k = 0; k < UNROLL; ++k, i += (sizeof(__m128i) / sizeof (DATAIN)), ++data) {
                    if (data->code != TypeMapSSE42<DATAIN>::computeHamming(std::move(data->data))) {
                        throw ErrorInfo(__FILE__, __LINE__, data - this->out.template begin<hamming_sse42_t>(), config.numIterations);
                    }
                }
            }
            for (; i <= (numValues - 1); i += (sizeof(__m128i) / sizeof (DATAIN)), ++data) {
                if (data->code != TypeMapSSE42<DATAIN>::computeHamming(std::move(data->data))) {
                    throw ErrorInfo(__FILE__, __LINE__, data - this->out.template begin<hamming_sse42_t>(), config.numIterations);
                }
            }
            if (i < numValues) {
                for (auto data2 = reinterpret_cast<hamming_seq_t*>(data); i < numValues; ++i, ++data2) {
                    if (data2->code != TypeMapSeq<DATAIN>::computeHamming(std::move(data2->data))) {
                        throw ErrorInfo(__FILE__, __LINE__, data2 - this->out.template begin<hamming_seq_t>(), config.numIterations);
                    }
                }
            }
        }
    }

    bool DoDec() override {
        return true;
    }

    void RunDec(
            const DecodeConfiguration & config) override {
        for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
            const size_t VALUES_PER_SIMDREG = sizeof(__m128i) / sizeof (DATAIN);
            const size_t VALUES_PER_UNROLL = UNROLL * VALUES_PER_SIMDREG;
            size_t numValues = this->in.template end<DATAIN>() - this->in.template begin<DATAIN>();
            size_t i = 0;
            auto data = this->out.template begin<hamming_sse42_t>();
            auto dataOut = this->in.template begin<__m128i>();
            while (i <= (numValues - VALUES_PER_UNROLL)) {
                for (size_t k = 0; k < UNROLL; ++k, ++data, ++dataOut) {
                    *dataOut = data->data;
                }
                i += VALUES_PER_UNROLL;
            }
            for (; i <= (numValues - 1); i += VALUES_PER_SIMDREG, ++data, ++dataOut) {
                *dataOut = data->data;
            }
            if (i < numValues) {
                auto data2 = reinterpret_cast<hamming_seq_t*>(data);
                auto dataOut2 = reinterpret_cast<DATAIN*>(dataOut);
                for (; i < numValues; ++i, ++data2, ++dataOut2) {
                    *dataOut2 = data2->data;
                }
            }
        }
    }
};
