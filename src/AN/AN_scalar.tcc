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
#include <Util/Functors.hpp>
#include <Util/ArithmeticSelector.hpp>
#include <Util/AggregateSelector.hpp>
#include <Util/Helpers.hpp>

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
            template<template<typename = void> class func>
            void impl() {
                func<> functor;
                const size_t numValues = test.template getNumValues();
                size_t i = 0;
                auto dataIn = test.bufEncoded.template begin<DATAENC>();
                auto dataOut = test.bufResult.template begin<DATAENC>();
                DATAENC operandEnc = config.operand * test.A;
                for (; i <= (numValues - UNROLL); i += UNROLL) {
                    for (size_t k = 0; k < UNROLL; ++k) {
                        *dataOut++ = functor(*dataIn++, operandEnc);
                    }
                }
                for (; i < numValues; ++i) {
                    *dataOut++ = functor(*dataIn++, operandEnc);
                }
            }
            void operator()(
                    ArithmeticConfiguration::Add) {
                impl<add>();
            }
            void operator()(
                    ArithmeticConfiguration::Sub) {
                impl<sub>();
            }
            void operator()(
                    ArithmeticConfiguration::Mul) {
                impl<mul>();
            }
            void operator()(
                    ArithmeticConfiguration::Div) {
                impl<div>();
            }
        };

        void RunArithmetic(
                const ArithmeticConfiguration & config) override {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                _ReadWriteBarrier();
                std::visit(Arithmetor(*this, config), config.mode);
            }
        }

        bool DoAggregate(
                const AggregateConfiguration & config) override {
            return std::visit(AggregateSelector(), config.mode);
        }

        struct Aggregator {
            typedef typename Larger<DATAENC>::larger_t larger_t;
            AN_scalar & test;
            const AggregateConfiguration & config;
            Aggregator(
                    AN_scalar & test,
                    const AggregateConfiguration & config)
                    : test(test),
                      config(config) {
            }
            void operator()(
                    AggregateConfiguration::Sum) {
                const size_t numValues = test.template getNumValues();
                auto dataIn = test.bufEncoded.template begin<DATAENC>();
                auto dataInEnd = dataIn + numValues;
                auto dataOut = test.bufResult.template begin<larger_t>();
                // DATAENC operandEnc = config.operand * test.A;
                larger_t tmp(0);
                while (dataIn <= (dataInEnd - UNROLL)) {
                    for (size_t k = 0; k < UNROLL; ++k) {
                        tmp += *dataIn++;
                    }
                }
                while (dataIn < dataInEnd) {
                    tmp += *dataIn++;
                }
                *dataOut = tmp;
            }
            void operator()(
                    AggregateConfiguration::Min) {
                const size_t numValues = test.template getNumValues();
                auto dataIn = test.bufEncoded.template begin<DATAENC>();
                auto dataInEnd = dataIn + numValues;
                auto dataOut = test.bufResult.template begin<DATAENC>();
                // DATAENC operandEnc = config.operand * test.A;
                DATAENC minimum = std::numeric_limits<DATAENC>::max();
                while (dataIn <= (dataInEnd - UNROLL)) {
                    for (size_t k = 0; k < UNROLL; ++k) {
                        auto tmp = *dataIn++;
                        minimum = (tmp < minimum) ? tmp : minimum;
                    }
                }
                while (dataIn < dataInEnd) {
                    auto tmp = *dataIn++;
                    minimum = (tmp < minimum) ? tmp : minimum;
                }
                *dataOut = minimum;
            }
            void operator()(
                    AggregateConfiguration::Max) {
                const size_t numValues = test.template getNumValues();
                auto dataIn = test.bufEncoded.template begin<DATAENC>();
                auto dataInEnd = dataIn + numValues;
                auto dataOut = test.bufResult.template begin<DATAENC>();
                // DATAENC operandEnc = config.operand * test.A;
                DATAENC maximum = std::numeric_limits<DATAENC>::min();
                while (dataIn <= (dataInEnd - UNROLL)) {
                    for (size_t k = 0; k < UNROLL; ++k) {
                        auto tmp = *dataIn++;
                        maximum = (tmp > maximum) ? tmp : maximum;
                    }
                }
                while (dataIn < dataInEnd) {
                    auto tmp = *dataIn++;
                    maximum = (tmp > maximum) ? tmp : maximum;
                }
                *dataOut = maximum;
            }
            void operator()(
                    AggregateConfiguration::Avg) {
                const size_t numValues = test.template getNumValues();
                auto dataIn = test.bufEncoded.template begin<DATAENC>();
                auto dataInEnd = dataIn + numValues;
                auto dataOut = test.bufResult.template begin<larger_t>();
                larger_t tmp(0);
                while (dataIn <= (dataInEnd - UNROLL)) {
                    for (size_t k = 0; k < UNROLL; ++k) {
                        tmp += *dataIn++;
                    }
                }
                while (dataIn < dataInEnd) {
                    tmp += *dataIn++;
                }
                *dataOut = (tmp / (numValues * test.A)) * test.A; // TODO for division, decode all encoded values and encode afterwards again!
            }
        };

        void RunAggregate(
                const AggregateConfiguration & config) override {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                _ReadWriteBarrier();
                std::visit(Aggregator(*this, config), config.mode);
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
