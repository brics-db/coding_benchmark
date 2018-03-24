// Copyright 2017 Till Kolditz
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
 * File:   Hamming_simd.tcc
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 13-12-2017
 * Renamed in 19-03-2018 16:06
 */

#pragma once

#ifndef HAMMING_SIMD
#error "Clients must not include this file directly, but file <Hamming/Hamming_simd.hpp>!"
#endif

#include <Util/Test.hpp>
#include <Util/ErrorInfo.hpp>
#include <Util/ArithmeticSelector.hpp>
#include <Util/AggregateSelector.hpp>
#include <Util/Helpers.hpp>
#include <Hamming/Hamming_scalar.hpp>
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

#ifdef __SSE4_2__
    extern template struct hamming_t<uint16_t, __m128i > ;
    extern template struct hamming_t<uint32_t, __m128i > ;
#endif
#ifdef __AVX2__
    extern template struct hamming_t<uint16_t, __m256i > ;
    extern template struct hamming_t<uint32_t, __m256i > ;
#endif
#ifdef __AVX512F__
    extern template struct hamming_t<uint16_t, __m512i > ;
    extern template struct hamming_t<uint32_t, __m512i > ;
#endif

    template<typename DATAIN, typename VEC, size_t UNROLL, size_t StoreVersion = 1>
    struct Hamming_simd :
            public Test<DATAIN, hamming_t<DATAIN, VEC>>,
            public SIMDTest<VEC> {

        typedef hamming_t<DATAIN, VEC> hamming_simd_t;
        typedef hamming_t<DATAIN, DATAIN> hamming_scalar_t;

        using Test<DATAIN, hamming_simd_t>::Test;

        virtual ~Hamming_simd() {
        }

        void RunEncode(
                const EncodeConfiguration & config) override {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                _ReadWriteBarrier();
                auto inV = config.source.template begin<VEC>();
                const auto inVend = this->template ComputeEnd<DATAIN>(inV, config);
                auto outV = config.target.template begin<hamming_simd_t>();
                while (inV <= (inVend - UNROLL)) {
                    for (size_t k = 0; k < UNROLL; ++k, ++outV) {
                        outV->template storeI<StoreVersion>(*inV++);
                    }
                }
                for (; inV <= (inVend - 1); ++outV) {
                    outV->template storeI<StoreVersion>(*inV++);
                }
                if (inV < inVend) {
                    auto inS = reinterpret_cast<DATAIN*>(inV);
                    const auto inSend = reinterpret_cast<DATAIN* const >(inVend);
                    auto outS = reinterpret_cast<hamming_scalar_t*>(outV);
                    for (; inS < inSend; ++outS) {
                        outS->template storeI<StoreVersion>(*inS++);
                    }
                }
            }
        }

        bool DoCheck() override {
            return true;
        }

        void RunCheck(
                const CheckConfiguration & config) override {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                _ReadWriteBarrier();
                auto inV = config.target.template begin<hamming_simd_t>();
                const auto inVend = this->template ComputeEnd<hamming_scalar_t, hamming_simd_t>(inV, config);
                while (inV <= (inVend - UNROLL)) {
                    for (size_t k = 0; k < UNROLL; ++k) {
                        if (inV->isValid()) {
                            ++inV;
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATAIN*>(inV) - config.target.template begin<DATAIN>(), iteration);
                        }
                    }
                }
                while (inV <= (inVend - 1)) {
                    if (inV->isValid()) {
                        ++inV;
                    } else {
                        throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATAIN*>(inV) - config.target.template begin<DATAIN>(), iteration);
                    }
                }
                if (inV < inVend) {
                    auto inS = reinterpret_cast<hamming_scalar_t*>(inV);
                    const auto inSend = reinterpret_cast<hamming_scalar_t* const >(inVend);
                    while (inS < inSend) {
                        if (inS->isValid()) {
                            ++inS;
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATAIN*>(inS) - config.target.template begin<DATAIN>(), iteration);
                        }
                    }
                }
            }
        }

        bool DoArithmetic(
                const ArithmeticConfiguration & config) override {
            return std::visit(ArithmeticSelector(), config.mode);
        }

        template<bool check>
        struct Arithmetor {
            using hamming_sse24_t = Hamming_simd::hamming_simd_t;
            using hamming_scalar_t = Hamming_simd::hamming_scalar_t;
            Hamming_simd & test;
            const ArithmeticConfiguration & config;
            const size_t iteration;
            Arithmetor(
                    Hamming_simd & test,
                    const ArithmeticConfiguration & config,
                    const size_t iteration)
                    : test(test),
                      config(config),
                      iteration(iteration) {
            }
            template<template<typename = void> class Functor>
            void impl() {
                auto mmOperand = mm<VEC, DATAIN>::set1(config.operand);
                auto inV = config.source.template begin<hamming_simd_t>();
                const auto inVend = test.template ComputeEnd<hamming_scalar_t, hamming_simd_t>(inV, config);
                auto outV = config.target.template begin<hamming_simd_t>();
                while (inV <= (inVend - UNROLL)) {
                    for (size_t k = 0; k < UNROLL; ++k, ++inV, ++outV) {
                        if ((!check) || inV->isValid()) {
                            outV->template storeI<StoreVersion>(mm_op<VEC, DATAIN, Functor>::compute(inV->data, mmOperand));
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATAIN*>(inV) - config.source.template begin<DATAIN>(), iteration);
                        }
                    }
                }
                for (; inV < (inVend - 1); ++inV, ++outV) {
                    if ((!check) || inV->isValid()) {
                        outV->template storeI<StoreVersion>(mm_op<VEC, DATAIN, Functor>::compute(inV->data, mmOperand));
                    } else {
                        throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATAIN*>(inV) - config.source.template begin<DATAIN>(), iteration);
                    }
                }
                if (inV < inVend) {
                    Functor<> functor;
                    auto inS = reinterpret_cast<hamming_scalar_t*>(inV);
                    const auto inSend = reinterpret_cast<hamming_scalar_t* const >(inVend);
                    auto outS = reinterpret_cast<hamming_scalar_t*>(outV);
                    for (; inS < inSend; ++outS, ++inS) {
                        if ((!check) || inS->isValid()) {
                            outS->template storeI<StoreVersion>(functor(inS->data, config.operand));
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATAIN*>(inS) - config.source.template begin<DATAIN>(), iteration);
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

        void RunArithmetic(
                const ArithmeticConfiguration & config) override {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                _ReadWriteBarrier();
                std::visit(Arithmetor<false>(*this, config, iteration), config.mode);
            }
        }

        bool DoArithmeticChecked(
                const ArithmeticConfiguration & config) override {
            return std::visit(ArithmeticSelector(), config.mode);
        }

        void RunArithmeticChecked(
                const ArithmeticConfiguration & config) override {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                _ReadWriteBarrier();
                std::visit(Arithmetor<true>(*this, config, iteration), config.mode);
            }
        }

        bool DoAggregate(
                const AggregateConfiguration & config) override {
            return std::visit(AggregateSelector(), config.mode);
        }

        template<bool check>
        struct Aggregator {
            using hamming_sse24_t = Hamming_simd::hamming_simd_t;
            using hamming_scalar_t = Hamming_simd::hamming_scalar_t;
            typedef typename Larger<DATAIN>::larger_t larger_t;
            Hamming_simd & test;
            const AggregateConfiguration & config;
            size_t iteration;
            Aggregator(
                    Hamming_simd & test,
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
                auto mmAggr = funcInitVector();
                auto inV = config.source.template begin<hamming_simd_t>();
                const auto inVend = test.template ComputeEnd<hamming_scalar_t, hamming_simd_t>(inV, config);
                while (inV <= (inVend - UNROLL)) {
                    for (size_t k = 0; k < UNROLL; ++k) {
                        if ((!check) || inV->isValid()) {
                            mmAggr = funcKernelVector(mmAggr, inV++->data);
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<hamming_scalar_t*>(inV) - config.source.template begin<hamming_scalar_t>(), iteration);
                        }
                    }
                }
                while (inV < (inVend - 1)) {
                    if ((!check) || inV->isValid()) {
                        mmAggr = funcKernelVector(mmAggr, inV++->data);
                    } else {
                        throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<hamming_scalar_t*>(inV) - config.source.template begin<hamming_scalar_t>(), iteration);
                    }
                }
                Aggregate aggr = funcVectorToScalar(mmAggr);
                if (inV < inVend) {
                    auto inS = reinterpret_cast<hamming_scalar_t*>(inV);
                    const auto inSend = reinterpret_cast<hamming_scalar_t * const >(inVend);
                    while (inS < inSend) {
                        if ((!check) || inS->isValid()) {
                            aggr = funcKernelScalar(aggr, inS++->data);
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, inS - config.source.template begin<hamming_scalar_t>(), iteration);
                        }
                    }
                }
                auto dataOut = test.bufScratchPad.template begin<Aggregate>();
                *dataOut = funcFinal(aggr, config.numValues);
                EncodeConfiguration encConf(1, 2, test.bufScratchPad, config.target);
                test.RunEncode(encConf);
            }
            void operator()(
                    AggregateConfiguration::Sum) {
                impl<larger_t>([] {return mm<VEC, larger_t>::set1(0);}, [](VEC mmSum, VEC inV) {
                    mmSum = mm_op<VEC, larger_t, add>::compute(mmSum, mm<VEC, DATAIN>::cvt_larger_lo(inV));
                    return mm_op<VEC, larger_t, add>::compute(mmSum, mm<VEC, DATAIN>::cvt_larger_hi(inV));
                }, [] (VEC mmSum) {return mm<VEC, larger_t>::sum(mmSum);}, [] (larger_t sum, DATAIN tmp) {return sum + tmp;}, [] (larger_t sum, size_t numValues) {return sum;});
            }
            void operator()(
                    AggregateConfiguration::Min) {
                impl<DATAIN>([] {return mm<VEC, DATAIN>::set1(std::numeric_limits<DATAIN>::max());}, [](VEC mmMin, VEC inV) {return mm<VEC, DATAIN>::min(mmMin, inV);},
                        [](VEC mmMin) {return mm<VEC, DATAIN>::min(mmMin);}, [](DATAIN min, DATAIN tmp) {return std::min(min, tmp);}, [](DATAIN min, size_t numValues) {return min;});
            }
            void operator()(
                    AggregateConfiguration::Max) {
                impl<DATAIN>([] {return mm<VEC, DATAIN>::set1(std::numeric_limits<DATAIN>::min());}, [](VEC mmMax, VEC inV) {return mm<VEC, DATAIN>::max(mmMax, inV);},
                        [](VEC mmMax) {return mm<VEC, DATAIN>::max(mmMax);}, [](DATAIN max, DATAIN tmp) {return std::max(max, tmp);}, [](DATAIN max, size_t numValues) {return max;});
            }
            void operator()(
                    AggregateConfiguration::Avg) {
                impl<larger_t>([] {return mm<VEC, larger_t>::set1(0);}, [](VEC mmSum, VEC inV) {
                    mmSum = mm_op<VEC, larger_t, add>::compute(mmSum, mm<VEC, DATAIN>::cvt_larger_lo(inV));
                    return mm_op<VEC, larger_t, add>::compute(mmSum, mm<VEC, DATAIN>::cvt_larger_hi(inV));
                }, [] (VEC mmSum) {return mm<VEC, larger_t>::sum(mmSum);}, [] (larger_t sum, DATAIN tmp) {return sum + tmp;}, [] (larger_t sum, size_t numValues) {return sum / numValues;});
            }
        };

        void RunAggregate(
                const AggregateConfiguration & config) override {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                _ReadWriteBarrier();
                std::visit(Aggregator<false>(*this, config, iteration), config.mode);
            }
        }

        bool DoAggregateChecked(
                const AggregateConfiguration & config) override {
            return std::visit(AggregateSelector(), config.mode);
        }

        void RunAggregateChecked(
                const AggregateConfiguration & config) override {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                _ReadWriteBarrier();
                std::visit(Aggregator<true>(*this, config, iteration), config.mode);
            }
        }

        template<bool check>
        void RunDecodeInternal(
                const DecodeConfiguration & config) {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                _ReadWriteBarrier();
                auto inV = config.source.template begin<hamming_simd_t>();
                const auto inVend = this->template ComputeEnd<hamming_scalar_t, hamming_simd_t>(inV, config);
                auto outV = config.target.template begin<VEC>();
                while (inV <= (inVend - UNROLL)) {
                    for (size_t k = 0; k < UNROLL; ++k, ++inV) {
                        if ((!check) || inV->isValid()) {
                            *outV++ = inV->data;
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATAIN*>(inV) - config.source.template begin<DATAIN>(), iteration);
                        }
                    }
                }
                for (; inV < (inVend - 1); ++inV) {
                    if ((!check) || inV->isValid()) {
                        *outV++ = inV->data;
                    } else {
                        throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATAIN*>(inV) - config.source.template begin<DATAIN>(), iteration);
                    }
                }
                if (inV < inVend) {
                    auto inS = reinterpret_cast<hamming_scalar_t*>(inV);
                    const auto inSend = reinterpret_cast<hamming_scalar_t * const >(inVend);
                    auto outS = reinterpret_cast<DATAIN*>(outV);
                    for (; inS < inSend; ++inS) {
                        if ((!check) || inS->isValid()) {
                            *outS++ = inS->data;
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATAIN*>(inS) - config.source.template begin<DATAIN>(), iteration);
                        }
                    }
                }
            }
        }

        bool DoDecode() override {
            return true;
        }

        void RunDecode(
                const DecodeConfiguration & config) override {
            RunDecodeInternal<false>(config);
        }

        void RunDecodeChecked(
                const DecodeConfiguration & config) override {
            RunDecodeInternal<true>(config);
        }
    };

}
