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

#include <Hamming/Hamming_scalar.hpp>
#include <cstdint>

#include "../Test.hpp"
#include "../Util/Intrinsics.hpp"

struct hamming_sse42_8_t {
    __m128i data;
    __m128i code;
};

struct hamming_sse42_16_t {
    __m128i data;
    uint64_t code;
};

struct hamming_sse42_32_t {
    __m128i data;
    uint32_t code;
};

__m128i _mm_popcount_epi8(
        __m128i data);

__m128i _mm_popcount_epi8_2(
        __m128i data);

uint64_t _mm_popcount_epi16(
        __m128i data);

uint64_t _mm_popcount_epi16_2(
        __m128i data);

uint32_t _mm_popcount_epi32(
        __m128i data);

uint32_t _mm_popcount_epi32_2(
        __m128i data);

template<typename OrigType>
struct HammingSSE42;

template<>
struct HammingSSE42<uint16_t> {

    typedef hamming_sse42_16_t hamming_sse42_t;

    static uint64_t computeHamming(
            __m128i data);

    static uint64_t computeHamming2(
            __m128i data);
};

template<>
struct HammingSSE42<uint32_t> {

    typedef hamming_sse42_32_t hamming_sse42_t;

    static uint32_t computeHamming(
            __m128i data);

    static uint32_t computeHamming2(
            __m128i data);
};

template<typename DATAIN, size_t UNROLL>
struct Hamming_sse42 :
        public Test<DATAIN, typename HammingSSE42<DATAIN>::hamming_sse42_t>,
        public SSE42Test {

    typedef typename HammingSSE42<DATAIN>::hamming_sse42_t hamming_sse42_t;
    typedef typename HammingScalar<DATAIN>::hamming_scalar_t hamming_scalar_t;

    using Test<DATAIN, hamming_sse42_t>::Test;

    virtual ~Hamming_sse42() {
    }

    void RunEncode(
            const EncodeConfiguration & config) override {
        for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
            _ReadWriteBarrier();
            auto data = this->bufRaw.template begin<__m128i >();
            auto dataEnd = this->bufRaw.template end<__m128i >();
            auto dataOut = this->bufEncoded.template begin<hamming_sse42_t>();
            while (data <= (dataEnd - UNROLL)) {
                for (size_t k = 0; k < UNROLL; ++k, ++data, ++dataOut) {
                    dataOut->data = *data;
                    dataOut->code = HammingSSE42<DATAIN>::computeHamming(*data);
                }
            }
            for (; data <= (dataEnd - 1); ++data, ++dataEnd) {
                dataOut->data = *data;
                dataOut->code = HammingSSE42<DATAIN>::computeHamming(*data);
            }
            if (data < dataEnd) {
                auto data2 = reinterpret_cast<DATAIN*>(data);
                auto dataEnd2 = reinterpret_cast<DATAIN*>(dataEnd);
                auto dataOut2 = reinterpret_cast<hamming_scalar_t*>(dataOut);
                for (; data2 < dataEnd2; ++data2, ++dataOut2) {
                    dataOut2->data = *data2;
                    dataOut2->code = HammingScalar<DATAIN>::computeHamming(*data2);
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
            _ReadWriteBarrier();
            size_t numValues = this->bufRaw.template end<DATAIN>() - this->bufRaw.template begin<DATAIN>();
            size_t i = 0;
            auto data = this->bufEncoded.template begin<hamming_sse42_t>();
            while (i <= (numValues - UNROLL)) {
                for (size_t k = 0; k < UNROLL; ++k, i += (sizeof(__m128i) / sizeof (DATAIN)), ++data) {
                    if (data->code != HammingSSE42<DATAIN>::computeHamming(data->data)) {
                        throw ErrorInfo(__FILE__, __LINE__, data - this->bufEncoded.template begin<hamming_sse42_t>(), config.numIterations);
                    }
                }
            }
            for (; i <= (numValues - 1); i += (sizeof(__m128i) / sizeof (DATAIN)), ++data) {
                if (data->code != HammingSSE42<DATAIN>::computeHamming(data->data)) {
                    throw ErrorInfo(__FILE__, __LINE__, data - this->bufEncoded.template begin<hamming_sse42_t>(), config.numIterations);
                }
            }
            if (i < numValues) {
                for (auto data2 = reinterpret_cast<hamming_scalar_t*>(data); i < numValues; ++i, ++data2) {
                    if (data2->code != HammingScalar<DATAIN>::computeHamming(data2->data)) {
                        throw ErrorInfo(__FILE__, __LINE__, data2 - this->bufEncoded.template begin<hamming_scalar_t>(), config.numIterations);
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
            const size_t VALUES_PER_SIMDREG = sizeof(__m128i) / sizeof (DATAIN);
            const size_t VALUES_PER_UNROLL = UNROLL * VALUES_PER_SIMDREG;
            size_t numValues = this->bufRaw.template end<DATAIN>() - this->bufRaw.template begin<DATAIN>();
            size_t i = 0;
            auto data = this->bufEncoded.template begin<hamming_sse42_t>();
            auto dataOut = this->bufResult.template begin<__m128i >();
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
                auto data2 = reinterpret_cast<hamming_scalar_t*>(data);
                auto dataOut2 = reinterpret_cast<DATAIN*>(dataOut);
                for (; i < numValues; ++i, ++data2, ++dataOut2) {
                    *dataOut2 = data2->data;
                }
            }
        }
    }
};
