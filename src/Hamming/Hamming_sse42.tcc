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
/* 
 * File:   Hamming_sse42.hpp
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 07. December 2016, 13:12
 */

#pragma once

#include <Test.hpp>
#include <Util/ErrorInfo.hpp>
#include <Util/ArithmeticSelector.hpp>
#include <Util/AggregateSelector.hpp>
#include <Hamming/Hamming_scalar.hpp>
#include <Util/SIMD.hpp>

extern template struct hamming_t<uint16_t, __m128i > ;
extern template struct hamming_t<uint32_t, __m128i > ;

template<typename DATAIN, size_t UNROLL>
struct Hamming_sse42 :
        public Test<DATAIN, hamming_t<DATAIN, __m128i >>,
        public SSE42Test {

    typedef hamming_t<DATAIN, __m128i> hamming_sse42_t;
    typedef hamming_t<DATAIN, DATAIN> hamming_scalar_t;

    constexpr static const size_t VALUES_PER_VECTOR = sizeof(__m128i) / sizeof (DATAIN);
    constexpr static const size_t VALUES_PER_UNROLL = UNROLL * VALUES_PER_VECTOR;

    using Test<DATAIN, hamming_sse42_t>::Test;

    virtual ~Hamming_sse42() {
    }

    void RunEncode(
            const EncodeConfiguration & config) override {
        for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
            _ReadWriteBarrier();
            auto data = this->bufRaw.template begin<__m128i>();
            auto dataEnd = this->bufRaw.template end<__m128i>();
            auto dataOut = this->bufEncoded.template begin<hamming_sse42_t>();
            while (data <= (dataEnd - UNROLL)) {
                for (size_t k = 0; k < UNROLL; ++k, ++data, ++dataOut) {
                    dataOut->store(*data);
                }
            }
            for (; data <= (dataEnd - 1); ++data, ++dataEnd) {
                dataOut->store(*data);
            }
            if (data < dataEnd) {
                auto data2 = reinterpret_cast<DATAIN*>(data);
                auto dataEnd2 = reinterpret_cast<DATAIN*>(dataEnd);
                auto dataOut2 = reinterpret_cast<hamming_scalar_t*>(dataOut);
                for (; data2 < dataEnd2; ++data2, ++dataOut2) {
                    dataOut2->store(*data2);
                }
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
            size_t numValues = this->getNumValues();
            size_t i = 0;
            auto data = this->bufEncoded.template begin<hamming_sse42_t>();
            while (i <= (numValues - VALUES_PER_UNROLL)) {
                for (size_t k = 0; k < UNROLL; ++k, i += VALUES_PER_VECTOR, ++data) {
                    if (!data->isValid()) {
                        throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                    }
                }
            }
            for (; i <= (numValues - VALUES_PER_VECTOR); i += VALUES_PER_VECTOR, ++data) {
                if (!data->isValid()) {
                    throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                }
            }
            if (i < numValues) {
                for (auto data2 = reinterpret_cast<hamming_scalar_t*>(data); i < numValues; ++i, ++data2) {
                    if (!data2->isValid()) {
                        throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                    }
                }
            }
        }
    }

    bool DoArithmetic(
            const ArithmeticConfiguration & config) override {
        return std::visit(ArithmeticSelector(), config.mode);
    }

    struct Arithmetor {
        using hamming_sse24_t = Hamming_sse42::hamming_sse42_t;
        using hamming_scalar_t = Hamming_sse42::hamming_scalar_t;
        constexpr static const size_t VALUES_PER_VECTOR = sizeof(__m128i) / sizeof (DATAIN);
        constexpr static const size_t VALUES_PER_UNROLL = UNROLL * VALUES_PER_VECTOR;
        Hamming_sse42 & test;
        const ArithmeticConfiguration & config;
        const size_t iteration;
        Arithmetor(
                Hamming_sse42 & test,
                const ArithmeticConfiguration & config,
                const size_t iteration)
        : test(test),
        config(config),
        iteration(iteration) {
        }
        void operator()(
                ArithmeticConfiguration::Add) {
            const size_t numValues = test.getNumValues();
            auto mmOperand = SIMD<__m128i, DATAIN>::set1(config.operand);
            size_t i = 0;
            auto dataIn = test.bufEncoded.template begin<hamming_sse42_t>();
            auto dataOut = test.bufResult.template begin<hamming_sse42_t>();
            while (i <= (numValues - VALUES_PER_UNROLL)) {
                for (size_t k = 0; k < UNROLL; ++k, i += VALUES_PER_VECTOR, ++dataIn) {
                    dataOut->store(SIMD<__m128i, DATAIN>::add(dataIn->data, mmOperand));
                }
            }
            for (; i <= (numValues - VALUES_PER_VECTOR); i += VALUES_PER_VECTOR, ++dataIn) {
                dataOut->store(SIMD<__m128i, DATAIN>::add(dataIn->data, mmOperand));
            }
            if (i < numValues) {
                auto dataIn2 = reinterpret_cast<hamming_scalar_t*>(dataIn);
                auto dataOut2 = reinterpret_cast<hamming_scalar_t*>(dataOut);
                for (; i < numValues; ++i, ++dataIn2, ++dataOut2) {
                    dataOut2->store(dataIn2->data + config.operand);
                }
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
            std::visit(Arithmetor(*this, config, iteration), config.mode);
        }
    }

    bool DoArithmeticChecked(
            const ArithmeticConfiguration & config) override {
        return std::visit(ArithmeticSelector(), config.mode);
    }

    struct ArithmetorChecked {
        using hamming_sse24_t = Hamming_sse42::hamming_sse42_t;
        using hamming_scalar_t = Hamming_sse42::hamming_scalar_t;
        constexpr static const size_t VALUES_PER_VECTOR = sizeof(__m128i) / sizeof (DATAIN);
        constexpr static const size_t VALUES_PER_UNROLL = UNROLL * VALUES_PER_VECTOR;
        Hamming_sse42 & test;
        const ArithmeticConfiguration & config;
        const size_t iteration;
        ArithmetorChecked(
                Hamming_sse42 & test,
                const ArithmeticConfiguration & config,
                const size_t iteration)
        : test(test),
        config(config),
        iteration(iteration) {
        }
        void operator()(
                ArithmeticConfiguration::Add) {
            const size_t numValues = test.getNumValues();
            auto mmOperand = SIMD<__m128i, DATAIN>::set1(config.operand);
            size_t i = 0;
            auto dataIn = test.bufEncoded.template begin<hamming_sse42_t>();
            auto dataOut = test.bufResult.template begin<hamming_sse42_t>();
            while (i <= (numValues - VALUES_PER_UNROLL)) {
                for (size_t k = 0; k < UNROLL; ++k, i += VALUES_PER_VECTOR, ++dataIn) {
                    auto tmp = dataIn->data;
                    if (!dataIn->isValid()) {
                        throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                    }
                    dataOut->store(SIMD<__m128i, DATAIN>::add(tmp, mmOperand));
                }
            }
            for (; i <= (numValues - VALUES_PER_VECTOR); i += VALUES_PER_VECTOR, ++dataIn) {
                auto tmp = dataIn->data;
                if (!dataIn->isValid()) {
                    throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                }
                dataOut->store(SIMD<__m128i, DATAIN>::add(tmp, mmOperand));
            }
            if (i < numValues) {
                auto dataIn2 = reinterpret_cast<hamming_scalar_t*>(dataIn);
                auto dataOut2 = reinterpret_cast<hamming_scalar_t*>(dataOut);
                for (; i < numValues; ++i, ++dataIn2, ++dataOut2) {
                    auto tmp = dataIn2->data;
                    if (!dataIn2->isValid()) {
                        throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                    }
                    dataOut2->store(tmp + config.operand);
                }
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

    bool DoAggregate(
            const AggregateConfiguration & config) override {
        return std::visit(AggregateSelector(), config.mode);
    }

    struct Aggregator {
        using hamming_sse24_t = Hamming_sse42::hamming_sse42_t;
        using hamming_scalar_t = Hamming_sse42::hamming_scalar_t;
        typedef typename Larger<DATAIN>::larger_t larger_t;
        typedef hamming_t<larger_t, larger_t> hamming_larger_t;
        constexpr static const size_t VALUES_PER_VECTOR = sizeof(__m128i) / sizeof (DATAIN);
        constexpr static const size_t VALUES_PER_UNROLL = UNROLL * VALUES_PER_VECTOR;
        Hamming_sse42 & test;
        const AggregateConfiguration & config;
        Aggregator(
                Hamming_sse42 & test,
                const AggregateConfiguration & config)
        : test(test),
        config(config) {
        }
        void operator()(
                AggregateConfiguration::Sum) {
            const size_t VALUES_PER_VECTOR = sizeof(__m128i) / sizeof (DATAIN);
            const size_t VALUES_PER_UNROLL = UNROLL * VALUES_PER_VECTOR;
            const size_t numValues = test.getNumValues();
            size_t i = 0;
            auto dataIn = test.bufEncoded.template begin<hamming_sse42_t>();
            auto dataOut = test.bufResult.template begin<hamming_larger_t>();
            auto mmTmp = SIMD<__m128i, larger_t>::set1(0);
            while (i <= (numValues - VALUES_PER_UNROLL)) {
                for (size_t k = 0; k < UNROLL; ++k, i += VALUES_PER_VECTOR, ++dataIn) {
                    mmTmp = SIMD<__m128i, larger_t>::add(mmTmp, SIMD<__m128i, DATAIN>::cvt_larger_lo(dataIn->data));
                    mmTmp = SIMD<__m128i, larger_t>::add(mmTmp, SIMD<__m128i, DATAIN>::cvt_larger_hi(dataIn->data));
                }
            }
            for (; i <= (numValues - VALUES_PER_VECTOR); i += VALUES_PER_VECTOR, ++dataIn) {
                mmTmp = SIMD<__m128i, larger_t>::add(mmTmp, SIMD<__m128i, DATAIN>::cvt_larger_lo(dataIn->data));
                mmTmp = SIMD<__m128i, larger_t>::add(mmTmp, SIMD<__m128i, DATAIN>::cvt_larger_hi(dataIn->data));
            }
            auto tmp = SIMD<__m128i, larger_t>::sum(mmTmp);
            if (i < numValues) {
                auto dataIn2 = reinterpret_cast<hamming_scalar_t*>(dataIn);
                auto dataOut2 = reinterpret_cast<hamming_scalar_t*>(dataOut);
                for (; i < numValues; ++i, ++dataIn2, ++dataOut2) {
                    tmp += dataIn2->data;
                }
            }
            dataOut->store(tmp);
        }
        void operator()(
                AggregateConfiguration::Min) {
        }
        void operator()(
                AggregateConfiguration::Max) {
        }
        void operator()(
                AggregateConfiguration::Avg) {
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
        using hamming_sse24_t = Hamming_sse42::hamming_sse42_t;
        using hamming_scalar_t = Hamming_sse42::hamming_scalar_t;
        typedef typename Larger<DATAIN>::larger_t larger_t;
        typedef hamming_t<larger_t, larger_t> hamming_larger_t;
        constexpr static const size_t VALUES_PER_VECTOR = sizeof(__m128i) / sizeof (DATAIN);
        constexpr static const size_t VALUES_PER_UNROLL = UNROLL * VALUES_PER_VECTOR;
        Hamming_sse42 & test;
        const AggregateConfiguration & config;
        size_t iteration;
        AggregatorChecked(
                Hamming_sse42 & test,
                const AggregateConfiguration & config,
                size_t iteration)
        : test(test),
        config(config),
        iteration(iteration) {
        }
        void operator()(
                AggregateConfiguration::Sum) {
            const size_t numValues = test.getNumValues();
            size_t i = 0;
            auto dataIn = test.bufEncoded.template begin<hamming_sse42_t>();
            auto dataOut = test.bufResult.template begin<hamming_larger_t>();
            auto mmTmp = SIMD<__m128i, larger_t>::set1(0);
            while (i <= (numValues - VALUES_PER_UNROLL)) {
                for (size_t k = 0; k < UNROLL; ++k, i += VALUES_PER_VECTOR, ++dataIn) {
                    mmTmp = SIMD<__m128i, larger_t>::add(mmTmp, SIMD<__m128i, DATAIN>::cvt_larger_lo(dataIn->data));
                    mmTmp = SIMD<__m128i, larger_t>::add(mmTmp, SIMD<__m128i, DATAIN>::cvt_larger_hi(dataIn->data));
                }
            }
            for (; i <= (numValues - 1); i += VALUES_PER_VECTOR, ++dataIn) {
                mmTmp = SIMD<__m128i, larger_t>::add(mmTmp, SIMD<__m128i, DATAIN>::cvt_larger_lo(dataIn->data));
                mmTmp = SIMD<__m128i, larger_t>::add(mmTmp, SIMD<__m128i, DATAIN>::cvt_larger_hi(dataIn->data));
            }
            auto tmp = SIMD<__m128i, larger_t>::sum(mmTmp);
            if (i < numValues) {
                auto dataIn2 = reinterpret_cast<hamming_scalar_t*>(dataIn);
                auto dataOut2 = reinterpret_cast<hamming_scalar_t*>(dataOut);
                for (; i < numValues; ++i, ++dataIn2, ++dataOut2) {
                    tmp += dataIn2->data;
                }
            }
            dataOut->store(tmp);
        }
        void operator()(
                AggregateConfiguration::Min) {
        }
        void operator()(
                AggregateConfiguration::Max) {
        }
        void operator()(
                AggregateConfiguration::Avg) {
        }
    };

    void RunAggregateChecked(
            const AggregateConfiguration & config) override {
        for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
            _ReadWriteBarrier();
            std::visit(AggregatorChecked(*this, config, iteration), config.mode);
        }
    }

    bool DoDecode() override {
        return true;
    }

    void RunDecode(
            const DecodeConfiguration & config) override {
        for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
            _ReadWriteBarrier();
            const size_t VALUES_PER_VECTOR = sizeof(__m128i) / sizeof (DATAIN);
            const size_t VALUES_PER_UNROLL = UNROLL * VALUES_PER_VECTOR;
            size_t numValues = this->getNumValues();
            size_t i = 0;
            auto data = this->bufEncoded.template begin<hamming_sse42_t>();
            auto dataOut = this->bufResult.template begin<__m128i >();
            while (i <= (numValues - VALUES_PER_UNROLL)) {
                for (size_t k = 0; k < UNROLL; ++k, ++data, ++dataOut) {
                    *dataOut = data->data;
                }
                i += VALUES_PER_UNROLL;
            }
            for (; i <= (numValues - 1); i += VALUES_PER_VECTOR, ++data, ++dataOut) {
                *dataOut = data->data;
            }
            if (i < numValues) {
                auto data2 = reinterpret_cast<hamming_scalar_t*>(data);
                auto dataOut2 = reinterpret_cast<DATAIN*>(dataOut);
                for (; i < numValues; ++i, ++data2, ++dataOut2) {
                    *dataOut2 = data2->data;
                }
            }
        }
    }
};
