// Copyright (c) 2016 Till Kolditz, Stefan de Bruijn
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
 * File:   AN_avx2_16x16_16x32.hpp
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 13. Dezember 2016, 00:11
 */

#pragma once

#ifndef AN_AVX2
#error "Clients must not include this file directly, but file <AN/AN_avx2.hpp>!"
#endif

#include <AN/ANTest.hpp>
#include <SIMD/AVX2.hpp>
#include <Util/Functors.hpp>
#include <Util/Helpers.hpp>
#include <Util/ArithmeticSelector.hpp>
#include <Util/AggregateSelector.hpp>

using namespace coding_benchmark::simd;

namespace coding_benchmark {

    template<typename DATARAW, typename DATAENC, size_t UNROLL>
    struct AN_avx2_16x16_16x32 :
            public ANTest<DATARAW, DATAENC, UNROLL>,
            public AVX2Test {

        static const constexpr size_t NUM_VALUES_PER_SIMDREG = 32 / sizeof(DATAENC); // sizeof(__m256i) / sizeof(DATAENC)
        static const constexpr size_t NUM_VALUES_PER_UNROLL = UNROLL * NUM_VALUES_PER_SIMDREG;

        using ANTest<DATARAW, DATAENC, UNROLL>::ANTest;

        virtual ~AN_avx2_16x16_16x32() {
        }

        void RunEncode(
                const EncodeConfiguration & config) override {
            for (size_t iter = 0; iter < config.numIterations; ++iter) {
                _ReadWriteBarrier();
                auto *dataIn = this->bufRaw.template begin<__m128i >();
                auto *dataInEnd = this->bufRaw.template end<__m128i >();
                auto *dataOut = this->bufEncoded.template begin<__m256i >();
                auto mmA = _mm256_set1_epi32(this->A);

                const constexpr bool isSigned = std::is_signed<DATARAW>::value;
                while (dataIn <= (dataInEnd - UNROLL)) {
                    // let the compiler unroll the loop
                    for (size_t unroll = 0; unroll < UNROLL; ++unroll) {
                        auto mm128 = _mm_lddqu_si128(dataIn++);
                        auto mm256 = isSigned ? _mm256_cvtepi16_epi32(mm128) : _mm256_cvtepu16_epi32(mm128);
                        _mm256_storeu_si256(dataOut++, _mm256_mullo_epi32(mm256, mmA));
                    }
                }

                while (dataIn <= (dataInEnd - 1)) {
                    auto mm128 = _mm_lddqu_si128(dataIn++);
                    auto mm256 = isSigned ? _mm256_cvtepi16_epi32(mm128) : _mm256_cvtepu16_epi32(mm128);
                    _mm256_storeu_si256(dataOut++, _mm256_mullo_epi32(mm256, mmA));
                }

                // multiply remaining numbers sequentially
                if (dataIn < dataInEnd) {
                    auto data16 = reinterpret_cast<DATARAW*>(dataIn);
                    auto data16End = reinterpret_cast<DATARAW*>(dataInEnd);
                    auto out32 = reinterpret_cast<DATAENC*>(dataOut);
                    do {
                        *out32++ = static_cast<DATAENC>(*data16++) * this->A;
                    } while (data16 < data16End);
                }
            }
        }

        bool DoArithmetic(
                const ArithmeticConfiguration & config) override {
            return std::visit(ArithmeticSelector(), config.mode);
        }

        struct Arithmetor {
            AN_avx2_16x16_16x32 & test;
            const ArithmeticConfiguration & config;
            Arithmetor(
                    AN_avx2_16x16_16x32 & test,
                    const ArithmeticConfiguration & config)
                    : test(test),
                      config(config) {
            }
            template<template<typename = void> class Functor>
            void impl() {
                auto *mmData = test.bufEncoded.template begin<__m256i >();
                auto * const mmDataEnd = test.bufEncoded.template end<__m256i >();
                auto *mmOut = test.bufResult.template begin<__m256i >();
                DATAENC operandEnc = config.operand * test.A;
                auto mmOperandEnc = mm<__m256i, DATAENC>::set1(operandEnc);
                while (mmData <= (mmDataEnd - UNROLL)) {
                    // let the compiler unroll the loop
                    for (size_t unroll = 0; unroll < UNROLL; ++unroll) {
                        _mm256_storeu_si256(mmOut++, mm_op<__m256i, DATAENC, Functor>::compute(_mm256_lddqu_si256(mmData++), mmOperandEnc));
                    }
                }
                // remaining numbers
                while (mmData <= (mmDataEnd - 1)) {
                    _mm256_storeu_si256(mmOut++, mm_op<__m256i, DATAENC, Functor>::compute(_mm256_lddqu_si256(mmData++), mmOperandEnc));
                }
                if (mmData < mmDataEnd) {
                    Functor<> functor;
                    auto data32End = reinterpret_cast<DATAENC*>(mmDataEnd);
                    auto out32 = reinterpret_cast<DATAENC*>(mmOut);
                    for (auto data32 = reinterpret_cast<DATAENC*>(mmData); data32 < data32End; ++data32, ++out32)
                        *out32 = functor(*data32, operandEnc);
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
            AN_avx2_16x16_16x32 & test;
            const AggregateConfiguration & config;
            Aggregator(
                    AN_avx2_16x16_16x32 & test,
                    const AggregateConfiguration & config)
                    : test(test),
                      config(config) {
            }
            template<typename Aggregate, typename InitializeVector, typename KernelVector, typename KernelScalar, typename VectorToScalar, typename Finalize>
            void impl(
                    InitializeVector && funcInitVector,
                    KernelVector && funcKernelVector,
                    VectorToScalar && funcVectorToScalar,
                    KernelScalar && funcKernelScalar,
                    Finalize && funcFinal) {
                const size_t numValues = test.template getNumValues();
                auto *mmData = test.bufEncoded.template begin<__m256i >();
                auto * const mmDataEnd = test.bufEncoded.template end<__m256i >();
                auto mmValue = funcInitVector();
                while (mmData <= (mmDataEnd - UNROLL)) {
                    for (size_t k = 0; k < UNROLL; ++k) {
                        mmValue = funcKernelVector(mmValue, *mmData++);
                    }
                }
                while (mmData <= (mmDataEnd - 1)) {
                    mmValue = funcKernelVector(mmValue, *mmData++);
                }
                Aggregate value = funcVectorToScalar(mmValue);
                if (mmData < mmDataEnd) {
                    auto dataIn = reinterpret_cast<DATAENC*>(mmData);
                    const auto dataInEnd = reinterpret_cast<DATAENC*>(mmDataEnd);
                    while (dataIn < dataInEnd) {
                        value = funcKernelScalar(value, *dataIn++);
                    }
                }
                auto dataOut = test.bufResult.template begin<Aggregate>();
                *dataOut = funcFinal(value, numValues);
            }
            void operator()(
                    AggregateConfiguration::Sum) {
                impl<DATAENC>([] {return simd::mm<__m256i, DATAENC>::set1(0);}, [](__m256i mmValue, __m256i mmTmp) {return simd::mm_op<__m256i, DATAENC, add>::compute(mmValue, mmTmp);},
                        [](__m256i mmValue) {return simd::mm<__m256i, DATAENC>::sum(mmValue);}, [](DATAENC value, DATAENC tmp) {return value + tmp;},
                        [](DATAENC value, size_t numValues) {return value;});
            }
            void operator()(
                    AggregateConfiguration::Min) {
                impl<DATAENC>([] {return simd::mm<__m256i, DATAENC>::set1(std::numeric_limits<DATAENC>::max());},
                        [](__m256i mmValue, __m256i mmTmp) {return simd::mm<__m256i, DATAENC>::min(mmValue, mmTmp);}, [](__m256i mmValue) {return simd::mm<__m256i, DATAENC>::min(mmValue);},
                        [](DATAENC value, DATAENC tmp) {return value < tmp ? value : tmp;}, [](DATAENC value, size_t numValues) {return value;});
            }
            void operator()(
                    AggregateConfiguration::Max) {
                impl<DATAENC>([] {return simd::mm<__m256i, DATAENC>::set1(std::numeric_limits<DATAENC>::min());},
                        [](__m256i mmValue, __m256i mmTmp) {return simd::mm<__m256i, DATAENC>::max(mmValue, mmTmp);}, [](__m256i mmValue) {return simd::mm<__m256i, DATAENC>::max(mmValue);},
                        [](DATAENC value, DATAENC tmp) {return value > tmp ? value : tmp;}, [](DATAENC value, size_t numValues) {return value;});
            }
            void operator()(
                    AggregateConfiguration::Avg) {
                impl<DATAENC>([] {return simd::mm<__m256i, DATAENC>::set1(0);}, [](__m256i mmValue, __m256i mmTmp) {return simd::mm_op<__m256i, DATAENC, add>::compute(mmValue, mmTmp);},
                        [](__m256i mmValue) {return simd::mm<__m256i, DATAENC>::sum(mmValue);}, [](DATAENC value, DATAENC tmp) {return value + tmp;},
                        [this](DATAENC value, size_t numValues) {return (value / (numValues * test.A)) * test.A;});
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