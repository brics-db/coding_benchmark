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
 * Hamming_simd.tcc
 *
 *  Created on: 13.12.2017
 *      Author: Till Kolditz - Till.Kolditz@gmail.com
 */

#pragma once

#ifndef HAMMING_SIMD
#error "Clients must not include this file directly, but file <Hamming/Hamming_simd.hpp>!"
#endif

#include <Util/Test.hpp>
#include <Util/ErrorInfo.hpp>
#include <Util/ArithmeticSelector.hpp>
#include <Util/AggregateSelector.hpp>
#include <Util/Helpers.hpp>
#include <Hamming/Hamming_scalar.hpp>
#ifdef __SSE4_2__
#include <SIMD/SSE.hpp>
#endif
#ifdef __AVX2__
#include <SIMD/AVX2.hpp>
#endif
#ifdef __AVX512F__
#include <SIMD/AVX512.hpp>
#endif

using namespace coding_benchmark::simd;

namespace coding_benchmark {

#ifdef __SSE4_2__
    extern template struct hamming_t<uint16_t, __m128i > ;
    extern template struct hamming_t<uint32_t, __m128i > ;
#endif
#ifdef __AVX2__
    extern template struct hamming_t<uint16_t, __m256i > ;
    extern template struct hamming_t<uint32_t, __m256i > ;
#endif
#ifdef __AVX512F__
    extern template struct hamming_t<uint16_t, __m512i > ;
    extern template struct hamming_t<uint32_t, __m512i > ;
#endif

