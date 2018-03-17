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

#pragma once

#ifndef AN_SSE42
#error "Clients must not include this file directly, but file <AN/AN_sse42.hpp>!"
#endif

#include <AN/AN_sse42_8x16_8x32_inv.tcc>

namespace coding_benchmark {

    template<size_t UNROLL>
    struct AN_sse42_8x16_8x32_u_inv :
            public AN_sse42_8x16_8x32_inv<uint16_t, uint32_t, UNROLL> {

        typedef AN_sse42_8x16_8x32_inv<uint16_t, uint32_t, UNROLL> BASE;
        typedef simd::mm<__m128i, uint32_t> mmEnc;
        typedef simd::mm_op<__m128i, uint32_t, std::less_equal> mmEncLE;

        using BASE::NUM_VALUES_PER_UNROLL;
        using BASE::NUM_VALUES_PER_SIMDREG;

        using BASE::AN_sse42_8x16_8x32_inv;

        virtual ~AN_sse42_8x16_8x32_u_inv() {
        }

        virtual bool DoCheck() override {
            return true;
        }

        virtual void RunCheck(
                const CheckConfiguration & config) override {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                auto mmData = config.target.template begin<__m128i >();
                auto mmDataEnd = config.target.template end<__m128i >();
                uint32_t dMax = std::numeric_limits<uint16_t>::max();
                __m128i mmDMax = _mm_set1_epi32(dMax); // we assume 16-bit input data
                __m128i mmAInv = _mm_set1_epi32(this->A_INV);
                while (mmData <= (mmDataEnd - UNROLL)) {
                    // let the compiler unroll the loop
                    for (size_t k = 0; k < UNROLL; ++k) {
                        auto mmInDec = _mm_mullo_epi32(_mm_lddqu_si128(mmData), mmAInv);
                        if (mmEncLE::cmp_mask(mmInDec, mmDMax) == mmEnc::FULL_MASK) {
                            ++mmData;
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<uint32_t*>(mmData) - config.target.template begin<uint32_t>(), iteration);
                        }
                    }
                }
                // here follows the non-unrolled remainder
                while (mmData <= (mmDataEnd - 1)) {
                    auto mmInDec = _mm_mullo_epi32(_mm_lddqu_si128(mmData), mmAInv);
                    if (mmEncLE::cmp_mask(mmInDec, mmDMax) == mmEnc::FULL_MASK) {
                        ++mmData;
                    } else {
                        throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<uint32_t*>(mmData) - config.target.template begin<uint32_t>(), iteration);
                    }
                }
                if (mmData < mmDataEnd) {
                    auto dataEnd2 = reinterpret_cast<uint32_t*>(mmDataEnd);
                    auto data2 = reinterpret_cast<uint32_t*>(mmData);
                    while (data2 < dataEnd2) {
                        if ((*data2 * this->A_INV) <= dMax) {
                            ++data2;
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, data2 - config.target.template begin<uint32_t>(), iteration);
                        }
                    }
                }
            }
        }

        bool DoArithmeticChecked(
                const ArithmeticConfiguration & config) override {
            return std::visit(ArithmeticSelector(), config.mode);
        }

        struct ArithmetorChecked {
            AN_sse42_8x16_8x32_u_inv & test;
            const ArithmeticConfiguration & config;
            const size_t iteration;
            ArithmetorChecked(
                    AN_sse42_8x16_8x32_u_inv & test,
                    const ArithmeticConfiguration & config,
                    const size_t iteration)
                    : test(test),
                      config(config),
                      iteration(iteration) {
            }
            template<template<typename = void> class Functor>
            void impl() {
                uint32_t dMax = std::numeric_limits<uint16_t>::max();
                __m128i mmDMax = mm<__m128i, uint32_t>::set1(dMax); // we assume 16-bit input data
                __m128i mmAInv = mm<__m128i, uint32_t>::set1(test.A_INV);
                auto mmData = config.source.template begin<__m128i >();
                const auto mmDataEnd = config.source.template end<__m128i >();
                auto mmOut = config.target.template begin<__m128i >();
                uint32_t operand = config.operand;
                if constexpr (std::is_same_v<Functor<void>, add<void>> || std::is_same_v<Functor<void>, sub<void>> || std::is_same_v<Functor<void>, div<void>>) {
                    operand = config.operand * test.A;
                } else if constexpr (std::is_same_v<Functor<void>, mul<void>>) {
                    // do not encode operand here, otherwise we will have non-code values after the operation!
                } else {
                    throw std::runtime_error("Functor not known!");
                }
                __m128i mmOperand = mm<__m128i, uint32_t>::set1(operand);
                __m128i mmA = mm<__m128i, uint32_t>::set1(test.A);
                while (mmData <= (mmDataEnd - UNROLL)) {
                    // let the compiler unroll the loop
                    for (size_t unroll = 0; unroll < UNROLL; ++unroll) {
                        auto mmIn = _mm_lddqu_si128(mmData++);
                        auto mmInDec = mm_op<__m128i, uint32_t, mul>::compute(mmIn, mmAInv);
                        if (mmEncLE::cmp_mask(mmInDec, mmDMax) == mmEnc::FULL_MASK) {
                            auto x = mm_op<__m128i, uint32_t, Functor>::compute(mmIn, mmOperand);
                            if (std::is_same_v<Functor<void>, div<void>>) {
                                x = mm_op<__m128i, uint32_t, mul>::compute(x, mmA); // make sure we get a code word again
                            }
                            _mm_storeu_si128(mmOut++, x);
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<uint32_t*>(mmData) - config.source.template begin<uint32_t>(), iteration);
                        }
                    }
                }
                // remaining numbers
                while (mmData <= (mmDataEnd - 1)) {
                    auto mmIn = _mm_lddqu_si128(mmData++);
                    auto mmInDec = mm_op<__m128i, uint32_t, mul>::compute(mmIn, mmAInv);
                    if (mmEncLE::cmp_mask(mmInDec, mmDMax) == mmEnc::FULL_MASK) {
                        auto x = mm_op<__m128i, uint32_t, Functor>::compute(mmIn, mmOperand);
                        if (std::is_same_v<Functor<void>, div<void>>) {
                            x = mm_op<__m128i, uint32_t, mul>::compute(x, mmA); // make sure we get a code word again
                        }
                        _mm_storeu_si128(mmOut++, x);
                    } else {
                        throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<uint32_t*>(mmData) - config.source.template begin<uint32_t>(), iteration);
                    }
                }
                if (mmData < mmDataEnd) {
                    Functor<> functor;
                    auto data = reinterpret_cast<uint32_t*>(mmData);
                    auto dataEnd = reinterpret_cast<uint32_t*>(mmDataEnd);
                    auto out = reinterpret_cast<uint32_t*>(mmOut);
                    for (; data < dataEnd; ++data) {
                        auto tmp = *data;
                        if ((tmp * test.A_INV) <= dMax) {
                            auto x = functor(tmp, operand);
                            if (std::is_same_v<Functor<void>, div<void>>) {
                                x *= test.A; // make sure we get a code word again
                            }
                            *out = x;
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, data - config.source.template begin<uint32_t>(), iteration);
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

        bool DoAggregateChecked(
                const AggregateConfiguration & config) override {
            return std::visit(AggregateSelector(), config.mode);
        }

        struct AggregatorChecked {
            typedef typename Larger<uint32_t>::larger_t larger_t;
            AN_sse42_8x16_8x32_u_inv & test;
            const AggregateConfiguration & config;
            size_t iteration;
            AggregatorChecked(
                    AN_sse42_8x16_8x32_u_inv & test,
                    const AggregateConfiguration & config,
                    size_t iteration)
                    : test(test),
                      config(config),
                      iteration(iteration) {
            }
            template<typename Aggregate, typename InitializeVector, typename KernelVector, typename KernelScalar, typename VectorToScalar, typename Finalize>
            void impl(
                    InitializeVector && funcInitVector,
                    KernelVector && funcKernelVector,
                    VectorToScalar && funcVectorToScalar,
                    KernelScalar && funcKernelScalar,
                    Finalize && funcFinal) {
                uint32_t dMax = std::numeric_limits<uint16_t>::max();
                __m128i mmDMax = mm<__m128i, uint32_t>::set1(dMax); // we assume 16-bit input data
                __m128i mmAInv = mm<__m128i, uint32_t>::set1(test.A_INV);
                auto *mmData = config.source.template begin<__m128i >();
                auto * const mmDataEnd = config.source.template end<__m128i >();
                auto mmValue = funcInitVector();
                while (mmData <= (mmDataEnd - UNROLL)) {
                    for (size_t k = 0; k < UNROLL; ++k) {
                        auto mmIn = _mm_lddqu_si128(mmData++);
                        auto mmInDec = mm_op<__m128i, uint32_t, mul>::compute(mmIn, mmAInv);
                        if (mmEncLE::cmp_mask(mmInDec, mmDMax) == mmEnc::FULL_MASK) {
                            mmValue = funcKernelVector(mmValue, mmIn);
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<uint32_t*>(mmData) - config.source.template begin<uint32_t>(), iteration);
                        }
                    }
                }
                while (mmData <= (mmDataEnd - 1)) {
                    auto mmIn = _mm_lddqu_si128(mmData++);
                    auto mmInDec = mm_op<__m128i, uint32_t, mul>::compute(mmIn, mmAInv);
                    if (mmEncLE::cmp_mask(mmInDec, mmDMax) == mmEnc::FULL_MASK) {
                        mmValue = funcKernelVector(mmValue, mmIn);
                    } else {
                        throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<uint32_t*>(mmData) - config.source.template begin<uint32_t>(), iteration);
                    }
                }
                Aggregate value = funcVectorToScalar(mmValue);
                if (mmData < mmDataEnd) {
                    auto data = reinterpret_cast<uint32_t*>(mmData);
                    const auto dataEnd = reinterpret_cast<uint32_t*>(mmDataEnd);
                    for (; data < dataEnd; ++data) {
                        auto tmp = *data * test.A_INV;
                        if (tmp <= dMax) {
                            value = funcKernelScalar(value, *data);
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, data - config.source.template begin<uint32_t>(), iteration);
                        }
                    }
                }
                auto dataOut = config.target.template begin<Aggregate>();
                *dataOut = funcFinal(value, config.numValues);
            }
            void operator()(
                    AggregateConfiguration::Sum) {
                impl<uint32_t>([] {return simd::mm<__m128i, uint32_t>::set1(0);}, [](__m128i mmValue, __m128i mmTmp) {return simd::mm_op<__m128i, uint32_t, add>::compute(mmValue, mmTmp);},
                        [](__m128i mmValue) {return simd::mm<__m128i, uint32_t>::sum(mmValue);}, [](uint32_t value, uint32_t tmp) {return value + tmp;},
                        [](uint32_t value, size_t numValues) {return value;});
            }
            void operator()(
                    AggregateConfiguration::Min) {
                impl<uint32_t>([] {return simd::mm<__m128i, uint32_t>::set1(std::numeric_limits<uint32_t>::max());},
                        [](__m128i mmValue, __m128i mmTmp) {return simd::mm<__m128i, uint32_t>::min(mmValue, mmTmp);}, [](__m128i mmValue) {return simd::mm<__m128i, uint32_t>::min(mmValue);},
                        [](uint32_t value, uint32_t tmp) {return value < tmp ? value : tmp;}, [](uint32_t value, size_t numValues) {return value;});
            }
            void operator()(
                    AggregateConfiguration::Max) {
                impl<uint32_t>([] {return simd::mm<__m128i, uint32_t>::set1(std::numeric_limits<uint32_t>::min());},
                        [](__m128i mmValue, __m128i mmTmp) {return simd::mm<__m128i, uint32_t>::max(mmValue, mmTmp);}, [](__m128i mmValue) {return simd::mm<__m128i, uint32_t>::max(mmValue);},
                        [](uint32_t value, uint32_t tmp) {return value > tmp ? value : tmp;}, [](uint32_t value, size_t numValues) {return value;});
            }
            void operator()(
                    AggregateConfiguration::Avg) {
                impl<uint32_t>([] {return simd::mm<__m128i, uint32_t>::set1(0);}, [](__m128i mmValue, __m128i mmTmp) {return simd::mm_op<__m128i, uint32_t, add>::compute(mmValue, mmTmp);},
                        [](__m128i mmValue) {return simd::mm<__m128i, uint32_t>::sum(mmValue);}, [](uint32_t value, uint32_t tmp) {return value + tmp;},
                        [this](uint32_t value, size_t numValues) {return (value / (numValues * test.A)) * test.A;});
            }
        };

        void RunAggregateChecked(
                const AggregateConfiguration & config) override {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                _ReadWriteBarrier();
                std::visit(AggregatorChecked(*this, config, iteration), config.mode);
            }
        }

        bool DoDecodeChecked() override {
            return true;
        }

        void RunDecodeChecked(
                const DecodeConfiguration & config) override {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                size_t i = 0;
                auto dataIn = config.source.template begin<__m128i >();
                auto dataOut = config.target.template begin<int64_t>();
                uint32_t dMax = std::numeric_limits<uint16_t>::max();
                __m128i mmDMax = mm<__m128i, uint32_t>::set1(dMax); // we assume 16-bit input data
                __m128i mmAInv = mm<__m128i, uint32_t>::set1(this->A_INV);
                auto mmShuffle = _mm_set_epi64x(0xFFFFFFFFFFFFFFFF, 0x0D0C090805040100);
                for (; i <= (config.numValues - NUM_VALUES_PER_UNROLL); i += NUM_VALUES_PER_UNROLL) {
                    // let the compiler unroll the loop
                    for (size_t unroll = 0; unroll < UNROLL; ++unroll) {
                        auto mmIn = _mm_lddqu_si128(dataIn++);
                        auto mmInDec = mm_op<__m128i, uint32_t, mul>::compute(mmIn, mmAInv);
                        if (mmEncLE::cmp_mask(mmInDec, mmDMax) == mmEnc::FULL_MASK) {
                            *dataOut++ = _mm_extract_epi64(_mm_shuffle_epi8(mmInDec, mmShuffle), 0);
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<uint32_t*>(dataIn - 1) - config.source.template begin<uint32_t>(), iteration);
                        }
                    }
                }
                // remaining numbers
                for (; i <= (config.numValues - NUM_VALUES_PER_SIMDREG); i += NUM_VALUES_PER_SIMDREG) {
                    auto mmIn = _mm_lddqu_si128(dataIn++);
                    auto mmInDec = mm_op<__m128i, uint32_t, mul>::compute(mmIn, mmAInv);
                    if (mmEncLE::cmp_mask(mmInDec, mmDMax) == mmEnc::FULL_MASK) {
                        *dataOut++ = _mm_extract_epi64(_mm_shuffle_epi8(mmInDec, mmShuffle), 0);
                    } else {
                        throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<uint32_t*>(dataIn - 1) - config.source.template begin<uint32_t>(), iteration);
                    }
                }
                if (i < config.numValues) {
                    auto out = reinterpret_cast<uint16_t*>(dataOut);
                    auto in = reinterpret_cast<uint32_t*>(dataIn);
                    for (; i < config.numValues; ++i) {
                        auto tmp = *in++ * this->A_INV;
                        if (tmp <= dMax) {
                            *out++ = tmp;
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, in - config.source.template begin<uint32_t>(), iteration);
                        }
                    }
                }
            }
        }
    };

}
