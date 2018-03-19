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
 * File:   AN_simd.tcc
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 13-12-2016 01:01
 * Renamed in 19-03-2018 16:06
 */

#pragma once

#include <stdexcept>

#ifndef AN_SIMD
#error "Clients must not include this file directly, but file <AN/AN_simd.hpp>!"
#endif

#include <AN/ANTest.hpp>
#include <Util/Functors.hpp>
#include <Util/Helpers.hpp>
#include <Util/ArithmeticSelector.hpp>
#include <Util/AggregateSelector.hpp>
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

    template <typename DATARAW, typename DATAENC, typename VEC>
    void writeout(VEC mmIn, DATARAW* mmOut);

#ifdef __SSE4_2__
    inline void writeout_16_32_128(
            __m128i mmIn,
            uint16_t* mmOut) {
        static __m128i mmShuffle = _mm_set_epi64x(0xFFFFFFFFFFFFFFFF, 0x0D0C090805040100);
        *reinterpret_cast<uint64_t*>(mmOut) = _mm_extract_epi64(_mm_shuffle_epi8(mmIn, mmShuffle), 0);
    }

    template<>
    inline void writeout<uint16_t, uint32_t, __m128i>(
            __m128i mmIn,
            uint16_t* mmOut) {
        writeout_16_32_128(mmIn, mmOut);
    }

    template<>
    inline void writeout<int16_t, int32_t, __m128i>(
            __m128i mmIn,
            int16_t* mmOut) {
        writeout_16_32_128(mmIn, reinterpret_cast<uint16_t*>(mmOut));
    }
#endif /* __SSE4_2__ */

#ifdef __AVX2__
    inline void writeout_16_32_256(
            __m256i mmIn,
            uint16_t* mmOut) {
        static __m256i mmShuffle = _mm256_set_epi64x(0xFFFFFFFFFFFFFFFF, 0x0D0C090805040100, 0xFFFFFFFFFFFFFFFF, 0x0D0C090805040100);
        uint64_t * out64 = reinterpret_cast<uint64_t*>(mmOut);
        __m256i mm = _mm256_shuffle_epi8(mmIn, mmShuffle);
        *out64++ = _mm256_extract_epi64(mm, 0);
        *out64 = _mm256_extract_epi64(mm, 2);
    }

    template<>
    inline void writeout<uint16_t, uint32_t, __m256i>(
            __m256i mmIn,
            uint16_t* mmOut) {
        writeout_16_32_256(mmIn, mmOut);
    }

    template<>
    inline void writeout<int16_t, int32_t, __m256i>(
            __m256i mmIn,
            int16_t* mmOut) {
        writeout_16_32_256(mmIn, reinterpret_cast<uint16_t*>(mmOut));
    }
