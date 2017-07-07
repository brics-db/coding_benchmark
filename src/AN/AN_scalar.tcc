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
 * File:   AN_seq_16_32.hpp
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 13. Dezember 2016, 00:34
 */

#pragma once

#include <AN/ANTest.hpp>

template<typename DATARAW, typename DATAENC, size_t UNROLL>
struct AN_seq :
        public ANTest<DATARAW, DATAENC, UNROLL>,
        public SequentialTest {

    using ANTest<DATARAW, DATAENC, UNROLL>::ANTest;

    virtual ~AN_seq() {
    }

    void RunEnc(
            const EncodeConfiguration & config) override {
        for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
            _ReadWriteBarrier();
            const size_t numValues = this->in.template end<DATARAW>() - this->in.template begin<DATARAW>();
            size_t i = 0;
            auto dataIn = this->in.template begin<DATARAW>();
            auto dataOut = this->out.template begin<DATAENC>();
            for (; i <= (numValues - UNROLL); i += UNROLL) {
                // let the compiler unroll the loop
                for (size_t unroll = 0; unroll < UNROLL; ++unroll, ++dataOut, ++dataIn) {
                    *dataOut = static_cast<DATAENC>(static_cast<DATAENC>(*dataIn) * this->A);
                }
            }
            // remaining numbers
            for (; i < numValues; ++i, ++dataOut, ++dataIn) {
                *dataOut = static_cast<DATAENC>(static_cast<DATAENC>(*dataIn) * this->A);
            }
        }
    }

    bool DoDec() override {
        return true;
    }

    void RunDec(
            const DecodeConfiguration & config) override {
        for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
            _ReadWriteBarrier();
            const size_t numValues = this->in.template end<DATARAW>() - this->in.template begin<DATARAW>();
            size_t i = 0;
            auto dataIn = this->out.template begin<DATAENC>();
            auto dataOut = this->in.template begin<DATARAW>();
            for (; i <= (numValues - UNROLL); i += UNROLL) {
                // let the compiler unroll the loop
                for (size_t unroll = 0; unroll < UNROLL; ++unroll, ++dataOut, ++dataIn) {
                    *dataOut = static_cast<DATARAW>(*dataIn * this->A_INV);
                }
            }
            // remaining numbers
            for (; i < numValues; ++i, ++dataOut, ++dataIn) {
                *dataOut = static_cast<DATARAW>(*dataIn * this->A_INV);
            }
        }
    }
};
