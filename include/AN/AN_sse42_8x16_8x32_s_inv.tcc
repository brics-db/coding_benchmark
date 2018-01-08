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
    struct AN_sse42_8x16_8x32_s_inv :
            public AN_sse42_8x16_8x32_inv<int16_t, int32_t, UNROLL> {

        using AN_sse42_8x16_8x32_inv<int16_t, int32_t, UNROLL>::NUM_VALUES_PER_UNROLL;
        using AN_sse42_8x16_8x32_inv<int16_t, int32_t, UNROLL>::NUM_VALUES_PER_SIMDREG;

        using AN_sse42_8x16_8x32_inv<int16_t, int32_t, UNROLL>::AN_sse42_8x16_8x32_inv;

        virtual ~AN_sse42_8x16_8x32_s_inv() {
        }

        virtual bool DoCheck() override {
            return true;
        }

        virtual void RunCheck(
                const CheckConfiguration & config) override {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                auto data = this->bufEncoded.template begin<__m128i >();
                auto dataEnd = this->bufEncoded.template end<__m128i >();
                int32_t dMin = std::numeric_limits<int16_t>::min();
                int32_t dMax = std::numeric_limits<int16_t>::max();
                __m128i mmDMin = _mm_set1_epi32(dMin); // we assume 16-bit input data
                __m128i mmDMax = _mm_set1_epi32(dMax); // we assume 16-bit input data
                __m128i mmAInv = _mm_set1_epi32(this->A_INV);
                while (data <= (dataEnd - UNROLL)) {
                    // let the compiler unroll the loop
                    for (size_t k = 0; k < UNROLL; ++k) {
                        auto mmIn = _mm_mullo_epi32(_mm_lddqu_si128(data), mmAInv);
                        if (_mm_movemask_epi8(_mm_cmplt_epi32(mmIn, mmDMin)) | _mm_movemask_epi8(_mm_cmpgt_epi32(mmIn, mmDMax))) {
                            throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<int32_t*>(data) - this->bufEncoded.template begin<int32_t>(), iteration);
                        }
                        ++data;
                    }
                }
                // here follows the non-unrolled remainder
                while (data <= (dataEnd - 1)) {
                    auto mmIn = _mm_mullo_epi32(_mm_lddqu_si128(data), mmAInv);
                    if (_mm_movemask_epi8(_mm_cmplt_epi32(mmIn, mmDMin)) | _mm_movemask_epi8(_mm_cmpgt_epi32(mmIn, mmDMax))) {
                        throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<int32_t*>(data) - this->bufEncoded.template begin<int32_t>(), iteration);
                    }
                    ++data;
                }
                if (data < dataEnd) {
                    auto dataEnd2 = reinterpret_cast<int32_t*>(dataEnd);
                    for (auto data2 = reinterpret_cast<int32_t*>(data); data2 < dataEnd2; ++data2) {
                        auto data = *data2 * this->A_INV;
                        if (data < dMin || data > dMax) {
                            throw ErrorInfo(__FILE__, __LINE__, data2 - this->bufEncoded.template begin<int32_t>(), iteration);
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
            AN_sse42_8x16_8x32_s_inv & test;
            const ArithmeticConfiguration & config;
            const size_t iteration;
            ArithmetorChecked(
                    AN_sse42_8x16_8x32_s_inv & test,
                    const ArithmeticConfiguration & config,
                    const size_t iteration)
                    : test(test),
                      config(config),
                      iteration(iteration) {
            }
            template<template<typename = void> class func>
            void impl() {
                int32_t dMin = std::numeric_limits<int16_t>::min();
                int32_t dMax = std::numeric_limits<int16_t>::max();
                __m128i mmDMin = mm<__m128i, int32_t>::set1(dMin); // we assume 16-bit input data
                __m128i mmDMax = mm<__m128i, int32_t>::set1(dMax); // we assume 16-bit input data
                __m128i mmAInv = mm<__m128i, int32_t>::set1(test.A_INV);
                auto mmData = test.bufEncoded.template begin<__m128i >();
                auto const mmDataEnd = test.bufEncoded.template end<__m128i >();
                auto mmOut = test.bufResult.template begin<__m128i >();
                int32_t operandEnc = config.operand * test.A;
                auto mmOperandEnc = mm<__m128i, int32_t>::set1(operandEnc);
                while (mmData <= (mmDataEnd - UNROLL)) {
                    // let the compiler unroll the loop
                    for (size_t unroll = 0; unroll < UNROLL; ++unroll) {
                        auto mmIn = _mm_lddqu_si128(mmData);
                        auto mmInDec = mm_op<__m128i, int32_t, mul>::compute(mmIn, mmAInv);
                        if (mm_op<__m128i, int32_t, std::greater_equal>::cmp_mask(mmInDec, mmDMin) && mm_op<__m128i, int32_t, std::less_equal>::cmp_mask(mmInDec, mmDMax)) {
                            _mm_storeu_si128(mmOut++, mm_op<__m128i, int32_t, func>::compute(_mm_lddqu_si128(mmData++), mmOperandEnc));
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<int32_t*>(mmData) - test.bufEncoded.template begin<int32_t>(), iteration);
                        }
                    }
                }
                // remaining numbers
                while (mmData <= (mmDataEnd - 1)) {
                    auto mmIn = _mm_lddqu_si128(mmData);
                    auto mmInDec = mm_op<__m128i, int32_t, mul>::compute(mmIn, mmAInv);
                    if (mm_op<__m128i, int32_t, std::greater_equal>::cmp_mask(mmInDec, mmDMin) && mm_op<__m128i, int32_t, std::less_equal>::cmp_mask(mmInDec, mmDMax)) {
                        _mm_storeu_si128(mmOut++, mm_op<__m128i, int32_t, func>::compute(_mm_lddqu_si128(mmData++), mmOperandEnc));
                    } else {
                        throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<int32_t*>(mmData) - test.bufEncoded.template begin<int32_t>(), iteration);
                    }
                }
                if (mmData < mmDataEnd) {
                    func<> functor;
                    auto data = reinterpret_cast<int32_t*>(mmData);
                    auto dataEnd = reinterpret_cast<int32_t*>(mmDataEnd);
                    auto out = reinterpret_cast<int32_t*>(mmOut);
                    while (data < dataEnd) {
                        auto tmp = *data * test.A_INV;
                        if ((tmp >= dMin) & (tmp <= dMax)) {
                            *out++ = functor(*data++, operandEnc);
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, data - test.bufEncoded.template begin<int32_t>(), iteration);
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
            AN_sse42_8x16_8x32_s_inv & test;
            const AggregateConfiguration & config;
            size_t iteration;
            AggregatorChecked(
                    AN_sse42_8x16_8x32_s_inv & test,
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
                int32_t dMin = std::numeric_limits<int16_t>::min();
                int32_t dMax = std::numeric_limits<int16_t>::max();
                __m128i mmDMin = mm<__m128i, int32_t>::set1(dMin); // we assume 16-bit input data
                __m128i mmDMax = mm<__m128i, int32_t>::set1(dMax); // we assume 16-bit input data
                __m128i mmAInv = mm<__m128i, int32_t>::set1(test.A_INV);
                const size_t numValues = test.template getNumValues();
                auto *mmData = test.bufEncoded.template begin<__m128i >();
                auto * const mmDataEnd = test.bufEncoded.template end<__m128i >();
                auto mmValue = funcInitVector();
                while (mmData <= (mmDataEnd - UNROLL)) {
                    for (size_t k = 0; k < UNROLL; ++k) {
                        auto mmIn = _mm_lddqu_si128(mmData++);
                        auto mmInDec = mm_op<__m128i, int32_t, mul>::compute(mmIn, mmAInv);
                        if (mm_op<__m128i, int32_t, std::greater_equal>::cmp_mask(mmInDec, mmDMin) && mm_op<__m128i, int32_t, std::less_equal>::cmp_mask(mmInDec, mmDMax)) {
                            mmValue = funcKernelVector(mmValue, mmIn);
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<int32_t*>(mmData) - test.bufEncoded.template begin<int32_t>(), iteration);
                        }
                    }
                }
                while (mmData <= (mmDataEnd - 1)) {
                    auto mmIn = _mm_lddqu_si128(mmData++);
                    auto mmInDec = mm_op<__m128i, int32_t, mul>::compute(mmIn, mmAInv);
                    if (mm_op<__m128i, int32_t, std::greater_equal>::cmp_mask(mmInDec, mmDMin) && mm_op<__m128i, int32_t, std::less_equal>::cmp_mask(mmInDec, mmDMax)) {
                        mmValue = funcKernelVector(mmValue, mmIn);
                    } else {
                        throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<int32_t*>(mmData) - test.bufEncoded.template begin<int32_t>(), iteration);
                    }
                }
                Aggregate value = funcVectorToScalar(mmValue);
                if (mmData < mmDataEnd) {
                    auto data = reinterpret_cast<int32_t*>(mmData);
                    auto const dataEnd = reinterpret_cast<int32_t*>(mmDataEnd);
                    while (data < dataEnd) {
                        auto tmp = *data * test.A_INV;
                        if ((tmp >= dMin) & (tmp <= dMax)) {
                            value = funcKernelScalar(value, *data++);
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, data - test.bufEncoded.template begin<int32_t>(), iteration);
                        }
                    }
                }
                auto dataOut = test.bufResult.template begin<Aggregate>();
                *dataOut = funcFinal(value, numValues);
            }
            void operator()(
                    AggregateConfiguration::Sum) {
                impl<int32_t>([] {return simd::mm<__m128i, int32_t>::set1(0);}, [](__m128i mmValue, __m128i mmTmp) {return simd::mm_op<__m128i, int32_t, add>::compute(mmValue, mmTmp);},
                        [](__m128i mmValue) {return simd::mm<__m128i, int32_t>::sum(mmValue);}, [](int32_t value, int32_t tmp) {return value + tmp;},
                        [](int32_t value, size_t numValues) {return value;});
            }
            void operator()(
                    AggregateConfiguration::Min) {
                impl<int32_t>([] {return simd::mm<__m128i, int32_t>::set1(std::numeric_limits<int32_t>::max());},
                        [](__m128i mmValue, __m128i mmTmp) {return simd::mm<__m128i, int32_t>::min(mmValue, mmTmp);}, [](__m128i mmValue) {return simd::mm<__m128i, int32_t>::min(mmValue);},
                        [](int32_t value, int32_t tmp) {return value < tmp ? value : tmp;}, [](int32_t value, size_t numValues) {return value;});
            }
            void operator()(
                    AggregateConfiguration::Max) {
                impl<int32_t>([] {return simd::mm<__m128i, int32_t>::set1(std::numeric_limits<int32_t>::min());},
                        [](__m128i mmValue, __m128i mmTmp) {return simd::mm<__m128i, int32_t>::max(mmValue, mmTmp);}, [](__m128i mmValue) {return simd::mm<__m128i, int32_t>::max(mmValue);},
                        [](int32_t value, int32_t tmp) {return value > tmp ? value : tmp;}, [](int32_t value, size_t numValues) {return value;});
            }
            void operator()(
                    AggregateConfiguration::Avg) {
                impl<int32_t>([] {return simd::mm<__m128i, int32_t>::set1(0);}, [](__m128i mmValue, __m128i mmTmp) {return simd::mm_op<__m128i, int32_t, add>::compute(mmValue, mmTmp);},
                        [](__m128i mmValue) {return simd::mm<__m128i, int32_t>::sum(mmValue);}, [](int32_t value, int32_t tmp) {return value + tmp;},
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
                size_t numValues = this->template getNumValues();
                size_t i = 0;
                auto dataIn = this->bufEncoded.template begin<__m128i >();
                auto dataOut = this->bufResult.template begin<int64_t>();
                int32_t dMin = std::numeric_limits<int16_t>::min();
                int32_t dMax = std::numeric_limits<int16_t>::max();
                __m128i mmDMin = mm<__m128i, int32_t>::set1(dMin); // we assume 16-bit input data
                __m128i mmDMax = mm<__m128i, int32_t>::set1(dMax); // we assume 16-bit input data
                __m128i mmAInv = mm<__m128i, int32_t>::set1(this->A_INV);
                auto mmShuffle = _mm_set_epi64x(0xFFFFFFFFFFFFFFFF, 0x0D0C090805040100);
                for (; i <= (numValues - NUM_VALUES_PER_UNROLL); i += NUM_VALUES_PER_UNROLL) {
                    // let the compiler unroll the loop
                    for (size_t unroll = 0; unroll < UNROLL; ++unroll) {
                        auto mmIn = _mm_lddqu_si128(dataIn++);
                        auto mmInDec = mm_op<__m128i, int32_t, mul>::compute(mmIn, mmAInv);
                        if (mm_op<__m128i, int32_t, std::greater_equal>::cmp_mask(mmInDec, mmDMin) && mm_op<__m128i, int32_t, std::less_equal>::cmp_mask(mmInDec, mmDMax)) {
                            *dataOut++ = _mm_extract_epi64(_mm_shuffle_epi8(mmInDec, mmShuffle), 0);
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<int32_t*>(dataIn - 1) - this->bufEncoded.template begin<int32_t>(), iteration);
                        }
                    }
                }
                // remaining numbers
                for (; i <= (numValues - NUM_VALUES_PER_SIMDREG); i += NUM_VALUES_PER_SIMDREG) {
                    auto mmIn = _mm_lddqu_si128(dataIn++);
                    auto mmInDec = mm_op<__m128i, int32_t, mul>::compute(mmIn, mmAInv);
                    if (mm_op<__m128i, int32_t, std::greater_equal>::cmp_mask(mmInDec, mmDMin) && mm_op<__m128i, int32_t, std::less_equal>::cmp_mask(mmInDec, mmDMax)) {
                        *dataOut++ = _mm_extract_epi64(_mm_shuffle_epi8(mmInDec, mmShuffle), 0);
                    } else {
                        throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<int32_t*>(dataIn - 1) - this->bufEncoded.template begin<int32_t>(), iteration);
                    }
                }
                if (i < numValues) {
                    auto out = reinterpret_cast<int16_t*>(dataOut);
                    auto in = reinterpret_cast<int32_t*>(dataIn);
                    for (; i < numValues; ++i) {
                        auto tmp = *in++ * this->A_INV;
                        if ((tmp >= dMin) & (tmp <= dMax)) {
                            *out++ = tmp;
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, in - this->bufEncoded.template begin<int32_t>(), iteration);
                        }
                    }
                }
            }
        }
    };

}
