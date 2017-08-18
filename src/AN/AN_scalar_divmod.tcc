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

#pragma once

#include <AN/AN_scalar.tcc>
#include <Util/ArithmeticSelector.hpp>

template<typename DATARAW, typename DATAENC, size_t UNROLL>
struct AN_seq_divmod :
        public AN_seq<DATARAW, DATAENC, UNROLL> {

    using AN_seq<DATARAW, DATAENC, UNROLL>::AN_seq;

    virtual ~AN_seq_divmod() {
    }

    size_t getNumValues() {
        return this->bufRaw.template end<DATARAW>() - this->bufRaw.template begin<DATARAW>();
    }

    virtual bool DoCheck() override {
        return true;
    }

    virtual void RunCheck(
            const CheckConfiguration & config) {
        for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
            _ReadWriteBarrier();
            const size_t numValues = getNumValues();
            size_t i = 0;
            auto data = this->bufEncoded.template begin<DATAENC>();
            while (i <= (numValues - UNROLL)) {
                // let the compiler unroll the loop
                for (size_t k = 0; k < UNROLL; ++k) {
                    if ((*data % this->A) != 0) {
                        throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                    }
                    ++data;
                }
                i += UNROLL;
            }
            // remaining numbers
            for (; i < numValues; ++i, ++data) {
                if ((*data % this->A) != 0) {
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
        AN_seq_divmod & test;
        const ArithmeticConfiguration & config;
        Arithmetor(
                AN_seq_divmod & test,
                const ArithmeticConfiguration & config)
                : test(test),
                  config(config) {
        }
        void operator()(
                ArithmeticConfiguration::Add) {
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

    void RunArithmetic(
            const ArithmeticConfiguration & config) override {
        for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
            _ReadWriteBarrier();
            std::visit(Arithmetor(*this, config), config.mode);
        }
    }

    bool DoDecode() override {
        return true;
    }

    void RunDecode(
            const DecodeConfiguration & config) override {
        for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
            _ReadWriteBarrier();
            size_t numValues = getNumValues();
            size_t i = 0;
            auto dataIn = this->bufEncoded.template begin<DATAENC>();
            auto dataOut = this->bufResult.template begin<DATARAW>();
            for (; i <= (numValues - UNROLL); i += UNROLL) {
                // let the compiler unroll the loop
                for (size_t unroll = 0; unroll < UNROLL; ++unroll, ++dataOut, ++dataIn) {
                    *dataOut = *dataIn / this->A;
                }
            }
            // remaining numbers
            for (; i < numValues; ++i, ++dataOut, ++dataIn) {
                *dataOut = *dataIn / this->A;
            }
        }
    }

    bool DoDecodeChecked() override {
        return true;
    }

    void RunDecodeChecked(
            const DecodeConfiguration & config) override {
        for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
            _ReadWriteBarrier();
            size_t numValues = getNumValues();
            size_t i = 0;
            auto dataIn = this->bufEncoded.template begin<DATAENC>();
            auto dataOut = this->bufResult.template begin<DATARAW>();
            for (; i <= (numValues - UNROLL); i += UNROLL) {
                // let the compiler unroll the loop
                for (size_t unroll = 0; unroll < UNROLL; ++unroll, ++dataOut, ++dataIn) {
                    if ((*dataIn % this->A) != 0) {
                        throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                    }
                    *dataOut = *dataIn / this->A;
                }
            }
            // remaining numbers
            for (; i < numValues; ++i, ++dataOut, ++dataIn) {
                if ((*dataIn % this->A) != 0) {
                    throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                }
                *dataOut = *dataIn / this->A;
            }
        }
    }
};