#endif /* __AVX2__ */

    template<typename DATARAW, typename DATAENC, typename VEC, size_t UNROLL>
    struct AN_simd :
            public ANTest<DATARAW, DATAENC, UNROLL>,
            public SIMDTest<VEC> {

        using ANTest<DATARAW, DATAENC, UNROLL>::ANTest;

        virtual ~AN_simd() {
        }

        void RunEncode(
                const EncodeConfiguration & config) override {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                auto *inV = config.source.template begin<VEC>();
                auto * const inVend = this->template ComputeEnd<DATARAW>(inV, config);
                auto *outV = config.target.template begin<VEC>();
                auto mmA = mm<VEC, DATAENC>::set1(this->A);

                while (inV <= (inVend - UNROLL)) {
                    // let the compiler unroll the loop
                    for (size_t unroll = 0; unroll < UNROLL; ++unroll) {
                        auto mmIn = *inV++;
                        *outV++ = mm_op<VEC, DATAENC, mul>::compute(mm<VEC, DATARAW>::cvt_larger_lo(mmIn), mmA);
                        *outV++ = mm_op<VEC, DATAENC, mul>::compute(mm<VEC, DATARAW>::cvt_larger_hi(mmIn), mmA);
                    }
                }
                // remaining numbers
                while (inV <= (inVend - 1)) {
                    auto mmIn = *inV++;
                    *outV++ = mm_op<VEC, DATAENC, mul>::compute(mm<VEC, DATARAW>::cvt_larger_lo(mmIn), mmA);
                    *outV++ = mm_op<VEC, DATAENC, mul>::compute(mm<VEC, DATARAW>::cvt_larger_hi(mmIn), mmA);
                }
                if (inV < inVend) {
                    auto inS = reinterpret_cast<DATARAW*>(inV);
                    auto inSend = reinterpret_cast<DATARAW*>(inVend);
                    auto outS = reinterpret_cast<DATAENC*>(outV);
                    while (inS < inSend) {
                        *outS++ = *inS++ * this->A;
                    }
                }
            }
        }

        bool DoArithmetic(
                const ArithmeticConfiguration & config) override {
            return std::visit(ArithmeticSelector(), config.mode);
        }

        struct Arithmetor {
            AN_simd & test;
            const ArithmeticConfiguration & config;
            Arithmetor(
                    AN_simd & test,
                    const ArithmeticConfiguration & config)
                    : test(test),
                      config(config) {
            }
            template<template<typename = void> class Functor>
            void impl() {
                auto inV = config.source.template begin<VEC>();
                const auto inVend = test.template ComputeEnd<DATAENC>(inV, config);
                auto outV = config.target.template begin<VEC>();
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
                while (inV <= (inVend - UNROLL)) {
                    // let the compiler unroll the loop
                    for (size_t unroll = 0; unroll < UNROLL; ++unroll) {
                        auto x = mm_op<VEC, DATAENC, Functor>::compute(*inV++, mmOperand);
                        if constexpr (std::is_same_v<Functor<void>, div<void>>) {
                            x = mm_op<VEC, DATAENC, mul>::compute(x, mmA); // make sure we get a code word again
                        }
                        *outV++ = x;
                    }
                }
                // remaining numbers
                while (inV <= (inVend - 1)) {
                    auto x = mm_op<VEC, DATAENC, Functor>::compute(*inV++, mmOperand);
                    if constexpr (std::is_same_v<Functor<void>, div<void>>) {
                        x = mm_op<VEC, DATAENC, mul>::compute(x, mmA); // make sure we get a code word again
                    }
                    *outV++ = x;
                }
                if (inV < inVend) {
                    Functor<> functor;
                    auto inS = reinterpret_cast<DATAENC*>(inV);
                    const auto inSend = reinterpret_cast<DATAENC* const >(inVend);
                    auto outS = reinterpret_cast<DATAENC*>(outV);
                    while (inS < inSend) {
                        auto x = functor(*inS++, operand);
                        if (std::is_same_v<Functor<void>, div<void>>) {
                            x *= test.A; // make sure we get a code word again
                        }
                        *outS++ = x;
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
            AN_simd & test;
            const AggregateConfiguration & config;
            Aggregator(
                    AN_simd & test,
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
                auto mmResult = funcInitVector();
                while (in128 <= (in128end - UNROLL)) {
                    for (size_t k = 0; k < UNROLL; ++k) {
                        mmResult = funcKernelVector(mmResult, *in128++);
                    }
                }
                while (in128 <= (in128end - 1)) {
                    mmResult = funcKernelVector(mmResult, *in128++);
                }
                Aggregate value = funcVectorToScalar(mmResult);
                if (in128 < in128end) {
                    auto dataIn = reinterpret_cast<DATAENC*>(in128);
                    const auto dataInEnd = reinterpret_cast<DATAENC* const >(in128end);
                    while (dataIn < dataInEnd) {
                        value = funcKernelScalar(value, *dataIn++);
                    }
                }
                Aggregate final = funcFinal(value, config.numValues);
                auto out = test.bufScratchPad.template begin<Aggregate>();
                *out = final / test.A; // decode here, because we encode again in the next step. This is currently required!
                EncodeConfiguration encConf(1, 2, test.bufScratchPad, config.target);
                test.RunEncode(encConf);
            }
            void operator()(
                    AggregateConfiguration::Sum) {
                impl<larger_t>([] {return simd::mm<VEC, larger_t>::set1(0);}, [](VEC mmSum, VEC mmTmp) {
                    auto mmLo = simd::mm<VEC, DATAENC>::cvt_larger_lo(mmTmp);
                    mmLo = simd::mm_op<VEC, larger_t, add>::compute(mmSum, mmLo);
                    auto mmHi = simd::mm<VEC, DATAENC>::cvt_larger_hi(mmTmp);
                    return simd::mm_op<VEC, larger_t, add>::compute(mmLo, mmHi);
                }, [](VEC mmSum) {return simd::mm<VEC, larger_t>::sum(mmSum);}, [](larger_t sum, DATAENC tmp) {return sum + tmp;}, [](larger_t sum, size_t numValues) {return sum;});
            }
            void operator()(
                    AggregateConfiguration::Min) {
                impl<DATAENC>([] {return simd::mm<VEC, DATAENC>::set1(std::numeric_limits<DATAENC>::max());}, [](VEC mmMin, VEC mmTmp) {return simd::mm<VEC, DATAENC>::min(mmMin, mmTmp);},
                        [](VEC mmMin) {return simd::mm<VEC, DATAENC>::min(mmMin);}, [](DATAENC min, DATAENC tmp) {return min < tmp ? min : tmp;}, [](DATAENC min, size_t numValues) {return min;});
            }
            void operator()(
                    AggregateConfiguration::Max) {
                impl<DATAENC>([] {return simd::mm<VEC, DATAENC>::set1(std::numeric_limits<DATAENC>::min());}, [](VEC mmMax, VEC mmTmp) {return simd::mm<VEC, DATAENC>::max(mmMax, mmTmp);},
                        [](VEC mmMax) {return simd::mm<VEC, DATAENC>::max(mmMax);}, [](DATAENC max, DATAENC tmp) {return max > tmp ? max : tmp;}, [](DATAENC max, size_t numValues) {return max;});
            }
            void operator()(
                    AggregateConfiguration::Avg) {
                impl<larger_t>([] {return simd::mm<VEC, larger_t>::set1(0);}, [](VEC mmSum, VEC mmTmp) {
                    auto mmLo = simd::mm<VEC, DATAENC>::cvt_larger_lo(mmTmp);
                    mmLo = simd::mm_op<VEC, larger_t, add>::compute(mmSum, mmLo);
                    auto mmHi = simd::mm<VEC, DATAENC>::cvt_larger_hi(mmTmp);
                    return simd::mm_op<VEC, larger_t, add>::compute(mmLo, mmHi);
                }, [](VEC mmSum) {return simd::mm<VEC, larger_t>::sum(mmSum);}, [](larger_t sum, DATAENC tmp) {return sum + tmp;},
                        [this](larger_t sum, size_t numValues) {return (sum / (numValues * test.A)) * test.A;});
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
