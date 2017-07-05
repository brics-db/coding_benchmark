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

#include "ANTest.hpp"

template<typename DATAIN, typename DATAOUT, size_t UNROLL>
struct AN_seq : public ANTest<DATAIN, DATAOUT, UNROLL>, public SequentialTest {

    AN_seq (const char* const name, AlignedBlock & in, AlignedBlock & out, DATAOUT A, DATAOUT AInv) :
            ANTest<DATAIN, DATAOUT, UNROLL>(name, in, out, A, AInv) {
    }

    virtual
    ~AN_seq () {
    }

    void
    RunEnc (const size_t numIterations) override {
        for (size_t iteration = 0; iteration < numIterations; ++iteration) {
            auto dataIn = this->in.template begin<DATAIN>();
            auto dataInEnd = this->in.template end<DATAIN>();
            auto dataOut = this->out.template begin<DATAOUT>();
            while (dataIn <= (dataInEnd - UNROLL)) {
                // let the compiler unroll the loop
                for (size_t unroll = 0; unroll < UNROLL; ++unroll) {
                    *dataOut++ = *dataIn++ * this->A;
                }
            }
            // remaining numbers
            while (dataIn < dataInEnd) {
                *dataOut++ = *dataIn++ * this->A;
            }
        }
    }
};
