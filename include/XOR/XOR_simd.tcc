// Copyright 2016-2018 Till Kolditz, Stefan de Bruijn
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
 * File:   XOR_simd.tcc
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 07-07-2017 17:18
 * Renamed on 19-03-2018 10:25
 */

#pragma once

#ifndef XOR_SIMD
#error "Clients must not include this file directly, but file <XOR/XOR_simd.hpp>!"
#endif

#include <Util/Test.hpp>
#include <XOR/XOR_base.hpp>
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

namespace coding_benchmark {

#ifdef __SSE4_2__
    template<>
    struct XOR<__m128i, __m128i> {
    static __m128i computeFinalChecksum(
            __m128i & checksum);
    };

    template<>
    struct XOR<__m128i, uint32_t> {
        static uint32_t
        computeFinalChecksum(
                __m128i & checksum);
    };

    template<>
    struct XOR<__m128i, uint16_t> {
        static uint16_t
        computeFinalChecksum(
                __m128i & checksum);
    };

    template<>
    struct XOR<__m128i, uint8_t> {
        static uint8_t
        computeFinalChecksum(
                __m128i & checksum);
    };

    template<>
    struct XORdiff<__m128i> {
    static bool
    checksumsDiffer(
            __m128i checksum1,
            __m128i checksum2);
    };
#endif

#ifdef __AVX2__
    template<>
    struct XOR<__m256i, __m256i> {
    static __m256i computeFinalChecksum(
            __m256i & checksum);
    };

    template<>
    struct XOR<__m256i, uint32_t> {
        static uint32_t computeFinalChecksum(
                __m256i & checksum);
    };

    template<>
    struct XOR<__m256i, uint16_t> {
        static uint16_t computeFinalChecksum(
                __m256i & checksum);
    };

    template<>
    struct XOR<__m256i, uint8_t> {
        static uint8_t computeFinalChecksum(
                __m256i & checksum);
    };

    template<>
    struct XORdiff<__m256i> {
    static bool checksumsDiffer(
            __m256i checksum1,
            __m256i checksum2);
    };
#endif

    template<typename DATA, typename CS, typename VEC, size_t BLOCKSIZE>
    struct XOR_simd :
            public Test<DATA, CS>,
            SIMDTest<VEC> {

        static const constexpr size_t NUM_VALUES_PER_SIMDREG = sizeof(VEC) / sizeof(DATA);
        static const constexpr size_t NUM_VALUES_PER_BLOCK = NUM_VALUES_PER_SIMDREG * BLOCKSIZE;

        using Test<DATA, CS>::Test;

        virtual ~XOR_simd() {
        }

        void RunEncode(
                const EncodeConfiguration & config) override {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                _ReadWriteBarrier();
                size_t i = 0;
                auto inV = config.source.template begin<VEC>();
                auto outV = config.target.template begin<CS>();
                if (config.numValues >= NUM_VALUES_PER_BLOCK) {
                    for (; i <= (config.numValues - NUM_VALUES_PER_BLOCK); i += NUM_VALUES_PER_BLOCK) {
                        VEC checksum = simd::mm<VEC>::setzero();
                        auto pDataOut = reinterpret_cast<VEC *>(outV);
                        for (size_t k = 0; k < BLOCKSIZE; ++k) {
                            auto tmp = simd::mm<VEC>::loadu(inV++);
                            simd::mm<VEC>::storeu(pDataOut++, tmp);
                            checksum = simd::mm_op<VEC, DATA, xor_is>::cmp(checksum, tmp);
                        }
                        outV = reinterpret_cast<CS*>(pDataOut);
                        simd::mm<VEC>::storeu(outV++, XOR<VEC, CS>::computeFinalChecksum(checksum));
                    }
                }
                // checksum remaining values which do not fit in the block size
                if (config.numValues >= NUM_VALUES_PER_SIMDREG && i <= (config.numValues - NUM_VALUES_PER_SIMDREG)) {
                    VEC checksum = simd::mm<VEC>::setzero();
                    auto dataOut2 = reinterpret_cast<VEC *>(outV);
                    do {
                        auto tmp = simd::mm<VEC>::loadu(inV++);
                        *dataOut2++ = tmp;
                        checksum = simd::mm_op<VEC, DATA, xor_is>::cmp(checksum, tmp);
                        i += NUM_VALUES_PER_SIMDREG;
                    } while (i <= (config.numValues - NUM_VALUES_PER_SIMDREG));
                    outV = reinterpret_cast<CS*>(dataOut2);
                    simd::mm<VEC>::storeu(outV++, XOR<VEC, CS>::computeFinalChecksum(checksum));
                }
                // checksum remaining integers which do not fit in the SIMD register
                if (i < config.numValues) {
                    DATA checksum = 0;
                    auto inS = reinterpret_cast<DATA*>(inV);
                    auto outS = reinterpret_cast<DATA*>(outV);
                    for (; i < config.numValues; ++i) {
                        auto tmp = *inS++;
                        *outS++ = tmp;
                        checksum ^= tmp;
                    }
                    *outS = checksum;
                }
            }
        }

