// Copyright (c) 2016-2017 Till Kolditz, Stefan de Bruijn
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

#include <AN/AN_sse42_8x16_8x32.tcc>

namespace coding_benchmark {

    template<typename DATARAW, typename DATAENC, size_t UNROLL>
    struct AN_sse42_8x16_8x32_divmod :
            public AN_sse42_8x16_8x32<DATARAW, DATAENC, UNROLL> {

        typedef AN_sse42_8x16_8x32<DATARAW, DATAENC, UNROLL> BASE;

        using typename BASE::VEC;

        using BASE::AN_sse42_8x16_8x32;

        virtual ~AN_sse42_8x16_8x32_divmod() {
        }

        virtual bool DoCheck() override {
            return true;
        }

        virtual void RunCheck(
                const CheckConfiguration & config) override {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                auto in128 = config.target.template begin<VEC>();
                const auto in128end = this->template ComputeEnd<DATAENC>(in128, config);
                while (in128 <= (in128end - UNROLL)) {
                    // let the compiler unroll the loop
                    for (size_t k = 0; k < UNROLL; ++k) {
                        auto mmIn = _mm_lddqu_si128(in128);
                        if ((_mm_extract_epi32(mmIn, 0) % this->A == 0) && (_mm_extract_epi32(mmIn, 1) % this->A == 0) && (_mm_extract_epi32(mmIn, 2) % this->A == 0)
                                && (_mm_extract_epi32(mmIn, 3) % this->A == 0)) {
                            ++in128;
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATAENC*>(in128) - config.target.template begin<DATAENC>(), iteration);
                        }
                    }
                }
                // here follows the non-unrolled remainder
                while (in128 <= (in128end - 1)) {
                    auto mmIn = _mm_lddqu_si128(in128);
                    if ((_mm_extract_epi32(mmIn, 0) % this->A == 0) && (_mm_extract_epi32(mmIn, 1) % this->A == 0) && (_mm_extract_epi32(mmIn, 2) % this->A == 0)
                            && (_mm_extract_epi32(mmIn, 3) % this->A == 0)) {
                        ++in128;
                    } else {
                        throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATAENC*>(in128) - config.target.template begin<DATAENC>(), iteration);
                    }
                }
                if (in128 < in128end) {
                    auto in32 = reinterpret_cast<DATAENC*>(in128);
                    auto in32end = reinterpret_cast<DATAENC* const >(in128end);
                    while (in32 < in32end) {
                        if ((*in32 % this->A) == 0) {
                            ++in32;
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, in32 - config.target.template begin<DATAENC>(), iteration);
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
            AN_sse42_8x16_8x32_divmod & test;
            const ArithmeticConfiguration & config;
            const size_t iteration;
            ArithmetorChecked(
                    AN_sse42_8x16_8x32_divmod & test,
                    const ArithmeticConfiguration & config,
                    const size_t iteration)
                    : test(test),
                      config(config),
                      iteration(iteration) {
            }
            template<template<typename = void> class Functor>
            void impl() {
                auto in128 = config.source.template begin<VEC>();
                const auto in128end = test.template ComputeEnd<DATAENC>(in128, config);
                auto out128 = config.target.template begin<VEC>();
                DATAENC operand = config.operand;
                if constexpr (std::is_same_v<Functor<void>, add<void>> || std::is_same_v<Functor<void>, sub<void>> || std::is_same_v<Functor<void>, div<void>>) {
                    operand = config.operand * test.A;
                } else if constexpr (std::is_same_v<Functor<void>, mul<void>>) {
                    // do not encode operand here, otherwise we will have non-code values after the operation!
                } else {
                    throw std::runtime_error("Functor not known!");
                }
                VEC mmOperand = mm<VEC, DATAENC>::set1(operand);
                VEC mmA = mm<VEC, DATAENC>::set1(test.A);
                while (in128 <= (in128end - UNROLL)) {
                    // let the compiler unroll the loop
                    for (size_t unroll = 0; unroll < UNROLL; ++unroll) {
                        auto mmIn = _mm_lddqu_si128(in128++);
                        if ((_mm_extract_epi32(mmIn, 0) % test.A == 0) && (_mm_extract_epi32(mmIn, 1) % test.A == 0) && (_mm_extract_epi32(mmIn, 2) % test.A == 0)
                                && (_mm_extract_epi32(mmIn, 3) % test.A == 0)) {
                            auto x = mm_op<VEC, DATAENC, Functor>::compute(_mm_lddqu_si128(in128++), mmOperand);
                            if (std::is_same_v<Functor<void>, div<void>>) {
                                x = mm_op<VEC, DATAENC, mul>::compute(x, mmA); // make sure we get a code word again
                            }
                            _mm_storeu_si128(out128++, x);
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATAENC*>(in128 - 1) - config.source.template begin<DATAENC>(), iteration);
                        }
                    }
                }
                // remaining numbers
                while (in128 <= (in128end - 1)) {
                    auto mmIn = _mm_lddqu_si128(in128++);
                    if ((_mm_extract_epi32(mmIn, 0) % test.A == 0) && (_mm_extract_epi32(mmIn, 1) % test.A == 0) && (_mm_extract_epi32(mmIn, 2) % test.A == 0)
                            && (_mm_extract_epi32(mmIn, 3) % test.A == 0)) {
                        auto x = mm_op<VEC, DATAENC, Functor>::compute(_mm_lddqu_si128(in128++), mmOperand);
                        if (std::is_same_v<Functor<void>, div<void>>) {
                            x = mm_op<VEC, DATAENC, mul>::compute(x, mmA); // make sure we get a code word again
                        }
                        _mm_storeu_si128(out128++, x);
                    } else {
                        throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATAENC*>(in128 - 1) - config.source.template begin<DATAENC>(), iteration);
                    }
                }
                if (in128 < in128end) {
                    Functor<> functor;
                    auto in32 = reinterpret_cast<DATAENC*>(in128);
                    const auto in32end = reinterpret_cast<DATAENC* const >(in128end);
                    auto out32 = reinterpret_cast<DATAENC*>(out128);
                    while (in32 < in32end) {
                        if (*in32 % test.A == 0) {
                            auto x = functor(*in32++, operand);
                            if (std::is_same_v<Functor<void>, div<void>>) {
                                x *= test.A; // make sure we get a code word again
                            }
                            *out32++ = x;
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, in32 - config.source.template begin<DATAENC>(), iteration);
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
            typedef typename Larger<DATAENC>::larger_t larger_t;
            AN_sse42_8x16_8x32_divmod & test;
            const AggregateConfiguration & config;
            size_t iteration;
            AggregatorChecked(
                    AN_sse42_8x16_8x32_divmod & test,
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
                const auto in128end = test.template ComputeEnd<DATAENC>(in128, config);
                auto mmValue = funcInitVector();
                while (in128 <= (in128end - UNROLL)) {
                    for (size_t k = 0; k < UNROLL; ++k) {
                        auto mmIn = _mm_lddqu_si128(in128++);
                        if ((_mm_extract_epi32(mmIn, 0) % test.A == 0) && (_mm_extract_epi32(mmIn, 1) % test.A == 0) && (_mm_extract_epi32(mmIn, 2) % test.A == 0)
                                && (_mm_extract_epi32(mmIn, 3) % test.A == 0)) {
                            mmValue = funcKernelVector(mmValue, mmIn);
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATAENC*>(in128 - 1) - config.source.template begin<DATAENC>(), iteration);
                        }
                    }
                }
                while (in128 <= (in128end - 1)) {
                    auto mmIn = _mm_lddqu_si128(in128++);
                    if ((_mm_extract_epi32(mmIn, 0) % test.A == 0) && (_mm_extract_epi32(mmIn, 1) % test.A == 0) && (_mm_extract_epi32(mmIn, 2) % test.A == 0)
                            && (_mm_extract_epi32(mmIn, 3) % test.A == 0)) {
                        mmValue = funcKernelVector(mmValue, mmIn);
                    } else {
                        throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATAENC*>(in128 - 1) - config.source.template begin<DATAENC>(), iteration);
                    }
                }
                Aggregate value = funcVectorToScalar(mmValue);
                if (in128 < in128end) {
                    auto in32 = reinterpret_cast<DATAENC*>(in128);
                    const auto in32end = reinterpret_cast<DATAENC* const >(in128end);
                    while (in32 < in32end) {
                        if ((*in32 % test.A) == 0) {
                            value = funcKernelScalar(value, *in32++);
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, in32 - config.source.template begin<DATAENC>(), iteration);
                        }
                    }
                }
                auto final = funcFinal(value, config.numValues);
                auto dataOut = test.bufScratchPad.template begin<Aggregate>();
                *dataOut = final;
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
                auto in128 = config.source.template begin<VEC>();
                auto in128end = this->template ComputeEnd<DATAENC>(in128, config);
                auto out64 = config.target.template begin<int64_t>();
                auto mmA = _mm_set1_pd(static_cast<double>(this->A));
                auto mmShuffle = _mm_set_epi32(0xFFFFFFFF, 0xFFFFFFFF, 0x0B0A0302, 0x09080100);
                while (in128 <= (in128end - UNROLL)) { // let the compiler unroll the loop
                    for (size_t unroll = 0; unroll < UNROLL; ++unroll) {
                        auto tmp = *in128++;
                        auto tmp0d = _mm_cvtepi32_pd(tmp);
                        auto tmp1d = _mm_cvtepi32_pd(_mm_srli_si128(tmp, 8));
                        tmp0d = _mm_div_pd(tmp0d, mmA);
                        tmp1d = _mm_div_pd(tmp1d, mmA);
                        auto tmp0i = _mm_cvtpd_epi32(tmp0d);
                        auto tmp1i = _mm_cvtpd_epi32(tmp1d);
                        tmp = _mm_unpacklo_epi16(tmp0i, tmp1i);
                        tmp = _mm_shuffle_epi8(tmp, mmShuffle);
                        *out64++ = _mm_extract_epi64(tmp, 0);
                    }
                }
                // remaining numbers
                while (in128 <= (in128end - 1)) {
                    auto tmp = _mm_lddqu_si128(in128++);
                    auto tmp0d = _mm_cvtepi32_pd(tmp);
                    auto tmp1d = _mm_cvtepi32_pd(_mm_srli_si128(tmp, 8));
                    tmp0d = _mm_div_pd(tmp0d, mmA);
                    tmp1d = _mm_div_pd(tmp1d, mmA);
                    auto tmp0i = _mm_cvtpd_epi32(tmp0d);
                    auto tmp1i = _mm_cvtpd_epi32(tmp1d);
                    tmp = _mm_unpacklo_epi16(tmp0i, tmp1i);
                    tmp = _mm_shuffle_epi8(tmp, mmShuffle);
                    *out64++ = _mm_extract_epi64(tmp, 0);
                }
                if (in128 < in128end) {
                    auto out16 = reinterpret_cast<DATARAW*>(out64);
                    auto in32 = reinterpret_cast<DATAENC*>(in128);
                    auto in32end = reinterpret_cast<DATAENC * const >(in128end);
                    while (in32 < in32end) {
                        *out16++ = static_cast<DATARAW>(*in32++ / this->A);
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
                auto in128 = config.source.template begin<VEC>();
                const auto in128end = this->template ComputeEnd<DATAENC>(in128, config);
                auto out64 = config.target.template begin<int64_t>();
                auto mm_A = _mm_set1_pd(static_cast<double>(this->A));
                auto mmShuffle = _mm_set_epi32(0xFFFFFFFF, 0xFFFFFFFF, 0x0B0A0302, 0x09080100);
                while (in128 <= (in128end - UNROLL)) { // let the compiler unroll the loop
                    for (size_t unroll = 0; unroll < UNROLL; ++unroll) {
                        auto tmp = *in128++;
                        if ((_mm_extract_epi32(tmp, 0) % this->A == 0) && (_mm_extract_epi32(tmp, 1) % this->A == 0) && (_mm_extract_epi32(tmp, 2) % this->A == 0)
                                && (_mm_extract_epi32(tmp, 3) % this->A == 0)) {
                            auto tmp0d = _mm_cvtepi32_pd(tmp);
                            auto tmp1d = _mm_cvtepi32_pd(_mm_srli_si128(tmp, 8));
                            tmp0d = _mm_div_pd(tmp0d, mm_A);
                            tmp1d = _mm_div_pd(tmp1d, mm_A);
                            auto tmp0i = _mm_cvtpd_epi32(tmp0d);
                            auto tmp1i = _mm_cvtpd_epi32(tmp1d);
                            tmp = _mm_unpacklo_epi16(tmp0i, tmp1i);
                            tmp = _mm_shuffle_epi8(tmp, mmShuffle);
                            *out64++ = _mm_extract_epi64(tmp, 0);
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATAENC*>(in128 - 1) - config.source.template begin<DATAENC>(), iteration);
                        }
                    }
                }
                // remaining numbers
                while (in128 <= (in128end - 1)) {
                    auto tmp = *in128++;
                    if ((_mm_extract_epi32(tmp, 0) % this->A == 0) && (_mm_extract_epi32(tmp, 1) % this->A == 0) && (_mm_extract_epi32(tmp, 2) % this->A == 0)
                            && (_mm_extract_epi32(tmp, 3) % this->A == 0)) {
                        auto tmp0d = _mm_cvtepi32_pd(tmp);
                        auto tmp1d = _mm_cvtepi32_pd(_mm_srli_si128(tmp, 8));
                        tmp0d = _mm_div_pd(tmp0d, mm_A);
                        tmp1d = _mm_div_pd(tmp1d, mm_A);
                        auto tmp0i = _mm_cvtpd_epi32(tmp0d);
                        auto tmp1i = _mm_cvtpd_epi32(tmp1d);
                        tmp = _mm_unpacklo_epi16(tmp0i, tmp1i);
                        tmp = _mm_shuffle_epi8(tmp, mmShuffle);
                        *out64++ = _mm_extract_epi64(tmp, 0);
                    } else {
                        throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATAENC*>(in128 - 1) - config.source.template begin<DATAENC>(), iteration);
                    }
                }
                if (in128 < in128end) {
                    auto in32 = reinterpret_cast<DATAENC*>(in128);
                    const auto in32end = reinterpret_cast<DATAENC* const >(in128end);
                    auto out16 = reinterpret_cast<DATARAW*>(out64);
                    while (in32 < in32end) {
                        if ((*in32 % this->A) == 0) {
                            *out16++ = static_cast<DATARAW>(*in32++ / this->A);
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, in32 - config.source.template begin<DATAENC>(), iteration);
                        }
                    }
                }
            }
        }
    };

    template<size_t UNROLL>
    struct AN_sse42_8x16_8x32_s_divmod :
            public AN_sse42_8x16_8x32_divmod<int16_t, int32_t, UNROLL> {

        using AN_sse42_8x16_8x32_divmod<int16_t, int32_t, UNROLL>::AN_sse42_8x16_8x32_divmod;

        virtual ~AN_sse42_8x16_8x32_s_divmod() {
        }
    };

    template<size_t UNROLL>
    struct AN_sse42_8x16_8x32_u_divmod :
            public AN_sse42_8x16_8x32_divmod<uint16_t, uint32_t, UNROLL> {

        using AN_sse42_8x16_8x32_divmod<uint16_t, uint32_t, UNROLL>::AN_sse42_8x16_8x32_divmod;

        virtual ~AN_sse42_8x16_8x32_u_divmod() {
        }
    };

}
