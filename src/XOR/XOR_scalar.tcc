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

#include <XOR/XOR_base.hpp>
#include <Test.hpp>
#include <Util/Intrinsics.hpp>
#include <Util/ErrorInfo.hpp>

template<typename DATA, typename CS, size_t BLOCKSIZE>
struct XOR_scalar :
        public Test<DATA, CS> {

    using Test<DATA, CS>::Test;

    virtual ~XOR_scalar() {
    }

    size_t getNumValues() {
        return this->bufRaw.template end<DATA>() - this->bufRaw.template begin<DATA>();
    }

    void RunEncode(
            const EncodeConfiguration & config) override {
        for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
            _ReadWriteBarrier();
            auto dataIn = this->bufRaw.template begin<DATA>();
            auto dataInEnd = this->bufRaw.template end<DATA>();
            auto dataOut = this->bufEncoded.template begin<CS>();
            while (dataIn <= (dataInEnd - BLOCKSIZE)) {
                DATA checksum = 0;
                auto dataOut2 = reinterpret_cast<DATA*>(dataOut);
                for (size_t k = 0; k < BLOCKSIZE; ++k) {
                    auto& tmp = *dataIn++;
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
                    auto& tmp = *dataIn++;
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
            size_t numValues = getNumValues();
            size_t i = 0;
            auto data = this->bufEncoded.template begin<CS>();
            while (i <= (numValues - BLOCKSIZE)) {
                auto data2 = reinterpret_cast<DATA*>(data); // first, iterate over sizeof(IN)-bit values
                DATA checksum = 0;
                for (size_t k = 0; k < BLOCKSIZE; ++k) {
                    checksum ^= *data2++;
                }
                i += BLOCKSIZE;
                data = reinterpret_cast<CS*>(data2); // second, advance data2 up to the checksum
                if (XORdiff<CS>::checksumsDiffer(*data, XOR<DATA, CS>::computeFinalChecksum(checksum))) // third, test checksum
                        {
                    throw ErrorInfo(__FILE__, __LINE__, data - this->bufEncoded.template begin<CS>(), iteration);
                }
                ++data; // fourth, advance after the checksum to the next block of values
            }
            // checksum remaining values which do not fit in the block size
            if (i < numValues) {
                auto data2 = reinterpret_cast<DATA*>(data); // first, iterate over sizeof(IN)-bit values
                DATA checksum = 0;
                do {
                    ++i;
                    checksum ^= *data2++;
                } while (i < numValues);
                data = reinterpret_cast<CS*>(data2); // second, advance data2 up to the checksum
                if (XORdiff<CS>::checksumsDiffer(*data, XOR<DATA, CS>::computeFinalChecksum(checksum))) // third, test checksum
                        {
                    throw ErrorInfo(__FILE__, __LINE__, data - this->bufEncoded.template begin<CS>(), iteration);
                }
            }
        }
    }

    struct ArithmeticSelector {
        DATA operator()(
                ArithmeticConfiguration::Add) {
            return true;
        }
        DATA operator()(
                ArithmeticConfiguration::Sub) {
            return false;
        }
        DATA operator()(
                ArithmeticConfiguration::Mul) {
            return false;
        }
        DATA operator()(
                ArithmeticConfiguration::Div) {
            return false;
        }
    };

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
        void operator()(
                ArithmeticConfiguration::Add) {
            size_t numValues = test.getNumValues();
            size_t i = 0;
            auto dataIn = test.bufEncoded.template begin<CS>();
            auto dataOut = test.bufResult.template begin<DATA>();
            for (; i <= (numValues - BLOCKSIZE); i += BLOCKSIZE) {
                DATA checksum = 0;
                auto dataIn2 = reinterpret_cast<DATA*>(dataIn);
                for (size_t k = 0; k < BLOCKSIZE; ++k) {
                    const auto tmp = (*dataIn2++) + config.operand;
                    *dataOut++ = tmp;
                    checksum ^= tmp;
                }
                auto chkOut = reinterpret_cast<CS*>(dataOut);
                *chkOut++ = XOR<DATA, CS>::computeFinalChecksum(checksum);
                dataOut = reinterpret_cast<DATA*>(chkOut);
                dataIn = reinterpret_cast<CS*>(dataIn2) + 1; // ignore the original checksum
            }
            // checksum remaining values which do not fit in the block size
            if (i < numValues) {
                DATA checksum = 0;
                auto dataIn2 = reinterpret_cast<DATA*>(dataIn);
                for (; i < numValues; ++i) {
                    const auto tmp = (*dataIn2++) + config.operand;
                    *dataOut++ = tmp;
                    checksum ^= tmp;
                }
                auto chkOut = reinterpret_cast<CS*>(dataOut);
                *chkOut = XOR<DATA, CS>::computeFinalChecksum(checksum);
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
        ArithmetorChecked(
                XOR_scalar & test,
                const ArithmeticConfiguration & config)
                : test(test),
                  config(config) {
        }
        void operator()(
                ArithmeticConfiguration::Add) {
            size_t numValues = test.getNumValues();
            size_t i = 0;
            auto dataIn = test.bufEncoded.template begin<CS>();
            auto dataOut = test.bufResult.template begin<DATA>();
            for (; i <= (numValues - BLOCKSIZE); i += BLOCKSIZE) {
                DATA oldChecksum = 0;
                DATA newChecksum = 0;
                auto dataIn2 = reinterpret_cast<DATA*>(dataIn);
                for (size_t k = 0; k < BLOCKSIZE; ++k) {
                    auto tmp = *dataIn2++;
                    oldChecksum ^= tmp;
                    tmp += config.operand;
                    newChecksum ^= tmp;
                    *dataOut++ = tmp;
                }
                const auto finalOLdChecksum = XOR<DATA, CS>::computeFinalChecksum(oldChecksum);
                const auto finalNewChecksum = XOR<DATA, CS>::computeFinalChecksum(newChecksum);
                if (XORdiff<CS>::checksumsDiffer(finalOLdChecksum, finalNewChecksum)) // third, test checksum
                        {
                    throw ErrorInfo(__FILE__, __LINE__, i, 0);
                }
                auto chkOut = reinterpret_cast<CS*>(dataOut);
                *chkOut++ = finalNewChecksum;
                dataOut = reinterpret_cast<DATA*>(chkOut);
                dataIn = reinterpret_cast<CS*>(dataIn2) + 1;
            }
            // checksum remaining values which do not fit in the block size
            if (i < numValues) {
                DATA oldChecksum = 0;
                DATA newChecksum = 0;
                auto dataIn2 = reinterpret_cast<DATA*>(dataIn);
                for (; i < numValues; ++i) {
                    auto tmp = *dataIn2++;
                    oldChecksum ^= tmp;
                    tmp += config.operand;
                    newChecksum ^= tmp;
                    *dataOut++ = tmp;
                }
                dataIn = reinterpret_cast<CS*>(dataIn2);
                const auto finalOLdChecksum = XOR<DATA, CS>::computeFinalChecksum(oldChecksum);
                const auto finalNewChecksum = XOR<DATA, CS>::computeFinalChecksum(newChecksum);
                if (XORdiff<CS>::checksumsDiffer(finalOLdChecksum, finalNewChecksum)) // third, test checksum
                        {
                    throw ErrorInfo(__FILE__, __LINE__, i, 0);
                }
                auto chkOut = reinterpret_cast<CS*>(dataOut);
                *chkOut = finalNewChecksum;
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
            std::visit(Arithmetor(*this, config), config.mode);
        }
    }

    virtual bool DoDecode() override {
        return true;
    }

    void RunDecode(
            const DecodeConfiguration & config) override {
        for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
            _ReadWriteBarrier();
            size_t numValues = getNumValues();
            size_t i = 0;
            auto dataIn = this->bufEncoded.template begin<CS>();
            auto dataOut = this->bufResult.template begin<DATA>();
            while (i <= (numValues - BLOCKSIZE)) {
                auto dataIn2 = reinterpret_cast<DATA*>(dataIn);
                for (size_t k = 0; k < BLOCKSIZE; ++k) {
                    *dataOut++ = *dataIn2++;
                }
                i += BLOCKSIZE;
                dataIn = reinterpret_cast<CS*>(dataIn2);
                dataIn++;
            }
            // checksum remaining values which do not fit in the block size
            if (i < numValues) {
                for (auto dataIn2 = reinterpret_cast<DATA*>(dataIn); i < numValues; ++i) {
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
            size_t numValues = getNumValues();
            size_t i = 0;
            auto dataIn = this->bufEncoded.template begin<CS>();
            auto dataOut = this->bufResult.template begin<DATA>();
            while (i <= (numValues - BLOCKSIZE)) {
                auto dataIn2 = reinterpret_cast<DATA*>(dataIn);
                DATA checksum = 0;
                for (size_t k = 0; k < BLOCKSIZE; ++k) {
                    const auto tmp = *dataIn2++;
                    checksum ^= tmp;
                    *dataOut++ = tmp;
                }
                i += BLOCKSIZE;
                dataIn = reinterpret_cast<CS*>(dataIn2);
                if (XORdiff<CS>::checksumsDiffer(*dataIn, XOR<DATA, CS>::computeFinalChecksum(checksum))) // third, test checksum
                        {
                    throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                }
                dataIn++;
            }
            // checksum remaining values which do not fit in the block size
            if (i < numValues) {
                auto dataIn2 = reinterpret_cast<DATA*>(dataIn);
                DATA checksum = 0;
                for (; i < numValues; ++i) {
                    const auto tmp = *dataIn2++;
                    checksum ^= tmp;
                    *dataOut++ = tmp;
                }
                dataIn = reinterpret_cast<CS*>(dataIn2);
                if (XORdiff<CS>::checksumsDiffer(*dataIn, XOR<DATA, CS>::computeFinalChecksum(checksum))) // third, test checksum
                        {
                    throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                }
            }
        }
    }
};