        virtual bool DoCheck() override {
            return true;
        }

        virtual void RunCheck(
                const CheckConfiguration & config) override {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                _ReadWriteBarrier();
                size_t i = 0;
                auto inV = config.target.template begin<VEC>();
                if (config.numValues >= NUM_VALUES_PER_BLOCK) {
                    for (; i <= (config.numValues - NUM_VALUES_PER_BLOCK); i += NUM_VALUES_PER_BLOCK) {
                        VEC checksum = simd::mm<VEC>::setzero();
                        for (size_t k = 0; k < BLOCKSIZE; ++k) {
                            checksum = simd::mm_op<VEC, DATA, xor_is>::cmp(checksum, simd::mm<VEC>::loadu(inV++));
                        }
                        auto pChksum = reinterpret_cast<CS*>(inV);
                        if (XORdiff<CS>::checksumsDiffer(*pChksum, XOR<VEC, CS>::computeFinalChecksum(checksum))) {
                            throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATA*>(inV) - config.source.template begin<DATA>(), iteration);
                        }
                        inV = reinterpret_cast<VEC *>(pChksum + 1);
                    }
                }
                // checksum remaining values which do not fit in the block size
                if (config.numValues >= NUM_VALUES_PER_SIMDREG && i <= (config.numValues - NUM_VALUES_PER_SIMDREG)) {
                    VEC checksum = simd::mm<VEC>::setzero();
                    do {
                        checksum = simd::mm_op<VEC, DATA, xor_is>::cmp(checksum, simd::mm<VEC>::loadu(inV++));
                        i += NUM_VALUES_PER_SIMDREG;
                    } while (i <= (config.numValues - NUM_VALUES_PER_SIMDREG));
                    auto pChksum = reinterpret_cast<CS*>(inV);
                    if (XORdiff<CS>::checksumsDiffer(*pChksum, XOR<VEC, CS>::computeFinalChecksum(checksum))) {
                        throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATA*>(inV) - config.source.template begin<DATA>(), iteration);
                    }
                    inV = reinterpret_cast<VEC *>(pChksum + 1);
                }
                // checksum remaining integers which do not fit in the SIMD register
                if (i < config.numValues) {
                    DATA checksum = 0;
                    auto inS = reinterpret_cast<DATA*>(inV);
                    for (; i < config.numValues; ++i) {
                        checksum ^= *inS++;
                    }
                    if (XORdiff<DATA>::checksumsDiffer(*inS, checksum)) {
                        throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATA*>(inS) - config.source.template begin<DATA>(), iteration);
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
            XOR_simd & test;
            const ArithmeticConfiguration & config;
            size_t iteration;
            Arithmetor(
                    XOR_simd & test,
                    const ArithmeticConfiguration & config,
                    size_t iteration)
                    : test(test),
                      config(config),
                      iteration(iteration) {
            }
            template<template<typename = void> class Functor>
            void impl() {
                size_t i = 0;
                auto inV = config.source.template begin<VEC>();
                auto outV = config.target.template begin<VEC>();
                auto mmOperand = simd::mm<VEC, DATA>::set1(config.operand);
                if (config.numValues >= NUM_VALUES_PER_BLOCK) {
                    for (; i <= (config.numValues - NUM_VALUES_PER_BLOCK); i += NUM_VALUES_PER_BLOCK) {
                        VEC __attribute__((unused)) oldChecksum = simd::mm<VEC>::setzero();
                        VEC newChecksum = simd::mm<VEC>::setzero();
                        for (size_t k = 0; k < BLOCKSIZE; ++k) {
                            auto mmTmp = simd::mm<VEC>::loadu(inV++);
                            if constexpr (check) {
                                oldChecksum = simd::mm_op<VEC, DATA, xor_is>::cmp(oldChecksum, mmTmp);
                            }
                            mmTmp = simd::mm_op<VEC, DATA, Functor>::compute(mmTmp, mmOperand);
                            newChecksum = simd::mm_op<VEC, DATA, xor_is>::cmp(newChecksum, mmTmp);
                            simd::mm<VEC>::storeu(outV++, mmTmp);
                        }
                        CS * const pStoredChecksum = reinterpret_cast<CS*>(inV);
                        if constexpr (check) {
                            if (XORdiff<CS>::checksumsDiffer(*pStoredChecksum, XOR<VEC, CS>::computeFinalChecksum(oldChecksum))) {
                                throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATA*>(inV) - config.source.template begin<DATA>(), iteration);
                            }
                        }
                        auto pChkOut = reinterpret_cast<CS*>(outV);
                        *pChkOut++ = XOR<VEC, CS>::computeFinalChecksum(newChecksum);
                        outV = reinterpret_cast<VEC *>(pChkOut);
                        inV = reinterpret_cast<VEC *>(pStoredChecksum + 1);
                    }
                }
                // checksum remaining values which do not fit in the block size
                if (config.numValues >= NUM_VALUES_PER_SIMDREG && i <= (config.numValues - NUM_VALUES_PER_SIMDREG)) {
                    VEC __attribute__((unused)) oldChecksum = simd::mm<VEC>::setzero();
                    VEC newChecksum = simd::mm<VEC>::setzero();
                    do {
                        auto mmTmp = simd::mm<VEC>::loadu(inV++);
                        if constexpr (check) {
                            oldChecksum = simd::mm_op<VEC, DATA, xor_is>::cmp(oldChecksum, mmTmp);
                        }
                        mmTmp = simd::mm_op<VEC, DATA, Functor>::compute(mmTmp, mmOperand);
                        newChecksum = simd::mm_op<VEC, DATA, xor_is>::cmp(newChecksum, mmTmp);
                        simd::mm<VEC>::storeu(outV++, mmTmp);
                        i += NUM_VALUES_PER_SIMDREG;
                    } while (i <= (config.numValues - NUM_VALUES_PER_SIMDREG));
                    CS * const pStoredChecksum = reinterpret_cast<CS*>(inV);
                    if constexpr (check) {
                        if (XORdiff<CS>::checksumsDiffer(*pStoredChecksum, XOR<VEC, CS>::computeFinalChecksum(oldChecksum))) {
                            throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATA*>(inV) - config.source.template begin<DATA>(), iteration);
                        }
                    }
                    auto pChkOut = reinterpret_cast<CS*>(outV);
                    *pChkOut++ = XOR<VEC, CS>::computeFinalChecksum(newChecksum);
                    outV = reinterpret_cast<VEC *>(pChkOut);
                    inV = reinterpret_cast<VEC *>(pStoredChecksum + 1);
                }
                // checksum remaining integers which do not fit in the SIMD register, so we do it on the actual data width denoted by template parameter IN
                if (i < config.numValues) {
                    Functor<> functor;
                    DATA __attribute__((unused)) oldChecksum = 0;
                    DATA newChecksum = 0;
                    auto inS = reinterpret_cast<DATA*>(inV);
                    auto outS = reinterpret_cast<DATA*>(outV);
                    for (; i < config.numValues; ++i) {
                        auto tmp = *inS++;
                        if constexpr (check) {
                            oldChecksum ^= tmp;
                        }
                        tmp = functor(tmp, config.operand);
                        newChecksum ^= tmp;
                        *outS++ = tmp;
                    }
                    *outS = newChecksum;
                    if constexpr (check) {
                        if (XORdiff<DATA>::checksumsDiffer(*inS, oldChecksum)) {
                            throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATA*>(inS) - config.source.template begin<DATA>(), iteration);
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

        template<bool check>
        struct Aggregator {
            typedef typename Larger<DATA>::larger_t larger_t;
            XOR_simd & test;
            const AggregateConfiguration & config;
            size_t iteration;
            Aggregator(
                    XOR_simd & test,
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
                size_t i = 0;
                auto inV = config.source.template begin<VEC>();
                auto mmValue = funcInitVector();
                if (config.numValues >= NUM_VALUES_PER_BLOCK) {
                    for (; i <= (config.numValues - NUM_VALUES_PER_BLOCK); i += NUM_VALUES_PER_BLOCK) {
                        VEC __attribute__((unused)) oldChecksum = simd::mm<VEC>::setzero();
                        for (size_t k = 0; k < BLOCKSIZE; ++k) {
                            auto mmTmp = simd::mm<VEC>::loadu(inV++);
                            if constexpr (check) {
                                oldChecksum = simd::mm_op<VEC, DATA, xor_is>::cmp(oldChecksum, mmTmp);
                            }
                            mmValue = funcKernelVector(mmValue, mmTmp);
                        }
                        CS * const pStoredChecksum = reinterpret_cast<CS*>(inV);
                        if constexpr (check) {
                            if (XORdiff<CS>::checksumsDiffer(*pStoredChecksum, XOR<VEC, CS>::computeFinalChecksum(oldChecksum))) {
                                throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATA*>(inV) - config.source.template begin<DATA>(), iteration);
                            }
                        }
                        inV = reinterpret_cast<VEC *>(pStoredChecksum + 1);
                    }
                }
                // checksum remaining values which do not fit in the block size
                if (config.numValues >= NUM_VALUES_PER_SIMDREG && i <= (config.numValues - NUM_VALUES_PER_SIMDREG)) {
                VEC __attribute__((unused)) oldChecksum = simd::mm<VEC>::setzero();
                    do {
                        auto mmTmp = simd::mm<VEC>::loadu(inV++);
                        if constexpr (check) {
                            oldChecksum = simd::mm_op<VEC, DATA, xor_is>::cmp(oldChecksum, mmTmp);
                        }
                        mmValue = funcKernelVector(mmValue, mmTmp);
                        i += NUM_VALUES_PER_SIMDREG;
                    } while (i <= (config.numValues - NUM_VALUES_PER_SIMDREG));
                    CS * const pStoredChecksum = reinterpret_cast<CS*>(inV);
                    if constexpr (check) {
                        if (XORdiff<CS>::checksumsDiffer(*pStoredChecksum, XOR<VEC, CS>::computeFinalChecksum(oldChecksum))) {
                            throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATA*>(inV) - config.source.template begin<DATA>(), iteration);
                        }
                    }
                    inV = reinterpret_cast<VEC *>(pStoredChecksum + 1);
                }
                Aggregate value = funcVectorToScalar(mmValue);
                // checksum remaining integers which do not fit in the SIMD register, so we do it on the actual data width denoted by template parameter IN
                if (i < config.numValues) {
                    DATA __attribute__((unused)) oldChecksum = 0;
                    auto inS = reinterpret_cast<DATA*>(inV);
                    for (; i < config.numValues; ++i) {
                        auto tmp = *inS++;
                        if constexpr (check) {
                            oldChecksum ^= tmp;
                        }
                        value = funcKernelScalar(value, tmp);
                    }
                    if constexpr (check) {
                        if (XORdiff<DATA>::checksumsDiffer(*inS, oldChecksum)) {
                            throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATA*>(inS) - config.source.template begin<DATA>(), iteration);
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
                impl<larger_t>([] {return simd::mm<VEC>::setzero();}, [](VEC mmSum, VEC mmTmp) {
                    auto mmLo = simd::mm<VEC, DATA>::cvt_larger_lo(mmTmp);
                    mmLo = simd::mm_op<VEC, larger_t, add>::compute(mmSum, mmLo);
                    auto mmHi = simd::mm<VEC, DATA>::cvt_larger_hi(mmTmp);
                    return simd::mm_op<VEC, larger_t, add>::compute(mmLo, mmHi);
                }, [](VEC mmValue) {return simd::mm<VEC, larger_t>::sum(mmValue);}, [](larger_t value, DATA tmp) {return value + tmp;}, [](larger_t sum, size_t numValues) {return sum;});
            }
            void operator()(
                    AggregateConfiguration::Min) {
                impl<DATA>([] {return simd::mm<VEC, DATA>::set1(std::numeric_limits<DATA>::max());}, [](VEC mmValue, VEC mmTmp) {return simd::mm<VEC, DATA>::min(mmValue, mmTmp);},
                        [](VEC mmValue) {return simd::mm<VEC, DATA>::min(mmValue);}, [](DATA value, DATA tmp) {return value < tmp ? value : tmp;}, [](DATA min, size_t numValues) {return min;});
            }
            void operator()(
                    AggregateConfiguration::Max) {
                impl<DATA>([] {return simd::mm<VEC, DATA>::set1(std::numeric_limits<DATA>::min());}, [](VEC mmValue, VEC mmTmp) {return simd::mm<VEC, DATA>::max(mmValue, mmTmp);},
                        [](VEC mmValue) {return simd::mm<VEC, DATA>::max(mmValue);}, [](DATA value, DATA tmp) {return value > tmp ? value : tmp;}, [](DATA max, size_t numValues) {return max;});
            }
            void operator()(
                    AggregateConfiguration::Avg) {
                impl<larger_t>([] {return simd::mm<VEC>::setzero();}, [](VEC mmSum, VEC mmTmp) {
                    auto mmLo = simd::mm<VEC, DATA>::cvt_larger_lo(mmTmp);
                    mmLo = simd::mm_op<VEC, larger_t, add>::compute(mmSum, mmLo);
                    auto mmHi = simd::mm<VEC, DATA>::cvt_larger_hi(mmTmp);
                    return simd::mm_op<VEC, larger_t, add>::compute(mmLo, mmHi);
                }, [](VEC mmValue) {return simd::mm<VEC, larger_t>::sum(mmValue);}, [](larger_t value, DATA tmp) {return value + tmp;}, [](larger_t sum, size_t numValues) {return sum / numValues;});
            }
        };

        bool DoAggregate(
                const AggregateConfiguration & config) override {
            return std::visit(AggregateSelector(), config.mode);
        }

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

private:
        template<bool check>
        void Decode(
                const DecodeConfiguration & config) {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                _ReadWriteBarrier();
                size_t i = 0;
                auto inV = config.source.template begin<VEC>();
                auto outV = config.target.template begin<VEC>();
                if (config.numValues >= NUM_VALUES_PER_BLOCK) {
                    for (; i <= (config.numValues - NUM_VALUES_PER_BLOCK); i += NUM_VALUES_PER_BLOCK) {
                        VEC __attribute__((unused)) checksum = simd::mm<VEC>::setzero();
                        for (size_t k = 0; k < BLOCKSIZE; ++k) {
                            VEC mmTmp = simd::mm<VEC>::loadu(inV++);
                            if constexpr (check) {
                                checksum = simd::mm_op<VEC, DATA, xor_is>::cmp(checksum, mmTmp);
                            }
                            simd::mm<VEC>::storeu(outV++, mmTmp);
                        }
                        auto pChksum = reinterpret_cast<CS*>(inV);
                        if constexpr (check) {
                            if (XORdiff<CS>::checksumsDiffer(*pChksum, XOR<VEC, CS>::computeFinalChecksum(checksum))) {
                                throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATA*>(inV) - config.source.template begin<DATA>(), iteration);
                            }
                        }
                        inV = reinterpret_cast<VEC *>(pChksum + 1);
                    }
                }
                // checksum remaining values which do not fit in the block size
                if (config.numValues >= NUM_VALUES_PER_SIMDREG && i <= (config.numValues - NUM_VALUES_PER_SIMDREG)) {
                    VEC __attribute__((unused)) checksum = simd::mm<VEC>::setzero();
                    do {
                        VEC mmTmp = simd::mm<VEC>::loadu(inV++);
                        if constexpr (check) {
                            checksum = simd::mm_op<VEC, DATA, xor_is>::cmp(checksum, mmTmp);
                        }
                        simd::mm<VEC>::storeu(outV++, mmTmp);
                        i += NUM_VALUES_PER_SIMDREG;
                    } while (i <= (config.numValues - NUM_VALUES_PER_SIMDREG));
                    auto pChksum = reinterpret_cast<CS*>(inV);
                    if constexpr (check) {
                        if (XORdiff<CS>::checksumsDiffer(*pChksum, XOR<VEC, CS>::computeFinalChecksum(checksum))) {
                            throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATA*>(inV) - config.source.template begin<DATA>(), iteration);
                        }
                    }
                    inV = reinterpret_cast<VEC *>(pChksum + 1);
                }
                // checksum remaining integers which do not fit in the SIMD register
                if (i < config.numValues) {
                    DATA __attribute__((unused)) oldChecksum = 0;
                    auto inS = reinterpret_cast<DATA*>(inV);
                    auto outS = reinterpret_cast<DATA*>(outV);
                    for (; i < config.numValues; ++i) {
                        DATA tmp = *inS++;
                        if constexpr (check) {
                            oldChecksum ^= tmp;
                        }
                        *outS++ = tmp;
                    }
                    if constexpr (check) {
                        if (XORdiff<DATA>::checksumsDiffer(*inS, oldChecksum)) {
                            throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATA*>(inS) - config.source.template begin<DATA>(), iteration);
                        }
                    }
                }
            }
        }

public:

        bool DoDecode() override {
            return true;
        }

        virtual void RunDecode(
                const DecodeConfiguration & config) override {
            this->Decode<false>(config);
        }

        virtual void RunDecodeChecked(
                const DecodeConfiguration & config) override {
            this->Decode<true>(config);
        }
    };

}
