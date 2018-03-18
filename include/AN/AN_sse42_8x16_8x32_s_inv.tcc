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

        typedef __m128i VEC;

        typedef AN_sse42_8x16_8x32_inv<int16_t, int32_t, UNROLL> BASE;
        typedef simd::mm<VEC, int32_t> mmEnc;
        typedef simd::mm_op<VEC, int32_t, std::less_equal> mmEncLE;
        typedef simd::mm_op<VEC, int32_t, std::greater_equal> mmEncGE;

        using BASE::AN_sse42_8x16_8x32_inv;
        using BASE::ComputeEnd;

        virtual ~AN_sse42_8x16_8x32_s_inv() {
        }

        virtual bool DoCheck() override {
            return true;
        }

        virtual void RunCheck(
                const CheckConfiguration & config) override {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                _ReadWriteBarrier();
                auto in128 = config.target.template begin<VEC>();
                const auto in128end = this->template ComputeEnd<int32_t>(in128, config);
                const constexpr int32_t d16Min = std::numeric_limits<int16_t>::min();
                const constexpr int32_t d16Max = std::numeric_limits<int16_t>::max();
                VEC mmDMin = _mm_set1_epi32(d16Min);
                VEC mmDMax = _mm_set1_epi32(d16Max);
                VEC mmAInv = _mm_set1_epi32(this->A_INV);
                while (in128 <= (in128end - UNROLL)) {
                    // let the compiler unroll the loop
                    for (size_t k = 0; k < UNROLL; ++k) {
                        auto mmInDec = _mm_mullo_epi32(_mm_lddqu_si128(in128), mmAInv);
                        if ((mmEncGE::cmp_mask(mmInDec, mmDMin) == mmEnc::FULL_MASK) && (mmEncLE::cmp_mask(mmInDec, mmDMax) == mmEnc::FULL_MASK)) {
                            ++in128;
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<int32_t*>(in128) - config.target.template begin<int32_t>(), iteration);
                        }
                    }
                }
                // here follows the non-unrolled remainder
                while (in128 <= (in128end - 1)) {
                    auto mmInDec = _mm_mullo_epi32(*in128, mmAInv);
                    if ((mmEncGE::cmp_mask(mmInDec, mmDMin) == mmEnc::FULL_MASK) && (mmEncLE::cmp_mask(mmInDec, mmDMax) == mmEnc::FULL_MASK)) {
                        ++in128;
                    } else {
                        throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<int32_t*>(in128) - config.target.template begin<int32_t>(), iteration);
                    }
                }
                if (in128 < in128end) {
                    auto in32 = reinterpret_cast<int32_t*>(in128);
                    const auto in32end = reinterpret_cast<int32_t* const >(in128end);
                    while (in32 < in32end) {
                        auto data = *in32 * this->A_INV;
                        if ((data >= d16Min) && (data <= d16Max)) {
                            ++in32;
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, in32 - config.target.template begin<int32_t>(), iteration);
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
            template<template<typename = void> class Functor>
            void impl() {
                auto in128 = config.source.template begin<VEC>();
                const auto in128end = test.template ComputeEnd<int32_t>(in128, config);
                const constexpr int32_t dMin = std::numeric_limits<int16_t>::min();
                const constexpr int32_t dMax = std::numeric_limits<int16_t>::max();
                VEC mmDMin = mm<VEC, int32_t>::set1(dMin); // we assume 16-bit input data
                VEC mmDMax = mm<VEC, int32_t>::set1(dMax); // we assume 16-bit input data
                VEC mmAInv = mm<VEC, int32_t>::set1(test.A_INV);
                auto out128 = config.target.template begin<VEC>();
                uint32_t operand = config.operand;
                if constexpr (std::is_same_v<Functor<void>, add<void>> || std::is_same_v<Functor<void>, sub<void>> || std::is_same_v<Functor<void>, div<void>>) {
                    operand = config.operand * test.A;
                } else if constexpr (std::is_same_v<Functor<void>, mul<void>>) {
                    // do not encode operand here, otherwise we will have non-code values after the operation!
                } else {
                    throw std::runtime_error("Functor not known!");
                }
                VEC mmOperand = mm<VEC, uint32_t>::set1(operand);
                VEC mmA = mm<VEC, uint32_t>::set1(test.A);
                while (in128 <= (in128end - UNROLL)) {
                    // let the compiler unroll the loop
                    for (size_t unroll = 0; unroll < UNROLL; ++unroll) {
                        auto mmIn = *in128++;
                        auto mmInDec = mm_op<VEC, int32_t, mul>::compute(mmIn, mmAInv);
                        if ((mmEncGE::cmp_mask(mmInDec, mmDMin) == mmEnc::FULL_MASK) && (mmEncLE::cmp_mask(mmInDec, mmDMax) == mmEnc::FULL_MASK)) {
                            auto x = mm_op<VEC, uint32_t, Functor>::compute(mmIn, mmOperand);
                            if (std::is_same_v<Functor<void>, div<void>>) {
                                x = mm_op<VEC, uint32_t, mul>::compute(x, mmA); // make sure we get a code word again
                            }
                            _mm_storeu_si128(out128++, x);
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<int32_t*>(in128) - config.source.template begin<int32_t>(), iteration);
                        }
                    }
                }
                // remaining numbers
                while (in128 <= (in128end - 1)) {
                    auto mmIn = *in128++;
                    auto mmInDec = mm_op<VEC, int32_t, mul>::compute(mmIn, mmAInv);
                    if ((mmEncGE::cmp_mask(mmInDec, mmDMin) == mmEnc::FULL_MASK) && (mmEncLE::cmp_mask(mmInDec, mmDMax) == mmEnc::FULL_MASK)) {
                        auto x = mm_op<VEC, uint32_t, Functor>::compute(mmIn, mmOperand);
                        if (std::is_same_v<Functor<void>, div<void>>) {
                            x = mm_op<VEC, uint32_t, mul>::compute(x, mmA); // make sure we get a code word again
                        }
                        _mm_storeu_si128(out128++, x);
                    } else {
                        throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<int32_t*>(in128) - config.source.template begin<int32_t>(), iteration);
                    }
                }
                if (in128 < in128end) {
                    Functor<> functor;
                    auto in32 = reinterpret_cast<int32_t*>(in128);
                    const auto in32end = reinterpret_cast<int32_t* const >(in128end);
                    auto out32 = reinterpret_cast<int32_t*>(out128);
                    while (in32 < in32end) {
                        auto tmp = *in32 * test.A_INV;
                        if ((tmp >= dMin) & (tmp <= dMax)) {
                            auto x = functor(*in32++, operand);
                            if (std::is_same_v<Functor<void>, div<void>>) {
                                x *= test.A; // make sure we get a code word again
                            }
                            *out32++ = x;
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, in32 - config.source.template begin<int32_t>(), iteration);
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
                auto in128 = config.source.template begin<VEC>();
                const auto in128end = test.template ComputeEnd<int32_t>(in128, config);
                const constexpr int32_t d16Min = std::numeric_limits<int16_t>::min();
                const constexpr int32_t d16Max = std::numeric_limits<int16_t>::max();
                VEC mmDMin = mm<VEC, int32_t>::set1(d16Min); // we assume 16-bit input data
                VEC mmDMax = mm<VEC, int32_t>::set1(d16Max); // we assume 16-bit input data
                VEC mmAInv = mm<VEC, int32_t>::set1(test.A_INV);
                auto mmValue = funcInitVector();
                while (in128 <= (in128end - UNROLL)) {
                    for (size_t k = 0; k < UNROLL; ++k) {
                        auto mmIn = *in128++;
                        auto mmInDec = mm_op<VEC, int32_t, mul>::compute(mmIn, mmAInv);
                        if ((mmEncGE::cmp_mask(mmInDec, mmDMin) == mmEnc::FULL_MASK) && (mmEncLE::cmp_mask(mmInDec, mmDMax) == mmEnc::FULL_MASK)) {
                            mmValue = funcKernelVector(mmValue, mmIn);
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<int32_t*>(in128) - config.source.template begin<int32_t>(), iteration);
                        }
                    }
                }
                while (in128 <= (in128end - 1)) {
                    auto mmIn = *in128++;
                    auto mmInDec = mm_op<VEC, int32_t, mul>::compute(mmIn, mmAInv);
                    if ((mmEncGE::cmp_mask(mmInDec, mmDMin) == mmEnc::FULL_MASK) && (mmEncLE::cmp_mask(mmInDec, mmDMax) == mmEnc::FULL_MASK)) {
                        mmValue = funcKernelVector(mmValue, mmIn);
                    } else {
                        throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<int32_t*>(in128) - config.source.template begin<int32_t>(), iteration);
                    }
                }
                Aggregate value = funcVectorToScalar(mmValue);
                if (in128 < in128end) {
                    auto in32 = reinterpret_cast<int32_t*>(in128);
                    const auto in32end = reinterpret_cast<int32_t* const >(in128end);
                    while (in32 < in32end) {
                        auto tmp = *in32 * test.A_INV;
                        if ((tmp >= d16Min) & (tmp <= d16Max)) {
                            value = funcKernelScalar(value, *in32++);
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, in32 - config.source.template begin<int32_t>(), iteration);
                        }
                    }
                }
                auto final = funcFinal(value, config.numValues);
                auto dataOut = test.bufScratchPad.template begin<Aggregate>();
                *dataOut = final / test.A; // decode here, because we encode again in the next step. This is currently required!
                EncodeConfiguration encConf(1, 2, test.bufScratchPad, config.target);
                test.RunEncode(encConf);
            }
            void operator()(
                    AggregateConfiguration::Sum) {
                impl<larger_t>([] {return simd::mm<VEC, larger_t>::set1(0);}, [](VEC mmSum, VEC mmTmp) {
                    auto mmLo = simd::mm<VEC, int32_t>::cvt_larger_lo(mmTmp);
                    mmLo = simd::mm_op<VEC, larger_t, add>::compute(mmSum, mmLo);
                    auto mmHi = simd::mm<VEC, int32_t>::cvt_larger_hi(mmTmp);
                    return simd::mm_op<VEC, larger_t, add>::compute(mmLo, mmHi);
                }, [](VEC mmSum) {return simd::mm<VEC, larger_t>::sum(mmSum);}, [](larger_t sum, int32_t tmp) {return sum + tmp;}, [](larger_t sum, size_t numValues) {return sum;});
            }
            void operator()(
                    AggregateConfiguration::Min) {
                impl<int32_t>([] {return simd::mm<VEC, int32_t>::set1(std::numeric_limits<int32_t>::max());}, [](VEC mmMin, VEC mmTmp) {return simd::mm<VEC, int32_t>::min(mmMin, mmTmp);},
                        [](VEC mmMin) {return simd::mm<VEC, int32_t>::min(mmMin);}, [](int32_t min, int32_t tmp) {return min < tmp ? min : tmp;}, [](int32_t min, size_t numValues) {return min;});
            }
            void operator()(
                    AggregateConfiguration::Max) {
                impl<int32_t>([] {return simd::mm<VEC, int32_t>::set1(std::numeric_limits<int32_t>::min());}, [](VEC mmMax, VEC mmTmp) {return simd::mm<VEC, int32_t>::max(mmMax, mmTmp);},
                        [](VEC mmMax) {return simd::mm<VEC, int32_t>::max(mmMax);}, [](int32_t max, int32_t tmp) {return max > tmp ? max : tmp;}, [](int32_t max, size_t numValues) {return max;});
            }
            void operator()(
                    AggregateConfiguration::Avg) {
                impl<larger_t>([] {return simd::mm<VEC, larger_t>::set1(0);}, [](VEC mmSum, VEC mmTmp) {
                    auto mmLo = simd::mm<VEC, int32_t>::cvt_larger_lo(mmTmp);
                    mmLo = simd::mm_op<VEC, larger_t, add>::compute(mmSum, mmLo);
                    auto mmHi = simd::mm<VEC, int32_t>::cvt_larger_hi(mmTmp);
                    return simd::mm_op<VEC, larger_t, add>::compute(mmLo, mmHi);
                }, [](VEC mmSum) {return simd::mm<VEC, larger_t>::sum(mmSum);}, [](larger_t sum, int32_t tmp) {return sum + tmp;},
                        [this](larger_t sum, size_t numValues) {return (sum / (numValues * test.A)) * test.A;});
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
                auto in128 = config.source.template begin<VEC>();
                const auto in128end = this->template ComputeEnd<int32_t>(in128, config);
                auto out64 = config.target.template begin<int64_t>();
                const constexpr int32_t dMin = std::numeric_limits<int16_t>::min();
                const constexpr int32_t dMax = std::numeric_limits<int16_t>::max();
                VEC mmDMin = mm<VEC, int32_t>::set1(dMin); // we assume 16-bit input data
                VEC mmDMax = mm<VEC, int32_t>::set1(dMax); // we assume 16-bit input data
                VEC mmAInv = mm<VEC, int32_t>::set1(this->A_INV);
                auto mmShuffle = _mm_set_epi64x(0xFFFFFFFFFFFFFFFF, 0x0D0C090805040100);
                while (in128 <= (in128end - UNROLL)) {
                    // let the compiler unroll the loop
                    for (size_t unroll = 0; unroll < UNROLL; ++unroll) {
                        auto mmInDec = mm_op<VEC, int32_t, mul>::compute(*in128++, mmAInv);
                        if ((mmEncGE::cmp_mask(mmInDec, mmDMin) == mmEnc::FULL_MASK) && (mmEncLE::cmp_mask(mmInDec, mmDMax) == mmEnc::FULL_MASK)) {
                            *out64++ = _mm_extract_epi64(_mm_shuffle_epi8(mmInDec, mmShuffle), 0);
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<int32_t*>(in128 - 1) - config.source.template begin<int32_t>(), iteration);
                        }
                    }
                }
                // remaining numbers
                while (in128 <= (in128end - 1)) {
                    auto mmInDec = mm_op<VEC, int32_t, mul>::compute(*in128++, mmAInv);
                    if ((mmEncGE::cmp_mask(mmInDec, mmDMin) == mmEnc::FULL_MASK) && (mmEncLE::cmp_mask(mmInDec, mmDMax) == mmEnc::FULL_MASK)) {
                        *out64++ = _mm_extract_epi64(_mm_shuffle_epi8(mmInDec, mmShuffle), 0);
                    } else {
                        throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<int32_t*>(in128 - 1) - config.source.template begin<int32_t>(), iteration);
                    }
                }
                if (in128 < in128end) {
                    auto in32 = reinterpret_cast<int32_t*>(in128);
                    const auto in32end = reinterpret_cast<int32_t* const >(in128end);
                    auto out16 = reinterpret_cast<int16_t*>(out64);
                    while (in32 < in32end) {
                        auto tmp = *in32++ * this->A_INV;
                        if ((tmp >= dMin) & (tmp <= dMax)) {
                            *out16++ = tmp;
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, in32 - config.source.template begin<int32_t>(), iteration);
                        }
                    }
                }
            }
        }
    };

}
