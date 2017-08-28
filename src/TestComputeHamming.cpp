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
 * File:   TestComputeHamming.cpp
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 22-08-2017 10:31
 */

#include <iostream>
#include <sstream>

#include <Util/AlignedBlock.hpp>
#include <Util/Stopwatch.hpp>
#include <Util/SIMD.hpp>
#include <Util/TestInfo.hpp>
#include <Util/ErrorInfo.hpp>
#include <Util/Intrinsics.hpp>
#include <Util/TestConfiguration.hpp>
#include <Util/VFunc.hpp>
#include <Util/ComputeNumRuns.hpp>
#include <Util/ExpandTest.hpp>

#include <Hamming/Hamming_compute.hpp>

#include <Output.hpp>

#define TestCase(...) VFUNC(TestCase, __VA_ARGS__)

#define TestCase4(type, name, bufRawdata, bufResult) \
    do { \
        std::clog << "# " << std::setw(4) << (vecTestInfos.size() + 2) <<  ": Testing " << #type << " (" << name << ")" << std::endl; \
        vecTestInfos.emplace_back(); \
        auto & vec = *vecTestInfos.rbegin(); \
        vec.reserve(ComputeNumRuns<UNROLL_LO, UNROLL_HI>::value); \
        ExpandTest<type, UNROLL_LO, UNROLL_HI>::Execute(vec, name, testConfig, dataGenConfig, bufRawdata, bufResult, bufResult); \
        std::clog << '#' << std::endl; \
    } while (0)

#define TestCase5(type, name, bufRawdata, bufResult, ref) \
    do { \
        std::clog << "# " << std::setw(4) << (vecTestInfos.size() + 2) <<  ": Testing " << #type << " (" << name << ")" << std::endl; \
        vecTestInfos.emplace_back(); \
        auto & vec = *vecTestInfos.rbegin(); \
        vec.reserve(ComputeNumRuns<UNROLL_LO, UNROLL_HI>::value); \
        ExpandTest<type, UNROLL_LO, UNROLL_HI>::Execute(vec, name, testConfig, dataGenConfig, bufRawdata, bufResult, bufResult); \
        std::clog << '#' << std::endl; \
        setTestInfosReference(*vecTestInfos.rbegin(), ref); \
    } while (0)

int main() {
    const size_t rawDataSize = 1024 * 1024; // size in BYTES
    const size_t iterations = 10;
    const size_t UNROLL_LO = 1;
    const size_t UNROLL_HI = 1024;
    AlignedBlock bufRawdata16(2 * rawDataSize, 64);
    AlignedBlock bufRawdata32(4 * rawDataSize, 64);
    AlignedBlock bufResult(16 * rawDataSize, 64);
    std::vector<std::vector<TestInfos>> vecTestInfos;
    TestConfiguration testConfig(iterations);
    DataGenerationConfiguration dataGenConfig;

    TestCase(Hamming_compute_scalar_16, "Scalar 16", bufRawdata16, bufResult);
    auto & ref = *vecTestInfos.rbegin();
    TestCase(Hamming_compute_sse42_1_16, "SSE4.2 1 16", bufRawdata16, bufResult, ref);
    TestCase(Hamming_compute_sse42_2_16, "SSE4.2 2 16", bufRawdata16, bufResult, ref);
    TestCase(Hamming_compute_sse42_3_16, "SSE4.2 3 16", bufRawdata16, bufResult, ref);
#ifdef __AVX2__
    TestCase(Hamming_compute_avx2_1_16, "AVX2 1 16", bufRawdata16, bufResult, ref);
    TestCase(Hamming_compute_avx2_2_16, "AVX2 2 16", bufRawdata16, bufResult, ref);
    TestCase(Hamming_compute_avx2_3_16, "AVX2 3 16", bufRawdata16, bufResult, ref);
#endif

    TestCase(Hamming_compute_scalar_32, "Scalar 32", bufRawdata32, bufResult, ref);
    ref = *vecTestInfos.rbegin();
    TestCase(Hamming_compute_sse42_1_32, "SSE4.2 1 32", bufRawdata32, bufResult, ref);
    TestCase(Hamming_compute_sse42_2_32, "SSE4.2 2 32", bufRawdata32, bufResult, ref);
    TestCase(Hamming_compute_sse42_3_32, "SSE4.2 3 32", bufRawdata32, bufResult, ref);
#ifdef __AVX2__
    TestCase(Hamming_compute_avx2_1_32, "AVX2 1 32", bufRawdata32, bufResult, ref);
    TestCase(Hamming_compute_avx2_2_32, "AVX2 2 32", bufRawdata32, bufResult, ref);
    TestCase(Hamming_compute_avx2_3_32, "AVX2 3 32", bufRawdata32, bufResult, ref);
#endif

    printResults<false>(vecTestInfos, OutputConfiguration(false, false));
    std::cout << "\n\n";
    printResults<true>(vecTestInfos, OutputConfiguration(false, false));

    return 0;
}
