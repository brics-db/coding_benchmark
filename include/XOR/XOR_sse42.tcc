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
#include <Util/ErrorInfo.hpp>
#include <Util/Functors.hpp>
#include <Util/Helpers.hpp>
#include <Util/ArithmeticSelector.hpp>
#include <Util/AggregateSelector.hpp>

namespace coding_benchmark {

    template<>
    struct XOR<__m128i, __m128i > {
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

        static const constexpr size_t NUM_VALUES_PER_SIMDREG = 16 / sizeof(DATA); // sizeof(__m128i) / sizeof (DATA);
        static const constexpr size_t NUM_VALUES_PER_BLOCK = NUM_VALUES_PER_SIMDREG * BLOCKSIZE;

        using Test<DATA, CS>::Test;

        virtual ~XOR_sse42() {
        }

        void RunEncode(
                const EncodeConfiguration & config) override {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                _ReadWriteBarrier();
                auto dataIn = this->bufRaw.template begin<__m128i >();
                auto dataInEnd = this->bufRaw.template end<__m128i >();
                auto pChkOut = this->bufEncoded.template begin<CS>();
                while (dataIn <= (dataInEnd - BLOCKSIZE)) {
                    __m128i checksum = _mm_setzero_si128();
                    auto pDataOut = reinterpret_cast<__m128i *>(pChkOut);
                    for (size_t k = 0; k < BLOCKSIZE; ++k) {
                        auto tmp = _mm_lddqu_si128(dataIn++);
                        _mm_storeu_si128(pDataOut++, tmp);
                        checksum = _mm_xor_si128(checksum, tmp);
                    }
                    pChkOut = reinterpret_cast<CS*>(pDataOut);
                    *pChkOut++ = XOR<__m128i, CS>::computeFinalChecksum(checksum);
                }
                // checksum remaining values which do not fit in the block size
                if (dataIn <= (dataInEnd - 1)) {
                    __m128i checksum = _mm_setzero_si128();
                    auto dataOut2 = reinterpret_cast<__m128i *>(pChkOut);
                    do {
                        auto tmp = _mm_lddqu_si128(dataIn++);
                        _mm_storeu_si128(dataOut2++, tmp);
                        checksum = _mm_xor_si128(checksum, tmp);
                    } while (dataIn <= (dataInEnd - 1));
                    pChkOut = reinterpret_cast<CS*>(dataOut2);
                    *pChkOut++ = XOR<__m128i, CS>::computeFinalChecksum(checksum);
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
                const size_t NUM_VALUES_PER_SIMDREG = sizeof(__m128i) / sizeof (DATA);
                const size_t NUM_VALUES_PER_BLOCK = NUM_VALUES_PER_SIMDREG * BLOCKSIZE;
                size_t numValues = this->getNumValues();
                size_t i = 0;
                auto data128 = this->bufEncoded.template begin<__m128i >();
                while (i <= (numValues - NUM_VALUES_PER_BLOCK)) {
                    __m128i checksum = _mm_setzero_si128();
                    for (size_t k = 0; k < BLOCKSIZE; ++k) {
                        checksum = _mm_xor_si128(checksum, _mm_lddqu_si128(data128++));
                    }
                    auto pChksum = reinterpret_cast<CS*>(data128);
                    if (XORdiff<CS>::checksumsDiffer(*pChksum, XOR<__m128i, CS>::computeFinalChecksum(checksum))) {
                        throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                    }
                    ++pChksum; // fourth, advance after the checksum to the next block of values
                    data128 = reinterpret_cast<__m128i *>(pChksum);
                    i += NUM_VALUES_PER_BLOCK;
                }
                // checksum remaining values which do not fit in the block size
                if (i <= (numValues - NUM_VALUES_PER_SIMDREG)) {
                    __m128i checksum = _mm_setzero_si128();
                    do {
                        checksum = _mm_xor_si128(checksum, _mm_lddqu_si128(data128++));
                        i += NUM_VALUES_PER_SIMDREG;
                    } while (i <= (numValues - NUM_VALUES_PER_SIMDREG));
                    auto pChksum = reinterpret_cast<CS*>(data128);
                    if (XORdiff<CS>::checksumsDiffer(*pChksum, XOR<__m128i, CS>::computeFinalChecksum(checksum))) {
                        throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                    }
                    ++pChksum; // fourth, advance after the checksum to the next block of values
                    data128 = reinterpret_cast<__m128i *>(pChksum);
                }
                // checksum remaining integers which do not fit in the SIMD register, so we do it on the actual data width denoted by template parameter IN
                if (i < numValues) {
                    DATA checksum = 0;
                    auto data = reinterpret_cast<DATA*>(data128);
                    for (; i < numValues; ++i) {
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
            template<template<typename = void> class func>
            void impl() {
                func<> functor;
                const constexpr size_t NUM_VALUES_PER_SIMDREG = sizeof(__m128i) /sizeof (DATA);
                const constexpr size_t NUM_VALUES_PER_BLOCK = NUM_VALUES_PER_SIMDREG * BLOCKSIZE;
                size_t numValues = test.template getNumValues();
                size_t i = 0;
                auto data128In = test.bufEncoded.template begin<__m128i >();
                auto data128Out = test.bufResult.template begin<__m128i >();
                auto mmOperand = simd::mm<__m128i, DATA>::set1(config.operand);
                while (i <= (numValues - NUM_VALUES_PER_BLOCK)) {
                    __m128i checksum = _mm_setzero_si128();
                    for (size_t k = 0; k < BLOCKSIZE; ++k) {
                        auto mmTmp = simd::mm_op<__m128i, DATA, func>::compute(_mm_lddqu_si128(data128In++), mmOperand);
                        checksum = _mm_xor_si128(checksum, mmTmp);
                        _mm_storeu_si128(data128Out++, mmTmp);
                    }
                    auto pChkOut = reinterpret_cast<CS*>(data128Out);
                    *pChkOut++ = XOR<__m128i, CS>::computeFinalChecksum(checksum);
                    data128Out = reinterpret_cast<__m128i *>(pChkOut);
                    data128In = reinterpret_cast<__m128i *>(reinterpret_cast<CS*>(data128In) + 1);
                    i += NUM_VALUES_PER_BLOCK;
                }
                // checksum remaining values which do not fit in the block size
                if (i <= (numValues - NUM_VALUES_PER_SIMDREG)) {
                    __m128i checksum = _mm_setzero_si128();
                    do {
                        auto mmTmp = simd::mm_op<__m128i, DATA, func>::compute(_mm_lddqu_si128(data128In++), mmOperand);
                        checksum = _mm_xor_si128(checksum, mmTmp);
                        _mm_storeu_si128(data128Out++, mmTmp);
                        i += NUM_VALUES_PER_SIMDREG;
                    } while (i <= (numValues - NUM_VALUES_PER_SIMDREG));
                    auto pChkOut = reinterpret_cast<CS*>(data128Out);
                    *pChkOut++ = XOR<__m128i, CS>::computeFinalChecksum(checksum);
                    data128Out = reinterpret_cast<__m128i *>(pChkOut);
                    data128In = reinterpret_cast<__m128i *>(reinterpret_cast<CS*>(data128In) + 1);
                }
                // checksum remaining integers which do not fit in the SIMD register, so we do it on the actual data width denoted by template parameter IN
                if (i < numValues) {
                    DATA checksum = 0;
                    auto dataIn = reinterpret_cast<DATA*>(data128In);
                    auto dataOut = reinterpret_cast<DATA*>(data128Out);
                    for (; i < numValues; ++i) {
                        const auto tmp = functor(*dataIn++, config.operand);
                        checksum ^= tmp;
                        *dataOut++ = tmp;
                    }
                    *dataOut = XOR<DATA, DATA>::computeFinalChecksum(checksum);
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
                const constexpr size_t NUM_VALUES_PER_SIMDREG = sizeof(__m128i) /sizeof (DATA);
                const constexpr size_t NUM_VALUES_PER_BLOCK = NUM_VALUES_PER_SIMDREG * BLOCKSIZE;
                size_t numValues = test.template getNumValues();
                size_t i = 0;
                auto data128In = test.bufEncoded.template begin<__m128i >();
                auto data128Out = test.bufResult.template begin<__m128i >();
                auto mmOperand = simd::mm<__m128i, DATA>::set1(config.operand);
                while (i <= (numValues - NUM_VALUES_PER_BLOCK)) {
                    __m128i oldChecksum = _mm_setzero_si128();
                    __m128i newChecksum = _mm_setzero_si128();
                    for (size_t k = 0; k < BLOCKSIZE; ++k) {
                        auto mmTmp = _mm_lddqu_si128(data128In++);
                        oldChecksum = _mm_xor_si128(oldChecksum, mmTmp);
                        mmTmp = simd::mm_op<__m128i, DATA, func>::compute(mmTmp, mmOperand);
                        newChecksum = _mm_xor_si128(newChecksum, mmTmp);
                        _mm_storeu_si128(data128Out++, mmTmp);
                    }
                    auto storedChecksum = reinterpret_cast<CS*>(data128In);
                    if (XORdiff<CS>::checksumsDiffer(*storedChecksum, XOR<__m128i, CS>::computeFinalChecksum(oldChecksum))) {
                        throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                    }
                    data128In = reinterpret_cast<__m128i *>(storedChecksum + 1);
                    auto newChecksumOut = reinterpret_cast<CS*>(data128Out);
                    *newChecksumOut++ = XOR<__m128i, CS>::computeFinalChecksum(newChecksum);
                    data128Out = reinterpret_cast<__m128i *>(newChecksumOut);
                    i += NUM_VALUES_PER_BLOCK;
                }
                // checksum remaining values which do not fit in the block size
                if (i <= (numValues - NUM_VALUES_PER_SIMDREG)) {
                    __m128i oldChecksum = _mm_setzero_si128();
                    __m128i newChecksum = _mm_setzero_si128();
                    do {
                        auto mmTmp = _mm_lddqu_si128(data128In++);
                        oldChecksum = _mm_xor_si128(oldChecksum, mmTmp);
                        mmTmp = simd::mm_op<__m128i, DATA, func>::compute(mmTmp, mmOperand);
                        newChecksum = _mm_xor_si128(newChecksum, mmTmp);
                        _mm_storeu_si128(data128Out++, mmTmp);
                        i += NUM_VALUES_PER_SIMDREG;
                    } while (i <= (numValues - NUM_VALUES_PER_SIMDREG));
                    auto storedChecksum = reinterpret_cast<CS*>(data128In);
                    if (XORdiff<CS>::checksumsDiffer(*storedChecksum, XOR<__m128i, CS>::computeFinalChecksum(oldChecksum))) {
                        throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                    }
                    data128In = reinterpret_cast<__m128i *>(storedChecksum + 1);
                    auto newChecksumOut = reinterpret_cast<CS*>(data128Out);
                    *newChecksumOut++ = XOR<__m128i, CS>::computeFinalChecksum(newChecksum);
                    data128Out = reinterpret_cast<__m128i *>(newChecksumOut);
                }
                // checksum remaining integers which do not fit in the SIMD register, so we do it on the actual data width denoted by template parameter IN
                if (i < numValues) {
                    DATA oldChecksum = 0;
                    DATA newChecksum = 0;
                    auto dataIn = reinterpret_cast<DATA*>(data128In);
                    auto dataOut = reinterpret_cast<DATA*>(data128Out);
                    for (; i < numValues; ++i) {
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
                size_t numValues = test.template getNumValues();
                size_t i = 0;
                auto data128In = test.bufEncoded.template begin<__m128i >();
                auto mmValue = funcInitVector(); // simd::mm<__m128i, DATA>::set1(config.operand);
                while (i <= (numValues - NUM_VALUES_PER_BLOCK)) {
                    for (size_t k = 0; k < BLOCKSIZE; ++k) {
                        auto mmTmp = _mm_lddqu_si128(data128In++);
                        mmValue = funcKernelVector(mmValue, mmTmp);
                    }
                    data128In = reinterpret_cast<__m128i *>(reinterpret_cast<CS*>(data128In) + 1);
                    i += NUM_VALUES_PER_BLOCK;
                }
                // checksum remaining values which do not fit in the block size
                if (i <= (numValues - NUM_VALUES_PER_SIMDREG)) {
                    do {
                        auto mmTmp = _mm_lddqu_si128(data128In++);
                        mmValue = funcKernelVector(mmValue, mmTmp);
                        i += NUM_VALUES_PER_SIMDREG;
                    } while (i <= (numValues - NUM_VALUES_PER_SIMDREG));
                    data128In = reinterpret_cast<__m128i *>(reinterpret_cast<CS*>(data128In) + 1);
                }
                Aggregate value = funcVectorToScalar(mmValue);
                // checksum remaining integers which do not fit in the SIMD register, so we do it on the actual data width denoted by template parameter IN
                if (i < numValues) {
                    auto dataIn = reinterpret_cast<DATA*>(data128In);
                    for (; i < numValues; ++i) {
                        const DATA tmp = *dataIn++;
                        value = funcKernelScalar(value, tmp);
                    }
                }
                auto dataOut = test.bufResult.template begin<Aggregate>();
                Aggregate final = funcFinal(value, numValues);
                *dataOut++ = final;
                *dataOut++ = final; // "XOR checksum"
            }
            void operator()(
                    AggregateConfiguration::Sum) {
                impl<DATA>([] {return simd::mm<__m128i, DATA>::set1(0);}, [](__m128i mmValue, __m128i mmTmp) {return simd::mm_op<__m128i, DATA, add>::compute(mmValue, mmTmp);},
                        [](__m128i mmValue) {return simd::mm<__m128i, DATA>::sum(mmValue);}, [](DATA value, DATA tmp) {return value + tmp;}, [](DATA value, size_t numValues) {return value;});
            }
            void operator()(
                    AggregateConfiguration::Min) {
                impl<DATA>([] {return simd::mm<__m128i, DATA>::set1(std::numeric_limits<DATA>::max());}, [](__m128i mmValue, __m128i mmTmp) {return simd::mm<__m128i, DATA>::min(mmValue, mmTmp);},
                        [](__m128i mmValue) {return simd::mm<__m128i, DATA>::min(mmValue);}, [](DATA value, DATA tmp) {return value < tmp ? value : tmp;},
                        [](DATA value, size_t numValues) {return value;});
            }
            void operator()(
                    AggregateConfiguration::Max) {
                impl<DATA>([] {return simd::mm<__m128i, DATA>::set1(std::numeric_limits<DATA>::min());}, [](__m128i mmValue, __m128i mmTmp) {return simd::mm<__m128i, DATA>::max(mmValue, mmTmp);},
                        [](__m128i mmValue) {return simd::mm<__m128i, DATA>::max(mmValue);}, [](DATA value, DATA tmp) {return value > tmp ? value : tmp;},
                        [](DATA value, size_t numValues) {return value;});
            }
            void operator()(
                    AggregateConfiguration::Avg) {
                impl<DATA>([] {return simd::mm<__m128i, DATA>::set1(0);}, [](__m128i mmValue, __m128i mmTmp) {return simd::mm_op<__m128i, DATA, add>::compute(mmValue, mmTmp);},
                        [](__m128i mmValue) {return simd::mm<__m128i, DATA>::sum(mmValue);}, [](DATA value, DATA tmp) {return value + tmp;},
                        [](DATA value, size_t numValues) {return value / numValues;});
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
                size_t numValues = test.template getNumValues();
                size_t i = 0;
                auto data128In = test.bufEncoded.template begin<__m128i >();
                auto mmValue = funcInitVector(); // simd::mm<__m128i, DATA>::set1(config.operand);
                while (i <= (numValues - NUM_VALUES_PER_BLOCK)) {
                    __m128i checksum = _mm_setzero_si128();
                    for (size_t k = 0; k < BLOCKSIZE; ++k) {
                        auto mmTmp = _mm_lddqu_si128(data128In++);
                        checksum = _mm_xor_si128(checksum, mmTmp);
                        mmValue = funcKernelVector(mmValue, mmTmp);
                    }
                    CS storedChecksum = *reinterpret_cast<CS*>(data128In);
                    data128In = reinterpret_cast<__m128i *>(reinterpret_cast<CS*>(data128In) + 1);
                    if (XORdiff<CS>::checksumsDiffer(storedChecksum, XOR<__m128i, CS>::computeFinalChecksum(checksum))) {
                        throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                    }
                    i += NUM_VALUES_PER_BLOCK;
                }
                // checksum remaining values which do not fit in the block size
                if (i <= (numValues - NUM_VALUES_PER_SIMDREG)) {
                    __m128i checksum = _mm_setzero_si128();
                    do {
                        auto mmTmp = _mm_lddqu_si128(data128In++);
                        checksum = _mm_xor_si128(checksum, mmTmp);
                        mmValue = funcKernelVector(mmValue, mmTmp);
                        i += NUM_VALUES_PER_SIMDREG;
                    } while (i <= (numValues - NUM_VALUES_PER_SIMDREG));
                    CS storedChecksum = *reinterpret_cast<CS*>(data128In);
                    data128In = reinterpret_cast<__m128i *>(reinterpret_cast<CS*>(data128In) + 1);
                    if (XORdiff<CS>::checksumsDiffer(storedChecksum, XOR<__m128i, CS>::computeFinalChecksum(checksum))) {
                        throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                    }
                }
                Aggregate value = funcVectorToScalar(mmValue);
                // checksum remaining integers which do not fit in the SIMD register, so we do it on the actual data width denoted by template parameter IN
                if (i < numValues) {
                    DATA checksum = 0;
                    auto dataIn = reinterpret_cast<DATA*>(data128In);
                    for (; i < numValues; ++i) {
                        const DATA tmp = *dataIn++;
                        checksum ^= tmp;
                        value = funcKernelScalar(value, tmp);
                    }
                    DATA storedChecksum = *dataIn;
                    if (XORdiff<DATA>::checksumsDiffer(storedChecksum, XOR<DATA, DATA>::computeFinalChecksum(checksum))) {
                        throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                    }
                }
                auto dataOut = test.bufResult.template begin<Aggregate>();
                Aggregate final = funcFinal(value, numValues);
                *dataOut++ = final;
                *dataOut++ = final; // "XOR checksum"
            }
            void operator()(
                    AggregateConfiguration::Sum) {
                impl<DATA>([] {return simd::mm<__m128i, DATA>::set1(0);}, [](__m128i mmValue, __m128i mmTmp) {return simd::mm_op<__m128i, DATA, add>::compute(mmValue, mmTmp);},
                        [](__m128i mmValue) {return simd::mm<__m128i, DATA>::sum(mmValue);}, [](DATA value, DATA tmp) {return value + tmp;}, [](DATA value, size_t numValues) {return value;});
            }
            void operator()(
                    AggregateConfiguration::Min) {
                impl<DATA>([] {return simd::mm<__m128i, DATA>::set1(std::numeric_limits<DATA>::max());}, [](__m128i mmValue, __m128i mmTmp) {return simd::mm<__m128i, DATA>::min(mmValue, mmTmp);},
                        [](__m128i mmValue) {return simd::mm<__m128i, DATA>::min(mmValue);}, [](DATA value, DATA tmp) {return value < tmp ? value : tmp;},
                        [](DATA value, size_t numValues) {return value;});
            }
            void operator()(
                    AggregateConfiguration::Max) {
                impl<DATA>([] {return simd::mm<__m128i, DATA>::set1(std::numeric_limits<DATA>::min());}, [](__m128i mmValue, __m128i mmTmp) {return simd::mm<__m128i, DATA>::max(mmValue, mmTmp);},
                        [](__m128i mmValue) {return simd::mm<__m128i, DATA>::max(mmValue);}, [](DATA value, DATA tmp) {return value > tmp ? value : tmp;},
                        [](DATA value, size_t numValues) {return value;});
            }
            void operator()(
                    AggregateConfiguration::Avg) {
                impl<DATA>([] {return simd::mm<__m128i, DATA>::set1(0);}, [](__m128i mmValue, __m128i mmTmp) {return simd::mm_op<__m128i, DATA, add>::compute(mmValue, mmTmp);},
                        [](__m128i mmValue) {return simd::mm<__m128i, DATA>::sum(mmValue);}, [](DATA value, DATA tmp) {return value + tmp;},
                        [](DATA value, size_t numValues) {return value / numValues;});
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
                size_t numValues = this->getNumValues();
                size_t i = 0;
                auto dataIn = this->bufEncoded.template begin<CS>();
                auto dataOut = this->bufResult.template begin<__m128i >();
                for (; i <= (numValues - NUM_VALUES_PER_BLOCK); i += NUM_VALUES_PER_BLOCK, dataIn++) {
                    auto dataIn2 = reinterpret_cast<__m128i *>(dataIn);
                    for (size_t k = 0; k < BLOCKSIZE; ++k) {
                        _mm_storeu_si128(dataOut++, _mm_lddqu_si128(dataIn2++));
                    }
                    dataIn = reinterpret_cast<CS*>(dataIn2);
                }
                // checksum remaining values which do not fit in the block size
                if (i <= (numValues - NUM_VALUES_PER_SIMDREG)) {
                    auto dataIn2 = reinterpret_cast<__m128i *>(dataIn);
                    for (; i <= (numValues - NUM_VALUES_PER_SIMDREG); i += NUM_VALUES_PER_SIMDREG) {
                        _mm_storeu_si128(dataOut++, _mm_lddqu_si128(dataIn2++));
                    }
                    dataIn = reinterpret_cast<CS*>(dataIn2);
                    dataOut++;
                }
                // checksum remaining integers which do not fit in the SIMD register
                if (i < numValues) {
                    auto dataIn2 = reinterpret_cast<DATA*>(dataIn);
                    auto dataOut2 = reinterpret_cast<DATA*>(dataOut);
                    for (; i < numValues; ++i) {
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
                size_t numValues = this->getNumValues();
                size_t i = 0;
                auto dataIn = this->bufEncoded.template begin<CS>();
                auto dataOut = this->bufResult.template begin<__m128i >();
                for (; i <= (numValues - NUM_VALUES_PER_BLOCK); i += NUM_VALUES_PER_BLOCK, dataIn++) {
                    __m128i checksum = _mm_setzero_si128();
                    auto dataIn2 = reinterpret_cast<__m128i *>(dataIn);
                    for (size_t k = 0; k < BLOCKSIZE; ++k) {
                        auto mmTmp = _mm_lddqu_si128(dataIn2++);
                        checksum = _mm_xor_si128(checksum, mmTmp);
                        _mm_storeu_si128(dataOut++, mmTmp);
                    }
                    CS storedChecksum = *reinterpret_cast<CS*>(dataIn2);
                    if (XORdiff<CS>::checksumsDiffer(storedChecksum, XOR<__m128i, CS>::computeFinalChecksum(checksum))) {
                        throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                    }
                    dataIn = reinterpret_cast<CS*>(dataIn2);
                }
                // checksum remaining values which do not fit in the block size
                if (i <= (numValues - NUM_VALUES_PER_SIMDREG)) {
                    __m128i checksum = _mm_setzero_si128();
                    auto dataIn2 = reinterpret_cast<__m128i *>(dataIn);
                    for (; i <= (numValues - NUM_VALUES_PER_SIMDREG); i += NUM_VALUES_PER_SIMDREG) {
                        auto mmTmp = _mm_lddqu_si128(dataIn2++);
                        checksum = _mm_xor_si128(checksum, mmTmp);
                        _mm_storeu_si128(dataOut++, mmTmp);
                    }
                    CS storedChecksum = *reinterpret_cast<CS*>(dataIn2);
                    if (XORdiff<CS>::checksumsDiffer(storedChecksum, XOR<__m128i, CS>::computeFinalChecksum(checksum))) {
                        throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                    }
                    dataIn = reinterpret_cast<CS*>(dataIn2) + 1;
                }
                // checksum remaining integers which do not fit in the SIMD register
                if (i < numValues) {
                    DATA checksum = 0;
                    auto dataIn2 = reinterpret_cast<DATA*>(dataIn);
                    auto dataOut2 = reinterpret_cast<DATA*>(dataOut);
                    for (; i < numValues; ++i) {
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
