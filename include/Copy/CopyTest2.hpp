// Copyright (c) 2016 Till Kolditz
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
 * File:   CopyTest.hpp
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 30. November 2016, 13:42
 */

#pragma once

#include <cstring>

#include <Util/Test.hpp>
#include <Util/ErrorInfo.hpp>
#include <Util/Intrinsics.hpp>

template<typename DATARAW, typename DATAENC, size_t UNROLL>
struct CopyTest2 :
        public Test<DATARAW, DATAENC>,
        public ScalarTest {

    CopyTest2(
            const char* const name,
            AlignedBlock & bufRaw,
            AlignedBlock & bufEncoded,
            AlignedBlock & bufResult)
            : Test<DATARAW, DATAENC>(name, bufRaw, bufEncoded, bufResult) {
    }

    virtual ~CopyTest2() {
    }

    void RunEncode(
            const EncodeConfiguration & config) override {
        for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
            _ReadWriteBarrier();
            const size_t numValues = config.numValues;
            size_t i = 0;
            auto dataRaw = this->bufRaw.template begin<DATARAW>();
            auto dataEnc = this->bufEncoded.template begin<DATAENC>();
            for (; i <= (numValues - UNROLL); i += UNROLL) {
                // let the compiler unroll the loop
                for (size_t unroll = 0; unroll < UNROLL; ++unroll, ++dataEnc, ++dataRaw) {
                    *dataEnc = static_cast<DATAENC>(*dataRaw);
                }
            }
            // remaining numbers
            for (; i < numValues; ++i, ++dataEnc, ++dataRaw) {
                *dataEnc = static_cast<DATAENC>(*dataRaw);
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
            const size_t numValues = config.numValues;
            size_t i = 0;
            auto dataRaw = this->bufRaw.template begin<DATARAW>();
            auto dataEnc = config.target.template begin<DATAENC>();
            while (i <= (numValues - UNROLL)) {
                // let the compiler unroll the loop
                for (size_t unroll = 0; unroll < UNROLL; ++unroll, ++dataRaw, ++dataEnc, ++i) {
                    if (*dataRaw != *dataEnc) {
                        throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                    }
                }
            }
            // remaining numbers
            for (; i < numValues; ++i, ++dataEnc, ++dataRaw) {
                if (*dataRaw != *dataEnc) {
                    throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                }
            }
        }
    }

    bool DoReencodeChecked() override {
        return false;
    }

    void RunReencodeChecked(
            const ReencodeConfiguration & config) override {
        for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
            _ReadWriteBarrier();
            const size_t numValues = config.numValues;
            size_t i = 0;
            auto dataRaw = this->bufRaw.template begin<DATARAW>();
            auto dataEnc = this->bufEncoded.template begin<DATAENC>();
            auto dataRes = this->bufResult.template begin<DATAENC>();
            while (i <= (numValues - UNROLL)) {
                // let the compiler unroll the loop
                for (size_t unroll = 0; unroll < UNROLL; ++unroll, ++dataRaw, ++dataEnc, ++dataRes, ++i) {
                    if (*dataRaw != *dataEnc) {
                        throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                    }
                    *dataRes = static_cast<DATAENC>(*dataEnc);
                }
            }
            // remaining numbers
            for (; i < numValues; ++i, ++dataRaw, ++dataEnc, ++dataRes) {
                if (*dataRaw != *dataEnc) {
                    throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                }
                *dataRes = static_cast<DATAENC>(*dataEnc);
            }
        }
    }

    bool DoDecode() override {
        return true;
    }

    void RunDecode(
            const DecodeConfiguration & config) override {
        for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
            _ReadWriteBarrier();
            const size_t numValues = config.numValues;
            size_t i = 0;
            auto dataEnc = this->bufEncoded.template begin<DATAENC>();
            auto dataRes = this->bufResult.template begin<DATARAW>();
            for (; i <= (numValues - UNROLL); i += UNROLL) {
                // let the compiler unroll the loop
                for (size_t unroll = 0; unroll < UNROLL; ++unroll, ++dataRes, ++dataEnc) {
                    *dataRes = static_cast<DATARAW>(*dataEnc);
                }
            }
            // remaining numbers
            for (; i < numValues; ++i, ++dataRes, ++dataEnc) {
                *dataRes = static_cast<DATARAW>(*dataEnc);
            }
        }
    }

    void RunDecodeChecked(
            const DecodeConfiguration & config) override {
        for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
            _ReadWriteBarrier();
            const size_t numValues = config.numValues;
            size_t i = 0;
            auto dataRaw = this->bufRaw.template begin<DATARAW>();
            auto dataEnc = this->bufEncoded.template begin<DATAENC>();
            auto dataRes = this->bufResult.template begin<DATARAW>();
            while (i <= (numValues - UNROLL)) {
                // let the compiler unroll the loop
                for (size_t unroll = 0; unroll < UNROLL; ++unroll, ++dataRaw, ++dataEnc, ++dataRes, ++i) {
                    if (*dataRaw != *dataEnc) {
                        throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                    }
                    *dataRes = static_cast<DATARAW>(*dataEnc);
                }
            }
            // remaining numbers
            for (; i < numValues; ++i, ++dataRaw, ++dataEnc, ++dataRes) {
                if (*dataRaw != *dataEnc) {
                    throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                }
                *dataRes = static_cast<DATARAW>(*dataEnc);
            }
        }
    }
};

template<size_t UNROLL>
struct CopyTest2_8_16 :
        public CopyTest2<uint8_t, uint16_t, UNROLL> {
    using CopyTest2<uint8_t, uint16_t, UNROLL>::CopyTest2;
};

template<size_t UNROLL>
struct CopyTest2_32_64 :
        public CopyTest2<uint32_t, uint64_t, UNROLL> {
    using CopyTest2<uint32_t, uint64_t, UNROLL>::CopyTest2;
};
