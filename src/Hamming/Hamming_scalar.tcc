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

#include <Test.hpp>
#include <Hamming/Hamming_base.hpp>
#include <Util/ErrorInfo.hpp>
#include <Util/Functors.hpp>
#include <Util/ArithmeticSelector.hpp>
#include <Util/AggregateSelector.hpp>
#include <Util/Helpers.hpp>

namespace coding_benchmark {

    extern template struct hamming_t<uint16_t, uint16_t> ;
    extern template struct hamming_t<uint32_t, uint32_t> ;
    extern template struct hamming_t<uint64_t, uint64_t> ;

    template<typename DATAIN, size_t UNROLL>
    struct Hamming_scalar :
            public Test<DATAIN, hamming_t<DATAIN, DATAIN>>,
            public ScalarTest {

        typedef hamming_t<DATAIN, DATAIN> hamming_scalar_t;
        typedef typename hamming_scalar_t::code_t code_t;

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
                        dataOut->store(*data);
                    }
                }
                for (; data < dataEnd; ++data, ++dataOut) {
                    dataOut->store(*data);
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
                size_t numValues = this->getNumValues();
                size_t i = 0;
                auto data = this->bufEncoded.template begin<hamming_scalar_t>();
                while (i <= (numValues - UNROLL)) {
                    for (size_t k = 0; k < UNROLL; ++k, ++i, ++data) {
                        if (!data->isValid()) {
                            throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                        }
                    }
                }
                for (; i < numValues; ++i, ++data) {
                    if (!data->isValid()) {
                        throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                    }
                }
            }
        }

        bool DoArithmetic(
                const ArithmeticConfiguration & config) override {
            return std::visit(ArithmeticSelector(), config.mode);
        }

        struct Arithmetor {
            using hamming_scalar_t = Hamming_scalar::hamming_scalar_t;
            Hamming_scalar & test;
            const ArithmeticConfiguration & config;
            Arithmetor(
                    Hamming_scalar & test,
                    const ArithmeticConfiguration & config)
                    : test(test),
                      config(config) {
            }
            template<template<typename = void> class func>
            void impl() {
                func<> functor;
                const size_t numValues = test.getNumValues();
                auto data = test.bufEncoded.template begin<hamming_scalar_t>();
                auto dataEnd = data + numValues;
                auto dataOut = test.bufResult.template begin<hamming_scalar_t>();
                while (data <= (dataEnd - UNROLL)) {
                    for (size_t k = 0; k < UNROLL; ++k, ++data, ++dataOut) {
                        auto tmp = functor(data->data, config.operand);
                        dataOut->store(tmp);
                    }
                }
                for (; data < dataEnd; ++data, ++dataOut) {
                    auto tmp = functor(data->data, config.operand);
                    dataOut->store(tmp);
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

        bool DoArithmeticChecked(
                const ArithmeticConfiguration & config) override {
            return std::visit(ArithmeticSelector(), config.mode);
        }

        struct ArithmetorChecked {
            using hamming_scalar_t = Hamming_scalar::hamming_scalar_t;
            Hamming_scalar & test;
            const ArithmeticConfiguration & config;
            const size_t iteration;
            ArithmetorChecked(
                    Hamming_scalar & test,
                    const ArithmeticConfiguration & config,
                    const size_t iteration)
                    : test(test),
                      config(config),
                      iteration(iteration) {
            }
            template<template<typename = void> class func>
            void impl() {
                func<> functor;
                const size_t numValues = test.getNumValues();
                auto data = test.bufEncoded.template begin<hamming_scalar_t>();
                auto dataEnd = data + numValues;
                auto dataOut = test.bufResult.template begin<hamming_scalar_t>();
                size_t i = 0;
                while (data <= (dataEnd - UNROLL)) {
                    for (size_t k = 0; k < UNROLL; ++k, ++data, ++dataOut, ++i) {
                        auto tmp = data->data;
                        if (!data->isValid()) {
                            throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                        }
                        dataOut->store(functor(tmp, config.operand));
                    }
                }
                for (; data < dataEnd; ++data, ++dataOut) {
                    auto tmp = data->data;
                    if (!data->isValid()) {
                        throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                    }
                    dataOut->store(functor(tmp, config.operand));
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

        void RunArithmeticChecked(
                const ArithmeticConfiguration & config) override {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                _ReadWriteBarrier();
                std::visit(ArithmetorChecked(*this, config, iteration), config.mode);
            }
        }

        bool DoAggregate(
                const AggregateConfiguration & config) override {
            return std::visit(AggregateSelector(), config.mode);
        }

        struct Aggregator {
            using hamming_scalar_t = Hamming_scalar::hamming_scalar_t;
            typedef typename Larger<DATAIN>::larger_t larger_t;
            typedef hamming_t<larger_t, larger_t> hamming_larger_t;
            Hamming_scalar & test;
            const AggregateConfiguration & config;
            Aggregator(
                    Hamming_scalar & test,
                    const AggregateConfiguration & config)
                    : test(test),
                      config(config) {
            }
            void operator()(
                    AggregateConfiguration::Sum) {
                const size_t numValues = test.getNumValues();
                auto data = test.bufEncoded.template begin<hamming_scalar_t>();
                auto dataEnd = data + numValues;
                auto dataOut = test.bufResult.template begin<hamming_larger_t>();
                larger_t tmp(0);
                while (data <= (dataEnd - UNROLL)) {
                    for (size_t k = 0; k < UNROLL; ++k, ++data) {
                        tmp += data->data;
                    }
                }
                for (; data < dataEnd; ++data) {
                    tmp += data->data;
                }
                dataOut->store(tmp);
            }
            void operator()(
                    AggregateConfiguration::Min) {
            }
            void operator()(
                    AggregateConfiguration::Max) {
            }
            void operator()(
                    AggregateConfiguration::Avg) {
            }
        };

        void RunAggregate(
                const AggregateConfiguration & config) override {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                _ReadWriteBarrier();
                std::visit(Aggregator(*this, config), config.mode);
            }
        }

        bool DoAggregateChecked(
                const AggregateConfiguration & config) override {
            return std::visit(AggregateSelector(), config.mode);
        }

        struct AggregatorChecked {
            using hamming_scalar_t = Hamming_scalar::hamming_scalar_t;
            typedef typename Larger<DATAIN>::larger_t larger_t;
            typedef hamming_t<larger_t, larger_t> hamming_larger_t;
            Hamming_scalar & test;
            const AggregateConfiguration & config;
            const size_t iteration;
            AggregatorChecked(
                    Hamming_scalar & test,
                    const AggregateConfiguration & config,
                    const size_t iteration)
                    : test(test),
                      config(config),
                      iteration(iteration) {
            }
            void operator()(
                    AggregateConfiguration::Sum) {
                const size_t numValues = test.getNumValues();
                auto data = test.bufEncoded.template begin<hamming_scalar_t>();
                auto dataEnd = data + numValues;
                auto dataOut = test.bufResult.template begin<hamming_larger_t>();
                larger_t tmp(0);
                size_t i = 0;
                while (data <= (dataEnd - UNROLL)) {
                    for (size_t k = 0; k < UNROLL; ++k, ++data, ++i) {
                        if (!data->isValid()) {
                            throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                        }
                        tmp += data->data;
                    }
                }
                for (; data < dataEnd; ++data, ++i) {
                    if (!data->isValid()) {
                        throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                    }
                    tmp += data->data;
                }
                dataOut->store(tmp);
            }
            void operator()(
                    AggregateConfiguration::Min) {
            }
            void operator()(
                    AggregateConfiguration::Max) {
            }
            void operator()(
                    AggregateConfiguration::Avg) {
            }
        };

        void RunAggregateChecked(
                const AggregateConfiguration & config) override {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                _ReadWriteBarrier();
                std::visit(AggregatorChecked(*this, config, iteration), config.mode);
            }
        }

        bool DoDecode() override {
            return true;
        }

        void RunDecode(
                const DecodeConfiguration & config) override {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                _ReadWriteBarrier();
                size_t numValues = this->getNumValues();
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

}
