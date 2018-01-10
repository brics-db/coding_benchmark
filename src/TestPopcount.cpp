// Copyright 2017 Till Kolditz
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
 * TestPopcount.cpp
 *
 *  Created on: 10.01.2018
 *      Author: Till Kolditz - Till.Kolditz@gmail.com
 */

#include <iostream>

#include <Util/AlignedBlock.hpp>
#include <Util/Stopwatch.hpp>
#include <Util/TestInfo.hpp>
#include <Util/ErrorInfo.hpp>
#include <Util/Intrinsics.hpp>
#include <Util/TestConfiguration.hpp>
#include <Util/VFunc.hpp>
#include <Util/ComputeNumRuns.hpp>
#include <Util/ExpandTest.hpp>
#include <Util/TestCase.hpp>
#include <Util/Output.hpp>

#include <SIMD/SSE.hpp>
#include <SIMD/AVX2.hpp>
// #include <SIMD/AVX512.hpp>

namespace coding_benchmark {

#define STRCONCAT2_0(A, B) A##B
#define STRCONCAT2(A, B) STRCONCAT2_0(A, B)
#define STRCONCAT3_0(A, B, C) A##B##C
#define STRCONCAT3(A, B, C) STRCONCAT3_0(A, B, C)
#define STRCONCAT4_0(A, B, C, D) A##B##C##D
#define STRCONCAT4(A, B, C, D) STRCONCAT4_0(A, B, C, D)
#define STRCONCAT5_0(A, B, C, D, E) A##B##C##D##E
#define STRCONCAT5(A, B, C, D, E) STRCONCAT5_0(A, B, C, D, E)
#define STRCONCAT6_0(A, B, C, D, E, F) A##B##C##D##E##F
#define STRCONCAT6(A, B, C, D, E, F) STRCONCAT6_0(A, B, C, D, E, F)

    template<typename TIn, size_t UNROLL>
    struct popcount_scalar :
            public Test<TIn, uint8_t>,
            public ScalarTest {

        typedef uint8_t popcnt_t;

        using Test<TIn, uint8_t>::Test;

        virtual ~popcount_scalar() {
        }

        static inline uint8_t popcnt(
                TIn value) {
            if constexpr (sizeof(TIn) <= sizeof(unsigned int)) {
                return __builtin_popcount(static_cast<unsigned int>(value));
            } else if constexpr (sizeof(TIn) <= sizeof(unsigned long int)) {
                return __builtin_popcountl(static_cast<unsigned long int>(value));
            } else if constexpr (sizeof(TIn) <= sizeof(unsigned long long int)) {
                return __builtin_popcountll(static_cast<unsigned long long int>(value));
            }
            throw std::runtime_error("Type not supported for popcount!");
        }

        void RunEncode(
                const EncodeConfiguration & config) override {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                _ReadWriteBarrier();
                auto data = this->bufRaw.template begin<TIn>();
                auto dataEnd = this->bufRaw.template end<TIn>();
                auto dataOut = this->bufEncoded.template begin<uint8_t>();
                while (data <= (dataEnd - UNROLL)) {
                    for (size_t k = 0; k < UNROLL; ++k) {
                        *dataOut++ = popcnt(*data++);
                    }
                }
                while (data < dataEnd) {
                    *dataOut++ = popcnt(*data++);
                }
            }
        }
    };

