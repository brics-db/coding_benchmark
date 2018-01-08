// Copyright 2017 Till Kolditz, Stefan de Bruijn
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
 * File:   AN_scalar_s_inv.tcc
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 05. July 2017, 23:45
 */

#pragma once

#ifndef AN_SCALAR
#error "Clients must not include this file directly, but file <AN/AN_scalar.hpp>!"
#endif

#include <AN/AN_scalar.tcc>
#include <Util/ArithmeticSelector.hpp>

namespace coding_benchmark {

    template<typename DATARAW, typename DATAENC, size_t UNROLL>
    struct AN_scalar_s_inv :
            public AN_scalar<DATARAW, DATAENC, UNROLL> {

        AN_scalar_s_inv(
                const char* const name,
                AlignedBlock & bufRaw,
                AlignedBlock & bufEncoded,
                AlignedBlock & bufResult,
                DATAENC A,
                DATAENC AInv)
                : AN_scalar<DATARAW, DATAENC, UNROLL>(name, bufRaw, bufEncoded, bufResult, A, AInv) {
        }

        virtual ~AN_scalar_s_inv() {
        }

        bool DoCheck() override {
            return true;
        }

        void RunCheck(
                const CheckConfiguration & config) {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                _ReadWriteBarrier();
                const size_t numValues = this->getNumValues();
                auto data = this->bufEncoded.template begin<DATAENC>();
                const auto dataEnd = data + numValues;
                const constexpr DATAENC dMax = static_cast<DATAENC>(std::numeric_limits<DATARAW>::max());
                const constexpr DATAENC dMin = static_cast<DATAENC>(std::numeric_limits<DATARAW>::min());
                while (data <= (dataEnd - UNROLL)) { // let the compiler unroll the loop
                    for (size_t k = 0; k < UNROLL; ++k) {
                        DATAENC dec = static_cast<DATAENC>(*data * this->A_INV);
                        if (dec >= dMin && dec <= dMax) {
                            ++data;
                        } else {
                            std::stringstream ss;
                            ss << "A=" << this->A << ", A^-1=" << this->A_INV;
                            throw ErrorInfo(__FILE__, __LINE__, data - this->bufEncoded.template begin<DATAENC>(), iteration, ss.str().c_str());
                        }
                    }
                }
                // remaining numbers
                while (data < dataEnd) {
                    DATAENC dec = static_cast<DATAENC>(*data * this->A_INV);
                    if (dec >= dMin && dec <= dMax) {
                        ++data;
                    } else {
                        std::stringstream ss;
                        ss << "A=" << this->A << ", A^-1=" << this->A_INV;
                        throw ErrorInfo(__FILE__, __LINE__, data - this->bufEncoded.template begin<DATAENC>(), iteration, ss.str().c_str());
                    }
                }
            }
        }

        bool DoArithmeticChecked(
                const ArithmeticConfiguration & config) override {
            return std::visit(ArithmeticSelector(), config.mode);
        }

