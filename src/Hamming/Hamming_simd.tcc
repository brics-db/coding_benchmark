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

        static const constexpr size_t VALUES_PER_VECTOR = sizeof(V) / sizeof(DATAIN);
        static const constexpr size_t VALUES_PER_UNROLL = UNROLL * VALUES_PER_VECTOR;

        using Test<DATAIN, hamming_simd_t>::Test;

        virtual ~Hamming_simd() {
        }

        void RunEncode(
                const EncodeConfiguration & config) override {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                _ReadWriteBarrier();
                auto data = this->bufRaw.template begin<V>();
                auto dataEnd = this->bufRaw.template end<V>();
                auto dataOut = this->bufEncoded.template begin<hamming_simd_t>();
                while (data <= (dataEnd - UNROLL)) {
                    for (size_t k = 0; k < UNROLL; ++k, ++data, ++dataOut) {
                        dataOut->store(*data);
                    }
                }
                for (; data <= (dataEnd - 1); ++data, ++dataOut) {
                    dataOut->store(*data);
                }
                if (data < dataEnd) {
                    auto data2 = reinterpret_cast<DATAIN*>(data);
                    auto dataEnd2 = reinterpret_cast<DATAIN*>(dataEnd);
                    auto dataOut2 = reinterpret_cast<hamming_scalar_t*>(dataOut);
                    for (; data2 < dataEnd2; ++data2, ++dataOut2) {
                        dataOut2->store(*data2);
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
                size_t numValues = this->getNumValues();
                size_t i = 0;
                auto data = this->bufEncoded.template begin<hamming_simd_t>();
                while (i <= (numValues - VALUES_PER_UNROLL)) {
                    for (size_t k = 0; k < UNROLL; ++k, i += VALUES_PER_VECTOR, ++data) {
                        if (!data->isValid()) {
                            throw ErrorInfo(__FILE__, __LINE__, i + k, iteration);
                        }
                    }
                }
                for (; i <= (numValues - VALUES_PER_VECTOR); i += VALUES_PER_VECTOR, ++data) {
                    if (!data->isValid()) {
                        throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                    }
                }
                if (i < numValues) {
                    for (auto data2 = reinterpret_cast<hamming_scalar_t*>(data); i < numValues; ++i, ++data2) {
                        if (!data2->isValid()) {
                            throw ErrorInfo(__FILE__, __LINE__, i, iteration);
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
                const size_t numValues = test.getNumValues();
                auto mmOperand = mm<V, DATAIN>::set1(config.operand);
                size_t i = 0;
                auto dataIn = test.bufEncoded.template begin<hamming_simd_t>();
                auto dataOut = test.bufResult.template begin<hamming_simd_t>();
                while (i <= (numValues - VALUES_PER_UNROLL)) {
                    for (size_t k = 0; k < UNROLL; ++k, i += VALUES_PER_VECTOR, ++dataIn, ++dataOut) {
                        dataOut->store(mm_op<V, DATAIN, func>::compute(dataIn->data, mmOperand));
                    }
                }
                for (; i <= (numValues - VALUES_PER_VECTOR); i += VALUES_PER_VECTOR, ++dataIn, ++dataOut) {
                    dataOut->store(mm_op<V, DATAIN, func>::compute(dataIn->data, mmOperand));
                }
                if (i < numValues) {
                    auto dataIn2 = reinterpret_cast<hamming_scalar_t*>(dataIn);
                    auto dataOut2 = reinterpret_cast<hamming_scalar_t*>(dataOut);
                    for (; i < numValues; ++i, ++dataIn2, ++dataOut2) {
                        dataOut2->store(functor(dataIn2->data, config.operand));
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
            template<template<typename = void> class func>
            void impl() {
                func<> functor;
                const size_t numValues = test.getNumValues();
                auto mmOperand = mm<V, DATAIN>::set1(config.operand);
                size_t i = 0;
                auto dataIn = test.bufEncoded.template begin<hamming_simd_t>();
                auto dataOut = test.bufResult.template begin<hamming_simd_t>();
                while (i <= (numValues - VALUES_PER_UNROLL)) {
                    for (size_t k = 0; k < UNROLL; ++k, i += VALUES_PER_VECTOR, ++dataIn, ++dataOut) {
                        auto tmp = dataIn->data;
                        if (dataIn->isValid()) {
                            dataOut->store(mm_op<V, DATAIN, func>::compute(tmp, mmOperand));
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, i + k, iteration);
                        }
                    }
                }
                for (; i <= (numValues - VALUES_PER_VECTOR); i += VALUES_PER_VECTOR, ++dataIn, ++dataOut) {
                    auto tmp = dataIn->data;
                    if (dataIn->isValid()) {
                        dataOut->store(mm_op<V, DATAIN, func>::compute(tmp, mmOperand));
                    } else {
                        throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                    }
                }
                if (i < numValues) {
                    auto dataIn2 = reinterpret_cast<hamming_scalar_t*>(dataIn);
                    auto dataOut2 = reinterpret_cast<hamming_scalar_t*>(dataOut);
                    for (; i < numValues; ++i, ++dataIn2, ++dataOut2) {
                        auto tmp = dataIn2->data;
                        if (dataIn2->isValid()) {
                            dataOut2->store(functor(tmp, config.operand));
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, i, iteration);
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
            typedef hamming_t<larger_t, larger_t> hamming_larger_t;
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
                const size_t numValues = test.getNumValues();
                size_t i = 0;
                auto dataIn = test.bufEncoded.template begin<hamming_simd_t>();
                auto dataOut = test.bufResult.template begin<hamming_larger_t>();
                auto mmTmp = mm<V, larger_t>::set1(0);
                while (i <= (numValues - VALUES_PER_UNROLL)) {
                    for (size_t k = 0; k < UNROLL; ++k, i += VALUES_PER_VECTOR, ++dataIn) {
                        mmTmp = mm_op<V, larger_t, add>::compute(mmTmp, mm<V, DATAIN>::cvt_larger_lo(dataIn->data));
                        mmTmp = mm_op<V, larger_t, add>::compute(mmTmp, mm<V, DATAIN>::cvt_larger_hi(dataIn->data));
                    }
                }
                for (; i <= (numValues - VALUES_PER_VECTOR); i += VALUES_PER_VECTOR, ++dataIn) {
                    mmTmp = mm_op<V, larger_t, add>::compute(mmTmp, mm<V, DATAIN>::cvt_larger_lo(dataIn->data));
                    mmTmp = mm_op<V, larger_t, add>::compute(mmTmp, mm<V, DATAIN>::cvt_larger_hi(dataIn->data));
                }
                auto tmp = mm<V, larger_t>::sum(mmTmp);
                if (i < numValues) {
                    auto dataIn2 = reinterpret_cast<hamming_scalar_t*>(dataIn);
                    for (; i < numValues; ++i, ++dataIn2) {
                        tmp += dataIn2->data;
                    }
                }
                dataOut->store(tmp);
            }
            void operator()(
                    AggregateConfiguration::Min) {
                const size_t numValues = test.getNumValues();
                size_t i = 0;
                auto dataIn = test.bufEncoded.template begin<hamming_simd_t>();
                auto dataOut = test.bufResult.template begin<hamming_scalar_t>();
                auto mmTmp1 = mm<V, DATAIN>::set1(std::numeric_limits<DATAIN>::max());
                auto mmTmp2 = mm<V, DATAIN>::set1(std::numeric_limits<DATAIN>::max());
                while (i <= (numValues - VALUES_PER_UNROLL)) {
                    for (size_t k = 0; k < UNROLL; ++k, i += VALUES_PER_VECTOR, ++dataIn) {
                        mmTmp1 = mm<V, DATAIN>::min(mmTmp1, dataIn->data);
                        mmTmp2 = mm<V, DATAIN>::min(mmTmp2, dataIn->data);
                    }
                }
                for (; i <= (numValues - VALUES_PER_VECTOR); i += VALUES_PER_VECTOR, ++dataIn) {
                    mmTmp1 = mm<V, DATAIN>::min(mmTmp1, dataIn->data);
                    mmTmp2 = mm<V, DATAIN>::min(mmTmp2, dataIn->data);
                }
                auto tmp128 = mm<V, DATAIN>::min(mmTmp1, mmTmp2);
                DATAIN * pTmp = reinterpret_cast<DATAIN*>(&tmp128);
                DATAIN minimum = pTmp[0];
                for (size_t i = 1; i < VALUES_PER_VECTOR; ++i) {
                    minimum = (pTmp[i] < minimum) ? pTmp[i] : minimum;
                }
                if (i < numValues) {
                    auto dataIn2 = reinterpret_cast<hamming_scalar_t*>(dataIn);
                    for (; i < numValues; ++i, ++dataIn2) {
                        minimum = (dataIn2->data < minimum) ? dataIn2->data : minimum;
                    }
                }
                dataOut->store(minimum);
            }
            void operator()(
                    AggregateConfiguration::Max) {
                const size_t numValues = test.getNumValues();
                size_t i = 0;
                auto dataIn = test.bufEncoded.template begin<hamming_simd_t>();
                auto dataOut = test.bufResult.template begin<hamming_scalar_t>();
                auto mmTmp1 = mm<V, DATAIN>::set1(std::numeric_limits<DATAIN>::min());
                auto mmTmp2 = mm<V, DATAIN>::set1(std::numeric_limits<DATAIN>::min());
                while (i <= (numValues - VALUES_PER_UNROLL)) {
                    for (size_t k = 0; k < UNROLL; ++k, i += VALUES_PER_VECTOR, ++dataIn) {
                        mmTmp1 = mm<V, DATAIN>::max(mmTmp1, dataIn->data);
                        mmTmp2 = mm<V, DATAIN>::max(mmTmp2, dataIn->data);
                    }
                }
                for (; i <= (numValues - VALUES_PER_VECTOR); i += VALUES_PER_VECTOR, ++dataIn) {
                    mmTmp1 = mm<V, DATAIN>::max(mmTmp1, dataIn->data);
                    mmTmp2 = mm<V, DATAIN>::max(mmTmp2, dataIn->data);
                }
                auto tmp128 = mm<V, DATAIN>::max(mmTmp1, mmTmp2);
                DATAIN * pTmp = reinterpret_cast<DATAIN*>(&tmp128);
                DATAIN maximum = pTmp[0];
                for (size_t i = 1; i < VALUES_PER_VECTOR; ++i) {
                    maximum = (pTmp[i] > maximum) ? pTmp[i] : maximum;
                }
                if (i < numValues) {
                    auto dataIn2 = reinterpret_cast<hamming_scalar_t*>(dataIn);
                    for (; i < numValues; ++i, ++dataIn2) {
                        maximum = (dataIn2->data > maximum) ? dataIn2->data : maximum;
                    }
                }
                dataOut->store(maximum);
            }
            void operator()(
                    AggregateConfiguration::Avg) {
                const size_t numValues = test.getNumValues();
                size_t i = 0;
                auto dataIn = test.bufEncoded.template begin<hamming_simd_t>();
                auto dataOut = test.bufResult.template begin<hamming_larger_t>();
                auto mmTmp = mm<V, larger_t>::set1(0);
                while (i <= (numValues - VALUES_PER_UNROLL)) {
                    for (size_t k = 0; k < UNROLL; ++k, i += VALUES_PER_VECTOR, ++dataIn) {
                        mmTmp = mm_op<V, larger_t, add>::compute(mmTmp, mm<V, DATAIN>::cvt_larger_lo(dataIn->data));
                        mmTmp = mm_op<V, larger_t, add>::compute(mmTmp, mm<V, DATAIN>::cvt_larger_hi(dataIn->data));
                    }
                }
                for (; i <= (numValues - VALUES_PER_VECTOR); i += VALUES_PER_VECTOR, ++dataIn) {
                    mmTmp = mm_op<V, larger_t, add>::compute(mmTmp, mm<V, DATAIN>::cvt_larger_lo(dataIn->data));
                    mmTmp = mm_op<V, larger_t, add>::compute(mmTmp, mm<V, DATAIN>::cvt_larger_hi(dataIn->data));
                }
                auto tmp = mm<V, larger_t>::sum(mmTmp);
                if (i < numValues) {
                    auto dataIn2 = reinterpret_cast<hamming_scalar_t*>(dataIn);
                    for (; i < numValues; ++i, ++dataIn2) {
                        tmp += dataIn2->data;
                    }
                }
                dataOut->store(tmp / numValues);
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
                const size_t numValues = test.getNumValues();
                size_t i = 0;
                auto dataIn = test.bufEncoded.template begin<hamming_simd_t>();
                auto dataOut = test.bufResult.template begin<hamming_larger_t>();
                auto mmTmp = mm<V, larger_t>::set1(0);
                while (i <= (numValues - VALUES_PER_UNROLL)) {
                    for (size_t k = 0; k < UNROLL; ++k, i += VALUES_PER_VECTOR, ++dataIn) {
                        if (dataIn->isValid()) {
                            mmTmp = mm_op<V, larger_t, add>::compute(mmTmp, mm<V, DATAIN>::cvt_larger_lo(dataIn->data));
                            mmTmp = mm_op<V, larger_t, add>::compute(mmTmp, mm<V, DATAIN>::cvt_larger_hi(dataIn->data));
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, i + k, iteration);
                        }
                    }
                }
                for (; i <= (numValues - VALUES_PER_VECTOR); i += VALUES_PER_VECTOR, ++dataIn) {
                    if (dataIn->isValid()) {
                        mmTmp = mm_op<V, larger_t, add>::compute(mmTmp, mm<V, DATAIN>::cvt_larger_lo(dataIn->data));
                        mmTmp = mm_op<V, larger_t, add>::compute(mmTmp, mm<V, DATAIN>::cvt_larger_hi(dataIn->data));
                    } else {
                        throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                    }
                }
                auto tmp = mm<V, larger_t>::sum(mmTmp);
                if (i < numValues) {
                    auto dataIn2 = reinterpret_cast<hamming_scalar_t*>(dataIn);
                    for (; i < numValues; ++i, ++dataIn2) {
                        if (dataIn2->isValid()) {
                            tmp += dataIn2->data;
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                        }
                    }
                }
                dataOut->store(tmp);
            }
            void operator()(
                    AggregateConfiguration::Min) {
                const size_t numValues = test.getNumValues();
                size_t i = 0;
                auto dataIn = test.bufEncoded.template begin<hamming_simd_t>();
                auto dataOut = test.bufResult.template begin<hamming_scalar_t>();
                auto mmTmp1 = mm<V, DATAIN>::set1(std::numeric_limits<DATAIN>::max());
                auto mmTmp2 = mm<V, DATAIN>::set1(std::numeric_limits<DATAIN>::max());
                while (i <= (numValues - VALUES_PER_UNROLL)) {
                    for (size_t k = 0; k < UNROLL; ++k, i += VALUES_PER_VECTOR, ++dataIn) {
                        if (dataIn->isValid()) {
                            mmTmp1 = mm<V, DATAIN>::min(mmTmp1, dataIn->data);
                            mmTmp2 = mm<V, DATAIN>::min(mmTmp2, dataIn->data);
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, i + k, iteration);
                        }
                    }
                }
                for (; i <= (numValues - VALUES_PER_VECTOR); i += VALUES_PER_VECTOR, ++dataIn) {
                    if (dataIn->isValid()) {
                        mmTmp1 = mm<V, DATAIN>::min(mmTmp1, dataIn->data);
                        mmTmp2 = mm<V, DATAIN>::min(mmTmp2, dataIn->data);
                    } else {
                        throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                    }
                }
                auto tmp128 = mm<V, DATAIN>::min(mmTmp1, mmTmp2);
                DATAIN * pTmp = reinterpret_cast<DATAIN*>(&tmp128);
                DATAIN minimum = pTmp[0];
                for (size_t i = 1; i < VALUES_PER_VECTOR; ++i) {
                    minimum = (pTmp[i] < minimum) ? pTmp[i] : minimum;
                }
                if (i < numValues) {
                    auto dataIn2 = reinterpret_cast<hamming_scalar_t*>(dataIn);
                    for (; i < numValues; ++i, ++dataIn2) {
                        if (dataIn2->isValid()) {
                            minimum = (dataIn2->data < minimum) ? dataIn2->data : minimum;
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                        }
                    }
                }
                dataOut->store(minimum);
            }
            void operator()(
                    AggregateConfiguration::Max) {
                const size_t numValues = test.getNumValues();
                size_t i = 0;
                auto dataIn = test.bufEncoded.template begin<hamming_simd_t>();
                auto dataOut = test.bufResult.template begin<hamming_scalar_t>();
                auto mmTmp1 = mm<V, DATAIN>::set1(std::numeric_limits<DATAIN>::min());
                auto mmTmp2 = mm<V, DATAIN>::set1(std::numeric_limits<DATAIN>::min());
                while (i <= (numValues - VALUES_PER_UNROLL)) {
                    for (size_t k = 0; k < UNROLL; ++k, i += VALUES_PER_VECTOR, ++dataIn) {
                        if (dataIn->isValid()) {
                            mmTmp1 = mm<V, DATAIN>::max(mmTmp1, dataIn->data);
                            mmTmp2 = mm<V, DATAIN>::max(mmTmp2, dataIn->data);
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, i + k, iteration);
                        }
                    }
                }
                for (; i <= (numValues - VALUES_PER_VECTOR); i += VALUES_PER_VECTOR, ++dataIn) {
                    if (dataIn->isValid()) {
                        mmTmp1 = mm<V, DATAIN>::max(mmTmp1, dataIn->data);
                        mmTmp2 = mm<V, DATAIN>::max(mmTmp2, dataIn->data);
                    } else {
                        throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                    }
                }
                auto tmp128 = mm<V, DATAIN>::max(mmTmp1, mmTmp2);
                DATAIN * pTmp = reinterpret_cast<DATAIN*>(&tmp128);
                DATAIN maximum = pTmp[0];
                for (size_t i = 1; i < VALUES_PER_VECTOR; ++i) {
                    maximum = (pTmp[i] > maximum) ? pTmp[i] : maximum;
                }
                if (i < numValues) {
                    auto dataIn2 = reinterpret_cast<hamming_scalar_t*>(dataIn);
                    for (; i < numValues; ++i, ++dataIn2) {
                        if (dataIn2->isValid()) {
                            maximum = (dataIn2->data > maximum) ? dataIn2->data : maximum;
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                        }
                    }
                }
                dataOut->store(maximum);
            }
            void operator()(
                    AggregateConfiguration::Avg) {
                const size_t numValues = test.getNumValues();
                size_t i = 0;
                auto dataIn = test.bufEncoded.template begin<hamming_simd_t>();
                auto dataOut = test.bufResult.template begin<hamming_larger_t>();
                auto mmTmp = mm<V, larger_t>::set1(0);
                while (i <= (numValues - VALUES_PER_UNROLL)) {
                    for (size_t k = 0; k < UNROLL; ++k, i += VALUES_PER_VECTOR, ++dataIn) {
                        if (dataIn->isValid()) {
                            mmTmp = mm_op<V, larger_t, add>::compute(mmTmp, mm<V, DATAIN>::cvt_larger_lo(dataIn->data));
                            mmTmp = mm_op<V, larger_t, add>::compute(mmTmp, mm<V, DATAIN>::cvt_larger_hi(dataIn->data));
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, i + k, iteration);
                        }
                    }
                }
                for (; i <= (numValues - VALUES_PER_VECTOR); i += VALUES_PER_VECTOR, ++dataIn) {
                    if (dataIn->isValid()) {
                        mmTmp = mm_op<V, larger_t, add>::compute(mmTmp, mm<V, DATAIN>::cvt_larger_lo(dataIn->data));
                        mmTmp = mm_op<V, larger_t, add>::compute(mmTmp, mm<V, DATAIN>::cvt_larger_hi(dataIn->data));
                    } else {
                        throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                    }
                }
                auto tmp = mm<V, larger_t>::sum(mmTmp);
                if (i < numValues) {
                    auto dataIn2 = reinterpret_cast<hamming_scalar_t*>(dataIn);
                    for (; i < numValues; ++i, ++dataIn2) {
                        if (dataIn2->isValid()) {
                            tmp += dataIn2->data;
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                        }
                    }
                }
                dataOut->store(tmp / numValues);
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
                auto data = this->bufEncoded.template begin<hamming_simd_t>();
                auto dataOut = this->bufResult.template begin<V>();
                for (; i <= (numValues - VALUES_PER_UNROLL); i += VALUES_PER_UNROLL) {
                    for (size_t k = 0; k < UNROLL; ++k, ++data, ++dataOut) {
                        *dataOut = data->data;
                    }
                }
                for (; i <= (numValues - 1); i += VALUES_PER_VECTOR, ++data, ++dataOut) {
                    *dataOut = data->data;
                }
                if (i < numValues) {
                    auto data2 = reinterpret_cast<hamming_scalar_t*>(data);
                    auto dataOut2 = reinterpret_cast<DATAIN*>(dataOut);
                    for (; i < numValues; ++i, ++data2, ++dataOut2) {
                        *dataOut2 = data2->data;
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
                size_t numValues = this->getNumValues();
                size_t i = 0;
                auto data = this->bufEncoded.template begin<hamming_simd_t>();
                auto dataOut = this->bufResult.template begin<V>();
                for (; i <= (numValues - VALUES_PER_UNROLL); i += VALUES_PER_UNROLL) {
                    for (size_t k = 0; k < UNROLL; ++k, ++data, ++dataOut) {
                        if (data->isValid()) {
                            *dataOut = data->data;
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, i + k, iteration);
                        }
                    }
                }
                for (; i <= (numValues - 1); i += VALUES_PER_VECTOR, ++data, ++dataOut) {
                    *dataOut = data->data;
                }
                if (i < numValues) {
                    auto data2 = reinterpret_cast<hamming_scalar_t*>(data);
                    auto dataOut2 = reinterpret_cast<DATAIN*>(dataOut);
                    for (; i < numValues; ++i, ++data2, ++dataOut2) {
                        *dataOut2 = data2->data;
                    }
                }
            }
        }
    };

}
