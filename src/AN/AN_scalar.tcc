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
#include <Util/Euclidean.hpp>
#include <Util/ArithmeticSelector.hpp>

namespace coding_benchmark {

    template<typename DATARAW, typename DATAENC, size_t UNROLL>
    struct AN_scalar :
            public ANTest<DATARAW, DATAENC, UNROLL>,
            public ScalarTest {

        using ANTest<DATARAW, DATAENC, UNROLL>::ANTest;

        virtual ~AN_scalar() {
        }

        void RunEncode(
                const EncodeConfiguration & config) override {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                _ReadWriteBarrier();
                const size_t numValues = this->getNumValues();
                size_t i = 0;
                auto dataIn = this->bufRaw.template begin<DATARAW>();
                auto dataOut = this->bufEncoded.template begin<DATAENC>();
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

        bool DoArithmetic(
                const ArithmeticConfiguration & config) override {
            return std::visit(ArithmeticSelector(), config.mode);
        }

        struct Arithmetor {
            AN_scalar & test;
            const ArithmeticConfiguration & config;
            Arithmetor(
                    AN_scalar & test,
                    const ArithmeticConfiguration & config)
                    : test(test),
                      config(config) {
            }
            void operator()(
                    ArithmeticConfiguration::Add) {
                const size_t numValues = test.template getNumValues();
                size_t i = 0;
                auto dataIn = test.bufEncoded.template begin<DATAENC>();
                auto dataOut = test.bufResult.template begin<DATAENC>();
                DATAENC operandEnc = config.operand * test.A;
                while (i <= (numValues - UNROLL)) {
                    // let the compiler unroll the loop
                    for (size_t k = 0; k < UNROLL; ++k) {
                        *dataOut++ = *dataIn++ + operandEnc;
                    }
                    i += UNROLL;
                }
                // remaining numbers
                for (; i < numValues; ++i) {
                    *dataOut++ = *dataIn++ + operandEnc;
                }
            }
            void operator()(
                    ArithmeticConfiguration::Sub) {
            }
            void operator()(
                    ArithmeticConfiguration::Mul) {
            }
            void operator()(
                    ArithmeticConfiguration::Div) {
            }
        };

        void RunArithmetic(
                const ArithmeticConfiguration & config) override {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                _ReadWriteBarrier();
                std::visit(Arithmetor(*this, config), config.mode);
            }
        }

        bool DoDecode() override {
            return true;
        }

        void RunDecode(
                const DecodeConfiguration & config) override {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                _ReadWriteBarrier();
                const size_t numValues = this->getNumValues();
                size_t i = 0;
                auto dataIn = this->bufEncoded.template begin<DATAENC>();
                auto dataOut = this->bufResult.template begin<DATARAW>();
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

}
