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
 * File:   AN_sse42_8x16_8x32.hpp
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 13. Dezember 2016, 01:01
 */

#pragma once

#ifndef AN_SSE42
#error "Clients must not include this file directly, but file <AN/AN_sse42.hpp>!"
#endif

#include <AN/ANTest.hpp>
#include <SIMD/SSE.hpp>
#include <Util/Functors.hpp>
#include <Util/Helpers.hpp>
#include <Util/ArithmeticSelector.hpp>
#include <Util/AggregateSelector.hpp>

using namespace coding_benchmark::simd;

namespace coding_benchmark {

    template<typename DATARAW, typename DATAENC, size_t UNROLL>
    struct AN_sse42_8x16_8x32 :
            public ANTest<DATARAW, DATAENC, UNROLL>,
            public SSE42Test {

        using ANTest<DATARAW, DATAENC, UNROLL>::ANTest;

        virtual ~AN_sse42_8x16_8x32() {
        }

        void RunEncode(
                const EncodeConfiguration & config) override {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                auto *mmData = this->bufRaw.template begin<__m128i >();
                auto * const mmDataEnd = this->bufRaw.template end<__m128i >();
                auto *mmOut = this->bufEncoded.template begin<__m128i >();
                auto mmA = _mm_set1_epi32(this->A);

                const constexpr bool isSigned = std::is_signed<DATARAW>::value;
                auto mmShuffleS = _mm_set_epi32(0xFFFFFFFF, 0xFFFFFFFF, 0x0F0E0D0C, 0x0B0A0908);
                auto mmShuffleU = _mm_set_epi32(0xFFFF0F0E, 0xFFFF0D0C, 0xFFFF0B0A, 0xFFFF0908);

                while (mmData <= (mmDataEnd - UNROLL)) {
                    // let the compiler unroll the loop
                    for (size_t unroll = 0; unroll < UNROLL; ++unroll) {
                        auto mmIn = _mm_lddqu_si128(mmData++);
                        if (isSigned) {
                            _mm_storeu_si128(mmOut++, _mm_mullo_epi32(_mm_cvtepi16_epi32(mmIn), mmA));
                            _mm_storeu_si128(mmOut++, _mm_mullo_epi32(_mm_cvtepi16_epi32(_mm_shuffle_epi8(mmIn, mmShuffleS)), mmA));
                        } else {
                            _mm_storeu_si128(mmOut++, _mm_mullo_epi32(_mm_cvtepu16_epi32(mmIn), mmA));
                            _mm_storeu_si128(mmOut++, _mm_mullo_epi32(_mm_shuffle_epi8(mmIn, mmShuffleU), mmA));
                        }
                    }
                }
                // remaining numbers
                while (mmData <= (mmDataEnd - 1)) {
                    auto mmIn = _mm_lddqu_si128(mmData++);
                    if (isSigned) {
                        _mm_storeu_si128(mmOut++, _mm_mullo_epi32(_mm_cvtepi16_epi32(mmIn), mmA));
                        _mm_storeu_si128(mmOut++, _mm_mullo_epi32(_mm_cvtepi16_epi32(_mm_shuffle_epi8(mmIn, mmShuffleS)), mmA));
                    } else {
                        _mm_storeu_si128(mmOut++, _mm_mullo_epi32(_mm_cvtepu16_epi32(mmIn), mmA));
                        _mm_storeu_si128(mmOut++, _mm_mullo_epi32(_mm_shuffle_epi8(mmIn, mmShuffleU), mmA));
                    }
                }
                if (mmData < mmDataEnd) {
                    auto data16End = reinterpret_cast<DATARAW*>(mmDataEnd);
                    auto out32 = reinterpret_cast<DATAENC*>(mmOut);
                    for (auto data16 = reinterpret_cast<DATARAW*>(mmData); data16 < data16End; ++data16, ++out32)
                        *out32 = *data16 * this->A;
                }
            }
        }

        bool DoArithmetic(
                const ArithmeticConfiguration & config) override {
            return std::visit(ArithmeticSelector(), config.mode);
        }