        struct ArithmetorChecked {
            AN_scalar_s_inv & test;
            const ArithmeticConfiguration & config;
            const size_t iteration;
            ArithmetorChecked(
                    AN_scalar_s_inv & test,
                    const ArithmeticConfiguration & config,
                    const size_t iteration)
                    : test(test),
                      config(config),
                      iteration(iteration) {
            }
            template<template<typename = void> class func>
            void impl() {
                func<> functor;
                const size_t numValues = test.template getNumValues();
                auto dataIn = test.bufEncoded.template begin<DATAENC>();
                const auto dataInEnd = dataIn + numValues;
                auto dataOut = test.bufResult.template begin<DATAENC>();
                const constexpr DATAENC dMax = static_cast<DATAENC>(std::numeric_limits<DATARAW>::max());
                const constexpr DATAENC dMin = static_cast<DATAENC>(std::numeric_limits<DATARAW>::min());
                DATAENC operandEnc = config.operand * test.A;
                while (dataIn <= (dataInEnd - UNROLL)) { // let the compiler unroll the loop
                    for (size_t k = 0; k < UNROLL; ++k) {
                        DATAENC dec = static_cast<DATAENC>(*dataIn * test.A_INV);
                        if (dec >= dMin && dec <= dMax) {
                            *dataOut++ = functor(*dataIn++, operandEnc);
                        } else {
                            std::stringstream ss;
                            ss << "A=" << test.A << ", A^-1=" << test.A_INV;
                            throw ErrorInfo(__FILE__, __LINE__, dataIn - test.bufEncoded.template begin<DATAENC>(), iteration);
                        }
                    }
                }
                // remaining numbers
                while (dataIn < dataInEnd) {
                    DATAENC dec = static_cast<DATAENC>(*dataIn * test.A_INV);
                    if (dec >= dMin && dec <= dMax) {
                        *dataOut++ = functor(*dataIn++, operandEnc);
                    } else {
                        std::stringstream ss;
                        ss << "A=" << test.A << ", A^-1=" << test.A_INV;
                        throw ErrorInfo(__FILE__, __LINE__, dataIn - test.bufEncoded.template begin<DATAENC>(), iteration);
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
            AN_scalar_s_inv & test;
            const AggregateConfiguration & config;
            const size_t iteration;
            AggregatorChecked(
                    AN_scalar_s_inv & test,
                    const AggregateConfiguration & config,
                    const size_t iteration)
                    : test(test),
                      config(config),
                      iteration(iteration) {
            }
            template<typename Tout, typename Initializer, typename Kernel, typename Finalizer>
            void impl(
                    Initializer && funcInit,
                    Kernel && funcKernel,
                    Finalizer && funcFinal) {
                const size_t numValues = test.template getNumValues();
                auto dataIn = test.bufEncoded.template begin<DATAENC>();
                const auto dataInEnd = dataIn + numValues;
                auto dataOut = test.bufResult.template begin<Tout>();
                const constexpr DATAENC dMax = static_cast<DATAENC>(std::numeric_limits<DATARAW>::max());
                const constexpr DATAENC dMin = static_cast<DATAENC>(std::numeric_limits<DATARAW>::min());
                Tout value = funcInit();
                while (dataIn <= (dataInEnd - UNROLL)) {
                    for (size_t k = 0; k < UNROLL; ++k) {
                        DATAENC dec = static_cast<DATAENC>(*dataIn * test.A_INV);
                        if (dec >= dMin && dec <= dMax) {
                            value = funcKernel(value, *dataIn++);
                        } else {
                            std::stringstream ss;
                            ss << "A=" << test.A << ", A^-1=" << test.A_INV;
                            throw ErrorInfo(__FILE__, __LINE__, dataIn - test.bufEncoded.template begin<DATAENC>(), iteration);
                        }
                    }
                }
                while (dataIn < dataInEnd) {
                    DATAENC dec = static_cast<DATAENC>(*dataIn * test.A_INV);
                    if (dec >= dMin && dec <= dMax) {
                        value = funcKernel(value, *dataIn++);
                    } else {
                        std::stringstream ss;
                        ss << "A=" << test.A << ", A^-1=" << test.A_INV;
                        throw ErrorInfo(__FILE__, __LINE__, dataIn - test.bufEncoded.template begin<DATAENC>(), iteration);
                    }
                }
                *dataOut = funcFinal(value, numValues);
            }
            void operator()(
                    AggregateConfiguration::Sum) {
                impl<larger_t>([] {return (larger_t) 0;}, [] (larger_t sum, DATAENC dataIn) -> larger_t {return sum + dataIn;}, [] (larger_t sum, const size_t numValues) {return sum;});
            }
            void operator()(
                    AggregateConfiguration::Min) {
                impl<DATAENC>([] {return (DATAENC) std::numeric_limits<DATAENC>::max();}, [] (DATAENC minimum, DATAENC dataIn) -> DATAENC {return dataIn < minimum ? dataIn : minimum;},
                        [] (DATAENC minimum, const size_t numValues) {return minimum;});
            }
            void operator()(
                    AggregateConfiguration::Max) {
                impl<DATAENC>([] {return (DATAENC) std::numeric_limits<DATAENC>::min();}, [] (DATAENC minimum, DATAENC dataIn) -> DATAENC {return dataIn > minimum ? dataIn : minimum;},
                        [] (DATAENC minimum, const size_t numValues) {return minimum;});
            }
            void operator()(
                    AggregateConfiguration::Avg) {
                impl<larger_t>([] {return (larger_t) 0;}, [] (larger_t sum, DATAENC dataIn) -> larger_t {return sum + dataIn;},
                        [this] (larger_t sum, const size_t numValues) {return (sum / (numValues * test.A)) * test.A;});
            }
        };

        void RunAggregateChecked(
                const AggregateConfiguration & config) override {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                _ReadWriteBarrier();
                std::visit(AggregatorChecked(*this, config, iteration), config.mode);
            }
        }

        bool DoReencodeChecked() override {
            return false;
        }

        void RunReencodeChecked(
                const ReencodeConfiguration & config) override {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                _ReadWriteBarrier();
                const size_t numValues = this->bufRaw.template end<DATARAW>() - this->bufRaw.template begin<DATARAW>();
                auto dataIn = this->bufEncoded.template begin<DATAENC>();
                const auto dataInEnd = dataIn + numValues;
                auto dataOut = this->bufResult.template begin<DATAENC>();
                const constexpr DATAENC dMax = static_cast<DATAENC>(std::numeric_limits<DATARAW>::max());
                const constexpr DATAENC dMin = static_cast<DATAENC>(std::numeric_limits<DATARAW>::min());
                const DATAENC reenc = this->A_INV * config.newA;
                while (dataIn <= (dataInEnd - UNROLL)) { // let the compiler unroll the loop
                    for (size_t unroll = 0; unroll < UNROLL; ++unroll) {
                        DATAENC dec = static_cast<DATAENC>(*dataIn * this->A_INV);
                        if (dec >= dMin && dec <= dMax) {
                            *dataOut++ = static_cast<DATAENC>(*dataIn++ * reenc);
                        } else {
                            std::stringstream ss;
                            ss << "A=" << this->A << ", A^-1=" << this->A_INV;
                            throw ErrorInfo(__FILE__, __LINE__, dataIn - this->bufEncoded.template begin<DATAENC>(), iteration, ss.str().c_str());
                        }
                    }
                }
                // remaining numbers
                while (dataIn < dataInEnd) {
                    DATAENC dec = static_cast<DATAENC>(*dataIn * this->A_INV);
                    if (dec >= dMin && dec <= dMax) {
                        *dataOut++ = static_cast<DATAENC>(*dataIn++ * reenc);
                    } else {
                        std::stringstream ss;
                        ss << "A=" << this->A << ", A^-1=" << this->A_INV;
                        throw ErrorInfo(__FILE__, __LINE__, dataIn - this->bufEncoded.template begin<DATAENC>(), iteration, ss.str().c_str());
                    }
                }
                this->A = static_cast<DATAENC>(config.newA);
                this->A_INV = ext_euclidean(this->A, sizeof(DATAENC) * CHAR_BIT);
            }
        }

