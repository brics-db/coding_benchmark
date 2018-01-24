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

#ifndef AN_AVX2
#error "Clients must not include this file directly, but file <AN/AN_avx2.hpp>!"
#endif

#include <AN/AN_avx2_16x16_16x32_inv.tcc>

namespace coding_benchmark {

    template<size_t UNROLL>
    struct AN_avx2_16x16_16x32_s_inv :
            public AN_avx2_16x16_16x32_inv<int16_t, int32_t, UNROLL> {

        typedef AN_avx2_16x16_16x32_inv<int16_t, int32_t, UNROLL> BASE;
        typedef simd::mm<__m256i, int32_t> mmEnc;
        typedef simd::mm_op<__m256i, int32_t, std::less_equal> mmEncLE;
        typedef simd::mm_op<__m256i, int32_t, std::greater_equal> mmEncGE;

        using BASE::NUM_VALUES_PER_SIMDREG;
        using BASE::NUM_VALUES_PER_UNROLL;

        using BASE::AN_avx2_16x16_16x32_inv;

        virtual ~AN_avx2_16x16_16x32_s_inv() {
        }

        virtual bool DoCheck() override {
            return true;
        }

        virtual void RunCheck(
                const CheckConfiguration & config) override {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                _ReadWriteBarrier();
                auto mmData = this->bufEncoded.template begin<__m256i >();
                auto mmDataEnd = this->bufEncoded.template end<__m256i >();
                int32_t dMin = std::numeric_limits<int16_t>::min(); // we assume 16-bit input data
                int32_t dMax = std::numeric_limits<int16_t>::max(); // we assume 16-bit input data
                __m256i mmDMin = mm<__m256i, int32_t>::set1(dMin);
                __m256i mmDMax = mm<__m256i, int32_t>::set1(dMax);
                __m256i mmAInv = mm<__m256i, int32_t>::set1(this->A_INV);
                while (mmData <= (mmDataEnd - UNROLL)) {
                    // let the compiler unroll the loop
                    for (size_t k = 0; k < UNROLL; ++k) {
                        auto mmIn = mm<__m256i, int32_t>::loadu(mmData);
                        auto mmInDec = mm_op<__m256i, int32_t, mul>::compute(mmIn, mmAInv);
                        auto maskDMin = mmEncGE::cmp_mask(mmInDec, mmDMin);
                        auto maskDMax = mmEncLE::cmp_mask(mmInDec, mmDMax);
                        if ((maskDMin == mmEnc::FULL_MASK) && (maskDMax == mmEnc::FULL_MASK)) {
                            ++mmData;
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<int32_t*>(mmData) - this->bufEncoded.template begin<int32_t>(), iteration);
                        }
                    }
                }
                // here follows the non-unrolled remainder
                while (mmData <= (mmDataEnd - 1)) {
                    auto mmIn = mm<__m256i, int32_t>::loadu(mmData);
                    auto mmInDec = mm_op<__m256i, int32_t, mul>::compute(mmIn, mmAInv);
                    auto maskDMin = mmEncGE::cmp_mask(mmInDec, mmDMin);
                    auto maskDMax = mmEncLE::cmp_mask(mmInDec, mmDMax);
                    if ((maskDMin == mmEnc::FULL_MASK) && (maskDMax == mmEnc::FULL_MASK)) {
                        ++mmData;
                    } else {
                        throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<int32_t*>(mmData) - this->bufEncoded.template begin<int32_t>(), iteration);
                    }
                }
                if (mmData < mmDataEnd) {
                    auto data2 = reinterpret_cast<int32_t*>(mmData);
                    auto dataEnd2 = reinterpret_cast<int32_t*>(mmDataEnd);
                    while (data2 < dataEnd2) {
                        auto tmp = *data2 * this->A_INV;
                        if ((tmp >= dMin) && (tmp <= dMax)) {
                            ++data2;
                        } else {
                            std::stringstream ss;
                            ss << " Value:" << *data2;
                            throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<int32_t*>(data2) - this->bufEncoded.template begin<int32_t>(), iteration);
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
            AN_avx2_16x16_16x32_s_inv & test;
            const ArithmeticConfiguration & config;
            const size_t iteration;
            ArithmetorChecked(
                    AN_avx2_16x16_16x32_s_inv & test,
                    const ArithmeticConfiguration & config,
                    const size_t iteration)
                    : test(test),
                      config(config),
                      iteration(iteration) {
            }
            template<template<typename = void> class Functor>
            void impl() {
                int32_t dMin = std::numeric_limits<int16_t>::min(); // we assume 16-bit input data
                int32_t dMax = std::numeric_limits<int16_t>::max(); // we assume 16-bit input data
                __m256i mmDMin = mm<__m256i, int32_t>::set1(dMin);
                __m256i mmDMax = mm<__m256i, int32_t>::set1(dMax);
                __m256i mmAInv = mm<__m256i, int32_t>::set1(test.A_INV);
                auto mmData = test.bufEncoded.template begin<__m256i >();
                const auto mmDataEnd = test.bufEncoded.template end<__m256i >();
                auto mmOut = test.bufResult.template begin<__m256i >();
                int32_t operandEnc = config.operand * test.A;
                auto mmOperandEnc = mm<__m256i, int32_t>::set1(operandEnc);
                while (mmData <= (mmDataEnd - UNROLL)) {
                    // let the compiler unroll the loop
                    for (size_t unroll = 0; unroll < UNROLL; ++unroll) {
                        auto mmIn = _mm256_lddqu_si256(mmData++);
                        auto mmInDec = _mm256_mullo_epi32(mmIn, mmAInv);
                        if ((mmEncGE::cmp_mask(mmInDec, mmDMin) == mmEnc::FULL_MASK) && (mmEncLE::cmp_mask(mmInDec, mmDMax) == mmEnc::FULL_MASK)) {
                            _mm256_storeu_si256(mmOut++, mm_op<__m256i, int32_t, Functor>::compute(mmIn, mmOperandEnc));
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<int32_t*>(mmData) - test.bufEncoded.template begin<int32_t>(), iteration);
                        }
                    }
                }
                // remaining numbers
                while (mmData <= (mmDataEnd - 1)) {
                    auto mmIn = _mm256_lddqu_si256(mmData++);
                    auto mmInDec = _mm256_mullo_epi32(mmIn, mmAInv);
                    if ((mmEncGE::cmp_mask(mmInDec, mmDMin) == mmEnc::FULL_MASK) && (mmEncLE::cmp_mask(mmInDec, mmDMax) == mmEnc::FULL_MASK)) {
                        _mm256_storeu_si256(mmOut++, mm_op<__m256i, int32_t, Functor>::compute(mmIn, mmOperandEnc));
                    } else {
                        throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<int32_t*>(mmData) - test.bufEncoded.template begin<int32_t>(), iteration);
                    }
                }
                if (mmData < mmDataEnd) {
                    Functor<> functor;
                    auto dataIn = reinterpret_cast<int32_t*>(mmData);
                    auto dataInEnd = reinterpret_cast<int32_t*>(mmDataEnd);
                    auto dataOut = reinterpret_cast<int32_t*>(mmOut);
                    while (dataIn < dataInEnd) {
                        auto tmp = *dataIn * test.A_INV;
                        if ((tmp >= dMin) & (tmp <= dMax)) {
                            *dataOut++ = functor(*dataIn++, operandEnc);
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, dataIn - test.bufEncoded.template begin<int32_t>(), iteration);
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
            typedef typename Larger<int32_t>::larger_t larger_t;
            AN_avx2_16x16_16x32_s_inv & test;
            const AggregateConfiguration & config;
            size_t iteration;
            AggregatorChecked(
                    AN_avx2_16x16_16x32_s_inv & test,
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
                int32_t dMin = std::numeric_limits<int16_t>::min(); // we assume 16-bit input data
                int32_t dMax = std::numeric_limits<int16_t>::max(); // we assume 16-bit input data
                __m256i mmDMin = mm<__m256i, int32_t>::set1(dMin);
                __m256i mmDMax = mm<__m256i, int32_t>::set1(dMax);
                __m256i mmAInv = mm<__m256i, int32_t>::set1(test.A_INV);
                const size_t numValues = test.template getNumValues();
                auto *mmData = test.bufEncoded.template begin<__m256i >();
                auto * const mmDataEnd = test.bufEncoded.template end<__m256i >();
                auto mmValue = funcInitVector();
                while (mmData <= (mmDataEnd - UNROLL)) {
                    for (size_t k = 0; k < UNROLL; ++k) {
                        auto mmIn = _mm256_lddqu_si256(mmData++);
                        auto mmInDec = _mm256_mullo_epi32(mmIn, mmAInv);
                        if ((mmEncGE::cmp_mask(mmInDec, mmDMin) == mmEnc::FULL_MASK) && (mmEncLE::cmp_mask(mmInDec, mmDMax) == mmEnc::FULL_MASK)) {
                            mmValue = funcKernelVector(mmValue, mmIn);
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<int32_t*>(mmData) - test.bufEncoded.template begin<int32_t>(), iteration);
                        }
                    }
                }
                while (mmData <= (mmDataEnd - 1)) {
                    auto mmIn = _mm256_lddqu_si256(mmData++);
                    auto mmInDec = _mm256_mullo_epi32(mmIn, mmAInv);
                    if ((mmEncGE::cmp_mask(mmInDec, mmDMin) == mmEnc::FULL_MASK) && (mmEncLE::cmp_mask(mmInDec, mmDMax) == mmEnc::FULL_MASK)) {
                        mmValue = funcKernelVector(mmValue, mmIn);
                    } else {
                        throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<int32_t*>(mmData) - test.bufEncoded.template begin<int32_t>(), iteration);
                    }
                }
                Aggregate value = funcVectorToScalar(mmValue);
                if (mmData < mmDataEnd) {
                    auto dataIn = reinterpret_cast<int32_t*>(mmData);
                    const auto dataInEnd = reinterpret_cast<int32_t*>(mmDataEnd);
                    while (dataIn < dataInEnd) {
                        auto tmp = *dataIn * test.A_INV;
                        if ((tmp >= dMin) & (tmp <= dMax)) {
                            value = funcKernelScalar(value, *dataIn++);
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, dataIn - test.bufEncoded.template begin<int32_t>(), iteration);
                        }
                    }
                }
                auto dataOut = test.bufResult.template begin<Aggregate>();
                *dataOut = funcFinal(value, numValues);
            }
            void operator()(
                    AggregateConfiguration::Sum) {
                impl<int32_t>([] {return simd::mm<__m256i, int32_t>::set1(0);}, [](__m256i mmValue, __m256i mmTmp) {return simd::mm_op<__m256i, int32_t, add>::compute(mmValue, mmTmp);},
                        [](__m256i mmValue) {return simd::mm<__m256i, int32_t>::sum(mmValue);}, [](int32_t value, int32_t tmp) {return value + tmp;},
                        [](int32_t value, size_t numValues) {return value;});
            }
            void operator()(
                    AggregateConfiguration::Min) {
                impl<int32_t>([] {return simd::mm<__m256i, int32_t>::set1(std::numeric_limits<int32_t>::max());},
                        [](__m256i mmValue, __m256i mmTmp) {return simd::mm<__m256i, int32_t>::min(mmValue, mmTmp);}, [](__m256i mmValue) {return simd::mm<__m256i, int32_t>::min(mmValue);},
                        [](int32_t value, int32_t tmp) {return value < tmp ? value : tmp;}, [](int32_t value, size_t numValues) {return value;});
            }
            void operator()(
                    AggregateConfiguration::Max) {
                impl<int32_t>([] {return simd::mm<__m256i, int32_t>::set1(std::numeric_limits<int32_t>::min());},
                        [](__m256i mmValue, __m256i mmTmp) {return simd::mm<__m256i, int32_t>::max(mmValue, mmTmp);}, [](__m256i mmValue) {return simd::mm<__m256i, int32_t>::max(mmValue);},
                        [](int32_t value, int32_t tmp) {return value > tmp ? value : tmp;}, [](int32_t value, size_t numValues) {return value;});
            }
            void operator()(
                    AggregateConfiguration::Avg) {
                impl<int32_t>([] {return simd::mm<__m256i, int32_t>::set1(0);}, [](__m256i mmValue, __m256i mmTmp) {return simd::mm_op<__m256i, int32_t, add>::compute(mmValue, mmTmp);},
                        [](__m256i mmValue) {return simd::mm<__m256i, int32_t>::sum(mmValue);}, [](int32_t value, int32_t tmp) {return value + tmp;},
                        [this](int32_t value, size_t numValues) {return (value / (numValues * test.A)) * test.A;});
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
                _ReadWriteBarrier();
                size_t numValues = this->getNumValues();
                size_t i = 0;
                int32_t dMin = std::numeric_limits<int16_t>::min(); // we assume 16-bit input data
                int32_t dMax = std::numeric_limits<int16_t>::max(); // we assume 16-bit input data
                __m256i mmDMin = mm<__m256i, int32_t>::set1(dMin);
                __m256i mmDMax = mm<__m256i, int32_t>::set1(dMax);
                auto mmData = this->bufEncoded.template begin<__m256i >();
                auto mmOut = this->bufResult.template begin<__m128i >();
                auto mmAInv = _mm256_set1_epi32(this->A_INV);
                auto mmShuffle = _mm256_set_epi64x(0xFFFFFFFFFFFFFFFFll, 0xFFFFFFFFFFFFFFFFll, 0x1D1C191815141110ll, 0x0D0C090805040100ll);
                for (; i <= (numValues - NUM_VALUES_PER_UNROLL); i += NUM_VALUES_PER_UNROLL) {
                    // let the compiler unroll the loop
                    for (size_t unroll = 0; unroll < UNROLL; ++unroll) {
                        auto mmIn = _mm256_lddqu_si256(mmData++);
                        auto mmInDec = _mm256_mullo_epi32(mmIn, mmAInv);
                        if ((mmEncGE::cmp_mask(mmInDec, mmDMin) == mmEnc::FULL_MASK) && (mmEncLE::cmp_mask(mmInDec, mmDMax) == mmEnc::FULL_MASK)) {
                            _mm_storeu_si128(mmOut++, _mm256_extracti128_si256(_mm256_shuffle_epi8(mmInDec, mmShuffle), 0));
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<int32_t*>(mmData) - this->bufEncoded.template begin<int32_t>(), iteration);
                        }
                    }
                }
                // remaining numbers
                for (; i <= (numValues - NUM_VALUES_PER_SIMDREG); i += NUM_VALUES_PER_SIMDREG) {
                    auto mmIn = _mm256_lddqu_si256(mmData++);
                    auto mmInDec = _mm256_mullo_epi32(mmIn, mmAInv);
                    if ((mmEncGE::cmp_mask(mmInDec, mmDMin) == mmEnc::FULL_MASK) && (mmEncLE::cmp_mask(mmInDec, mmDMax) == mmEnc::FULL_MASK)) {
                        _mm_storeu_si128(mmOut++, _mm256_extracti128_si256(_mm256_shuffle_epi8(mmInDec, mmShuffle), 0));
                    } else {
                        throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<int32_t*>(mmData) - this->bufEncoded.template begin<int32_t>(), iteration);
                    }
                }
                if (i < numValues) {
                    auto out16 = reinterpret_cast<int16_t*>(mmOut);
                    auto data32 = reinterpret_cast<int32_t*>(mmData);
                    for (; i < numValues; ++i, ++data32, ++out16) {
                        auto tmp = *data32 * this->A_INV;
                        if ((tmp >= dMin) & (tmp <= dMax)) {
                            *out16 = tmp;
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, data32 - this->bufEncoded.template begin<int32_t>(), iteration);
                        }
                    }
                }
            }
        }
    };

}
