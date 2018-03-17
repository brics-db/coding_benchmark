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

        using BASE::NUM_VALUES_PER_SIMDREG;
        using BASE::NUM_VALUES_PER_UNROLL;

        using BASE::AN_sse42_8x16_8x32;

        virtual ~AN_sse42_8x16_8x32_divmod() {
        }

        virtual bool DoCheck() override {
            return true;
        }

        virtual void RunCheck(
                const CheckConfiguration & config) override {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                auto mmData = config.target.template begin<__m128i >();
                const auto mmDataEnd = config.target.template end<__m128i >();
                while (mmData <= (mmDataEnd - UNROLL)) {
                    // let the compiler unroll the loop
                    for (size_t k = 0; k < UNROLL; ++k) {
                        auto mmIn = _mm_lddqu_si128(mmData);
                        if ((_mm_extract_epi32(mmIn, 0) % this->A == 0) && (_mm_extract_epi32(mmIn, 1) % this->A == 0) && (_mm_extract_epi32(mmIn, 2) % this->A == 0)
                                && (_mm_extract_epi32(mmIn, 3) % this->A == 0)) {
                            ++mmData;
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATAENC*>(mmData) - config.target.template begin<DATAENC>(), iteration);
                        }
                    }
                }
                // here follows the non-unrolled remainder
                while (mmData <= (mmDataEnd - 1)) {
                    auto mmIn = _mm_lddqu_si128(mmData);
                    if ((_mm_extract_epi32(mmIn, 0) % this->A == 0) && (_mm_extract_epi32(mmIn, 1) % this->A == 0) && (_mm_extract_epi32(mmIn, 2) % this->A == 0)
                            && (_mm_extract_epi32(mmIn, 3) % this->A == 0)) {
                        ++mmData;
                    } else {
                        throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATAENC*>(mmData) - config.target.template begin<DATAENC>(), iteration);
                    }
                }
                if (mmData < mmDataEnd) {
                    auto dataEnd2 = reinterpret_cast<DATAENC*>(mmDataEnd);
                    for (auto data2 = reinterpret_cast<DATAENC*>(mmData); data2 < dataEnd2;) {
                        if ((*data2 % this->A) == 0) {
                            ++data2;
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, data2 - config.target.template begin<DATAENC>(), iteration);
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
                auto mmData = config.source.template begin<__m128i >();
                const auto mmDataEnd = config.source.template end<__m128i >();
                auto mmOut = config.target.template begin<__m128i >();
                DATAENC operand = config.operand;
                if constexpr (std::is_same_v<Functor<void>, add<void>> || std::is_same_v<Functor<void>, sub<void>> || std::is_same_v<Functor<void>, div<void>>) {
                    operand = config.operand * test.A;
                } else if constexpr (std::is_same_v<Functor<void>, mul<void>>) {
                    // do not encode operand here, otherwise we will have non-code values after the operation!
                } else {
                    throw std::runtime_error("Functor not known!");
                }
                __m128i mmOperand = mm<__m128i, DATAENC>::set1(operand);
                __m128i mmA = mm<__m128i, DATAENC>::set1(test.A);
                while (mmData <= (mmDataEnd - UNROLL)) {
                    // let the compiler unroll the loop
                    for (size_t unroll = 0; unroll < UNROLL; ++unroll) {
                        auto mmIn = _mm_lddqu_si128(mmData++);
                        if ((_mm_extract_epi32(mmIn, 0) % test.A == 0) && (_mm_extract_epi32(mmIn, 1) % test.A == 0) && (_mm_extract_epi32(mmIn, 2) % test.A == 0)
                                && (_mm_extract_epi32(mmIn, 3) % test.A == 0)) {
                            auto x = mm_op<__m128i, DATAENC, Functor>::compute(_mm_lddqu_si128(mmData++), mmOperand);
                            if (std::is_same_v<Functor<void>, div<void>>) {
                                x = mm_op<__m128i, DATAENC, mul>::compute(x, mmA); // make sure we get a code word again
                            }
                            _mm_storeu_si128(mmOut++, x);
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATAENC*>(mmData - 1) - config.source.template begin<DATAENC>(), iteration);
                        }
                    }
                }
                // remaining numbers
                while (mmData <= (mmDataEnd - 1)) {
                    auto mmIn = _mm_lddqu_si128(mmData++);
                    if ((_mm_extract_epi32(mmIn, 0) % test.A == 0) && (_mm_extract_epi32(mmIn, 1) % test.A == 0) && (_mm_extract_epi32(mmIn, 2) % test.A == 0)
                            && (_mm_extract_epi32(mmIn, 3) % test.A == 0)) {
                        auto x = mm_op<__m128i, DATAENC, Functor>::compute(_mm_lddqu_si128(mmData++), mmOperand);
                        if (std::is_same_v<Functor<void>, div<void>>) {
                            x = mm_op<__m128i, DATAENC, mul>::compute(x, mmA); // make sure we get a code word again
                        }
                        _mm_storeu_si128(mmOut++, x);
                    } else {
                        throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATAENC*>(mmData - 1) - config.source.template begin<DATAENC>(), iteration);
                    }
                }
                if (mmData < mmDataEnd) {
                    Functor<> functor;
                    auto data32End = reinterpret_cast<DATAENC*>(mmDataEnd);
                    auto out32 = reinterpret_cast<DATAENC*>(mmOut);
                    for (auto data32 = reinterpret_cast<DATAENC*>(mmData); data32 < data32End; ++data32, ++out32) {
                        if (*data32 % test.A == 0) {
                            auto x = functor(*data32, operand);
                            if (std::is_same_v<Functor<void>, div<void>>) {
                                x *= test.A; // make sure we get a code word again
                            }
                            *out32 = x;
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, data32 - config.source.template begin<DATAENC>(), iteration);
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
                auto *mmData = config.source.template begin<__m128i >();
                auto * const mmDataEnd = config.source.template end<__m128i >();
                auto mmValue = funcInitVector();
                while (mmData <= (mmDataEnd - UNROLL)) {
                    for (size_t k = 0; k < UNROLL; ++k) {
                        auto mmIn = _mm_lddqu_si128(mmData++);
                        if ((_mm_extract_epi32(mmIn, 0) % test.A == 0) && (_mm_extract_epi32(mmIn, 1) % test.A == 0) && (_mm_extract_epi32(mmIn, 2) % test.A == 0)
                                && (_mm_extract_epi32(mmIn, 3) % test.A == 0)) {
                            mmValue = funcKernelVector(mmValue, mmIn);
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATAENC*>(mmData - 1) - config.source.template begin<DATAENC>(), iteration);
                        }
                    }
                }
                while (mmData <= (mmDataEnd - 1)) {
                    auto mmIn = _mm_lddqu_si128(mmData++);
                    if ((_mm_extract_epi32(mmIn, 0) % test.A == 0) && (_mm_extract_epi32(mmIn, 1) % test.A == 0) && (_mm_extract_epi32(mmIn, 2) % test.A == 0)
                            && (_mm_extract_epi32(mmIn, 3) % test.A == 0)) {
                        mmValue = funcKernelVector(mmValue, mmIn);
                    } else {
                        throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATAENC*>(mmData - 1) - config.source.template begin<DATAENC>(), iteration);
                    }
                }
                Aggregate value = funcVectorToScalar(mmValue);
                if (mmData < mmDataEnd) {
                    auto data = reinterpret_cast<DATAENC*>(mmData);
                    const auto dataEnd = reinterpret_cast<DATAENC*>(mmDataEnd);
                    while (data < dataEnd) {
                        if ((*data % test.A) == 0) {
                            value = funcKernelScalar(value, *data++);
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, data - config.source.template begin<DATAENC>(), iteration);
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
                size_t i = 0;
                auto dataIn = this->bufEncoded.template begin<__m128i >();
                auto dataOut = this->bufResult.template begin<int64_t>();
                auto mmA = _mm_set1_pd(static_cast<double>(this->A));
                auto mmShuffle = _mm_set_epi32(0xFFFFFFFF, 0xFFFFFFFF, 0x0B0A0908, 0x03020100);
                for (; i <= (config.numValues - NUM_VALUES_PER_UNROLL); i += NUM_VALUES_PER_UNROLL) { // let the compiler unroll the loop
                    for (size_t unroll = 0; unroll < UNROLL; ++unroll) {
                        auto tmp = _mm_lddqu_si128(dataIn++);
                        auto tmp1 = _mm_cvtepi32_pd(tmp);
                        auto tmp2 = _mm_cvtepi32_pd(_mm_srli_si128(tmp, 8));
                        tmp1 = _mm_div_pd(tmp1, mmA);
                        tmp2 = _mm_div_pd(tmp2, mmA);
                        auto tmp3 = _mm_cvtpd_epi32(tmp1);
                        auto tmp4 = _mm_cvtpd_epi32(tmp2);
                        tmp = _mm_unpacklo_epi16(tmp3, tmp4);
                        tmp = _mm_shuffle_epi8(tmp, mmShuffle);
                        *dataOut++ = _mm_extract_epi64(tmp, 0);
                    }
                }
                // remaining numbers
                for (; i <= (config.numValues - NUM_VALUES_PER_SIMDREG); i += NUM_VALUES_PER_SIMDREG) {
                    auto tmp = _mm_lddqu_si128(dataIn++);
                    auto tmp1 = _mm_cvtepi32_pd(tmp);
                    auto tmp2 = _mm_cvtepi32_pd(_mm_srli_si128(tmp, 8));
                    tmp1 = _mm_div_pd(tmp1, mmA);
                    tmp2 = _mm_div_pd(tmp2, mmA);
                    auto tmp3 = _mm_cvtpd_epi32(tmp1);
                    auto tmp4 = _mm_cvtpd_epi32(tmp2);
                    tmp = _mm_unpacklo_epi16(tmp3, tmp4);
                    tmp = _mm_shuffle_epi8(tmp, mmShuffle);
                    *dataOut++ = _mm_extract_epi64(tmp, 0);
                }
                if (i < config.numValues) {
                    auto out = reinterpret_cast<DATARAW*>(dataOut);
                    auto in = reinterpret_cast<DATAENC*>(dataIn);
                    for (; i < config.numValues; ++i) {
                        *out++ = static_cast<DATARAW>(*in++ / this->A);
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
                size_t i = 0;
                auto dataIn = this->bufEncoded.template begin<__m128i >();
                auto dataOut = this->bufResult.template begin<int64_t>();
                auto mm_A = _mm_set1_pd(static_cast<double>(this->A));
                auto mmShuffle = _mm_set_epi32(0xFFFFFFFF, 0xFFFFFFFF, 0x0B0A0908, 0x03020100);
                for (; i <= (config.numValues - NUM_VALUES_PER_UNROLL); i += NUM_VALUES_PER_UNROLL) { // let the compiler unroll the loop
                    for (size_t unroll = 0; unroll < UNROLL; ++unroll) {
                        auto tmp = _mm_lddqu_si128(dataIn++);
                        if ((_mm_extract_epi32(tmp, 0) % this->A == 0) && (_mm_extract_epi32(tmp, 1) % this->A == 0) && (_mm_extract_epi32(tmp, 2) % this->A == 0)
                                && (_mm_extract_epi32(tmp, 3) % this->A == 0)) {
                            auto tmp1 = _mm_cvtepi32_pd(tmp);
                            auto tmp2 = _mm_cvtepi32_pd(_mm_srli_si128(tmp, 8));
                            tmp1 = _mm_div_pd(tmp1, mm_A);
                            tmp2 = _mm_div_pd(tmp2, mm_A);
                            auto tmp3 = _mm_cvtpd_epi32(tmp1);
                            auto tmp4 = _mm_cvtpd_epi32(tmp2);
                            tmp = _mm_unpacklo_epi16(tmp3, tmp4);
                            tmp = _mm_shuffle_epi8(tmp, mmShuffle);
                            *dataOut++ = _mm_extract_epi64(tmp, 0);
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATAENC*>(dataIn - 1) - this->bufEncoded.template begin<DATAENC>(), iteration);
                        }
                    }
                }
                // remaining numbers
                for (; i <= (config.numValues - NUM_VALUES_PER_SIMDREG); i += NUM_VALUES_PER_SIMDREG) {
                    auto tmp = _mm_lddqu_si128(dataIn++);
                    if ((_mm_extract_epi32(tmp, 0) % this->A == 0) && (_mm_extract_epi32(tmp, 1) % this->A == 0) && (_mm_extract_epi32(tmp, 2) % this->A == 0)
                            && (_mm_extract_epi32(tmp, 3) % this->A == 0)) {
                        auto tmp1 = _mm_cvtepi32_pd(tmp);
                        auto tmp2 = _mm_cvtepi32_pd(_mm_srli_si128(tmp, 8));
                        tmp1 = _mm_div_pd(tmp1, mm_A);
                        tmp2 = _mm_div_pd(tmp2, mm_A);
                        auto tmp3 = _mm_cvtpd_epi32(tmp1);
                        auto tmp4 = _mm_cvtpd_epi32(tmp2);
                        tmp = _mm_unpacklo_epi16(tmp3, tmp4);
                        tmp = _mm_shuffle_epi8(tmp, mmShuffle);
                        *dataOut++ = _mm_extract_epi64(tmp, 0);
                    } else {
                        throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATAENC*>(dataIn - 1) - this->bufEncoded.template begin<DATAENC>(), iteration);
                    }
                }
                if (i < config.numValues) {
                    auto out = reinterpret_cast<DATARAW*>(dataOut);
                    auto in = reinterpret_cast<DATAENC*>(dataIn);
                    for (; i < config.numValues; ++i) {
                        if ((*in % this->A) == 0) {
                            *out++ = static_cast<DATARAW>(*in++ / this->A);
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, in - this->bufEncoded.template begin<DATAENC>(), iteration);
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