        bool DoDecodeChecked() override {
            return true;
        }

        void RunDecodeChecked(
                const DecodeConfiguration & config) override {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                _ReadWriteBarrier();
                const size_t numValues = this->bufRaw.template end<DATARAW>() - this->bufRaw.template begin<DATARAW>();
                auto dataIn = this->bufEncoded.template begin<DATAENC>();
                const auto dataInEnd = dataIn + numValues;
                auto dataOut = this->bufResult.template begin<DATARAW>();
                const constexpr DATAENC dMax = static_cast<DATAENC>(std::numeric_limits<DATARAW>::max());
                const constexpr DATAENC dMin = static_cast<DATAENC>(std::numeric_limits<DATARAW>::min());
                while (dataIn <= (dataInEnd - UNROLL)) { // let the compiler unroll the loop
                    for (size_t unroll = 0; unroll < UNROLL; ++unroll) {
                        DATAENC dec = static_cast<DATAENC>(*dataIn++ * this->A_INV);
                        if (dec >= dMin && dec <= dMax) {
                            *dataOut++ = static_cast<DATARAW>(dec);
                        } else {
                            std::stringstream ss;
                            ss << "A=" << this->A << ", A^-1=" << this->A_INV;
                            throw ErrorInfo(__FILE__, __LINE__, dataIn - this->bufEncoded.template begin<DATAENC>(), iteration, ss.str().c_str());
                        }
                    }
                }
                // remaining numbers
                while (dataIn < dataInEnd) {
                    DATAENC dec = static_cast<DATAENC>(*dataIn++ * this->A_INV);
                    if (dec >= dMin && dec <= dMax) {
                        *dataOut++ = static_cast<DATARAW>(dec);
                    } else {
                        std::stringstream ss;
                        ss << "A=" << this->A << ", A^-1=" << this->A_INV;
                        throw ErrorInfo(__FILE__, __LINE__, dataIn - this->bufEncoded.template begin<DATAENC>(), iteration, ss.str().c_str());
                    }
                }
            }
        }
    };

}
