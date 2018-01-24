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
#include <variant>

#include <Util/Test.hpp>
#include <Util/ErrorInfo.hpp>
#include <Util/Intrinsics.hpp>
#include <Util/ArithmeticSelector.hpp>
#include <Util/AggregateSelector.hpp>

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
                throw ErrorInfo(__FILE__, __LINE__, ret, iteration);
            }
        }
    }

    bool DoArithmetic(
            const ArithmeticConfiguration & config) override {
        return std::visit(ArithmeticSelector(), config.mode);
    }

    struct Arithmetor {
        CopyTest & ct;
        const ArithmeticConfiguration & config;
        Arithmetor(
                CopyTest & ct,
                const ArithmeticConfiguration & config)
                : ct(ct),
                  config(config) {
        }
        void operator()(
                ArithmeticConfiguration::Add) {
            auto beg = ct.bufEncoded.template begin<DATA>();
            auto end = ct.bufEncoded.template end<DATA>();
            auto out = ct.bufResult.template begin<DATA>();
            for (; beg < end; ++beg) {
                *out = *beg + config.operand;
            }
        }
        void operator()(
                ArithmeticConfiguration::Sub) {
            auto beg = ct.bufEncoded.template begin<DATA>();
            auto end = ct.bufEncoded.template end<DATA>();
            auto out = ct.bufResult.template begin<DATA>();
            for (; beg < end; ++beg) {
                *out = *beg - config.operand;
            }
        }
        void operator()(
                ArithmeticConfiguration::Mul) {
            auto beg = ct.bufEncoded.template begin<DATA>();
            auto end = ct.bufEncoded.template end<DATA>();
            auto out = ct.bufResult.template begin<DATA>();
            for (; beg < end; ++beg) {
                *out = *beg * config.operand;
            }
        }
        void operator()(
                ArithmeticConfiguration::Div) {
            auto beg = ct.bufEncoded.template begin<DATA>();
            auto end = ct.bufEncoded.template end<DATA>();
            auto out = ct.bufResult.template begin<DATA>();
            for (; beg < end; ++beg) {
                *out = *beg / config.operand;
            }
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

    void RunArithmeticChecked(
            const ArithmeticConfiguration & config) override {
        for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
            _ReadWriteBarrier();
            int ret = memcmp(this->bufEncoded.begin(), this->bufRaw.begin(), getNumBytes());
            if (ret != 0) {
                throw ErrorInfo(__FILE__, __LINE__, ret, iteration);
            }
            std::visit(Arithmetor(*this, config), config.mode);
        }
    }

    bool DoAggregate(
            const AggregateConfiguration & config) override {
        return std::visit(AggregateSelector(), config.mode);
    }

    struct Aggregator {
        CopyTest & ct;
        Aggregator(
                CopyTest & ct)
                : ct(ct) {
        }
        void operator()(
                AggregateConfiguration::Sum) {
            DATA sum = DATA(0);
            auto beg = ct.bufEncoded.template begin<DATA>();
            auto end = ct.bufEncoded.template end<DATA>();
            auto out = ct.bufResult.template begin<DATA>();
            for (; beg < end; ++beg) {
                sum += *beg;
            }
            *out = sum;
        }
        void operator()(
                AggregateConfiguration::Min) {
            DATA min(std::numeric_limits<DATA>::max());
            auto beg = ct.bufEncoded.template begin<DATA>();
            auto end = ct.bufEncoded.template end<DATA>();
            auto out = ct.bufResult.template begin<DATA>();
            for (; beg < end; ++beg) {
                min = std::min(min, *beg);
            }
            *out = min;
        }
        void operator()(
                AggregateConfiguration::Max) {
            DATA max(std::numeric_limits<DATA>::min());
            auto beg = ct.bufEncoded.template begin<DATA>();
            auto end = ct.bufEncoded.template end<DATA>();
            auto out = ct.bufResult.template begin<DATA>();
            for (; beg < end; ++beg) {
                max = std::max(max, *beg);
            }
            *out = max;
        }
        void operator()(
                AggregateConfiguration::Avg) {
            DATA sum = DATA(0);
            auto beg = ct.bufEncoded.template begin<DATA>();
            auto end = ct.bufEncoded.template end<DATA>();
            auto out = ct.bufResult.template begin<DATA>();
            for (; beg < end; ++beg) {
                sum += *beg;
            }
            *out = sum / ct.getNumValues();
        }
    };

    void RunAggregate(
            const AggregateConfiguration & config) override {
        for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
            _ReadWriteBarrier();
            std::visit(Aggregator(*this), config.mode);
        }
    }

    bool DoAggregateChecked(
            const AggregateConfiguration & config) override {
        return std::visit(AggregateSelector(), config.mode);
    }

    void RunAggregateChecked(
            const AggregateConfiguration & config) override {
        for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
            _ReadWriteBarrier();
            int ret = memcmp(this->bufRaw.begin(), this->bufEncoded.begin(), getNumBytes());
            if (ret != 0) {
                throw ErrorInfo(__FILE__, __LINE__, ret, iteration);
            }
            std::visit(Aggregator(*this), config.mode);
        }
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
                throw ErrorInfo(__FILE__, __LINE__, ret, iteration);
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
                throw ErrorInfo(__FILE__, __LINE__, ret, iteration);
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
