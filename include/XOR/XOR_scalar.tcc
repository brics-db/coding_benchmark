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

#ifndef XOR_SCALAR
#error "Clients must not include this file directly, but file <XOR/XOR_scalar.hpp>!"
#endif

#include <Util/Test.hpp>
#include <XOR/XOR_base.hpp>
#include <Util/Intrinsics.hpp>
#include <Util/ErrorInfo.hpp>
#include <Util/Functors.hpp>
#include <Util/Helpers.hpp>
#include <Util/ArithmeticSelector.hpp>
#include <Util/AggregateSelector.hpp>

namespace coding_benchmark {

    template<typename DATA, typename CS, size_t BLOCKSIZE>
    struct XOR_scalar :
            public Test<DATA, CS> {

        using Test<DATA, CS>::Test;

        virtual ~XOR_scalar() {
        }

        void RunEncode(
                const EncodeConfiguration & config) override {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                _ReadWriteBarrier();
                auto dataIn = config.source.template begin<DATA>();
                auto dataInEnd = dataIn + config.numValues;
                auto dataOut = config.target.template begin<CS>();
                while (dataIn <= (dataInEnd - BLOCKSIZE)) {
                    DATA checksum = 0;
                    auto dataOut2 = reinterpret_cast<DATA*>(dataOut);
                    for (size_t k = 0; k < BLOCKSIZE; ++k) {
                        const auto tmp = *dataIn++;
                        *dataOut2++ = tmp;
                        checksum ^= tmp;
                    }
                    dataOut = reinterpret_cast<CS*>(dataOut2);
                    *dataOut++ = XOR<DATA, CS>::computeFinalChecksum(checksum);
                }
                // checksum remaining values which do not fit in the block size
                if (dataIn < dataInEnd) {
                    DATA checksum = 0;
                    auto dataOut2 = reinterpret_cast<DATA*>(dataOut);
                    do {
                        const auto tmp = *dataIn++;
                        *dataOut2++ = tmp;
                        checksum ^= tmp;
                    } while (dataIn < dataInEnd);
                    dataOut = reinterpret_cast<CS*>(dataOut2);
                    *dataOut++ = XOR<DATA, CS>::computeFinalChecksum(checksum);
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
                auto data = config.target.template begin<CS>();
                if (config.numValues >= BLOCKSIZE) {
                    while (i <= (config.numValues - BLOCKSIZE)) {
                        auto data2 = reinterpret_cast<DATA*>(data); // first, iterate over sizeof(IN)-bit values
                        DATA checksum = 0;
                        for (size_t k = 0; k < BLOCKSIZE; ++k) {
                            checksum ^= *data2++;
                        }
                        i += BLOCKSIZE;
                        data = reinterpret_cast<CS*>(data2); // second, advance data2 up to the checksum
                        if (XORdiff<CS>::checksumsDiffer(*data, XOR<DATA, CS>::computeFinalChecksum(checksum))) // third, test checksum
                                {
                            throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                        }
                        ++data; // fourth, advance after the checksum to the next block of values
                    }
                }
                // checksum remaining values which do not fit in the block size
                if (i < config.numValues) {
                    auto data2 = reinterpret_cast<DATA*>(data); // first, iterate over sizeof(IN)-bit values
                    DATA checksum = 0;
                    do {
                        ++i;
                        checksum ^= *data2++;
                    } while (i < config.numValues);
                    data = reinterpret_cast<CS*>(data2); // second, advance data2 up to the checksum
                    if (XORdiff<CS>::checksumsDiffer(*data, XOR<DATA, CS>::computeFinalChecksum(checksum))) // third, test checksum
                            {
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
            XOR_scalar & test;
            const ArithmeticConfiguration & config;
            Arithmetor(
                    XOR_scalar & test,
                    const ArithmeticConfiguration & config)
                    : test(test),
                      config(config) {
            }
            template<template<typename = void> class func>
            void impl() {
                func<> functor;
                size_t i = 0;
                auto dataIn = config.source.template begin<CS>();
                auto dataOut = config.target.template begin<DATA>();
                if (config.numValues >= BLOCKSIZE) {
                    for (; i <= (config.numValues - BLOCKSIZE); i += BLOCKSIZE) {
                        DATA checksum = 0;
                        auto dataIn2 = reinterpret_cast<DATA*>(dataIn);
                        for (size_t k = 0; k < BLOCKSIZE; ++k) {
                            const auto tmp = functor(*dataIn2++, config.operand);
                            *dataOut++ = tmp;
                            checksum ^= tmp;
                        }
                        auto chkOut = reinterpret_cast<CS*>(dataOut);
                        *chkOut++ = XOR<DATA, CS>::computeFinalChecksum(checksum);
                        dataOut = reinterpret_cast<DATA*>(chkOut);
                        dataIn = reinterpret_cast<CS*>(dataIn2) + 1; // ignore the original checksum
                    }
                }
                // checksum remaining values which do not fit in the block size
                if (i < config.numValues) {
                    DATA checksum = 0;
                    auto dataIn2 = reinterpret_cast<DATA*>(dataIn);
                    for (; i < config.numValues; ++i) {
                        const auto tmp = functor(*dataIn2++, config.operand);
                        *dataOut++ = tmp;
                        checksum ^= tmp;
                    }
                    auto chkOut = reinterpret_cast<CS*>(dataOut);
                    *chkOut = XOR<DATA, CS>::computeFinalChecksum(checksum);
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
            XOR_scalar & test;
            const ArithmeticConfiguration & config;
            const size_t iteration;
            ArithmetorChecked(
                    XOR_scalar & test,
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
                auto dataIn = config.source.template begin<CS>();
                auto dataOut = config.target.template begin<DATA>();
                if (config.numValues >= BLOCKSIZE) {
                    for (; i <= (config.numValues - BLOCKSIZE); i += BLOCKSIZE) {
                        DATA oldChecksum = 0;
                        DATA newChecksum = 0;
                        auto dataIn2 = reinterpret_cast<DATA*>(dataIn);
                        for (size_t k = 0; k < BLOCKSIZE; ++k) {
                            const auto tmp1 = *dataIn2++;
                            oldChecksum ^= tmp1;
                            const auto tmp2 = functor(tmp1, config.operand);
                            newChecksum ^= tmp2;
                            *dataOut++ = tmp2;
                        }
                        dataIn = reinterpret_cast<CS*>(dataIn2);
                        const auto finalOLdChecksum = XOR<DATA, CS>::computeFinalChecksum(oldChecksum);
                        const auto finalNewChecksum = XOR<DATA, CS>::computeFinalChecksum(newChecksum);
                        if (XORdiff<CS>::checksumsDiffer(*dataIn++, finalOLdChecksum)) {
                            throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                        }
                        auto chkOut = reinterpret_cast<CS*>(dataOut);
                        *chkOut++ = finalNewChecksum;
                        dataOut = reinterpret_cast<DATA*>(chkOut);
                    }
                }
                // checksum remaining values which do not fit in the block size
                if (i < config.numValues) {
                    DATA oldChecksum = 0;
                    DATA newChecksum = 0;
                    auto dataIn2 = reinterpret_cast<DATA*>(dataIn);
                    for (; i < config.numValues; ++i) {
                        const auto tmp1 = *dataIn2++;
                        oldChecksum ^= tmp1;
                        const auto tmp2 = functor(tmp1, config.operand);
                        newChecksum ^= tmp2;
                        *dataOut++ = tmp2;
                    }
                    dataIn = reinterpret_cast<CS*>(dataIn2);
                    const auto finalOLdChecksum = XOR<DATA, CS>::computeFinalChecksum(oldChecksum);
                    const auto finalNewChecksum = XOR<DATA, CS>::computeFinalChecksum(newChecksum);
                    if (XORdiff<CS>::checksumsDiffer(*dataIn++, finalOLdChecksum)) {
                        throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                    }
                    auto chkOut = reinterpret_cast<CS*>(dataOut);
                    *chkOut = finalNewChecksum;
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

        virtual bool DoDecode() override {
            return true;
        }

        bool DoAggregate(
                const AggregateConfiguration & config) override {
            return std::visit(AggregateSelector(), config.mode);
        }

        struct Aggregator {
            typedef typename Larger<DATA>::larger_t larger_t;
            XOR_scalar & test;
            const AggregateConfiguration & config;
            Aggregator(
                    XOR_scalar & test,
                    const AggregateConfiguration & config)
                    : test(test),
                      config(config) {
            }
            template<typename Aggregate, typename Initializer, typename Kernel, typename Finalizer>
            void impl(
                    Initializer && funcInit,
                    Kernel && funcKernel,
                    Finalizer && funcFinal) {
                size_t i = 0;
                auto dataIn = config.source.template begin<CS>();
                Aggregate value = funcInit();
                if (config.numValues >= BLOCKSIZE) {
                    while (i <= (config.numValues - BLOCKSIZE)) {
                        auto dataIn2 = reinterpret_cast<DATA*>(dataIn);
                        for (size_t k = 0; k < BLOCKSIZE; ++k) {
                            value = funcKernel(value, *dataIn2++);
                        }
                        dataIn = reinterpret_cast<CS*>(dataIn2);
                        ++dataIn; // here, simply jump over the checksum
                        i += BLOCKSIZE;
                    }
                }
                for (; i < config.numValues; ++i) {
                    value = funcKernel(value, *dataIn++);
                }
                auto final = funcFinal(value, config.numValues);
                auto dataOut = test.bufScratchPad.template begin<Aggregate>();
                *dataOut = final;
                EncodeConfiguration encConf(1, 2, test.bufScratchPad, config.target);
                test.RunEncode(encConf);
            }
            void operator()(
                    AggregateConfiguration::Sum) {
                impl<larger_t>([] {return (larger_t) 0;}, [] (larger_t sum, DATA dataIn) -> larger_t {return sum + dataIn;}, [] (larger_t sum, const size_t numValues) -> larger_t {return sum;});
            }
            void operator()(
                    AggregateConfiguration::Min) {
                impl<DATA>([] () {return std::numeric_limits<DATA>::max();}, [] (DATA minimum, DATA dataIn) {return dataIn < minimum ? dataIn : minimum;},
                        [] (DATA minimum, const size_t numValues) {return minimum;});
            }
            void operator()(
                    AggregateConfiguration::Max) {
                impl<DATA>([] {return std::numeric_limits<DATA>::min();}, [] (DATA maximum, DATA dataIn) {return dataIn > maximum ? dataIn : maximum;},
                        [] (DATA maximum, const size_t numValues) {return maximum;});
            }
            void operator()(
                    AggregateConfiguration::Avg) {
                impl<larger_t>([] {return (larger_t) 0;}, [] (larger_t sum, DATA dataIn) -> larger_t {return sum + dataIn;},
                        [] (larger_t sum, const size_t numValues) -> larger_t {return sum / numValues;});
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
            XOR_scalar & test;
            const AggregateConfiguration & config;
            const size_t iteration;
            AggregatorChecked(
                    XOR_scalar & test,
                    const AggregateConfiguration & config,
                    size_t iteration)
                    : test(test),
                      config(config),
                      iteration(iteration) {
            }
            template<typename Aggregate, typename Initializer, typename Kernel, typename Finalizer>
            void impl(
                    Initializer && funcInit,
                    Kernel && funcKernel,
                    Finalizer && funcFinal) {
                size_t i = 0;
                auto dataIn = config.source.template begin<CS>();
                Aggregate value = funcInit();
                if (config.numValues >= BLOCKSIZE) {
                    while (i <= (config.numValues - BLOCKSIZE)) {
                        auto dataIn2 = reinterpret_cast<DATA*>(dataIn);
                        CS checksum_computed(0);
                        for (size_t k = 0; k < BLOCKSIZE; ++k) {
                            checksum_computed ^= *dataIn2;
                            value = funcKernel(value, *dataIn2++);
                        }
                        dataIn = reinterpret_cast<CS*>(dataIn2);
                        CS checksum_read = *dataIn++;
                        if (XORdiff<CS>::checksumsDiffer(checksum_read, checksum_computed)) {
                            throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                        }
                        i += BLOCKSIZE;
                    }
                }
                if (i < config.numValues) {
                    auto dataIn2 = reinterpret_cast<DATA*>(dataIn);
                    CS checksum_computed(0);
                    for (; i < config.numValues; ++i) {
                        checksum_computed ^= *dataIn2;
                        value = funcKernel(value, *dataIn2++);
                    }
                    dataIn = reinterpret_cast<CS*>(dataIn2);
                    CS checksum_read = *dataIn++;
                    if (XORdiff<CS>::checksumsDiffer(checksum_read, checksum_computed)) {
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
                impl<larger_t>([] {return (larger_t) 0;}, [] (larger_t sum, DATA dataIn) -> larger_t {return sum + dataIn;}, [] (larger_t sum, const size_t numValues) -> larger_t {return sum;});
            }
            void operator()(
                    AggregateConfiguration::Min) {
                impl<DATA>([] () {return std::numeric_limits<DATA>::max();}, [] (DATA minimum, DATA dataIn) {return dataIn < minimum ? dataIn : minimum;},
                        [] (DATA minimum, const size_t numValues) {return minimum;});
            }
            void operator()(
                    AggregateConfiguration::Max) {
                impl<DATA>([] {return std::numeric_limits<DATA>::min();}, [] (DATA maximum, DATA dataIn) {return dataIn < maximum ? dataIn : maximum;},
                        [] (DATA maximum, const size_t numValues) {return maximum;});
            }
            void operator()(
                    AggregateConfiguration::Avg) {
                impl<larger_t>([] {return (larger_t) 0;}, [] (larger_t sum, DATA dataIn) -> larger_t {return sum + dataIn;},
                        [] (larger_t sum, const size_t numValues) -> larger_t {return sum / numValues;});
            }
        };

        void RunAggregateChecked(
                const AggregateConfiguration & config) override {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                _ReadWriteBarrier();
                std::visit(Aggregator(*this, config), config.mode);
            }
        }

        void RunDecode(
                const DecodeConfiguration & config) override {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                _ReadWriteBarrier();
                size_t i = 0;
                auto dataIn = config.source.template begin<CS>();
                auto dataOut = config.target.template begin<DATA>();
                if (config.numValues >= BLOCKSIZE) {
                    while (i <= (config.numValues - BLOCKSIZE)) {
                        auto dataIn2 = reinterpret_cast<DATA*>(dataIn);
                        for (size_t k = 0; k < BLOCKSIZE; ++k) {
                            *dataOut++ = *dataIn2++;
                        }
                        i += BLOCKSIZE;
                        dataIn = reinterpret_cast<CS*>(dataIn2);
                        dataIn++;
                    }
                }
                // checksum remaining values which do not fit in the block size
                if (i < config.numValues) {
                    for (auto dataIn2 = reinterpret_cast<DATA*>(dataIn); i < config.numValues; ++i) {
                        *dataOut++ = *dataIn2++;
                    }
                }
            }
        }

        void RunDecodeChecked(
                const DecodeConfiguration & config) override {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                _ReadWriteBarrier();
                size_t i = 0;
                auto dataIn = config.source.template begin<CS>();
                auto dataOut = config.target.template begin<DATA>();
                if (config.numValues >= BLOCKSIZE) {
                    while (i <= (config.numValues - BLOCKSIZE)) {
                        auto dataIn2 = reinterpret_cast<DATA*>(dataIn);
                        DATA checksum = 0;
                        for (size_t k = 0; k < BLOCKSIZE; ++k) {
                            const auto tmp = *dataIn2++;
                            checksum ^= tmp;
                            *dataOut++ = tmp;
                        }
                        i += BLOCKSIZE;
                        dataIn = reinterpret_cast<CS*>(dataIn2);
                        if (XORdiff<CS>::checksumsDiffer(*dataIn, XOR<DATA, CS>::computeFinalChecksum(checksum))) { // third, test checksum
                            throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                        }
                        dataIn++;
                    }
                }
                // checksum remaining values which do not fit in the block size
                if (i < config.numValues) {
                    auto dataIn2 = reinterpret_cast<DATA*>(dataIn);
                    DATA checksum = 0;
                    for (; i < config.numValues; ++i) {
                        const auto tmp = *dataIn2++;
                        checksum ^= tmp;
                        *dataOut++ = tmp;
                    }
                    dataIn = reinterpret_cast<CS*>(dataIn2);
                    if (XORdiff<CS>::checksumsDiffer(*dataIn, XOR<DATA, CS>::computeFinalChecksum(checksum))) { // third, test checksum
                        throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                    }
                }
            }
        }
    };

}