#define MAKE_SCALAR_TYPE(TYPE, TYPESIZE) \
    template<size_t UNROLL> \
    struct popcount_scalar_ ## TYPESIZE \
            : public popcount_scalar<TYPE, UNROLL> { \
        using popcount_scalar<TYPE, UNROLL>::popcount_scalar; \
        virtual ~popcount_scalar_ ## TYPESIZE() {} \
    };

    MAKE_SCALAR_TYPE(uint8_t, 8)
    MAKE_SCALAR_TYPE(uint16_t, 16)
    MAKE_SCALAR_TYPE(uint32_t, 32)
    MAKE_SCALAR_TYPE(uint64_t, 64)

    template<typename TIn, typename SIMD, size_t UNROLL>
    struct popcount_simd_1 :
            Test<TIn, typename simd::mm<SIMD, TIn>::popcnt_t> {

        typedef simd::mm<SIMD, TIn> MM;
        typedef typename MM::popcnt_t mm_popcnt_t;

        using Test<TIn, mm_popcnt_t>::Test;

        virtual ~popcount_simd_1() {
        }

        void RunEncode(
                const EncodeConfiguration & config) override {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                _ReadWriteBarrier();
                auto mmData = this->bufRaw.template begin<SIMD>();
                auto mmDataEnd = this->bufRaw.template end<SIMD>();
                auto mmDataOut = this->bufEncoded.template begin<mm_popcnt_t>();
                while (mmData <= (mmDataEnd - UNROLL)) {
                    for (size_t k = 0; k < UNROLL; ++k) {
                        *mmDataOut++ = MM::popcount(*mmData++);
                    }
                }
                while (mmData <= (mmDataEnd - 1)) {
                    *mmDataOut++ = MM::popcount(*mmData++);
                }
                if (mmData < mmDataEnd) {
                    auto data = reinterpret_cast<TIn*>(mmData);
                    auto dataEnd = reinterpret_cast<TIn*>(mmDataEnd);
                    auto dataOut = reinterpret_cast<typename popcount_scalar<TIn, UNROLL>::popcnt_t*>(mmDataOut);
                    while (data < dataEnd) {
                        *dataOut++ = popcount_scalar<TIn, UNROLL>::popcnt(*data++);
                    }
                }
            }
        }
    };

    template<typename TIn, typename SIMD, size_t UNROLL>
    struct popcount_simd_2 :
            Test<TIn, typename simd::mm<SIMD, TIn>::popcnt_t> {

        typedef simd::mm<SIMD, TIn> MM;
        typedef typename MM::popcnt_t mm_popcnt_t;

        using Test<TIn, mm_popcnt_t>::Test;

        virtual ~popcount_simd_2() {
        }

        void RunEncode(
                const EncodeConfiguration & config) override {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                _ReadWriteBarrier();
                auto mmData = this->bufRaw.template begin<SIMD>();
                auto mmDataEnd = this->bufRaw.template end<SIMD>();
                auto mmDataOut = this->bufEncoded.template begin<mm_popcnt_t>();
                while (mmData <= (mmDataEnd - UNROLL)) {
                    for (size_t k = 0; k < UNROLL; ++k) {
                        *mmDataOut++ = MM::popcount2(*mmData++);
                    }
                }
                while (mmData <= (mmDataEnd - 1)) {
                    *mmDataOut++ = MM::popcount2(*mmData++);
                }
                if (mmData < mmDataEnd) {
                    auto data = reinterpret_cast<TIn*>(mmData);
                    auto dataEnd = reinterpret_cast<TIn*>(mmDataEnd);
                    auto dataOut = reinterpret_cast<typename popcount_scalar<TIn, UNROLL>::popcnt_t*>(mmDataOut);
                    while (data < dataEnd) {
                        *dataOut++ = popcount_scalar<TIn, UNROLL>::popcnt(*data++);
                    }
                }
            }
        }
    };

    template<typename TIn, typename SIMD, size_t UNROLL>
    struct popcount_simd_3 :
            Test<TIn, typename simd::mm<SIMD, TIn>::popcnt_t> {

        typedef simd::mm<SIMD, TIn> MM;
        typedef typename MM::popcnt_t mm_popcnt_t;

        using Test<TIn, mm_popcnt_t>::Test;

        virtual ~popcount_simd_3() {
        }

        void RunEncode(
                const EncodeConfiguration & config) override {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                _ReadWriteBarrier();
                auto mmData = this->bufRaw.template begin<SIMD>();
                auto mmDataEnd = this->bufRaw.template end<SIMD>();
                auto mmDataOut = this->bufEncoded.template begin<mm_popcnt_t>();
                while (mmData <= (mmDataEnd - UNROLL)) {
                    for (size_t k = 0; k < UNROLL; ++k) {
                        *mmDataOut++ = MM::popcount3(*mmData++);
                    }
                }
                while (mmData <= (mmDataEnd - 1)) {
                    *mmDataOut++ = MM::popcount3(*mmData++);
                }
                if (mmData < mmDataEnd) {
                    auto data = reinterpret_cast<TIn*>(mmData);
                    auto dataEnd = reinterpret_cast<TIn*>(mmDataEnd);
                    auto dataOut = reinterpret_cast<typename popcount_scalar<TIn, UNROLL>::popcnt_t*>(mmDataOut);
                    while (data < dataEnd) {
                        *dataOut++ = popcount_scalar<TIn, UNROLL>::popcnt(*data++);
                    }
                }
            }
        }
    };

    template<typename TIn, size_t UNROLL>
    struct popcount_sse42_1 :
            public popcount_simd_1<TIn, __m128i, UNROLL>,
            public SSE42Test {

        using popcount_simd_1<TIn, __m128i, UNROLL>::popcount_simd_1;

        virtual ~popcount_sse42_1() {
        }
    };

    template<typename TIn, size_t UNROLL>
    struct popcount_sse42_2 :
            public popcount_simd_2<TIn, __m128i, UNROLL>,
            public SSE42Test {

        using popcount_simd_2<TIn, __m128i, UNROLL>::popcount_simd_2;

        virtual ~popcount_sse42_2() {
        }
    };

    template<typename TIn, size_t UNROLL>
    struct popcount_sse42_3 :
            public popcount_simd_3<TIn, __m128i, UNROLL>,
            public SSE42Test {

        using popcount_simd_3<TIn, __m128i, UNROLL>::popcount_simd_3;

        virtual ~popcount_sse42_3() {
        }
    };

    template<typename TIn, size_t UNROLL>
    struct popcount_avx2_1 :
            public popcount_simd_1<TIn, __m256i, UNROLL>,
            public AVX2Test {

        using popcount_simd_1<TIn, __m256i, UNROLL>::popcount_simd_1;

        virtual ~popcount_avx2_1() {
        }
    };

    template<typename TIn, size_t UNROLL>
    struct popcount_avx2_2 :
            public popcount_simd_2<TIn, __m256i, UNROLL>,
            public AVX2Test {

        using popcount_simd_2<TIn, __m256i, UNROLL>::popcount_simd_2;

        virtual ~popcount_avx2_2() {
        }
    };

    template<typename TIn, size_t UNROLL>
    struct popcount_avx2_3 :
            public popcount_simd_3<TIn, __m256i, UNROLL>,
            public AVX2Test {

        using popcount_simd_3<TIn, __m256i, UNROLL>::popcount_simd_3;

        virtual ~popcount_avx2_3() {
        }
    };

