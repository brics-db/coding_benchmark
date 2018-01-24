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
#include <Util/Functors.hpp>
#include <Util/Helpers.hpp>
#include <Util/ArithmeticSelector.hpp>
#include <Util/AggregateSelector.hpp>

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
                auto dataIn = this->bufRaw.template begin<DATARAW>();
                const auto dataInEnd = dataIn + numValues;
                auto dataOut = this->bufEncoded.template begin<DATAENC>();
                while (dataIn <= (dataInEnd - UNROLL)) { // let the compiler unroll the loop
                    for (size_t unroll = 0; unroll < UNROLL; ++unroll) {
                        *dataOut++ = static_cast<DATAENC>(static_cast<DATAENC>(*dataIn++) * this->A);
                    }
                }
                // remaining numbers
                while (dataIn < dataInEnd) {
                    *dataOut++ = static_cast<DATAENC>(static_cast<DATAENC>(*dataIn++) * this->A);
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
                auto dataIn = test.bufEncoded.template begin<DATAENC>();
                const auto dataInEnd = dataIn + numValues;
                auto dataOut = test.bufResult.template begin<DATAENC>();
                DATAENC operandEnc = config.operand * test.A;
                while (dataIn <= (dataInEnd - UNROLL)) { // let the compiler unroll the loop
                    for (size_t unroll = 0; unroll < UNROLL; ++unroll) {
                        *dataOut++ = functor(*dataIn++, operandEnc);
                    }
                }
                while (dataIn < dataInEnd) {
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
            template<typename Tout, typename Initializer, typename Kernel, typename Finalizer>
            void impl(
                    Initializer && funcInit,
                    Kernel && funcKernel,
                    Finalizer && funcFinal) {
                const size_t numValues = test.template getNumValues();
                auto dataIn = test.bufEncoded.template begin<DATAENC>();
                const auto dataInEnd = dataIn + numValues;
                auto dataOut = test.bufResult.template begin<Tout>();
                Tout value = funcInit();
                while (dataIn <= (dataInEnd - UNROLL)) {
                    for (size_t k = 0; k < UNROLL; ++k) {
                        value = funcKernel(value, *dataIn++);
                    }
                }
                while (dataIn < dataInEnd) {
                    value = funcKernel(value, *dataIn++);
                }
                *dataOut = funcFinal(value, numValues);
            }
            void operator()(
                    AggregateConfiguration::Sum) {
                impl<larger_t>([] {return (larger_t) 0;}, [] (larger_t sum, DATAENC dataIn) -> larger_t {return sum + dataIn;}, [] (larger_t sum, const size_t numValues) -> larger_t {return sum;});
            }
            void operator()(
                    AggregateConfiguration::Min) {
                impl<DATAENC>([] () -> DATAENC {return (DATAENC) std::numeric_limits<DATAENC>::max();}, [] (DATAENC minimum, DATAENC dataIn) -> DATAENC {return dataIn < minimum ? dataIn : minimum;},
                        [] (DATAENC minimum, const size_t numValues) -> DATAENC {return minimum;});
            }
            void operator()(
                    AggregateConfiguration::Max) {
                impl<DATAENC>([] () -> DATAENC {return (DATAENC) std::numeric_limits<DATAENC>::min();}, [] (DATAENC maximum, DATAENC dataIn) -> DATAENC {return dataIn > maximum ? dataIn : maximum;},
                        [] (DATAENC maximum, const size_t numValues) -> DATAENC {return maximum;});
            }
            void operator()(
                    AggregateConfiguration::Avg) {
                impl<larger_t>([] {return (larger_t) 0;}, [] (larger_t sum, DATAENC dataIn) -> larger_t {return sum + dataIn;},
                        [this] (larger_t sum, const size_t numValues) -> larger_t {return (sum / (numValues * test.A)) * test.A;});
            }
        };

        void RunAggregate(
                const AggregateConfiguration & config) override {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                _ReadWriteBarrier();
                std::visit(Aggregator(*this, config), config.mode);
            }
        }
    };

}
