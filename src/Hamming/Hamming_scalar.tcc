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
 * File:   HammingTest.hpp
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 30. November 2016, 13:12
 */

#pragma once

#include <cstdint>

#include <Test.hpp>
#include <Util/Intrinsics.hpp>
#include <Util/ErrorInfo.hpp>

struct hamming_scalar_16_t {

    uint16_t data;
    uint8_t code;
};

struct hamming_scalar_32_t {

    uint32_t data;
    uint8_t code;
};

template<typename OrigType>
struct HammingScalar;

template<>
struct HammingScalar<uint16_t> {

    typedef hamming_scalar_16_t hamming_scalar_t;

    static uint8_t computeHamming(
            uint16_t data);
};

template<>
struct HammingScalar<uint32_t> {

    typedef hamming_scalar_32_t hamming_scalar_t;

    static uint8_t computeHamming(
            uint32_t data);
};

template<typename DATAIN, size_t UNROLL>
struct Hamming_scalar :
        public Test<DATAIN, typename HammingScalar<DATAIN>::hamming_scalar_t>,
        public ScalarTest {

    typedef typename HammingScalar<DATAIN>::hamming_scalar_t hamming_scalar_t;

    using Test<DATAIN, hamming_scalar_t>::Test;

    virtual ~Hamming_scalar() {
    }

    void RunEncode(
            const EncodeConfiguration & config) override {
        for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
            _ReadWriteBarrier();
            auto data = this->bufRaw.template begin<DATAIN>();
            auto dataEnd = this->bufRaw.template end<DATAIN>();
            auto dataOut = this->bufEncoded.template begin<hamming_scalar_t>();
            while (data <= (dataEnd - UNROLL)) {
                for (size_t k = 0; k < UNROLL; ++k, ++data, ++dataOut) {
                    dataOut->data = *data;
                    dataOut->code = HammingScalar<DATAIN>::computeHamming(*data);
                }
            }
            for (; data < dataEnd; ++data, ++dataOut) {
                dataOut->data = *data;
                dataOut->code = HammingScalar<DATAIN>::computeHamming(*data);
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
            auto data = this->bufEncoded.template begin<hamming_scalar_t>();
            while (i <= (numValues - UNROLL)) {
                for (size_t k = 0; k < UNROLL; ++k, ++i, ++data) {
                    if (data->code != HammingScalar<DATAIN>::computeHamming(data->data)) {
                        throw ErrorInfo(__FILE__, __LINE__, data - this->bufEncoded.template begin<hamming_scalar_t>(), config.numIterations);
                    }
                }
            }
            for (; i < numValues; ++i, ++data) {
                if (data->code != HammingScalar<DATAIN>::computeHamming(data->data)) {
                    throw ErrorInfo(__FILE__, __LINE__, data - this->bufEncoded.template begin<hamming_scalar_t>(), config.numIterations);
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
            size_t numValues = this->bufRaw.template end<DATAIN>() - this->bufRaw.template begin<DATAIN>();
            size_t i = 0;
            auto data = this->bufEncoded.template begin<hamming_scalar_t>();
            auto dataOut = this->bufResult.template begin<DATAIN>();
            while (i <= (numValues - UNROLL)) {
                for (size_t k = 0; k < UNROLL; ++k, ++data, ++dataOut) {
                    *dataOut = data->data;
                }
                i += UNROLL;
            }
            for (; i < numValues; ++i, ++data, ++dataOut) {
                *dataOut = data->data;
            }
        }
    }
};