#define MAKE_SIMD_TYPE0(SIMD, TYPEIN, TYPEINSIZE, VARIANT) \
    template<size_t UNROLL> \
    struct STRCONCAT6(popcount_, SIMD, _, TYPEINSIZE, _, VARIANT) : \
            public STRCONCAT4(popcount_, SIMD, _, VARIANT)<TYPEIN, UNROLL> { \
        using STRCONCAT4(popcount_, SIMD, _, VARIANT)<TYPEIN, UNROLL>::STRCONCAT4(popcount_, SIMD, _, VARIANT); \
        virtual ~STRCONCAT6(popcount_, SIMD, _, TYPEINSIZE, _, VARIANT)() {} \
    };

#define MAKE_SIMD_TYPE(SIMD, TYPEINSIZE) \
        MAKE_SIMD_TYPE0(SIMD, STRCONCAT3(uint, TYPEINSIZE, _t), TYPEINSIZE, 1) \
        MAKE_SIMD_TYPE0(SIMD, STRCONCAT3(uint, TYPEINSIZE, _t), TYPEINSIZE, 2) \
        MAKE_SIMD_TYPE0(SIMD, STRCONCAT3(uint, TYPEINSIZE, _t), TYPEINSIZE, 3)

#ifdef __SSE4_2__
    MAKE_SIMD_TYPE(sse42, 8)
    MAKE_SIMD_TYPE(sse42, 16)
    MAKE_SIMD_TYPE(sse42, 32)
    MAKE_SIMD_TYPE(sse42, 64)
#endif

#ifdef __AVX2__
MAKE_SIMD_TYPE(avx2, 8)
MAKE_SIMD_TYPE(avx2, 16)
MAKE_SIMD_TYPE(avx2, 32)
MAKE_SIMD_TYPE(avx2, 64)
#endif

}

using namespace coding_benchmark;

