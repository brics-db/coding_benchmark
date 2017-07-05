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

#pragma once

#include <AN/AN_seq.hpp>

template<size_t UNROLL>
struct AN_seq_16_32_s_inv : public AN_seq<int16_t, int32_t, UNROLL> {

    AN_seq_16_32_s_inv (const char* const name, AlignedBlock & in, AlignedBlock & out, int32_t A, int32_t AInv) :
            AN_seq<int16_t, int32_t, UNROLL>(name, in, out, A, AInv) {
    }

    virtual
    ~AN_seq_16_32_s_inv () {
    }

    virtual bool
    DoCheck () override {
        return true;
    }

    virtual void
    RunCheck (const size_t numIterations) {
        for (size_t iteration = 0; iteration < numIterations; ++iteration) {
            const size_t numValues = this->in.template end<int16_t>() - this->in.template begin<int16_t>();
            size_t i = 0;
            auto data = this->out.template begin<int32_t>();
            int32_t dMax = std::numeric_limits<int16_t>::max();
            int32_t dMin = std::numeric_limits<int16_t>::min();
            while (i <= (numValues - UNROLL)) {
                // let the compiler unroll the loop
                for (size_t k = 0; k < UNROLL; ++k) {
                    int32_t d = (*data * this->A_INV);
                    if (d > dMax || d < dMin) {
                        throw ErrorInfo(__FILE__, __LINE__, data - this->out.template begin<int32_t>(), iteration);
                    }
                    ++data;
                }
                i += UNROLL;
            }
            // remaining numbers
            if (i < numValues) {
                do {
                    int32_t d = (*data * this->A_INV);
                    if (d > dMax || d < dMin) {
                        throw ErrorInfo(__FILE__, __LINE__, data - this->out.template begin<int32_t>(), numIterations);
                    }
                    ++data;
                    ++i;
                } while (i <= numValues);
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
            size_t numValues = this->in.template end<int16_t>() - this->in.template begin<int16_t>();
            size_t i = 0;
            auto dataIn = this->out.template begin<int32_t>();
            auto dataOut = this->in.template begin<int16_t>();
            for (; i <= (numValues - UNROLL); i += UNROLL) {
                // let the compiler unroll the loop
                for (size_t unroll = 0; unroll < UNROLL; ++unroll, ++dataOut, ++dataIn) {
                    *dataOut = *dataIn * this->A_INV;
                }
            }
            // remaining numbers
            for (; i < numValues; ++i, ++dataOut, ++dataIn) {
                *dataOut = *dataIn * this->A_INV;
            }
        }
    }
};
