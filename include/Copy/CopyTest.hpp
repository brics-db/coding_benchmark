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
#include <Util/ArithmeticSelector.hpp>
#include <Util/AggregateSelector.hpp>

template<typename DATA, size_t UNROLL>
struct CopyTest :
        public Test<DATA, DATA>,
        public ScalarTest {

    using Test<DATA, DATA>::Test;

    virtual ~CopyTest() {
    }

    size_t getNumBytes(
            const SubTestConfiguration & config) const {
        return sizeof(DATA) * config.numValues;
    }

    void RunEncode(
            const EncodeConfiguration & config) override {
        for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
            _ReadWriteBarrier();
            auto numBytes = getNumBytes(config);
            memmove(config.target.begin(), config.source.begin(), numBytes);
            memmove(config.target.template begin<char>() + numBytes, config.source.begin(), numBytes); // just duplicate
        }
    }

    bool DoCheck() override {
        return true;
    }

    void RunCheck(
            const CheckConfiguration & config) override {
        for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
            _ReadWriteBarrier();
            auto numBytes = getNumBytes(config);
            int ret = memcmp(config.target.begin(), config.target.template begin<char>() + numBytes, numBytes);
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
            auto beg = config.source.template begin<DATA>();
            auto end = beg + 2 * config.numValues;
            auto out = config.target.template begin<DATA>();
            while (beg < end) {
                *out++ = *beg++ + config.operand;
            }
        }
        void operator()(
                ArithmeticConfiguration::Sub) {
            auto beg = config.source.template begin<DATA>();
            auto end = beg + 2 * config.numValues;
            auto out = config.target.template begin<DATA>();
            while (beg < end) {
                *out++ = *beg++ - config.operand;
            }
        }
        void operator()(
                ArithmeticConfiguration::Mul) {
            auto beg = config.source.template begin<DATA>();
            auto end = beg + 2 * config.numValues;
            auto out = config.target.template begin<DATA>();
            while (beg < end) {
                *out++ = *beg++ * config.operand;
            }
        }
        void operator()(
                ArithmeticConfiguration::Div) {
            auto beg = config.source.template begin<DATA>();
            auto end = beg + 2 * config.numValues;
            auto out = config.target.template begin<DATA>();
            while (beg < end) {
                *out++ = *beg++ / config.operand;
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
            auto numBytes = getNumBytes(config);
            int ret = memcmp(config.source.begin(), config.source.template begin<char>() + numBytes, numBytes);
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
        typedef typename Larger<DATA>::larger_t larger_t;
        const CopyTest & ct;
        const AggregateConfiguration & config;
        Aggregator(
                const CopyTest & ct,
                const AggregateConfiguration & config)
                : ct(ct),
                  config(config) {
        }
        void operator()(
                AggregateConfiguration::Sum) {
            larger_t sum = larger_t(0);
            auto beg = config.source.template begin<DATA>();
            auto end = beg + config.numValues;
            auto out = config.target.template begin<larger_t>();
            while (beg < end) {
                sum += *beg++;
            }
            *out++ = sum;
            *out = sum;
        }
        void operator()(
                AggregateConfiguration::Min) {
            DATA min(std::numeric_limits<DATA>::max());
            auto beg = config.source.template begin<DATA>();
            auto end = beg + config.numValues;
            auto out = config.target.template begin<larger_t>();
            while (beg < end) {
                min = std::min(min, *beg++);
            }
            *out++ = min;
            *out = min;
        }
        void operator()(
                AggregateConfiguration::Max) {
            DATA max(std::numeric_limits<DATA>::min());
            auto beg = config.source.template begin<DATA>();
            auto end = beg + config.numValues;
            auto out = config.target.template begin<larger_t>();
            while (beg < end) {
                max = std::max(max, *beg++);
            }
            *out++ = max;
            *out = max;
        }
        void operator()(
                AggregateConfiguration::Avg) {
            larger_t sum = larger_t(0);
            auto beg = config.source.template begin<DATA>();
            auto end = beg + config.numValues;
            auto out = config.target.template begin<larger_t>();
            while (beg < end) {
                sum += *beg++;
            }
            larger_t avg = sum / config.numValues;
            *out++ = avg;
            *out = avg;
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

    void RunAggregateChecked(
            const AggregateConfiguration & config) override {
        for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
            _ReadWriteBarrier();
            auto numBytes = getNumBytes(config);
            int ret = memcmp(config.source.begin(), config.source.template begin<char>() + numBytes, numBytes);
            if (ret != 0) {
                throw ErrorInfo(__FILE__, __LINE__, ret, iteration);
            }
            std::visit(Aggregator(*this, config), config.mode);
        }
    }

    bool DoReencodeChecked() override {
        return false;
    }

    void RunReencodeChecked(
            const ReencodeConfiguration & config) override {
        for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
            _ReadWriteBarrier();
            auto numBytes = getNumBytes(config);
            int ret = memcmp(config.source.begin(), config.source.template begin<char>() + numBytes, numBytes);
            if (ret != 0) {
                throw ErrorInfo(__FILE__, __LINE__, ret, iteration);
            }
            memmove(config.target.begin(), config.source.begin(), numBytes);
            memmove(config.target.template begin<char>() + numBytes, config.source.begin(), numBytes); // just duplicate
        }
    }

    bool DoDecode() override {
        return true;
    }

    void RunDecode(
            const DecodeConfiguration & config) override {
        for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
            _ReadWriteBarrier();
            memmove(config.target.begin(), config.source.begin(), getNumBytes(config));
        }
    }

    void RunDecodeChecked(
            const DecodeConfiguration & config) override {
        for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
            _ReadWriteBarrier();
            auto numBytes = getNumBytes(config);
            int ret = memcmp(config.source.begin(), config.source.template begin<char>() + numBytes, numBytes);
            if (ret != 0) {
                throw ErrorInfo(__FILE__, __LINE__, ret, iteration);
            }
            memmove(config.target.begin(), config.source.begin(), numBytes);
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

template<size_t UNROLL>
struct CopyTest64 :
        public CopyTest<uint64_t, UNROLL> {
    using CopyTest<uint64_t, UNROLL>::CopyTest;
};
