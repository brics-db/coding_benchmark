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

template<typename DATARAW, typename DATAENC, size_t UNROLL>
struct CopyTest2 :
        public Test<DATARAW, DATAENC>,
        public SequentialTest {

    CopyTest2(
            const char* const name,
            AlignedBlock & in,
            AlignedBlock & out)
            : Test<DATARAW, DATAENC>(name, in, out) {
    }

    virtual ~CopyTest2() {
    }

    void RunEncode(
            const EncodeConfiguration & config) override {
        for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
            _ReadWriteBarrier();
            const size_t numValues = this->in.template end<DATARAW>() - this->in.template begin<DATARAW>();
            size_t i = 0;
            auto dataIn = this->in.template begin<DATARAW>();
            auto dataOut = this->out.template begin<DATAENC>();
            for (; i <= (numValues - UNROLL); i += UNROLL) {
                // let the compiler unroll the loop
                for (size_t unroll = 0; unroll < UNROLL; ++unroll, ++dataOut, ++dataIn) {
                    *dataOut = static_cast<DATAENC>(*dataIn);
                }
            }
            // remaining numbers
            for (; i < numValues; ++i, ++dataOut, ++dataIn) {
                *dataOut = static_cast<DATAENC>(*dataIn);
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
            const size_t numValues = this->in.template end<DATARAW>() - this->in.template begin<DATARAW>();
            size_t i = 0;
            auto dataOut = this->out.template begin<DATAENC>();
            for (; i <= (numValues - UNROLL); i += UNROLL) {
                // let the compiler unroll the loop
                for (size_t unroll = 0; unroll < UNROLL; ++unroll, ++dataOut) {
                    volatile DATAENC value = *dataOut;
                    (void) value;
                }
            }
            // remaining numbers
            for (; i < numValues; ++i, ++dataOut) {
                volatile DATAENC value = *dataOut;
                (void) value;
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
            const size_t numValues = this->in.template end<DATARAW>() - this->in.template begin<DATARAW>();
            size_t i = 0;
            auto dataOut = this->out.template begin<DATAENC>();
            for (; i <= (numValues - UNROLL); i += UNROLL) {
                // let the compiler unroll the loop
                for (size_t unroll = 0; unroll < UNROLL; ++unroll, ++dataOut) {
                    volatile DATAENC value = *dataOut;
                    *dataOut = static_cast<DATAENC>(value);
                }
            }
            // remaining numbers
            for (; i < numValues; ++i, ++dataOut) {
                volatile DATAENC value = *dataOut;
                *dataOut = static_cast<DATAENC>(value);
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
            const size_t numValues = this->in.template end<DATARAW>() - this->in.template begin<DATARAW>();
            size_t i = 0;
            auto dataIn = this->out.template begin<DATAENC>();
            auto dataOut = this->in.template begin<DATARAW>();
            for (; i <= (numValues - UNROLL); i += UNROLL) {
                // let the compiler unroll the loop
                for (size_t unroll = 0; unroll < UNROLL; ++unroll, ++dataOut, ++dataIn) {
                    *dataOut = static_cast<DATARAW>(*dataIn);
                }
            }
            // remaining numbers
            for (; i < numValues; ++i, ++dataOut, ++dataIn) {
                *dataOut = static_cast<DATARAW>(*dataIn);
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
            const size_t numValues = this->in.template end<DATARAW>() - this->in.template begin<DATARAW>();
            size_t i = 0;
            auto dataIn = this->out.template begin<DATAENC>();
            auto dataOut = this->in.template begin<DATARAW>();
            for (; i <= (numValues - UNROLL); i += UNROLL) {
                // let the compiler unroll the loop
                for (size_t unroll = 0; unroll < UNROLL; ++unroll, ++dataOut, ++dataIn) {
                    if (*dataIn != *dataOut) {
                        throw ErrorInfo(__FILE__, __LINE__, dataOut - this->in.template begin<DATARAW>(), config.numIterations);
                    }
                    *dataOut = static_cast<DATARAW>(*dataIn);
                }
            }
            // remaining numbers
            for (; i < numValues; ++i, ++dataOut, ++dataIn) {
                if (*dataIn != *dataOut) {
                    throw ErrorInfo(__FILE__, __LINE__, dataOut - this->in.template begin<DATARAW>(), config.numIterations);
                }
                *dataOut = static_cast<DATARAW>(*dataIn);
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