        struct Arithmetor {
            AN_sse42_8x16_8x32 & test;
            const ArithmeticConfiguration & config;
            Arithmetor(
                    AN_sse42_8x16_8x32 & test,
                    const ArithmeticConfiguration & config)
                    : test(test),
                      config(config) {
            }
            template<template<typename = void> class func>
            void impl() {
                func<> functor;
                auto mmData = test.bufEncoded.template begin<__m128i >();
                auto const mmDataEnd = test.bufEncoded.template end<__m128i >();
                auto mmOut = test.bufResult.template begin<__m128i >();
                DATAENC operandEnc = config.operand * test.A;
                auto mmOperandEnc = mm<__m128i, DATAENC>::set1(operandEnc);
                while (mmData <= (mmDataEnd - UNROLL)) {
                    // let the compiler unroll the loop
                    for (size_t unroll = 0; unroll < UNROLL; ++unroll) {
                        _mm_storeu_si128(mmOut++, mm_op<__m128i, DATAENC, func>::compute(_mm_lddqu_si128(mmData++), mmOperandEnc));
                    }
                }
                // remaining numbers
                while (mmData <= (mmDataEnd - 1)) {
                    _mm_storeu_si128(mmOut++, mm_op<__m128i, DATAENC, func>::compute(_mm_lddqu_si128(mmData++), mmOperandEnc));
                }
                if (mmData < mmDataEnd) {
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
            AN_sse42_8x16_8x32 & test;
            const AggregateConfiguration & config;
            Aggregator(
                    AN_sse42_8x16_8x32 & test,
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
                auto *mmData = test.bufEncoded.template begin<__m128i >();
                auto * const mmDataEnd = test.bufEncoded.template end<__m128i >();
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
                    auto const dataInEnd = reinterpret_cast<DATAENC*>(mmDataEnd);
                    while (dataIn < dataInEnd) {
                        value = funcKernelScalar(value, *dataIn++);
                    }
                }
                auto dataOut = test.bufResult.template begin<Aggregate>();
                *dataOut = funcFinal(value, numValues);
            }
            void operator()(
                    AggregateConfiguration::Sum) {
                impl<DATAENC>([] {return simd::mm<__m128i, DATAENC>::set1(0);}, [](__m128i mmValue, __m128i mmTmp) {return simd::mm_op<__m128i, DATAENC, add>::compute(mmValue, mmTmp);},
                        [](__m128i mmValue) {return simd::mm<__m128i, DATAENC>::sum(mmValue);}, [](DATAENC value, DATAENC tmp) {return value + tmp;},
                        [](DATAENC value, size_t numValues) {return value;});
            }
            void operator()(
                    AggregateConfiguration::Min) {
                impl<DATAENC>([] {return simd::mm<__m128i, DATAENC>::set1(std::numeric_limits<DATAENC>::max());},
                        [](__m128i mmValue, __m128i mmTmp) {return simd::mm<__m128i, DATAENC>::min(mmValue, mmTmp);}, [](__m128i mmValue) {return simd::mm<__m128i, DATAENC>::min(mmValue);},
                        [](DATAENC value, DATAENC tmp) {return value < tmp ? value : tmp;}, [](DATAENC value, size_t numValues) {return value;});
            }
            void operator()(
                    AggregateConfiguration::Max) {
                impl<DATAENC>([] {return simd::mm<__m128i, DATAENC>::set1(std::numeric_limits<DATAENC>::min());},
                        [](__m128i mmValue, __m128i mmTmp) {return simd::mm<__m128i, DATAENC>::max(mmValue, mmTmp);}, [](__m128i mmValue) {return simd::mm<__m128i, DATAENC>::max(mmValue);},
                        [](DATAENC value, DATAENC tmp) {return value > tmp ? value : tmp;}, [](DATAENC value, size_t numValues) {return value;});
            }
            void operator()(
                    AggregateConfiguration::Avg) {
                impl<DATAENC>([] {return simd::mm<__m128i, DATAENC>::set1(0);}, [](__m128i mmValue, __m128i mmTmp) {return simd::mm_op<__m128i, DATAENC, add>::compute(mmValue, mmTmp);},
                        [](__m128i mmValue) {return simd::mm<__m128i, DATAENC>::sum(mmValue);}, [](DATAENC value, DATAENC tmp) {return value + tmp;},
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
