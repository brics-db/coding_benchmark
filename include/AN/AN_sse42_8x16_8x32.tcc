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

#include <stdexcept>

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

        typedef __m128i VEC;

        using ANTest<DATARAW, DATAENC, UNROLL>::ANTest;

        virtual ~AN_sse42_8x16_8x32() {
        }

        template<typename T>
        VEC * const ComputeEnd(
                VEC * mmBeg,
                const SubTestConfiguration & config) const {
            return reinterpret_cast<VEC*>(reinterpret_cast<T*>(mmBeg) + config.numValues);
        }

        void RunEncode(
                const EncodeConfiguration & config) override {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                auto *mmData = config.source.template begin<VEC>();
                auto * const mmDataEnd = this->template ComputeEnd<DATARAW>(mmData, config);
                auto *mmOut = config.target.template begin<VEC>();
                auto mmA = _mm_set1_epi32(this->A);

                const constexpr bool isSigned = std::is_signed_v<DATARAW>;

                while (mmData <= (mmDataEnd - UNROLL)) {
                    // let the compiler unroll the loop
                    for (size_t unroll = 0; unroll < UNROLL; ++unroll) {
                        auto mmIn = _mm_lddqu_si128(mmData++);
                        if (isSigned) {
                            _mm_storeu_si128(mmOut++, _mm_mullo_epi32(_mm_cvtepi16_epi32(mmIn), mmA));
                            _mm_storeu_si128(mmOut++, _mm_mullo_epi32(_mm_cvtepi16_epi32(_mm_srli_si128(mmIn, sizeof(VEC) / 2)), mmA));
                        } else {
                            _mm_storeu_si128(mmOut++, _mm_mullo_epi32(_mm_cvtepu16_epi32(mmIn), mmA));
                            _mm_storeu_si128(mmOut++, _mm_mullo_epi32(_mm_cvtepu16_epi32(_mm_srli_si128(mmIn, sizeof(VEC) / 2)), mmA));
                        }
                    }
                }
                // remaining numbers
                while (mmData <= (mmDataEnd - 1)) {
                    auto mmIn = _mm_lddqu_si128(mmData++);
                    if (isSigned) {
                        _mm_storeu_si128(mmOut++, _mm_mullo_epi32(_mm_cvtepi16_epi32(mmIn), mmA));
                        _mm_storeu_si128(mmOut++, _mm_mullo_epi32(_mm_cvtepi16_epi32(_mm_srli_si128(mmIn, sizeof(VEC) / 2)), mmA));
                    } else {
                        _mm_storeu_si128(mmOut++, _mm_mullo_epi32(_mm_cvtepu16_epi32(mmIn), mmA));
                        _mm_storeu_si128(mmOut++, _mm_mullo_epi32(_mm_cvtepu16_epi32(_mm_srli_si128(mmIn, sizeof(VEC) / 2)), mmA));
                    }
                }
                if (mmData < mmDataEnd) {
                    auto data16End = reinterpret_cast<DATARAW*>(mmDataEnd);
                    auto out32 = reinterpret_cast<DATAENC*>(mmOut);
                    auto data16 = reinterpret_cast<DATARAW*>(mmData);
                    while (data16 < data16End) {
                        *out32++ = *data16++ * this->A;
                    }
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
            template<template<typename = void> class Functor>
            void impl() {
                auto in128 = config.source.template begin<VEC>();
                const auto in128end = test.template ComputeEnd<DATAENC>(in128, config);
                auto out128 = config.target.template begin<VEC>();
                auto mmA __attribute__((unused)) = mm<VEC, DATAENC>::set1(test.A);
                DATAENC operand = config.operand;
                if constexpr (std::is_same_v<Functor<void>, add<void>> || std::is_same_v<Functor<void>, sub<void>> || std::is_same_v<Functor<void>, div<void>>) {
                    operand *= test.A;
                } else if constexpr (std::is_same_v<Functor<void>, mul<void>>) {
                    // do not encode operand here, otherwise we will have non-code values after the operation!
                } else {
                    throw std::runtime_error("Functor not known!");
                }
                VEC mmOperand = mm<VEC, DATAENC>::set1(operand);
                while (in128 <= (in128end - UNROLL)) {
                    // let the compiler unroll the loop
                    for (size_t unroll = 0; unroll < UNROLL; ++unroll) {
                        auto x = mm_op<VEC, DATAENC, Functor>::compute(*in128++, mmOperand);
                        if constexpr (std::is_same_v<Functor<void>, div<void>>) {
                            x = mm_op<VEC, DATAENC, mul>::compute(x, mmA); // make sure we get a code word again
                        }
                        *out128++ = x;
                    }
                }
                // remaining numbers
                while (in128 <= (in128end - 1)) {
                    auto x = mm_op<VEC, DATAENC, Functor>::compute(*in128++, mmOperand);
                    if constexpr (std::is_same_v<Functor<void>, div<void>>) {
                        x = mm_op<VEC, DATAENC, mul>::compute(x, mmA); // make sure we get a code word again
                    }
                    *out128++ = x;
                }
                if (in128 < in128end) {
                    Functor<> functor;
                    auto in32 = reinterpret_cast<DATAENC*>(in128);
                    const auto in32end = reinterpret_cast<DATAENC* const >(in128end);
                    auto out32 = reinterpret_cast<DATAENC*>(out128);
                    while (in32 < in32end) {
                        auto x = functor(*in32++, operand);
                        if (std::is_same_v<Functor<void>, div<void>>) {
                            x *= test.A; // make sure we get a code word again
                        }
                        *out32++ = x;
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
                auto in128 = config.source.template begin<VEC>();
                const auto in128end = test.template ComputeEnd<DATAENC>(in128, config);
                auto mmValue = funcInitVector();
                while (in128 <= (in128end - UNROLL)) {
                    for (size_t k = 0; k < UNROLL; ++k) {
                        mmValue = funcKernelVector(mmValue, *in128++);
                    }
                }
                while (in128 <= (in128end - 1)) {
                    mmValue = funcKernelVector(mmValue, *in128++);
                }
                Aggregate value = funcVectorToScalar(mmValue);
                if (in128 < in128end) {
                    auto dataIn = reinterpret_cast<DATAENC*>(in128);
                    const auto dataInEnd = reinterpret_cast<DATAENC* const >(in128end);
                    while (dataIn < dataInEnd) {
                        value = funcKernelScalar(value, *dataIn++);
                    }
                }
                auto final = funcFinal(value, config.numValues);
                auto out32 = test.bufScratchPad.template begin<Aggregate>();
                *out32 = final;
                EncodeConfiguration encConf(1, 2, test.bufScratchPad, config.target);
                test.RunEncode(encConf);
            }
            void operator()(
                    AggregateConfiguration::Sum) {
                impl<DATAENC>([] {return simd::mm<VEC, DATAENC>::set1(0);}, [](VEC mmValue, VEC mmTmp) {return simd::mm_op<VEC, DATAENC, add>::compute(mmValue, mmTmp);},
                        [](VEC mmValue) {return simd::mm<VEC, DATAENC>::sum(mmValue);}, [](DATAENC value, DATAENC tmp) {return value + tmp;}, [](DATAENC value, size_t numValues) {return value;});
            }
            void operator()(
                    AggregateConfiguration::Min) {
                impl<DATAENC>([] {return simd::mm<VEC, DATAENC>::set1(std::numeric_limits<DATAENC>::max());}, [](VEC mmValue, VEC mmTmp) {return simd::mm<VEC, DATAENC>::min(mmValue, mmTmp);},
                        [](VEC mmValue) {return simd::mm<VEC, DATAENC>::min(mmValue);}, [](DATAENC value, DATAENC tmp) {return value < tmp ? value : tmp;},
                        [](DATAENC value, size_t numValues) {return value;});
            }
            void operator()(
                    AggregateConfiguration::Max) {
                impl<DATAENC>([] {return simd::mm<VEC, DATAENC>::set1(std::numeric_limits<DATAENC>::min());}, [](VEC mmValue, VEC mmTmp) {return simd::mm<VEC, DATAENC>::max(mmValue, mmTmp);},
                        [](VEC mmValue) {return simd::mm<VEC, DATAENC>::max(mmValue);}, [](DATAENC value, DATAENC tmp) {return value > tmp ? value : tmp;},
                        [](DATAENC value, size_t numValues) {return value;});
            }
            void operator()(
                    AggregateConfiguration::Avg) {
                impl<DATAENC>([] {return simd::mm<VEC, DATAENC>::set1(0);}, [](VEC mmValue, VEC mmTmp) {return simd::mm_op<VEC, DATAENC, add>::compute(mmValue, mmTmp);},
                        [](VEC mmValue) {return simd::mm<VEC, DATAENC>::sum(mmValue);}, [](DATAENC value, DATAENC tmp) {return value + tmp;},
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
