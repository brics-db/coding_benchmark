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

#include "../Test.hpp"
#include "../Util/Intrinsics.hpp"

struct hamming_seq_16_t {

    uint16_t data;
    uint8_t code;
};

struct hamming_seq_32_t {

    uint32_t data;
    uint8_t code;
};

template<typename OrigType>
struct TypeMapSeq;

template<>
struct TypeMapSeq<uint16_t> {

    typedef hamming_seq_16_t hamming_seq_t;

    static uint8_t
    computeHamming (uint16_t && data) {
        uint8_t hamming = 0;
        hamming |= (__builtin_popcount(data & 0xAD5B) & 0x1) << 1;
        hamming |= (__builtin_popcount(data & 0x366D) & 0x1) << 2;
        hamming |= (__builtin_popcount(data & 0xC78E) & 0x1) << 3;
        hamming |= (__builtin_popcount(data & 0x07F0) & 0x1) << 4;
        hamming |= (__builtin_popcount(data & 0xF800) & 0x1) << 5;
        hamming |= (__builtin_popcount(data) + __builtin_popcount(hamming)) & 0x1;
        return hamming;
    }
};

template<>
struct TypeMapSeq<uint32_t> {

    typedef hamming_seq_32_t hamming_seq_t;

    static uint8_t
    computeHamming (uint32_t && data) {
        uint8_t hamming = 0;
        hamming |= (__builtin_popcount(data & 0x56AAAD5B) & 0x1) << 1;
        hamming |= (__builtin_popcount(data & 0x9B33366D) & 0x1) << 2;
        hamming |= (__builtin_popcount(data & 0xE3C3C78E) & 0x1) << 3;
        hamming |= (__builtin_popcount(data & 0x03FC07F0) & 0x1) << 4;
        hamming |= (__builtin_popcount(data & 0x03FFF800) & 0x1) << 5;
        hamming |= (__builtin_popcount(data & 0xFC000000) & 0x1) << 6;
        hamming |= (__builtin_popcount(data) + __builtin_popcount(hamming)) & 0x1;
        return hamming;
    }
};

template<typename DATAIN, size_t UNROLL>
struct Hamming_seq : public Test<DATAIN, typename TypeMapSeq<DATAIN>::hamming_seq_t>, public SequentialTest {

    typedef typename TypeMapSeq<DATAIN>::hamming_seq_t hamming_seq_t;

    Hamming_seq (const char* const name, AlignedBlock & in, AlignedBlock & out) :
            Test<DATAIN, hamming_seq_t>(name, in, out) {
    }

    virtual
    ~Hamming_seq () {
    }

    void
    RunEnc (const size_t numIterations) override {
        for (size_t iteration = 0; iteration < numIterations; ++iteration) {
            auto data = this->in.template begin<DATAIN>();
            auto dataEnd = this->in.template end<DATAIN>();
            auto dataOut = this->out.template begin<hamming_seq_t>();
            while (data <= (dataEnd - UNROLL)) {
                for (size_t k = 0; k < UNROLL; ++k, ++data, ++dataOut) {
                    dataOut->data = *data;
                    dataOut->code = TypeMapSeq<DATAIN>::computeHamming(std::move(*data));
                }
            }
            for (; data < dataEnd; ++data, ++dataOut) {
                dataOut->data = *data;
                dataOut->code = TypeMapSeq<DATAIN>::computeHamming(std::move(*data));
            }
        }
    }

    bool
    DoCheck () override {
        return true;
    }

    void
    RunCheck (const size_t numIterations) override {
        for (size_t iteration = 0; iteration < numIterations; ++iteration) {
            size_t numValues = this->in.template end<DATAIN>() - this->in.template begin<DATAIN>();
            size_t i = 0;
            auto data = this->out.template begin<hamming_seq_t>();
            while (i <= (numValues - UNROLL)) {
                for (size_t k = 0; k < UNROLL; ++k, ++i, ++data) {
                    if (data->code != TypeMapSeq<DATAIN>::computeHamming(std::move(data->data))) {
                        throw ErrorInfo(data - this->out.template begin<hamming_seq_t>(), numIterations);
                    }
                }
            }
            for (; i < numValues; ++i, ++data) {
                if (data->code != TypeMapSeq<DATAIN>::computeHamming(std::move(data->data))) {
                    throw ErrorInfo(data - this->out.template begin<hamming_seq_t>(), numIterations);
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
            size_t numValues = this->in.template end<DATAIN>() - this->in.template begin<DATAIN>();
            size_t i = 0;
            auto data = this->out.template begin<hamming_seq_t>();
            auto dataOut = this->in.template begin<DATAIN>();
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
