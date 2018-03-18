// Copyright (c) 2018 Till Kolditz
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
 * File:   CRC_scalar.tcc
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 24-01-2018 10:11
 */

#pragma once

#ifndef CRC_SCALAR
#error "Clients must not include this file directly, but file <CRC/CRC_scalar.hpp>!"
#endif

#include <Util/Test.hpp>
#include <CRC/CRC_base.hpp>
#include <Util/Intrinsics.hpp>
#include <Util/ErrorInfo.hpp>
#include <Util/Functors.hpp>
#include <Util/Helpers.hpp>
#include <Util/ArithmeticSelector.hpp>
#include <Util/AggregateSelector.hpp>

namespace coding_benchmark {

    template<typename DATA, typename CS, size_t BLOCKSIZE>
    struct CRC_scalar :
            public Test<DATA, CS> {

        using Test<DATA, CS>::Test;

        virtual ~CRC_scalar() {
        }

        void RunEncode(
                const EncodeConfiguration & config) override {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                _ReadWriteBarrier();
                auto dataIn = config.source.template begin<DATA>();
                auto dataInEnd = dataIn + config.numValues;
                auto crcOut = config.target.template begin<CS>();
                while (dataIn <= (dataInEnd - BLOCKSIZE)) {
                    CS crc = 0;
                    auto dataOut = reinterpret_cast<DATA*>(crcOut);
                    for (size_t k = 0; k < BLOCKSIZE; ++k) {
                        const auto tmp = *dataIn++;
                        *dataOut++ = tmp;
                        crc = CRC<DATA, CS>::compute(crc, tmp);
                    }
                    crcOut = reinterpret_cast<CS*>(dataOut);
                    *crcOut++ = crc;
                }
                // checksum remaining values which do not fit in the block size
                if (dataIn < dataInEnd) {
                    CS crc = 0;
                    auto dataOut = reinterpret_cast<DATA*>(crcOut);
                    do {
                        const auto tmp = *dataIn++;
                        *dataOut++ = tmp;
                        crc = CRC<DATA, CS>::compute(crc, tmp);
                    } while (dataIn < dataInEnd);
                    crcOut = reinterpret_cast<CS*>(dataOut);
                    *crcOut = crc;
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
                auto crcIn = config.target.template begin<CS>();
                for (; i <= (config.numValues - BLOCKSIZE); i += BLOCKSIZE) {
                    auto dataIn = reinterpret_cast<DATA*>(crcIn); // first, iterate over sizeof(IN)-bit values
                    CS crc = 0;
                    for (size_t k = 0; k < BLOCKSIZE; ++k) {
                        crc = CRC<DATA, CS>::compute(crc, *dataIn++);
                    }
                    crcIn = reinterpret_cast<CS*>(dataIn); // second, advance data2 up to the checksum
                    if (crc != *crcIn) {
                        throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                    }
                    ++crcIn; // fourth, advance after the checksum to the next block of values
                }
                // checksum remaining values which do not fit in the block size
                if (i < config.numValues) {
                    auto dataIn = reinterpret_cast<DATA*>(crcIn); // first, iterate over sizeof(IN)-bit values
                    CS crc = 0;
                    do {
                        ++i;
                        crc = CRC<DATA, CS>::compute(crc, *dataIn++);
                    } while (i < config.numValues);
                    crcIn = reinterpret_cast<CS*>(dataIn); // second, advance data2 up to the checksum
                    if (crc != *crcIn) {
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
            CRC_scalar & test;
            const ArithmeticConfiguration & config;
            Arithmetor(
                    CRC_scalar & test,
                    const ArithmeticConfiguration & config)
                    : test(test),
                      config(config) {
            }
            template<template<typename = void> class func>
            void impl() {
                func<> functor;
                size_t i = 0;
                auto crcIn = config.source.template begin<CS>();
                auto dataOut = config.target.template begin<DATA>();
                for (; i <= (config.numValues - BLOCKSIZE); i += BLOCKSIZE) {
                    CS crc = 0;
                    auto dataIn = reinterpret_cast<DATA*>(crcIn);
                    for (size_t k = 0; k < BLOCKSIZE; ++k) {
                        const auto tmp = functor(*dataIn++, config.operand);
                        *dataOut++ = tmp;
                        crc = CRC<DATA, CS>::compute(crc, tmp);
                    }
                    auto chkOut = reinterpret_cast<CS*>(dataOut);
                    *chkOut++ = crc;
                    dataOut = reinterpret_cast<DATA*>(chkOut);
                    crcIn = reinterpret_cast<CS*>(dataIn) + 1; // ignore the original checksum
                }
                // checksum remaining values which do not fit in the block size
                if (i < config.numValues) {
                    CS crc = 0;
                    auto dataIn2 = reinterpret_cast<DATA*>(crcIn);
                    for (; i < config.numValues; ++i) {
                        const auto tmp = functor(*dataIn2++, config.operand);
                        *dataOut++ = tmp;
                        crc = CRC<DATA, CS>::compute(crc, tmp);
                    }
                    auto chkOut = reinterpret_cast<CS*>(dataOut);
                    *chkOut = crc;
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
            CRC_scalar & test;
            const ArithmeticConfiguration & config;
            const size_t iteration;
            ArithmetorChecked(
                    CRC_scalar & test,
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
                auto crcIn = config.source.template begin<CS>();
                auto dataOut = config.target.template begin<DATA>();
                for (; i <= (config.numValues - BLOCKSIZE); i += BLOCKSIZE) {
                    CS crcOld = 0;
                    CS crcNew = 0;
                    auto dataIn = reinterpret_cast<DATA*>(crcIn);
                    for (size_t k = 0; k < BLOCKSIZE; ++k) {
                        const auto tmp1 = *dataIn++;
                        crcOld = CRC<DATA, CS>::compute(crcOld, tmp1);
                        const auto tmp2 = functor(tmp1, config.operand);
                        *dataOut++ = tmp2;
                        crcNew = CRC<DATA, CS>::compute(crcNew, tmp2);
                    }
                    crcIn = reinterpret_cast<CS*>(dataIn);
                    if (*crcIn != crcOld) {
                        throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                    }
                    ++crcIn;
                    auto crcOut = reinterpret_cast<CS*>(dataOut);
                    *crcOut++ = crcNew;
                    dataOut = reinterpret_cast<DATA*>(crcOut);
                }
                // checksum remaining values which do not fit in the block size
                if (i < config.numValues) {
                    CS crcOld = 0;
                    CS crcNew = 0;
                    auto dataIn = reinterpret_cast<DATA*>(crcIn);
                    for (; i < config.numValues; ++i) {
                        const auto tmp1 = *dataIn++;
                        crcOld = CRC<DATA, CS>::compute(crcOld, tmp1);
                        const auto tmp2 = functor(tmp1, config.operand);
                        *dataOut++ = tmp2;
                        crcNew = CRC<DATA, CS>::compute(crcNew, tmp2);
                    }
                    crcIn = reinterpret_cast<CS*>(dataIn);
                    if (*crcIn != crcOld) {
                        throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                    }
                    auto crcOut = reinterpret_cast<CS*>(dataOut);
                    *crcOut = crcNew;
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
            CRC_scalar & test;
            const AggregateConfiguration & config;
            Aggregator(
                    CRC_scalar & test,
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
                auto crcIn = config.source.template begin<CS>();
                Aggregate value = funcInit();
                for (; i <= (config.numValues - BLOCKSIZE); i += BLOCKSIZE) {
                    auto dataIn = reinterpret_cast<DATA*>(crcIn);
                    for (size_t k = 0; k < BLOCKSIZE; ++k) {
                        value = funcKernel(value, *dataIn++);
                    }
                    crcIn = reinterpret_cast<CS*>(dataIn) + 1; // here, simply jump over the checksum
                }
                if (i < config.numValues) {
                    auto dataIn = reinterpret_cast<DATA*>(crcIn);
                    for (; i < config.numValues; ++i) {
                        value = funcKernel(value, *dataIn++);
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
            CRC_scalar & test;
            const AggregateConfiguration & config;
            const size_t iteration;
            AggregatorChecked(
                    CRC_scalar & test,
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
                auto crcIn = config.source.template begin<CS>();
                Aggregate value = funcInit();
                for (; i <= (config.numValues - BLOCKSIZE); i += BLOCKSIZE) {
                    auto dataIn = reinterpret_cast<DATA*>(crcIn);
                    CS crcOld = 0;
                    CS crcNew = 0;
                    for (size_t k = 0; k < BLOCKSIZE; ++k) {
                        DATA tmp = *dataIn++;
                        crcOld = CRC<DATA, CS>::compute(crcOld, tmp);
                        value = funcKernel(value, tmp);
                        crcNew = CRC<DATA, CS>::compute(crcNew, value);
                    }
                    crcIn = reinterpret_cast<CS*>(dataIn);
                    if (*crcIn != crcOld) {
                        throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                    }
                    ++crcIn;
                }
                if (i < config.numValues) {
                    auto dataIn = reinterpret_cast<DATA*>(crcIn);
                    CS crcOld = 0;
                    CS crcNew = 0;
                    for (; i < config.numValues; ++i) {
                        DATA tmp = *dataIn++;
                        crcOld = CRC<DATA, CS>::compute(crcOld, tmp);
                        value = funcKernel(value, tmp);
                        crcNew = CRC<DATA, CS>::compute(crcNew, value);
                    }
                    crcIn = reinterpret_cast<CS*>(dataIn);
                    if (*crcIn != crcOld) {
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
                impl<DATA>([] {return std::numeric_limits<DATA>::min();}, [] (DATA maximum, DATA dataIn) {return dataIn > maximum ? dataIn : maximum;},
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
                auto crcIn = config.source.template begin<CS>();
                auto dataOut = config.target.template begin<DATA>();
                if (config.numValues >= BLOCKSIZE) {
                    while (i <= (config.numValues - BLOCKSIZE)) {
                        auto dataIn = reinterpret_cast<DATA*>(crcIn);
                        for (size_t k = 0; k < BLOCKSIZE; ++k) {
                            *dataOut++ = *dataIn++;
                        }
                        i += BLOCKSIZE;
                        crcIn = reinterpret_cast<CS*>(dataIn) + 1; // ignore the checksum
                    }
                }
                // checksum remaining values which do not fit in the block size
                if (i < config.numValues) {
                    for (auto dataIn2 = reinterpret_cast<DATA*>(crcIn); i < config.numValues; ++i) {
                        *dataOut++ = *dataIn2++;
                    }
                }
            }
        }

        virtual bool DoDecodeChecked() override {
            return true;
        }

        void RunDecodeChecked(
                const DecodeConfiguration & config) override {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                _ReadWriteBarrier();
                size_t i = 0;
                auto crcIn = config.source.template begin<CS>();
                auto dataOut = config.target.template begin<DATA>();
                while (i <= (config.numValues - BLOCKSIZE)) {
                    auto dataIn = reinterpret_cast<DATA*>(crcIn);
                    CS crc = 0;
                    for (size_t k = 0; k < BLOCKSIZE; ++k) {
                        const auto tmp = *dataIn++;
                        crc = CRC<DATA, CS>::compute(crc, tmp);
                        *dataOut++ = tmp;
                    }
                    i += BLOCKSIZE;
                    crcIn = reinterpret_cast<CS*>(dataIn);
                    if (crc != *crcIn) { // third, test checksum
                        throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                    }
                    ++crcIn;
                }
                // checksum remaining values which do not fit in the block size
                if (i < config.numValues) {
                    auto dataIn = reinterpret_cast<DATA*>(crcIn);
                    CS crc = 0;
                    for (size_t k = 0; k < BLOCKSIZE; ++k) {
                        const auto tmp = *dataIn++;
                        crc = CRC<DATA, CS>::compute(crc, tmp);
                        *dataOut++ = tmp;
                    }
                    crcIn = reinterpret_cast<CS*>(dataIn);
                    if (crc != *crcIn) { // third, test checksum
                        throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                    }
                }
            }
        }
    };

}
