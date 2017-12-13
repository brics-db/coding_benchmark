// Copyright (c) 2017 Till Kolditz
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
 * File:   Hamming_compute.tcc
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 22-08-2017 10:47
 */

#pragma once

#include <Test.hpp>
#include <Hamming/Hamming_scalar.hpp>
#include <Hamming/Hamming_simd.hpp>

namespace coding_benchmark {

    template<typename DATAIN, size_t UNROLL>
    struct Hamming_compute_scalar :
            public Test<DATAIN, hamming_t<DATAIN, DATAIN>>,
            public ScalarTest {
        typedef hamming_t<DATAIN, DATAIN> hamming_scalar_t;
        typedef typename hamming_scalar_t::code_t code_t;

        using Test<DATAIN, hamming_scalar_t>::Test;

        virtual ~Hamming_compute_scalar() {
        }

        void RunEncode(
                const EncodeConfiguration & config) override {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                _ReadWriteBarrier();
                auto data = this->bufRaw.template begin<DATAIN>();
                auto dataEnd = this->bufRaw.template end<DATAIN>();
                auto dataOut = this->bufEncoded.template begin<hamming_scalar_t>();
                while (data <= (dataEnd - UNROLL)) {
                    for (size_t k = 0; k < UNROLL; ++k, ++data, ++dataOut) {
                        dataOut->store(*data);
                    }
                }
                for (; data < dataEnd; ++data, ++dataOut) {
                    dataOut->store(*data);
                }
            }
        }
    };

#ifdef __SSE4_2__

    template<typename DATAIN, size_t UNROLL>
    struct Hamming_compute_sse42_1 :
            public Test<DATAIN, hamming_t<DATAIN, __m128i >>,
            public SSE42Test {

        typedef hamming_t<DATAIN, __m128i> hamming_sse42_t;
        typedef hamming_t<DATAIN, DATAIN> hamming_scalar_t;

        static const constexpr size_t VALUES_PER_VECTOR = sizeof(__m128i) / sizeof (DATAIN);
        static const constexpr size_t VALUES_PER_UNROLL = UNROLL * VALUES_PER_VECTOR;

        using Test<DATAIN, hamming_sse42_t>::Test;

        virtual ~Hamming_compute_sse42_1() {
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
                for (; data <= (dataEnd - 1); ++data, ++dataOut) {
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
    };

    template<typename DATAIN, size_t UNROLL>
    struct Hamming_compute_sse42_2 :
            public Test<DATAIN, hamming_t<DATAIN, __m128i >>,
            public SSE42Test {

        typedef hamming_t<DATAIN, __m128i> hamming_sse42_t;
        typedef hamming_t<DATAIN, DATAIN> hamming_scalar_t;

        static const constexpr size_t VALUES_PER_VECTOR = sizeof(__m128i) / sizeof (DATAIN);
        static const constexpr size_t VALUES_PER_UNROLL = UNROLL * VALUES_PER_VECTOR;

        using Test<DATAIN, hamming_sse42_t>::Test;

        virtual ~Hamming_compute_sse42_2() {
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
                        dataOut->store2(*data);
                    }
                }
                for (; data <= (dataEnd - 1); ++data, ++dataOut) {
                    dataOut->store2(*data);
                }
                if (data < dataEnd) {
                    auto data2 = reinterpret_cast<DATAIN*>(data);
                    auto dataEnd2 = reinterpret_cast<DATAIN*>(dataEnd);
                    auto dataOut2 = reinterpret_cast<hamming_scalar_t*>(dataOut);
                    for (; data2 < dataEnd2; ++data2, ++dataOut2) {
                        dataOut2->store2(*data2);
                    }
                }
            }
        }
    };

    template<typename DATAIN, size_t UNROLL>
    struct Hamming_compute_sse42_3 :
            public Test<DATAIN, hamming_t<DATAIN, __m128i >>,
            public SSE42Test {

        typedef hamming_t<DATAIN, __m128i> hamming_sse42_t;
        typedef hamming_t<DATAIN, DATAIN> hamming_scalar_t;

        static const constexpr size_t VALUES_PER_VECTOR = sizeof(__m128i) / sizeof (DATAIN);
        static const constexpr size_t VALUES_PER_UNROLL = UNROLL * VALUES_PER_VECTOR;

        using Test<DATAIN, hamming_sse42_t>::Test;

        virtual ~Hamming_compute_sse42_3() {
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
                        dataOut->store3(*data);
                    }
                }
                for (; data <= (dataEnd - 1); ++data, ++dataOut) {
                    dataOut->store3(*data);
                }
                if (data < dataEnd) {
                    auto data2 = reinterpret_cast<DATAIN*>(data);
                    auto dataEnd2 = reinterpret_cast<DATAIN*>(dataEnd);
                    auto dataOut2 = reinterpret_cast<hamming_scalar_t*>(dataOut);
                    for (; data2 < dataEnd2; ++data2, ++dataOut2) {
                        dataOut2->store3(*data2);
                    }
                }
            }
        }
    };

