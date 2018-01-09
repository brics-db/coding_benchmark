// Copyright 2017 Till Kolditz
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
 * AN_scalar_inv.tcc
 *
 *  Created on: 09.01.2018
 *      Author: Till Kolditz - Till.Kolditz@gmail.com
 */

#pragma once

#ifndef AN_SCALAR
#error "Clients must not include this file directly, but file <AN/AN_scalar.hpp>!"
#endif

#include <AN/AN_scalar.tcc>
#include <Util/ArithmeticSelector.hpp>

namespace coding_benchmark {

    template<typename DATARAW, typename DATAENC, size_t UNROLL>
    struct AN_scalar_inv :
            public AN_scalar<DATARAW, DATAENC, UNROLL> {

        typedef AN_scalar<DATARAW, DATAENC, UNROLL> BASE;

        using BASE::AN_scalar;

        bool DoDecode() override {
            return true;
        }

        void RunDecode(
                const DecodeConfiguration & config) override {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                _ReadWriteBarrier();
                const size_t numValues = this->getNumValues();
                auto dataIn = this->bufEncoded.template begin<DATAENC>();
                const auto dataInEnd = dataIn + numValues;
                auto dataOut = this->bufResult.template begin<DATARAW>();
                while (dataIn <= (dataInEnd - UNROLL)) {
                    for (size_t k = 0; k < UNROLL; ++k) {
                        *dataOut++ = static_cast<DATARAW>(*dataIn++ * this->A_INV);
                    }
                }
                // remaining numbers
                while (dataIn < dataInEnd) {
                    *dataOut++ = static_cast<DATARAW>(*dataIn++ * this->A_INV);
                }
            }
        }
    };

}
