// Copyright 2016 Till Kolditz, Stefan de Bruijn
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

#ifndef XOR_SSE42
#error "Clients must not include this file directly, but file <XOR/XOR_sse42.hpp>!"
#endif

#include <Util/Test.hpp>
#include <XOR/XOR_base.hpp>
#include <SIMD/SSE.hpp>
#include <Util/ArithmeticSelector.hpp>
#include <Util/AggregateSelector.hpp>

namespace coding_benchmark {

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

    template<typename DATA, typename CS, size_t BLOCKSIZE>
    struct XOR_sse42 :
            public Test<DATA, CS> {

        typedef __m128i VEC;

        static const constexpr size_t NUM_VALUES_PER_SIMDREG = sizeof(VEC) / sizeof(DATA); // sizeof(VEC) / sizeof (DATA);
        static const constexpr size_t NUM_VALUES_PER_BLOCK = NUM_VALUES_PER_SIMDREG * BLOCKSIZE;

        using Test<DATA, CS>::Test;

        virtual ~XOR_sse42() {
        }

        void RunEncode(
                const EncodeConfiguration & config) override {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                _ReadWriteBarrier();
                auto dataIn = config.source.template begin<VEC>();
                auto dataInEnd = reinterpret_cast<VEC *>(reinterpret_cast<DATA*>(dataIn) + config.numValues);
                auto pChkOut = config.target.template begin<CS>();
                if (config.numValues >= NUM_VALUES_PER_BLOCK) {
                    while (dataIn <= (dataInEnd - BLOCKSIZE)) {
                        VEC checksum = _mm_setzero_si128();
                        auto pDataOut = reinterpret_cast<VEC *>(pChkOut);
                        for (size_t k = 0; k < BLOCKSIZE; ++k) {
                            auto tmp = _mm_lddqu_si128(dataIn++);
                            _mm_storeu_si128(pDataOut++, tmp);
                            checksum = _mm_xor_si128(checksum, tmp);
                        }
                        pChkOut = reinterpret_cast<CS*>(pDataOut);
                        *pChkOut++ = XOR<VEC, CS>::computeFinalChecksum(checksum);
                    }
                }
                // checksum remaining values which do not fit in the block size
                if (config.numValues >= NUM_VALUES_PER_SIMDREG) {
                    if (dataIn <= (dataInEnd - 1)) {
                        VEC checksum = _mm_setzero_si128();
                        auto dataOut2 = reinterpret_cast<VEC *>(pChkOut);
                        do {
                            auto tmp = _mm_lddqu_si128(dataIn++);
                            _mm_storeu_si128(dataOut2++, tmp);
                            checksum = _mm_xor_si128(checksum, tmp);
                        } while (dataIn <= (dataInEnd - 1));
                        pChkOut = reinterpret_cast<CS*>(dataOut2);
                        *pChkOut++ = XOR<VEC, CS>::computeFinalChecksum(checksum);
                    }
                }
                // checksum remaining integers which do not fit in the SIMD register
                if (dataIn < dataInEnd) {
                    DATA checksum = 0;
                    auto dataEnd = reinterpret_cast<DATA*>(dataInEnd);
                    auto pDataOut = reinterpret_cast<DATA*>(pChkOut);
                    for (auto data = reinterpret_cast<DATA*>(dataIn); data < dataEnd; ++data) {
                        auto & tmp = *data;
                        *pDataOut++ = tmp;
                        checksum ^= tmp;
                    }
                    *pDataOut = XOR<DATA, DATA>::computeFinalChecksum(checksum);
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
                auto data128 = config.target.template begin<VEC>();
                if (config.numValues >= NUM_VALUES_PER_BLOCK) {
                    while (i <= (config.numValues - NUM_VALUES_PER_BLOCK)) {
                        VEC checksum = _mm_setzero_si128();
                        for (size_t k = 0; k < BLOCKSIZE; ++k) {
                            checksum = _mm_xor_si128(checksum, _mm_lddqu_si128(data128++));
                        }
                        auto pChksum = reinterpret_cast<CS*>(data128);
                        if (XORdiff<CS>::checksumsDiffer(*pChksum, XOR<VEC, CS>::computeFinalChecksum(checksum))) {
                            throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                        }
                        ++pChksum; // fourth, advance after the checksum to the next block of values
                        data128 = reinterpret_cast<VEC *>(pChksum);
                        i += NUM_VALUES_PER_BLOCK;
                    }
                }
                // checksum remaining values which do not fit in the block size
                if (config.numValues >= NUM_VALUES_PER_SIMDREG) {
                    if (i <= (config.numValues - NUM_VALUES_PER_SIMDREG)) {
                        VEC checksum = _mm_setzero_si128();
                        do {
                            checksum = _mm_xor_si128(checksum, _mm_lddqu_si128(data128++));
                            i += NUM_VALUES_PER_SIMDREG;
                        } while (i <= (config.numValues - NUM_VALUES_PER_SIMDREG));
                        auto pChksum = reinterpret_cast<CS*>(data128);
                        if (XORdiff<CS>::checksumsDiffer(*pChksum, XOR<VEC, CS>::computeFinalChecksum(checksum))) {
                            throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                        }
                        ++pChksum; // fourth, advance after the checksum to the next block of values
                        data128 = reinterpret_cast<VEC *>(pChksum);
                    }
                }
                // checksum remaining integers which do not fit in the SIMD register, so we do it on the actual data width denoted by template parameter IN
                if (i < config.numValues) {
                    DATA checksum = 0;
                    auto data = reinterpret_cast<DATA*>(data128);
                    for (; i < config.numValues; ++i) {
                        checksum ^= *data++;
                    }
                    if (XORdiff<DATA>::checksumsDiffer(*data, XOR<DATA, DATA>::computeFinalChecksum(checksum))) {
                        throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                    }
                }
            }
        }

        bool DoArithmetic(
                const ArithmeticConfiguration & config) override {
            return std::visit(ArithmeticSelector(), config.mode);
        }

        struct Arithmetor {
            XOR_sse42 & test;
            const ArithmeticConfiguration & config;
            Arithmetor(
                    XOR_sse42 & test,
                    const ArithmeticConfiguration & config)
                    : test(test),
                      config(config) {
            }
            template<template<typename = void> class Functor>
            void impl() {
                Functor<> functor;
                size_t i = 0;
                auto data128In = config.source.template begin<VEC>();
                auto data128Out = config.target.template begin<VEC>();
                auto mmOperand = simd::mm<VEC, DATA>::set1(config.operand);
                if (config.numValues >= NUM_VALUES_PER_BLOCK) {
                    while (i <= (config.numValues - NUM_VALUES_PER_BLOCK)) {
                        VEC checksum = _mm_setzero_si128();
                        for (size_t k = 0; k < BLOCKSIZE; ++k) {
                            auto mmTmp = simd::mm_op<VEC, DATA, Functor>::compute(*data128In++, mmOperand);
                            checksum = simd::mm_op<VEC, DATA, xor_is>::cmp(checksum, mmTmp);
                            *data128Out++ = mmTmp;
                        }
                        auto pChkOut = reinterpret_cast<CS*>(data128Out);
                        *pChkOut++ = XOR<VEC, CS>::computeFinalChecksum(checksum);
                        data128Out = reinterpret_cast<VEC *>(pChkOut);
                        data128In = reinterpret_cast<VEC *>(reinterpret_cast<CS*>(data128In) + 1);
                        i += NUM_VALUES_PER_BLOCK;
                    }
                }
                // checksum remaining values which do not fit in the block size
                if ((config.numValues - i) >= NUM_VALUES_PER_SIMDREG) {
                    if (i <= (config.numValues - NUM_VALUES_PER_SIMDREG)) {
                        VEC checksum = _mm_setzero_si128();
                        do {
                            auto mmTmp = simd::mm_op<VEC, DATA, Functor>::compute(_mm_lddqu_si128(data128In++), mmOperand);
                            checksum = _mm_xor_si128(checksum, mmTmp);
                            _mm_storeu_si128(data128Out++, mmTmp);
                            i += NUM_VALUES_PER_SIMDREG;
                        } while (i <= (config.numValues - NUM_VALUES_PER_SIMDREG));
                        auto pChkOut = reinterpret_cast<CS*>(data128Out);
                        *pChkOut++ = XOR<VEC, CS>::computeFinalChecksum(checksum);
                        data128Out = reinterpret_cast<VEC *>(pChkOut);
                        data128In = reinterpret_cast<VEC *>(reinterpret_cast<CS*>(data128In) + 1);
                    }
                }
                // checksum remaining integers which do not fit in the SIMD register, so we do it on the actual data width denoted by template parameter IN
                if (i < config.numValues) {
                    DATA checksum = 0;
                    auto dataIn = reinterpret_cast<DATA*>(data128In);
                    auto dataOut = reinterpret_cast<DATA*>(data128Out);
                    for (; i < config.numValues; ++i) {
                        const auto tmp = functor(*dataIn++, config.operand);
                        checksum ^= tmp;
                        *dataOut++ = tmp;
                    }
                    *dataOut = checksum;
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

        bool DoArithmeticChecked(
                const ArithmeticConfiguration & config) override {
            return std::visit(ArithmeticSelector(), config.mode);
        }

        struct ArithmetorChecked {
            XOR_sse42 & test;
            const ArithmeticConfiguration & config;
            const size_t iteration;
            ArithmetorChecked(
                    XOR_sse42 & test,
                    const ArithmeticConfiguration & config,
                    const size_t iteration)
                    : test(test),
                      config(config),
                      iteration(iteration) {
            }
            template<template<typename = void> class func>
            void impl() {
                func<> functor;
                size_t i = 0;
                auto data128In = config.source.template begin<VEC>();
                auto data128Out = config.target.template begin<VEC>();
                auto mmOperand = simd::mm<VEC, DATA>::set1(config.operand);
                if (config.numValues >= NUM_VALUES_PER_BLOCK) {
                    while (i <= (config.numValues - NUM_VALUES_PER_BLOCK)) {
                        VEC oldChecksum = _mm_setzero_si128();
                        VEC newChecksum = _mm_setzero_si128();
                        for (size_t k = 0; k < BLOCKSIZE; ++k) {
                            auto mmTmp = _mm_lddqu_si128(data128In++);
                            oldChecksum = _mm_xor_si128(oldChecksum, mmTmp);
                            mmTmp = simd::mm_op<VEC, DATA, func>::compute(mmTmp, mmOperand);
                            newChecksum = _mm_xor_si128(newChecksum, mmTmp);
                            _mm_storeu_si128(data128Out++, mmTmp);
                        }
                        auto storedChecksum = reinterpret_cast<CS*>(data128In);
                        if (XORdiff<CS>::checksumsDiffer(*storedChecksum, XOR<VEC, CS>::computeFinalChecksum(oldChecksum))) {
                            throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                        }
                        data128In = reinterpret_cast<VEC *>(storedChecksum + 1);
                        auto newChecksumOut = reinterpret_cast<CS*>(data128Out);
                        *newChecksumOut++ = XOR<VEC, CS>::computeFinalChecksum(newChecksum);
                        data128Out = reinterpret_cast<VEC *>(newChecksumOut);
                        i += NUM_VALUES_PER_BLOCK;
                    }
                }
                if (config.numValues >= NUM_VALUES_PER_SIMDREG) {
                    // checksum remaining values which do not fit in the block size
                    if (i <= (config.numValues - NUM_VALUES_PER_SIMDREG)) {
                        VEC oldChecksum = _mm_setzero_si128();
                        VEC newChecksum = _mm_setzero_si128();
                        do {
                            auto mmTmp = _mm_lddqu_si128(data128In++);
                            oldChecksum = _mm_xor_si128(oldChecksum, mmTmp);
                            mmTmp = simd::mm_op<VEC, DATA, func>::compute(mmTmp, mmOperand);
                            newChecksum = _mm_xor_si128(newChecksum, mmTmp);
                            _mm_storeu_si128(data128Out++, mmTmp);
                            i += NUM_VALUES_PER_SIMDREG;
                        } while (i <= (config.numValues - NUM_VALUES_PER_SIMDREG));
                        auto storedChecksum = reinterpret_cast<CS*>(data128In);
                        if (XORdiff<CS>::checksumsDiffer(*storedChecksum, XOR<VEC, CS>::computeFinalChecksum(oldChecksum))) {
                            throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                        }
                        data128In = reinterpret_cast<VEC *>(storedChecksum + 1);
                        auto newChecksumOut = reinterpret_cast<CS*>(data128Out);
                        *newChecksumOut++ = XOR<VEC, CS>::computeFinalChecksum(newChecksum);
                        data128Out = reinterpret_cast<VEC *>(newChecksumOut);
                    }
                }
                // checksum remaining integers which do not fit in the SIMD register, so we do it on the actual data width denoted by template parameter IN
                if (i < config.numValues) {
                    DATA oldChecksum = 0;
                    DATA newChecksum = 0;
                    auto dataIn = reinterpret_cast<DATA*>(data128In);
                    auto dataOut = reinterpret_cast<DATA*>(data128Out);
                    for (; i < config.numValues; ++i) {
                        auto tmp = *dataIn++;
                        oldChecksum ^= tmp;
                        tmp = functor(tmp, config.operand);
                        newChecksum ^= tmp;
                        *dataOut++ = tmp;
                    }
                    if (XORdiff<DATA>::checksumsDiffer(*dataIn, XOR<DATA, DATA>::computeFinalChecksum(oldChecksum))) // test checksum
                            {
                        throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                    }
                    *dataOut = XOR<DATA, DATA>::computeFinalChecksum(newChecksum);
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

        bool DoAggregate(
                const AggregateConfiguration & config) override {
            return std::visit(AggregateSelector(), config.mode);
        }

        struct Aggregator {
            typedef typename Larger<DATA>::larger_t larger_t;
            XOR_sse42 & test;
            const AggregateConfiguration & config;
            Aggregator(
                    XOR_sse42 & test,
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
                size_t i = 0;
                auto data128In = config.source.template begin<VEC>();
                auto mmValue = funcInitVector();
                if (config.numValues >= NUM_VALUES_PER_BLOCK) {
                    while (i <= (config.numValues - NUM_VALUES_PER_BLOCK)) {
                        for (size_t k = 0; k < BLOCKSIZE; ++k) {
                            auto mmTmp = _mm_lddqu_si128(data128In++);
                            mmValue = funcKernelVector(mmValue, mmTmp);
                        }
                        data128In = reinterpret_cast<VEC *>(reinterpret_cast<CS*>(data128In) + 1);
                        i += NUM_VALUES_PER_BLOCK;
                    }
                }
                // checksum remaining values which do not fit in the block size
                if (config.numValues >= NUM_VALUES_PER_SIMDREG) {
                    if (i <= (config.numValues - NUM_VALUES_PER_SIMDREG)) {
                        do {
                            auto mmTmp = _mm_lddqu_si128(data128In++);
                            mmValue = funcKernelVector(mmValue, mmTmp);
                            i += NUM_VALUES_PER_SIMDREG;
                        } while (i <= (config.numValues - NUM_VALUES_PER_SIMDREG));
                        data128In = reinterpret_cast<VEC *>(reinterpret_cast<CS*>(data128In) + 1);
                    }
                }
                Aggregate value = funcVectorToScalar(mmValue);
                // checksum remaining integers which do not fit in the SIMD register, so we do it on the actual data width denoted by template parameter IN
                if (i < config.numValues) {
                    auto dataIn = reinterpret_cast<DATA*>(data128In);
                    for (; i < config.numValues; ++i) {
                        const DATA tmp = *dataIn++;
                        value = funcKernelScalar(value, tmp);
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
                impl<larger_t>([] {return simd::mm<VEC, larger_t>::set1(0);}, [](VEC mmSum, VEC mmTmp) {
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
                impl<larger_t>([] {return simd::mm<VEC, larger_t>::set1(0);}, [](VEC mmSum, VEC mmTmp) {
                    auto mmLo = simd::mm<VEC, DATA>::cvt_larger_lo(mmTmp);
                    mmLo = simd::mm_op<VEC, larger_t, add>::compute(mmSum, mmLo);
                    auto mmHi = simd::mm<VEC, DATA>::cvt_larger_hi(mmTmp);
                    return simd::mm_op<VEC, larger_t, add>::compute(mmLo, mmHi);
                }, [](VEC mmValue) {return simd::mm<VEC, larger_t>::sum(mmValue);}, [](larger_t value, DATA tmp) {return value + tmp;}, [](larger_t sum, size_t numValues) {return sum / numValues;});
            }
        };

        void RunAggregate(
                const AggregateConfiguration & config) override {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                _ReadWriteBarrier();
                std::visit(Aggregator(*this, config), config.mode);
            }
        }

        bool DoAggregateChecked(
                const AggregateConfiguration & config) override {
            return std::visit(AggregateSelector(), config.mode);
        }

        struct AggregatorChecked {
            typedef typename Larger<DATA>::larger_t larger_t;
            XOR_sse42 & test;
            const AggregateConfiguration & config;
            size_t iteration;
            AggregatorChecked(
                    XOR_sse42 & test,
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
                auto data128In = config.source.template begin<VEC>();
                auto mmValue = funcInitVector();
                if (config.numValues >= NUM_VALUES_PER_BLOCK) {
                    while (i <= (config.numValues - NUM_VALUES_PER_BLOCK)) {
                        VEC checksum = _mm_setzero_si128();
                        for (size_t k = 0; k < BLOCKSIZE; ++k) {
                            auto mmTmp = _mm_lddqu_si128(data128In++);
                            checksum = _mm_xor_si128(checksum, mmTmp);
                            mmValue = funcKernelVector(mmValue, mmTmp);
                        }
                        CS storedChecksum = *reinterpret_cast<CS*>(data128In);
                        data128In = reinterpret_cast<VEC *>(reinterpret_cast<CS*>(data128In) + 1);
                        if (XORdiff<CS>::checksumsDiffer(storedChecksum, XOR<VEC, CS>::computeFinalChecksum(checksum))) {
                            throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                        }
                        i += NUM_VALUES_PER_BLOCK;
                    }
                }
                // checksum remaining values which do not fit in the block size
                if (config.numValues >= NUM_VALUES_PER_SIMDREG) {
                    if (i <= (config.numValues - NUM_VALUES_PER_SIMDREG)) {
                        VEC checksum = _mm_setzero_si128();
                        do {
                            auto mmTmp = _mm_lddqu_si128(data128In++);
                            checksum = _mm_xor_si128(checksum, mmTmp);
                            mmValue = funcKernelVector(mmValue, mmTmp);
                            i += NUM_VALUES_PER_SIMDREG;
                        } while (i <= (config.numValues - NUM_VALUES_PER_SIMDREG));
                        CS storedChecksum = *reinterpret_cast<CS*>(data128In);
                        data128In = reinterpret_cast<VEC *>(reinterpret_cast<CS*>(data128In) + 1);
                        if (XORdiff<CS>::checksumsDiffer(storedChecksum, XOR<VEC, CS>::computeFinalChecksum(checksum))) {
                            throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                        }
                    }
                }
                Aggregate value = funcVectorToScalar(mmValue);
                // checksum remaining integers which do not fit in the SIMD register, so we do it on the actual data width denoted by template parameter IN
                if (i < config.numValues) {
                    DATA checksum = 0;
                    auto dataIn = reinterpret_cast<DATA*>(data128In);
                    for (; i < config.numValues; ++i) {
                        const DATA tmp = *dataIn++;
                        checksum ^= tmp;
                        value = funcKernelScalar(value, tmp);
                    }
                    DATA storedChecksum = *dataIn;
                    if (XORdiff<DATA>::checksumsDiffer(storedChecksum, XOR<DATA, DATA>::computeFinalChecksum(checksum))) {
                        throw ErrorInfo(__FILE__, __LINE__, i, iteration);
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
                impl<larger_t>([] {return simd::mm<VEC, larger_t>::set1(0);}, [](VEC mmSum, VEC mmTmp) {
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
                impl<larger_t>([] {return simd::mm<VEC, larger_t>::set1(0);}, [](VEC mmSum, VEC mmTmp) {
                    auto mmLo = simd::mm<VEC, DATA>::cvt_larger_lo(mmTmp);
                    mmLo = simd::mm_op<VEC, larger_t, add>::compute(mmSum, mmLo);
                    auto mmHi = simd::mm<VEC, DATA>::cvt_larger_hi(mmTmp);
                    return simd::mm_op<VEC, larger_t, add>::compute(mmLo, mmHi);
                }, [](VEC mmValue) {return simd::mm<VEC, larger_t>::sum(mmValue);}, [](larger_t value, DATA tmp) {return value + tmp;}, [](larger_t sum, size_t numValues) {return sum / numValues;});
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

        virtual void RunDecode(
                const DecodeConfiguration & config) override {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                _ReadWriteBarrier();
                size_t i = 0;
                auto dataIn = config.source.template begin<CS>();
                auto dataOut = config.target.template begin<VEC>();
                if (config.numValues >= NUM_VALUES_PER_BLOCK) {
                    for (; i <= (config.numValues - NUM_VALUES_PER_BLOCK); i += NUM_VALUES_PER_BLOCK) {
                        auto dataIn2 = reinterpret_cast<VEC *>(dataIn);
                        for (size_t k = 0; k < BLOCKSIZE; ++k) {
                            *dataOut++ = *dataIn2++;
                        }
                        dataIn = reinterpret_cast<CS*>(dataIn2) + 1;
                    }
                }
                // checksum remaining values which do not fit in the block size
                if (config.numValues >= NUM_VALUES_PER_SIMDREG) {
                    if (i <= (config.numValues - NUM_VALUES_PER_SIMDREG)) {
                        auto dataIn2 = reinterpret_cast<VEC *>(dataIn);
                        for (; i <= (config.numValues - NUM_VALUES_PER_SIMDREG); i += NUM_VALUES_PER_SIMDREG) {
                            *dataOut++ = *dataIn2++;
                        }
                        dataIn = reinterpret_cast<CS*>(dataIn2) + 1;
                    }
                }
                // checksum remaining integers which do not fit in the SIMD register
                if (i < config.numValues) {
                    auto dataIn2 = reinterpret_cast<DATA*>(dataIn);
                    auto dataOut2 = reinterpret_cast<DATA*>(dataOut);
                    for (; i < config.numValues; ++i) {
                        *dataOut2++ = *dataIn2++;
                    }
                }
            }
        }

        bool DoDecodeChecked() override {
            return true;
        }

        virtual void RunDecodeChecked(
                const DecodeConfiguration & config) override {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                _ReadWriteBarrier();
                size_t i = 0;
                auto dataIn = config.source.template begin<CS>();
                auto dataOut = config.target.template begin<VEC>();
                if (config.numValues >= NUM_VALUES_PER_BLOCK) {
                    for (; i <= (config.numValues - NUM_VALUES_PER_BLOCK); i += NUM_VALUES_PER_BLOCK) {
                        VEC checksum = _mm_setzero_si128();
                        auto dataIn2 = reinterpret_cast<VEC *>(dataIn);
                        for (size_t k = 0; k < BLOCKSIZE; ++k) {
                            auto mmTmp = _mm_lddqu_si128(dataIn2++);
                            checksum = _mm_xor_si128(checksum, mmTmp);
                            _mm_storeu_si128(dataOut++, mmTmp);
                        }
                        CS storedChecksum = *reinterpret_cast<CS*>(dataIn2);
                        if (XORdiff<CS>::checksumsDiffer(storedChecksum, XOR<VEC, CS>::computeFinalChecksum(checksum))) {
                            throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                        }
                        dataIn = reinterpret_cast<CS*>(dataIn2) + 1;
                    }
                }
                // checksum remaining values which do not fit in the block size
                if (config.numValues >= NUM_VALUES_PER_SIMDREG) {
                    if (i <= (config.numValues - NUM_VALUES_PER_SIMDREG)) {
                        VEC checksum = _mm_setzero_si128();
                        auto dataIn2 = reinterpret_cast<VEC *>(dataIn);
                        for (; i <= (config.numValues - NUM_VALUES_PER_SIMDREG); i += NUM_VALUES_PER_SIMDREG) {
                            auto mmTmp = _mm_lddqu_si128(dataIn2++);
                            checksum = _mm_xor_si128(checksum, mmTmp);
                            _mm_storeu_si128(dataOut++, mmTmp);
                        }
                        CS storedChecksum = *reinterpret_cast<CS*>(dataIn2);
                        if (XORdiff<CS>::checksumsDiffer(storedChecksum, XOR<VEC, CS>::computeFinalChecksum(checksum))) {
                            throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                        }
                        dataIn = reinterpret_cast<CS*>(dataIn2) + 1;
                    }
                }
                // checksum remaining integers which do not fit in the SIMD register
                if (i < config.numValues) {
                    DATA checksum = 0;
                    auto dataIn2 = reinterpret_cast<DATA*>(dataIn);
                    auto dataOut2 = reinterpret_cast<DATA*>(dataOut);
                    for (; i < config.numValues; ++i) {
                        const DATA tmp = *dataIn2++;
                        checksum ^= tmp;
                        *dataOut2++ = tmp;
                    }
                    DATA storedChecksum = *dataIn2;
                    if (XORdiff<DATA>::checksumsDiffer(storedChecksum, XOR<DATA, DATA>::computeFinalChecksum(checksum))) {
                        throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                    }
                }
            }
        }
    };

}