    template<typename DATAIN, typename V, size_t UNROLL>
    struct Hamming_simd :
            public Test<DATAIN, hamming_t<DATAIN, V>>,
            public SIMDTest<V> {

        typedef hamming_t<DATAIN, V> hamming_simd_t;
        typedef hamming_t<DATAIN, DATAIN> hamming_scalar_t;

        using Test<DATAIN, hamming_simd_t>::Test;

        virtual ~Hamming_simd() {
        }

        void RunEncode(
                const EncodeConfiguration & config) override {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                _ReadWriteBarrier();
                auto inV = config.source.template begin<V>();
                const auto inVend = this->template ComputeEnd<DATAIN>(inV, config);
                auto outV = config.target.template begin<hamming_simd_t>();
                while (inV <= (inVend - UNROLL)) {
                    for (size_t k = 0; k < UNROLL; ++k, ++outV) {
                        outV->store(*inV++);
                    }
                }
                for (; inV <= (inVend - 1); ++outV) {
                    outV->store(*inV++);
                }
                if (inV < inVend) {
                    auto inS = reinterpret_cast<DATAIN*>(inV);
                    const auto inSend = reinterpret_cast<DATAIN* const >(inVend);
                    auto outS = reinterpret_cast<hamming_scalar_t*>(outV);
                    for (; inS < inSend; ++outS) {
                        outS->store(*inS++);
                    }
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
                auto inV = config.target.template begin<hamming_simd_t>();
                const auto inVend = this->template ComputeEnd<hamming_scalar_t, hamming_simd_t>(inV, config);
                while (inV <= (inVend - UNROLL)) {
                    for (size_t k = 0; k < UNROLL; ++k) {
                        if (inV->isValid()) {
                            ++inV;
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATAIN*>(inV) - config.target.template begin<DATAIN>(), iteration);
                        }
                    }
                }
                while (inV <= (inVend - 1)) {
                    if (inV->isValid()) {
                        ++inV;
                    } else {
                        throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATAIN*>(inV) - config.target.template begin<DATAIN>(), iteration);
                    }
                }
                if (inV < inVend) {
                    auto inS = reinterpret_cast<hamming_scalar_t*>(inV);
                    const auto inSend = reinterpret_cast<hamming_scalar_t* const >(inVend);
                    while (inS < inSend) {
                        if (inS->isValid()) {
                            ++inS;
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATAIN*>(inS) - config.target.template begin<DATAIN>(), iteration);
                        }
                    }
                }
            }
        }

        bool DoArithmetic(
                const ArithmeticConfiguration & config) override {
            return std::visit(ArithmeticSelector(), config.mode);
        }

        struct Arithmetor {
            using hamming_sse24_t = Hamming_simd::hamming_simd_t;
            using hamming_scalar_t = Hamming_simd::hamming_scalar_t;
            Hamming_simd & test;
            const ArithmeticConfiguration & config;
            const size_t iteration;
            Arithmetor(
                    Hamming_simd & test,
                    const ArithmeticConfiguration & config,
                    const size_t iteration)
                    : test(test),
                      config(config),
                      iteration(iteration) {
            }
            template<template<typename = void> class func>
            void impl() {
                func<> functor;
                auto mmOperand = mm<V, DATAIN>::set1(config.operand);
                auto inV = config.source.template begin<hamming_simd_t>();
                const auto inVend = test.template ComputeEnd<hamming_scalar_t, hamming_simd_t>(inV, config);
                auto outV = config.target.template begin<hamming_simd_t>();
                while (inV <= (inVend - UNROLL)) {
                    for (size_t k = 0; k < UNROLL; ++k, ++inV, ++outV) {
                        outV->store(mm_op<V, DATAIN, func>::compute(inV->data, mmOperand));
                    }
                }
                for (; inV < (inVend - 1); ++inV, ++outV) {
                    outV->store(mm_op<V, DATAIN, func>::compute(inV->data, mmOperand));
                }
                if (inV < inVend) {
                    auto inS = reinterpret_cast<hamming_scalar_t*>(inV);
                    const auto inSend = reinterpret_cast<hamming_scalar_t* const >(inVend);
                    auto outS = reinterpret_cast<hamming_scalar_t*>(outV);
                    for (; inS < inSend; ++outS, ++inS) {
                        outS->store(functor(inS->data, config.operand));
                    }
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
                std::visit(Arithmetor(*this, config, iteration), config.mode);
            }
        }

        bool DoArithmeticChecked(
                const ArithmeticConfiguration & config) override {
            return std::visit(ArithmeticSelector(), config.mode);
        }

        struct ArithmetorChecked {
            using hamming_sse24_t = Hamming_simd::hamming_simd_t;
            using hamming_scalar_t = Hamming_simd::hamming_scalar_t;
            Hamming_simd & test;
            const ArithmeticConfiguration & config;
            const size_t iteration;
            ArithmetorChecked(
                    Hamming_simd & test,
                    const ArithmeticConfiguration & config,
                    const size_t iteration)
                    : test(test),
                      config(config),
                      iteration(iteration) {
            }
            template<template<typename = void> class Functor>
            void impl() {
                auto mmOperand = mm<V, DATAIN>::set1(config.operand);
                auto inV = config.source.template begin<hamming_simd_t>();
                const auto inVend = test.template ComputeEnd<hamming_scalar_t, hamming_simd_t>(inV, config);
                auto outV = config.target.template begin<hamming_simd_t>();
                while (inV <= (inVend - UNROLL)) {
                    for (size_t k = 0; k < UNROLL; ++k, ++inV, ++outV) {
                        auto tmp = inV->data;
                        if (inV->isValid()) {
                            outV->store(mm_op<V, DATAIN, Functor>::compute(tmp, mmOperand));
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATAIN*>(inV) - config.source.template begin<DATAIN>(), iteration);
                        }
                    }
                }
                for (; inV < (inVend - 1); ++inV, ++outV) {
                    auto tmp = inV->data;
                    if (inV->isValid()) {
                        outV->store(mm_op<V, DATAIN, Functor>::compute(tmp, mmOperand));
                    } else {
                        throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATAIN*>(inV) - config.source.template begin<DATAIN>(), iteration);
                    }
                }
                if (inV < inVend) {
                    Functor<> functor;
                    auto inS = reinterpret_cast<hamming_scalar_t*>(inV);
                    const auto inSend = reinterpret_cast<hamming_scalar_t * const >(inVend);
                    auto outS = reinterpret_cast<hamming_scalar_t*>(outV);
                    for (; inS < inSend; ++outS, ++inS) {
                        auto tmp = inS->data;
                        if (inS->isValid()) {
                            outS->store(functor(tmp, config.operand));
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATAIN*>(inS) - config.source.template begin<DATAIN>(), iteration);
                        }
                    }
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
            using hamming_sse24_t = Hamming_simd::hamming_simd_t;
            using hamming_scalar_t = Hamming_simd::hamming_scalar_t;
            typedef typename Larger<DATAIN>::larger_t larger_t;
            Hamming_simd & test;
            const AggregateConfiguration & config;
            Aggregator(
                    Hamming_simd & test,
                    const AggregateConfiguration & config)
                    : test(test),
                      config(config) {
            }
            void operator()(
                    AggregateConfiguration::Sum) {
                auto mmSum = mm<V, larger_t>::set1(0);
                auto inV = config.source.template begin<hamming_simd_t>();
                const auto inVend = test.template ComputeEnd<hamming_scalar_t, hamming_simd_t>(inV, config);
                while (inV <= (inVend - UNROLL)) {
                    for (size_t k = 0; k < UNROLL; ++k, ++inV) {
                        mmSum = mm_op<V, larger_t, add>::compute(mmSum, mm<V, DATAIN>::cvt_larger_lo(inV->data));
                        mmSum = mm_op<V, larger_t, add>::compute(mmSum, mm<V, DATAIN>::cvt_larger_hi(inV->data));
                    }
                }
                for (; inV < (inVend - 1); ++inV) {
                    mmSum = mm_op<V, larger_t, add>::compute(mmSum, mm<V, DATAIN>::cvt_larger_lo(inV->data));
                    mmSum = mm_op<V, larger_t, add>::compute(mmSum, mm<V, DATAIN>::cvt_larger_hi(inV->data));
                }
                larger_t sum = mm<V, larger_t>::sum(mmSum);
                if (inV < inVend) {
                    auto inS = reinterpret_cast<hamming_scalar_t*>(inV);
                    const auto inSend = reinterpret_cast<hamming_scalar_t * const >(inVend);
                    for (; inS < inSend; ++inS) {
                        sum += inS->data;
                    }
                }
                auto dataOut = test.bufScratchPad.template begin<larger_t>();
                *dataOut = sum;
                EncodeConfiguration encConf(1, 2, test.bufScratchPad, config.target);
                test.RunEncode(encConf);
            }
            void operator()(
                    AggregateConfiguration::Min) {
                auto mmMin = mm<V, DATAIN>::set1(std::numeric_limits<DATAIN>::max());
                auto inV = config.source.template begin<hamming_simd_t>();
                const auto inVend = test.template ComputeEnd<hamming_scalar_t, hamming_simd_t>(inV, config);
                while (inV <= (inVend - UNROLL)) {
                    for (size_t k = 0; k < UNROLL; ++k, ++inV) {
                        mmMin = mm<V, DATAIN>::min(mmMin, inV->data);
                    }
                }
                for (; inV < (inVend - 1); ++inV) {
                    mmMin = mm<V, DATAIN>::min(mmMin, inV->data);
                }
                DATAIN min = mm<V, DATAIN>::min(mmMin);
                if (inV < inVend) {
                    auto inS = reinterpret_cast<hamming_scalar_t*>(inV);
                    const auto inSend = reinterpret_cast<hamming_scalar_t * const >(inVend);
                    for (; inS < inSend; ++inS) {
                        if (inS->data < min) {
                            min = inS->data;
                        }
                    }
                }
                auto dataOut = test.bufScratchPad.template begin<DATAIN>();
                *dataOut = min;
                EncodeConfiguration encConf(1, 2, test.bufScratchPad, config.target);
                test.RunEncode(encConf);
            }
            void operator()(
                    AggregateConfiguration::Max) {
                auto mmMax = mm<V, DATAIN>::set1(std::numeric_limits<DATAIN>::min());
                auto inV = config.source.template begin<hamming_simd_t>();
                const auto inVend = test.template ComputeEnd<hamming_scalar_t, hamming_simd_t>(inV, config);
                while (inV <= (inVend - UNROLL)) {
                    for (size_t k = 0; k < UNROLL; ++k, ++inV) {
                        mmMax = mm<V, DATAIN>::max(mmMax, inV->data);
                    }
                }
                for (; inV < (inVend - 1); ++inV) {
                    mmMax = mm<V, DATAIN>::max(mmMax, inV->data);
                }
                DATAIN max = mm<V, DATAIN>::max(mmMax);
                if (inV < inVend) {
                    auto inS = reinterpret_cast<hamming_scalar_t*>(inV);
                    const auto inSend = reinterpret_cast<hamming_scalar_t * const >(inVend);
                    for (; inS < inSend; ++inS) {
                        if (inS->data > max) {
                            max = inS->data;
                        }
                    }
                }
                auto dataOut = test.bufScratchPad.template begin<DATAIN>();
                *dataOut = max;
                EncodeConfiguration encConf(1, 2, test.bufScratchPad, config.target);
                test.RunEncode(encConf);
            }
            void operator()(
                    AggregateConfiguration::Avg) {
                auto mmSum = mm<V, larger_t>::set1(0);
                auto inV = config.source.template begin<hamming_simd_t>();
                const auto inVend = test.template ComputeEnd<hamming_scalar_t, hamming_simd_t>(inV, config);
                while (inV <= (inVend - UNROLL)) {
                    for (size_t k = 0; k < UNROLL; ++k, ++inV) {
                        mmSum = mm_op<V, larger_t, add>::compute(mmSum, mm<V, DATAIN>::cvt_larger_lo(inV->data));
                        mmSum = mm_op<V, larger_t, add>::compute(mmSum, mm<V, DATAIN>::cvt_larger_hi(inV->data));
                    }
                }
                for (; inV < (inVend - 1); ++inV) {
                    mmSum = mm_op<V, larger_t, add>::compute(mmSum, mm<V, DATAIN>::cvt_larger_lo(inV->data));
                    mmSum = mm_op<V, larger_t, add>::compute(mmSum, mm<V, DATAIN>::cvt_larger_hi(inV->data));
                }
                auto sum = mm<V, larger_t>::sum(mmSum);
                if (inV < inVend) {
                    auto inS = reinterpret_cast<hamming_scalar_t*>(inV);
                    const auto inSend = reinterpret_cast<hamming_scalar_t * const >(inVend);
                    for (; inS < inSend; ++inS) {
                        sum += inS->data;
                    }
                }
                auto dataOut = test.bufScratchPad.template begin<larger_t>();
                *dataOut = sum / config.numValues;
                EncodeConfiguration encConf(1, 2, test.bufScratchPad, config.target);
                test.RunEncode(encConf);
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
            using hamming_sse24_t = Hamming_simd::hamming_simd_t;
            using hamming_scalar_t = Hamming_simd::hamming_scalar_t;
            typedef typename Larger<DATAIN>::larger_t larger_t;
            typedef hamming_t<larger_t, larger_t> hamming_larger_t;
            Hamming_simd & test;
            const AggregateConfiguration & config;
            size_t iteration;
            AggregatorChecked(
                    Hamming_simd & test,
                    const AggregateConfiguration & config,
                    size_t iteration)
                    : test(test),
                      config(config),
                      iteration(iteration) {
            }
            void operator()(
                    AggregateConfiguration::Sum) {
                auto mmSum = mm<V, larger_t>::set1(0);
                auto inV = config.source.template begin<hamming_simd_t>();
                const auto inVend = test.template ComputeEnd<hamming_scalar_t, hamming_simd_t>(inV, config);
                while (inV <= (inVend - UNROLL)) {
                    for (size_t k = 0; k < UNROLL; ++k, ++inV) {
                        if (inV->isValid()) {
                            mmSum = mm_op<V, larger_t, add>::compute(mmSum, mm<V, DATAIN>::cvt_larger_lo(inV->data));
                            mmSum = mm_op<V, larger_t, add>::compute(mmSum, mm<V, DATAIN>::cvt_larger_hi(inV->data));
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATAIN*>(inV) - config.source.template begin<DATAIN>(), iteration);
                        }
                    }
                }
                for (; inV < (inVend - 1); ++inV) {
                    if (inV->isValid()) {
                        mmSum = mm_op<V, larger_t, add>::compute(mmSum, mm<V, DATAIN>::cvt_larger_lo(inV->data));
                        mmSum = mm_op<V, larger_t, add>::compute(mmSum, mm<V, DATAIN>::cvt_larger_hi(inV->data));
                    } else {
                        throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATAIN*>(inV) - config.source.template begin<DATAIN>(), iteration);
                    }
                }
                auto sum = mm<V, larger_t>::sum(mmSum);
                if (inV < inVend) {
                    auto inS = reinterpret_cast<hamming_scalar_t*>(inV);
                    const auto inSend = reinterpret_cast<hamming_scalar_t * const >(inVend);
                    for (; inS < inSend; ++inS) {
                        if (inS->isValid()) {
                            sum += inS->data;
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATAIN*>(inS) - config.source.template begin<DATAIN>(), iteration);
                        }
                    }
                }
                auto dataOut = test.bufScratchPad.template begin<larger_t>();
                *dataOut = sum;
                EncodeConfiguration encConf(1, 2, test.bufScratchPad, config.target);
                test.RunEncode(encConf);
            }
            void operator()(
                    AggregateConfiguration::Min) {
                auto mmMin = mm<V, DATAIN>::set1(std::numeric_limits<DATAIN>::max());
                auto inV = config.source.template begin<hamming_simd_t>();
                const auto inVend = test.template ComputeEnd<hamming_scalar_t, hamming_simd_t>(inV, config);
                while (inV <= (inVend - UNROLL)) {
                    for (size_t k = 0; k < UNROLL; ++k, ++inV) {
                        if (inV->isValid()) {
                            mmMin = mm<V, DATAIN>::min(mmMin, inV->data);
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATAIN*>(inV) - config.source.template begin<DATAIN>(), iteration);
                        }
                    }
                }
                for (; inV < (inVend - 1); ++inV) {
                    if (inV->isValid()) {
                        mmMin = mm<V, DATAIN>::min(mmMin, inV->data);
                    } else {
                        throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATAIN*>(inV) - config.source.template begin<DATAIN>(), iteration);
                    }
                }
                DATAIN min = mm<V, DATAIN>::min(mmMin);
                if (inV < inVend) {
                    auto inS = reinterpret_cast<hamming_scalar_t*>(inV);
                    const auto inSend = reinterpret_cast<hamming_scalar_t * const >(inVend);
                    for (; inS < inSend; ++inS) {
                        if (inS->isValid()) {
                            min = (inS->data < min) ? inS->data : min;
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATAIN*>(inS) - config.source.template begin<DATAIN>(), iteration);
                        }
                    }
                }
                auto dataOut = test.bufScratchPad.template begin<DATAIN>();
                *dataOut = min;
                EncodeConfiguration encConf(1, 2, test.bufScratchPad, config.target);
                test.RunEncode(encConf);
            }
            void operator()(
                    AggregateConfiguration::Max) {
                auto mmTmp = mm<V, DATAIN>::set1(std::numeric_limits<DATAIN>::min());
                auto inV = config.source.template begin<hamming_simd_t>();
                const auto inVend = test.template ComputeEnd<hamming_scalar_t, hamming_simd_t>(inV, config);
                while (inV <= (inVend - UNROLL)) {
                    for (size_t k = 0; k < UNROLL; ++k, ++inV) {
                        if (inV->isValid()) {
                            mmTmp = mm<V, DATAIN>::max(mmTmp, inV->data);
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATAIN*>(inV) - config.source.template begin<DATAIN>(), iteration);
                        }
                    }
                }
                for (; inV < (inVend - 1); ++inV) {
                    if (inV->isValid()) {
                        mmTmp = mm<V, DATAIN>::max(mmTmp, inV->data);
                    } else {
                        throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATAIN*>(inV) - config.source.template begin<DATAIN>(), iteration);
                    }
                }
                DATAIN max = mm<V, DATAIN>::max(mmTmp);
                if (inV < inVend) {
                    auto inS = reinterpret_cast<hamming_scalar_t*>(inV);
                    const auto inSend = reinterpret_cast<hamming_scalar_t * const >(inVend);
                    for (; inS < inSend; ++inS) {
                        if (inS->isValid()) {
                            max = (inS->data > max) ? inS->data : max;
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATAIN*>(inS) - config.source.template begin<DATAIN>(), iteration);
                        }
                    }
                }
                auto dataOut = test.bufScratchPad.template begin<DATAIN>();
                *dataOut = max;
                EncodeConfiguration encConf(1, 2, test.bufScratchPad, config.target);
                test.RunEncode(encConf);
            }
            void operator()(
                    AggregateConfiguration::Avg) {
                auto mmSum = mm<V, larger_t>::set1(0);
                auto inV = config.source.template begin<hamming_simd_t>();
                const auto inVend = test.template ComputeEnd<hamming_scalar_t, hamming_simd_t>(inV, config);
                while (inV <= (inVend - UNROLL)) {
                    for (size_t k = 0; k < UNROLL; ++k, ++inV) {
                        if (inV->isValid()) {
                            mmSum = mm_op<V, larger_t, add>::compute(mmSum, mm<V, DATAIN>::cvt_larger_lo(inV->data));
                            mmSum = mm_op<V, larger_t, add>::compute(mmSum, mm<V, DATAIN>::cvt_larger_hi(inV->data));
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATAIN*>(inV) - config.source.template begin<DATAIN>(), iteration);
                        }
                    }
                }
                for (; inV < (inVend - 1); ++inV) {
                    if (inV->isValid()) {
                        mmSum = mm_op<V, larger_t, add>::compute(mmSum, mm<V, DATAIN>::cvt_larger_lo(inV->data));
                        mmSum = mm_op<V, larger_t, add>::compute(mmSum, mm<V, DATAIN>::cvt_larger_hi(inV->data));
                    } else {
                        throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATAIN*>(inV) - config.source.template begin<DATAIN>(), iteration);
                    }
                }
                auto sum = mm<V, larger_t>::sum(mmSum);
                if (inV < inVend) {
                    auto inS = reinterpret_cast<hamming_scalar_t*>(inV);
                    const auto inSend = reinterpret_cast<hamming_scalar_t * const >(inVend);
                    for (; inS < inSend; ++inS) {
                        if (inS->isValid()) {
                            sum += inS->data;
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATAIN*>(inS) - config.source.template begin<DATAIN>(), iteration);
                        }
                    }
                }
                auto dataOut = test.bufScratchPad.template begin<larger_t>();
                *dataOut = sum / config.numValues;
                EncodeConfiguration encConf(1, 2, test.bufScratchPad, config.target);
                test.RunEncode(encConf);
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
                auto inV = config.source.template begin<hamming_simd_t>();
                const auto inVend = this->template ComputeEnd<hamming_scalar_t, hamming_simd_t>(inV, config);
                auto outV = config.target.template begin<V>();
                while (inV <= (inVend - UNROLL)) {
                    for (size_t k = 0; k < UNROLL; ++k, ++inV) {
                        *outV++ = inV->data;
                    }
                }
                for (; inV < (inVend - 1); ++inV) {
                    *outV++ = inV->data;
                }
                if (inV < inVend) {
                    auto inS = reinterpret_cast<hamming_scalar_t*>(inV);
                    const auto inSend = reinterpret_cast<hamming_scalar_t * const >(inVend);
                    auto outS = reinterpret_cast<DATAIN*>(outV);
                    for (; inS < inSend; ++inS) {
                        *outS++ = inS->data;
                    }
                }
            }
        }

        bool DoDecodeChecked() override {
            return true;
        }

        void RunDecodeChecked(
                const DecodeConfiguration & config) override {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                _ReadWriteBarrier();
                auto inV = config.source.template begin<hamming_simd_t>();
                const auto inVend = this->template ComputeEnd<hamming_scalar_t, hamming_simd_t>(inV, config);
                auto outV = config.target.template begin<V>();
                while (inV <= (inVend - UNROLL)) {
                    for (size_t k = 0; k < UNROLL; ++k, ++inV) {
                        if (inV->isValid()) {
                            *outV++ = inV->data;
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATAIN*>(inV) - config.source.template begin<DATAIN>(), iteration);
                        }
                    }
                }
                for (; inV < (inVend - 1); ++inV) {
                    if (inV->isValid()) {
                        *outV++ = inV->data;
                    } else {
                        throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATAIN*>(inV) - config.source.template begin<DATAIN>(), iteration);
                    }
                }
                if (inV < inVend) {
                    auto inS = reinterpret_cast<hamming_scalar_t*>(inV);
                    const auto inSend = reinterpret_cast<hamming_scalar_t * const >(inVend);
                    auto outS = reinterpret_cast<DATAIN*>(outV);
                    for (; inS < inSend; ++inS) {
                        if (inS->isValid()) {
                            *outS++ = inS->data;
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATAIN*>(inS) - config.source.template begin<DATAIN>(), iteration);
                        }
                    }
                }
            }
        }
    }
    ;

}
