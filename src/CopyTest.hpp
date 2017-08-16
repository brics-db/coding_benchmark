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

#include <Test.hpp>
#include <Util/ErrorInfo.hpp>
#include <Util/Intrinsics.hpp>

template<typename DATA, size_t UNROLL>
struct CopyTest :
        public Test<DATA, DATA>,
        public ScalarTest {

    using Test<DATA, DATA>::Test;

    virtual ~CopyTest() {
    }

    size_t getNumBytes() {
        return sizeof(DATA) * (this->bufRaw.template end<DATA>() - this->bufRaw.template begin<DATA>());
    }

    void RunEncode(
            const EncodeConfiguration & config) override {
        for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
            _ReadWriteBarrier();
            memmove(this->bufEncoded.begin(), this->bufRaw.begin(), getNumBytes());
        }
    }

    bool DoCheck() override {
        return true;
    }

    void RunCheck(
            const CheckConfiguration & config) override {
        for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
            _ReadWriteBarrier();
            int ret = memcmp(this->bufEncoded.begin(), this->bufRaw.begin(), getNumBytes());
            if (ret != 0) {
                throw ErrorInfo(__FILE__, __LINE__, ret, config.numIterations);
            }
        }
    }

    bool DoArithmetic() override {
        return false;
    }

    void RunArithmetic(
            const ArithmeticConfiguration & config) override {
        for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
            _ReadWriteBarrier();
        }
    }

    bool DoArithmeticChecked() override {
        return false;
    }

    void RunArithmeticChecked(
            const ArithmeticConfiguration & config) override {
    }

    bool DoAggregate(
            const AggregateConfiguration & config) override {
        return false;
    }

    void RunAggregate(
            const AggregateConfiguration & config) override {
    }

    bool DoAggregateChecked(
            const AggregateConfiguration & config) override {
        return false;
    }

    void RunAggregateChecked(
            const AggregateConfiguration & config) override {
    }

    bool DoReencodeChecked() override {
        return false;
    }

    void RunReencodeChecked(
            const ReencodeConfiguration & config) override {
        for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
            _ReadWriteBarrier();
            size_t numBytes = getNumBytes();
            int ret = memcmp(this->bufRaw.begin(), this->bufEncoded.begin(), numBytes);
            if (ret != 0) {
                throw ErrorInfo(__FILE__, __LINE__, ret, config.numIterations);
            }
            memmove(this->bufResult.begin(), this->bufEncoded.begin(), numBytes);
        }
    }

    bool DoDecode() override {
        return true;
    }

    void RunDecode(
            const DecodeConfiguration & config) override {
        for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
            _ReadWriteBarrier();
            memmove(this->bufResult.begin(), this->bufEncoded.begin(), getNumBytes());
        }
    }

    bool DoDecodeChecked() override {
        return true;
    }

    void RunDecodeChecked(
            const DecodeConfiguration & config) override {
        for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
            _ReadWriteBarrier();
            size_t numBytes = getNumBytes();
            int ret = memcmp(this->bufEncoded.begin(), this->bufRaw.begin(), numBytes);
            if (ret != 0) {
                throw ErrorInfo(__FILE__, __LINE__, ret, config.numIterations);
            }
            memmove(this->bufResult.begin(), this->bufEncoded.begin(), numBytes);
        }
    }
};

template<size_t UNROLL>
struct CopyTest8 :
        public CopyTest<uint8_t, UNROLL> {
    using CopyTest<uint8_t, UNROLL>::CopyTest;
};

template<size_t UNROLL>
struct CopyTest16 :
        public CopyTest<uint16_t, UNROLL> {
    using CopyTest<uint16_t, UNROLL>::CopyTest;
};

template<size_t UNROLL>
struct CopyTest32 :
        public CopyTest<uint32_t, UNROLL> {
    using CopyTest<uint32_t, UNROLL>::CopyTest;
};