#endif

#ifdef __AVX2__

    template<typename DATAIN, size_t UNROLL>
    struct Hamming_compute_avx2_1 :
            public Test<DATAIN, hamming_t<DATAIN, __m256i >>,
            public AVX2Test {

        typedef hamming_t<DATAIN, __m256i> hamming_avx2_t;
        typedef hamming_t<DATAIN, DATAIN> hamming_scalar_t;

        static const constexpr size_t VALUES_PER_VECTOR = sizeof(__m256i) / sizeof (DATAIN);
        static const constexpr size_t VALUES_PER_UNROLL = UNROLL * VALUES_PER_VECTOR;

        using Test<DATAIN, hamming_avx2_t>::Test;

        virtual ~Hamming_compute_avx2_1() {
        }

        void RunEncode(
                const EncodeConfiguration & config) override {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                _ReadWriteBarrier();
                auto data = this->bufRaw.template begin<__m256i >();
                auto dataEnd = this->bufRaw.template end<__m256i >();
                auto dataOut = this->bufEncoded.template begin<hamming_avx2_t>();
                while (data <= (dataEnd - UNROLL)) {
                    for (size_t k = 0; k < UNROLL; ++k, ++data, ++dataOut) {
                        dataOut->store(*data);
                    }
                }
                for (; data <= (dataEnd - 1); ++data, ++dataOut) {
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
    };

    template<typename DATAIN, size_t UNROLL>
    struct Hamming_compute_avx2_2 :
            public Test<DATAIN, hamming_t<DATAIN, __m256i >>,
            public AVX2Test {

        typedef hamming_t<DATAIN, __m256i> hamming_avx2_t;
        typedef hamming_t<DATAIN, DATAIN> hamming_scalar_t;

        static const constexpr size_t VALUES_PER_VECTOR = sizeof(__m256i) / sizeof (DATAIN);
        static const constexpr size_t VALUES_PER_UNROLL = UNROLL * VALUES_PER_VECTOR;

        using Test<DATAIN, hamming_avx2_t>::Test;

        virtual ~Hamming_compute_avx2_2() {
        }

        void RunEncode(
                const EncodeConfiguration & config) override {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                _ReadWriteBarrier();
                auto data = this->bufRaw.template begin<__m256i >();
                auto dataEnd = this->bufRaw.template end<__m256i >();
                auto dataOut = this->bufEncoded.template begin<hamming_avx2_t>();
                while (data <= (dataEnd - UNROLL)) {
                    for (size_t k = 0; k < UNROLL; ++k, ++data, ++dataOut) {
                        dataOut->store2(*data);
                    }
                }
                for (; data <= (dataEnd - 1); ++data, ++dataOut) {
                    dataOut->store2(*data);
                }
                if (data < dataEnd) {
                    auto data2 = reinterpret_cast<DATAIN*>(data);
                    auto dataEnd2 = reinterpret_cast<DATAIN*>(dataEnd);
                    auto dataOut2 = reinterpret_cast<hamming_scalar_t*>(dataOut);
                    for (; data2 < dataEnd2; ++data2, ++dataOut2) {
                        dataOut2->store2(*data2);
                    }
                }
            }
        }
    };

    template<typename DATAIN, size_t UNROLL>
    struct Hamming_compute_avx2_3 :
            public Test<DATAIN, hamming_t<DATAIN, __m256i >>,
            public AVX2Test {

        typedef hamming_t<DATAIN, __m256i> hamming_avx2_t;
    typedef hamming_t<DATAIN, DATAIN> hamming_scalar_t;

    static const constexpr size_t VALUES_PER_VECTOR = sizeof(__m256i) / sizeof (DATAIN);
    static const constexpr size_t VALUES_PER_UNROLL = UNROLL * VALUES_PER_VECTOR;

    using Test<DATAIN, hamming_avx2_t>::Test;

    virtual ~Hamming_compute_avx2_3() {
    }

    void RunEncode(
            const EncodeConfiguration & config) override {
        for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
            _ReadWriteBarrier();
            auto data = this->bufRaw.template begin<__m256i >();
            auto dataEnd = this->bufRaw.template end<__m256i >();
            auto dataOut = this->bufEncoded.template begin<hamming_avx2_t>();
            while (data <= (dataEnd - UNROLL)) {
                for (size_t k = 0; k < UNROLL; ++k, ++data, ++dataOut) {
                    dataOut->store3(*data);
                }
            }
            for (; data <= (dataEnd - 1); ++data, ++dataOut) {
                dataOut->store3(*data);
            }
            if (data < dataEnd) {
                auto data2 = reinterpret_cast<DATAIN*>(data);
                auto dataEnd2 = reinterpret_cast<DATAIN*>(dataEnd);
                auto dataOut2 = reinterpret_cast<hamming_scalar_t*>(dataOut);
                for (; data2 < dataEnd2; ++data2, ++dataOut2) {
                    dataOut2->store3(*data2);
                }
            }
        }
    }
};

#endif

}
