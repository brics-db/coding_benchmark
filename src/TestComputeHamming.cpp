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
#include <Util/TestInfo.hpp>
#include <Util/ErrorInfo.hpp>
#include <Util/Intrinsics.hpp>
#include <Util/TestConfiguration.hpp>
#include <Util/VFunc.hpp>
#include <Util/ComputeNumRuns.hpp>
#include <Util/ExpandTest.hpp>
#include <Util/TestCase.hpp>
#include <Util/Output.hpp>

#include <Hamming/Hamming_compute.hpp>

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

    TestCase<Hamming_compute_scalar_16, UNROLL_LO, UNROLL_HI>("Hamming_compute_scalar_16", "Scalar 16", bufRawdata16, bufResult, bufResult, testConfig, dataGenConfig, vecTestInfos);
    auto idx = vecTestInfos.size() - 1;
    TestCase<Hamming_compute_sse42_1_16, UNROLL_LO, UNROLL_HI>("Hamming_compute_sse42_1_16", "SSE4.2 1 16", bufRawdata16, bufResult, bufResult, testConfig, dataGenConfig, vecTestInfos, idx);
    TestCase<Hamming_compute_sse42_2_16, UNROLL_LO, UNROLL_HI>("Hamming_compute_sse42_2_16", "SSE4.2 2 16", bufRawdata16, bufResult, bufResult, testConfig, dataGenConfig, vecTestInfos, idx);
    TestCase<Hamming_compute_sse42_3_16, UNROLL_LO, UNROLL_HI>("Hamming_compute_sse42_3_16", "SSE4.2 3 16", bufRawdata16, bufResult, bufResult, testConfig, dataGenConfig, vecTestInfos, idx);
#ifdef __AVX2__
    TestCase<Hamming_compute_avx2_1_16, UNROLL_LO, UNROLL_HI>("Hamming_compute_avx2_1_16", "AVX2 1 16", bufRawdata16, bufResult, bufResult, testConfig, dataGenConfig, vecTestInfos, idx);
    TestCase<Hamming_compute_avx2_2_16, UNROLL_LO, UNROLL_HI>("Hamming_compute_avx2_2_16", "AVX2 2 16", bufRawdata16, bufResult, bufResult, testConfig, dataGenConfig, vecTestInfos, idx);
    TestCase<Hamming_compute_avx2_3_16, UNROLL_LO, UNROLL_HI>("Hamming_compute_avx2_3_16", "AVX2 3 16", bufRawdata16, bufResult, bufResult, testConfig, dataGenConfig, vecTestInfos, idx);
#endif

    TestCase<Hamming_compute_scalar_32, UNROLL_LO, UNROLL_HI>("Hamming_compute_scalar_32", "Scalar 32", bufRawdata32, bufResult, bufResult, testConfig, dataGenConfig, vecTestInfos);
    idx = vecTestInfos.size() - 1;
    TestCase<Hamming_compute_sse42_1_32, UNROLL_LO, UNROLL_HI>("Hamming_compute_sse42_1_32", "SSE4.2 1 32", bufRawdata32, bufResult, bufResult, testConfig, dataGenConfig, vecTestInfos, idx);
    TestCase<Hamming_compute_sse42_2_32, UNROLL_LO, UNROLL_HI>("Hamming_compute_sse42_2_32", "SSE4.2 2 32", bufRawdata32, bufResult, bufResult, testConfig, dataGenConfig, vecTestInfos, idx);
    TestCase<Hamming_compute_sse42_3_32, UNROLL_LO, UNROLL_HI>("Hamming_compute_sse42_3_32", "SSE4.2 3 32", bufRawdata32, bufResult, bufResult, testConfig, dataGenConfig, vecTestInfos, idx);
#ifdef __AVX2__
    TestCase<Hamming_compute_avx2_1_32, UNROLL_LO, UNROLL_HI>("Hamming_compute_avx2_1_32", "AVX2 1 32", bufRawdata32, bufResult, bufResult, testConfig, dataGenConfig, vecTestInfos, idx);
    TestCase<Hamming_compute_avx2_2_32, UNROLL_LO, UNROLL_HI>("Hamming_compute_avx2_2_32", "AVX2 2 32", bufRawdata32, bufResult, bufResult, testConfig, dataGenConfig, vecTestInfos, idx);
    TestCase<Hamming_compute_avx2_3_32, UNROLL_LO, UNROLL_HI>("Hamming_compute_avx2_3_32", "AVX2 3 32", bufRawdata32, bufResult, bufResult, testConfig, dataGenConfig, vecTestInfos, idx);
#endif

    printResults<false>(vecTestInfos, OutputConfiguration(false, false));
    std::cout << "\n\n";
    printResults<true>(vecTestInfos, OutputConfiguration(false, false));

    return 0;
}