int main() {
const constexpr size_t numElements = 1000001;
const constexpr size_t iterations = 10;
const constexpr size_t UNROLL_LO = 1;
const constexpr size_t UNROLL_HI = 1024;

std::cout << "# numElements = " << numElements << '\n';
std::cout << "# iterations = " << iterations << '\n';
std::cout << "# UNROLL_LO = " << UNROLL_LO << '\n';
std::cout << "# UNROLL_HI = " << UNROLL_HI << '\n';

AlignedBlock bufRawdata16(numElements * sizeof(uint16_t), 64);
AlignedBlock bufRawdata32(numElements * sizeof(uint32_t), 64);
AlignedBlock bufResult(numElements * sizeof(uint64_t), 64);
std::vector<std::vector<TestInfos>> vecTestInfos;
vecTestInfos.reserve(32); // Reserve space to store sub-vectors!
TestConfiguration testConfig(iterations);
DataGenerationConfiguration dataGenConfig;

TestCase<popcount_scalar_8, UNROLL_LO, UNROLL_HI>("popcount_scalar_8", "Scalar 8", bufRawdata16, bufResult, bufResult, testConfig, dataGenConfig, vecTestInfos);
auto idx = vecTestInfos.size() - 1;
#ifdef __SSE4_2__
TestCase<popcount_sse42_8_1, UNROLL_LO, UNROLL_HI>("popcount_sse42_8_1", "SSE4.2 1 8", bufRawdata16, bufResult, bufResult, testConfig, dataGenConfig, vecTestInfos, idx);
TestCase<popcount_sse42_8_2, UNROLL_LO, UNROLL_HI>("popcount_sse42_8_2", "SSE4.2 2 8", bufRawdata16, bufResult, bufResult, testConfig, dataGenConfig, vecTestInfos, idx);
TestCase<popcount_sse42_8_3, UNROLL_LO, UNROLL_HI>("popcount_sse42_8_3", "SSE4.2 3 8", bufRawdata16, bufResult, bufResult, testConfig, dataGenConfig, vecTestInfos, idx);
#endif
#ifdef __AVX2__
TestCase<popcount_avx2_8_1, UNROLL_LO, UNROLL_HI>("popcount_avx2_8_1", "AVX2 1 8", bufRawdata16, bufResult, bufResult, testConfig, dataGenConfig, vecTestInfos, idx);
TestCase<popcount_avx2_8_2, UNROLL_LO, UNROLL_HI>("popcount_avx2_8_2", "AVX2 2 8", bufRawdata16, bufResult, bufResult, testConfig, dataGenConfig, vecTestInfos, idx);
TestCase<popcount_avx2_8_3, UNROLL_LO, UNROLL_HI>("popcount_avx2_8_3", "AVX2 3 8", bufRawdata16, bufResult, bufResult, testConfig, dataGenConfig, vecTestInfos, idx);
#endif

TestCase<popcount_scalar_16, UNROLL_LO, UNROLL_HI>("popcount_scalar_16", "Scalar 16", bufRawdata16, bufResult, bufResult, testConfig, dataGenConfig, vecTestInfos);
idx = vecTestInfos.size() - 1;
#ifdef __SSE4_2__
TestCase<popcount_sse42_16_1, UNROLL_LO, UNROLL_HI>("popcount_sse42_16_1", "SSE4.2 1 16", bufRawdata16, bufResult, bufResult, testConfig, dataGenConfig, vecTestInfos, idx);
TestCase<popcount_sse42_16_2, UNROLL_LO, UNROLL_HI>("popcount_sse42_16_2", "SSE4.2 2 16", bufRawdata16, bufResult, bufResult, testConfig, dataGenConfig, vecTestInfos, idx);
TestCase<popcount_sse42_16_3, UNROLL_LO, UNROLL_HI>("popcount_sse42_16_3", "SSE4.2 3 16", bufRawdata16, bufResult, bufResult, testConfig, dataGenConfig, vecTestInfos, idx);
#endif
#ifdef __AVX2__
TestCase<popcount_avx2_16_1, UNROLL_LO, UNROLL_HI>("popcount_avx2_16_1", "AVX2 1 16", bufRawdata16, bufResult, bufResult, testConfig, dataGenConfig, vecTestInfos, idx);
TestCase<popcount_avx2_16_2, UNROLL_LO, UNROLL_HI>("popcount_avx2_16_2", "AVX2 2 16", bufRawdata16, bufResult, bufResult, testConfig, dataGenConfig, vecTestInfos, idx);
TestCase<popcount_avx2_16_3, UNROLL_LO, UNROLL_HI>("popcount_avx2_16_3", "AVX2 3 16", bufRawdata16, bufResult, bufResult, testConfig, dataGenConfig, vecTestInfos, idx);
#endif

TestCase<popcount_scalar_32, UNROLL_LO, UNROLL_HI>("popcount_scalar_32", "Scalar 32", bufRawdata16, bufResult, bufResult, testConfig, dataGenConfig, vecTestInfos);
idx = vecTestInfos.size() - 1;
#ifdef __SSE4_2__
TestCase<popcount_sse42_32_1, UNROLL_LO, UNROLL_HI>("popcount_sse42_32_1", "SSE4.2 1 32", bufRawdata16, bufResult, bufResult, testConfig, dataGenConfig, vecTestInfos, idx);
TestCase<popcount_sse42_32_2, UNROLL_LO, UNROLL_HI>("popcount_sse42_32_2", "SSE4.2 2 32", bufRawdata16, bufResult, bufResult, testConfig, dataGenConfig, vecTestInfos, idx);
TestCase<popcount_sse42_32_3, UNROLL_LO, UNROLL_HI>("popcount_sse42_32_3", "SSE4.2 3 32", bufRawdata16, bufResult, bufResult, testConfig, dataGenConfig, vecTestInfos, idx);
#endif
#ifdef __AVX2__
TestCase<popcount_avx2_32_1, UNROLL_LO, UNROLL_HI>("popcount_avx2_32_1", "AVX2 1 32", bufRawdata16, bufResult, bufResult, testConfig, dataGenConfig, vecTestInfos, idx);
TestCase<popcount_avx2_32_2, UNROLL_LO, UNROLL_HI>("popcount_avx2_32_2", "AVX2 2 32", bufRawdata16, bufResult, bufResult, testConfig, dataGenConfig, vecTestInfos, idx);
TestCase<popcount_avx2_32_3, UNROLL_LO, UNROLL_HI>("popcount_avx2_32_3", "AVX2 3 32", bufRawdata16, bufResult, bufResult, testConfig, dataGenConfig, vecTestInfos, idx);
#endif

TestCase<popcount_scalar_64, UNROLL_LO, UNROLL_HI>("popcount_scalar_64", "Scalar 64", bufRawdata16, bufResult, bufResult, testConfig, dataGenConfig, vecTestInfos);
idx = vecTestInfos.size() - 1;
#ifdef __SSE4_2__
TestCase<popcount_sse42_64_1, UNROLL_LO, UNROLL_HI>("popcount_sse42_64_1", "SSE4.2 1 64", bufRawdata16, bufResult, bufResult, testConfig, dataGenConfig, vecTestInfos, idx);
TestCase<popcount_sse42_64_2, UNROLL_LO, UNROLL_HI>("popcount_sse42_64_2", "SSE4.2 2 64", bufRawdata16, bufResult, bufResult, testConfig, dataGenConfig, vecTestInfos, idx);
TestCase<popcount_sse42_64_3, UNROLL_LO, UNROLL_HI>("popcount_sse42_64_3", "SSE4.2 3 64", bufRawdata16, bufResult, bufResult, testConfig, dataGenConfig, vecTestInfos, idx);
#endif
#ifdef __AVX2__
TestCase<popcount_avx2_64_1, UNROLL_LO, UNROLL_HI>("popcount_avx2_64_1", "AVX2 1 64", bufRawdata16, bufResult, bufResult, testConfig, dataGenConfig, vecTestInfos, idx);
TestCase<popcount_avx2_64_2, UNROLL_LO, UNROLL_HI>("popcount_avx2_64_2", "AVX2 2 64", bufRawdata16, bufResult, bufResult, testConfig, dataGenConfig, vecTestInfos, idx);
TestCase<popcount_avx2_64_3, UNROLL_LO, UNROLL_HI>("popcount_avx2_64_3", "AVX2 3 64", bufRawdata16, bufResult, bufResult, testConfig, dataGenConfig, vecTestInfos, idx);
#endif

printResults<false>(vecTestInfos, OutputConfiguration(false, false));
}
