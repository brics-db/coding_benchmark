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
 * File:   AN_scalar_u_inv.tcc
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 05. July 2017, 23:43
 */

#pragma once

#include <AN/AN_scalar.tcc>
#include <Util/ArithmeticSelector.hpp>

template<typename DATARAW, typename DATAENC, size_t UNROLL>
struct AN_scalar_u_inv :
        public AN_scalar<DATARAW, DATAENC, UNROLL> {

    AN_scalar_u_inv(
            const char* const name,
            AlignedBlock & bufRaw,
            AlignedBlock & bufEncoded,
            AlignedBlock & bufResult,
            DATAENC A,
            DATAENC AInv)
            : AN_scalar<DATARAW, DATAENC, UNROLL>(name, bufRaw, bufEncoded, bufResult, A, AInv) {
    }

    virtual ~AN_scalar_u_inv() {
    }

    bool DoCheck() override {
        return true;
    }

    void RunCheck(
            const CheckConfiguration & config) {
        for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
            _ReadWriteBarrier();
            const size_t numValues = this->getNumValues();
            size_t i = 0;
            auto data = this->bufEncoded.template begin<DATAENC>();
            DATAENC dMax = static_cast<DATAENC>(std::numeric_limits<DATARAW>::max());
            for (; i <= (numValues - UNROLL); i += UNROLL) {
                // let the compiler unroll the loop
                for (size_t k = 0; k < UNROLL; ++k) {
                    DATAENC dec = static_cast<DATAENC>(*data * this->A_INV);
                    if (dec > dMax) {
                        std::stringstream ss;
                        ss << "A=" << this->A << ", A^-1=" << this->A_INV;
                        throw ErrorInfo(__FILE__, __LINE__, data - this->bufEncoded.template begin<DATAENC>(), iteration, ss.str().c_str());
                    }
                    ++data;
                }
            }
            // remaining numbers
            for (; i < numValues; ++i, ++data) {
                DATAENC dec = static_cast<DATAENC>(*data * this->A_INV);
                if (dec > dMax) {
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
        AN_scalar_u_inv & test;
        const ArithmeticConfiguration & config;
        const size_t iteration;
        ArithmetorChecked(
                AN_scalar_u_inv & test,
                const ArithmeticConfiguration & config,
                const size_t iteration)
                : test(test),
                  config(config),
                  iteration(iteration) {
        }
        void operator()(
                ArithmeticConfiguration::Add) {
            const size_t numValues = test.template getNumValues();
            size_t i = 0;
            auto dataIn = test.bufEncoded.template begin<DATAENC>();
            auto dataOut = test.bufResult.template begin<DATAENC>();
            DATAENC dMax = static_cast<DATAENC>(std::numeric_limits<DATARAW>::max());
            DATAENC dMin = static_cast<DATAENC>(std::numeric_limits<DATARAW>::min());
            DATAENC operandEnc = config.operand * test.A;
            for (; i <= (numValues - UNROLL); i += UNROLL) { // let the compiler unroll the loop
                for (size_t k = 0; k < UNROLL; ++k) {
                    DATAENC dec = static_cast<DATAENC>(*dataIn * test.A_INV);
                    if (dec < dMin || dec > dMax) {
                        std::stringstream ss;
                        ss << "A=" << test.A << ", A^-1=" << test.A_INV;
                        throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                    }
                    *dataOut++ = *dataIn++ + operandEnc;
                }
            }
            // remaining numbers
            for (; i < numValues; ++i) {
                DATAENC dec = static_cast<DATAENC>(*dataIn * test.A_INV);
                if (dec < dMin || dec > dMax) {
                    std::stringstream ss;
                    ss << "A=" << test.A << ", A^-1=" << test.A_INV;
                    throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                }
                *dataOut++ = *dataIn++ + operandEnc;
            }
        }
        void operator()(
                ArithmeticConfiguration::Sub) {
        }
        void operator()(
                ArithmeticConfiguration::Mul) {
        }
        void operator()(
                ArithmeticConfiguration::Div) {
        }
    };

    void RunArithmeticChecked(
            const ArithmeticConfiguration & config) override {
        for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
            _ReadWriteBarrier();
            std::visit(ArithmetorChecked(*this, config, iteration), config.mode);
        }
    }

    bool DoReencodeChecked() override {
        return false;
    }

    void RunReencodeChecked(
            const ReencodeConfiguration & config) override {
        for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
            _ReadWriteBarrier();
            const size_t numValues = this->getNumValues();
            size_t i = 0;
            auto dataIn = this->bufEncoded.template begin<DATAENC>();
            auto dataOut = this->bufResult.template begin<DATAENC>();
            DATAENC dMax = static_cast<DATAENC>(std::numeric_limits<DATARAW>::max());
            const DATAENC reenc = this->A_INV * config.newA;
            for (; i <= (numValues - UNROLL); i += UNROLL) {
                // let the compiler unroll the loop
                for (size_t unroll = 0; unroll < UNROLL; ++unroll, ++dataIn, ++dataOut) {
                    DATAENC dec = static_cast<DATAENC>(*dataIn * this->A_INV);
                    if (dec > dMax) {
                        std::stringstream ss;
                        ss << "A=" << this->A << ", A^-1=" << this->A_INV;
                        throw ErrorInfo(__FILE__, __LINE__, dataIn - this->bufEncoded.template begin<DATAENC>(), iteration, ss.str().c_str());
                    }
                    *dataOut = static_cast<DATAENC>(*dataIn * reenc);
                }
            }
            // remaining numbers
            for (; i < numValues; ++i, ++dataIn, ++dataOut) {
                DATAENC dec = static_cast<DATAENC>(*dataIn * this->A_INV);
                if (dec > dMax) {
                    std::stringstream ss;
                    ss << "A=" << this->A << ", A^-1=" << this->A_INV;
                    throw ErrorInfo(__FILE__, __LINE__, dataIn - this->bufEncoded.template begin<DATAENC>(), iteration, ss.str().c_str());
                }
                *dataOut = static_cast<DATAENC>(*dataIn * reenc);
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
            const size_t numValues = this->getNumValues();
            size_t i = 0;
            auto dataIn = this->bufEncoded.template begin<DATAENC>();
            auto dataOut = this->bufResult.template begin<DATARAW>();
            DATAENC dMax = static_cast<DATAENC>(std::numeric_limits<DATARAW>::max());
            for (; i <= (numValues - UNROLL); i += UNROLL) {
                // let the compiler unroll the loop
                for (size_t unroll = 0; unroll < UNROLL; ++unroll, ++dataOut, ++dataIn) {
                    DATAENC dec = static_cast<DATAENC>(*dataIn * this->A_INV);
                    if (dec > dMax) {
                        std::stringstream ss;
                        ss << "A=" << this->A << ", A^-1=" << this->A_INV;
                        throw ErrorInfo(__FILE__, __LINE__, dataIn - this->bufEncoded.template begin<DATAENC>(), iteration, ss.str().c_str());
                    } else {
                        *dataOut = static_cast<DATARAW>(dec);
                    }
                }
            }
            // remaining numbers
            for (; i < numValues; ++i, ++dataOut, ++dataIn) {
                DATAENC dec = static_cast<DATAENC>(*dataIn * this->A_INV);
                if (dec > dMax) {
                    std::stringstream ss;
                    ss << "A=" << this->A << ", A^-1=" << this->A_INV;
                    throw ErrorInfo(__FILE__, __LINE__, dataIn - this->bufEncoded.template begin<DATAENC>(), iteration, ss.str().c_str());
                } else {
                    *dataOut = static_cast<DATARAW>(dec);
                }
            }
        }
    }
};
