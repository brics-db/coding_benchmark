// Copyright 2017 Till Kolditz, Stefan de Bruijn
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
 * File:   AN_seq_s_inv.hpp
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 05. July 2017, 23:45
 */

#pragma once

#include <AN/AN_seq.hpp>

template<typename DATARAW, typename DATAENC, size_t UNROLL>
struct AN_seq_s_inv :
        public AN_seq<DATARAW, DATAENC, UNROLL> {

    AN_seq_s_inv(
            const char* const name,
            AlignedBlock & in,
            AlignedBlock & out,
            DATAENC A,
            DATAENC AInv)
            : AN_seq<DATARAW, DATAENC, UNROLL>(name, in, out, A, AInv) {
    }

    virtual ~AN_seq_s_inv() {
    }

    virtual bool DoCheck() override {
        return true;
    }

    virtual void RunCheck(
            const size_t numIterations) {
        for (size_t iteration = 0; iteration < numIterations; ++iteration) {
            const size_t numValues = this->in.template end<DATARAW>() - this->in.template begin<DATARAW>();
            size_t i = 0;
            auto data = this->out.template begin<DATAENC>();
            DATAENC dMax = static_cast<DATAENC>(std::numeric_limits<DATARAW>::max());
            DATAENC dMin = static_cast<DATAENC>(std::numeric_limits<DATARAW>::min());
            while (i <= (numValues - UNROLL)) {
                // let the compiler unroll the loop
                for (size_t k = 0; k < UNROLL; ++k) {
                    DATAENC dec = static_cast<DATAENC>(*data * this->A_INV);
                    if (dec < dMin || dec > dMax) {
                        throw ErrorInfo(__FILE__, __LINE__, data - this->out.template begin<DATAENC>(), iteration);
                    }
                    ++data;
                }
                i += UNROLL;
            }
            // remaining numbers
            if (i < numValues) {
                do {
                    DATAENC dec = static_cast<DATAENC>(*data * this->A_INV);
                    if (dec < dMin || dec > dMax) {
                        throw ErrorInfo(__FILE__, __LINE__, data - this->out.template begin<DATAENC>(), numIterations);
                    }
                    ++data;
                    ++i;
                } while (i <= numValues);
            }
        }
    }

    virtual bool DoCheckDec() override {
        return true;
    }

    virtual void RunCheckDec(
            const size_t numIterations) override {
        for (size_t iteration = 0; iteration < numIterations; ++iteration) {
            const size_t numValues = this->in.template end<DATARAW>() - this->in.template begin<DATARAW>();
            size_t i = 0;
            auto dataIn = this->out.template begin<DATAENC>();
            auto dataOut = this->in.template begin<DATARAW>();
            DATAENC dMax = static_cast<DATAENC>(std::numeric_limits<DATARAW>::max());
            DATAENC dMin = static_cast<DATAENC>(std::numeric_limits<DATARAW>::min());
            for (; i <= (numValues - UNROLL); i += UNROLL) {
                // let the compiler unroll the loop
                for (size_t unroll = 0; unroll < UNROLL; ++unroll, ++dataOut, ++dataIn) {
                    DATAENC dec = static_cast<DATAENC>(*dataIn * this->A_INV);
                    if (dec < dMin || dec > dMax) {
                        throw ErrorInfo(__FILE__, __LINE__, dataIn - this->out.template begin<DATAENC>(), iteration);
                    } else {
                        *dataOut = static_cast<DATARAW>(dec);
                    }
                }
            }
            // remaining numbers
            for (; i < numValues; ++i, ++dataOut, ++dataIn) {
                DATAENC dec = static_cast<DATAENC>(*dataIn * this->A_INV);
                if (dec < dMin || dec > dMax) {
                    throw ErrorInfo(__FILE__, __LINE__, dataIn - this->out.template begin<DATAENC>(), iteration);
                } else {
                    *dataOut = static_cast<DATARAW>(dec);
                }
            }
        }
    }
};
