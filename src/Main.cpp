// Copyright (c) 2016 Till Kolditz, Stefan de Bruijn
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

#include <string>
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>

#ifdef _MSC_VER
// disable stupid diamond-inheritance warnings (the compiler does not see that there is only a single implementation for each of the functions)
#pragma warning(disable: 4250)
#endif

#include <Util/Test.hpp>
#include <Util/AlignedBlock.hpp>
#include <Util/TestInfo.hpp>
#include <Util/ErrorInfo.hpp>
#include <Util/VFunc.hpp>
#include <Util/Euclidean.hpp>
#include <Util/Intrinsics.hpp>
#include <Util/ComputeNumRuns.hpp>
#include <Util/ExpandTest.hpp>
#include <Util/TestCase.hpp>
#include <Util/Output.hpp>

#include <Copy/CopyTest.hpp>

#include <XOR/XOR_scalar.hpp>
#include <XOR/XOR_sse42.hpp>
#ifdef __AVX2__
#include <XOR/XOR_avx2.hpp>
#endif

#include <AN/AN_scalar.hpp>
#include <AN/AN_sse42.hpp>
#ifdef __AVX2__
#include <AN/AN_avx2.hpp>
#endif

#include <Hamming/Hamming_scalar.hpp>
#include <Hamming/Hamming_simd.hpp>

using namespace coding_benchmark;

int checkArgs(
        int argc,
        char* argv[],
        uint32_t & AUser1) {
    if (argc == 1) {
        return 0;
    }
    if (argc != 2) {
        printUsage(argv);
        return 1;
    }
    char* endPtr = nullptr;
    AUser1 = strtoul(argv[1], &endPtr, 0);
    if (endPtr == argv[1] || AUser1 == 0 || (AUser1 & 1) == 0) {
        std::cerr << "Error: A is not a valid positive, non-zero, odd integer!\n";
        printUsage(argv);
        return 2;
    }
    if (AUser1 > (1ull << 16)) {
        std::cerr << "Error: Given A is too large!\n";
        printUsage(argv);
        return 3;
    }
    return 0;
}

int main(
        int argc,
        char* argv[]) {
    const constexpr size_t numElements = 100001;
    const constexpr size_t iterations = 1;
    const constexpr size_t UNROLL_LO = 1;
    const constexpr size_t UNROLL_HI = 1024;

    uint32_t AUser = 64311ul;
    int result = checkArgs(argc, argv, AUser);
    if (result != 0) {
        return result;
    }
    _ReadWriteBarrier();
    uint32_t AUserInv = static_cast<uint32_t>(ext_euclidean<uint64_t>(AUser, 32));
    std::cout << "# A=" << AUser << " A^-1=" << AUserInv << std::endl;

    AlignedBlock bufRawdata16(numElements * sizeof(uint16_t), 64);
    AlignedBlock bufEncoded16(2 * numElements * sizeof(uint16_t), 64); // Coding may generate twice as much encoded output data as raw input data
    AlignedBlock bufResult16(2 * numElements * sizeof(uint16_t), 64); // Coding may generate twice as much encoded result data as raw input data (or the same amount as encoded data)

    AlignedBlock bufRawdata32(numElements * sizeof(uint32_t), 64);
    AlignedBlock bufEncoded32(2 * numElements * sizeof(uint32_t), 64); // Coding may generate twice as much encoded output data as raw input data
    AlignedBlock bufResult32(2 * numElements * sizeof(uint32_t), 64); // Coding may generate twice as much encoded result data as raw input data (or the same amount as encoded data)

    std::vector<std::vector<TestInfos>> vecTestInfos;

    TestConfiguration testConfig(iterations);
    DataGenerationConfiguration dataGenConfig;

    WarmUp<CopyTest16, UNROLL_LO, UNROLL_HI>("CopyTest16", "Copy", bufRawdata16, bufEncoded16, bufResult16, testConfig, dataGenConfig);

    std::clog << "# 16-bit Baseline (memcpy / memcmp) test:" << std::endl;
    TestCase<CopyTest16, UNROLL_LO, UNROLL_HI>("CopyTest16", "Copy", bufRawdata16, bufEncoded16, bufResult16, testConfig, dataGenConfig, vecTestInfos);
    size_t refIdx = vecTestInfos.size() - 1;

    // 16-bit data sequential tests
    std::clog << "# 16-bit scalar tests:" << std::endl;
    TestCase<XOR_scalar_16_16, UNROLL_LO, UNROLL_HI>("XOR_scalar_16_16", "XOR Scalar", bufRawdata16, bufEncoded16, bufResult16, testConfig, dataGenConfig, vecTestInfos, refIdx);
    TestCase<AN_scalar_16_32_u_divmod, UNROLL_LO, UNROLL_HI>("AN_scalar_16_32_u_divmod", "AN Scalar U DivMod", bufRawdata16, bufEncoded16, bufResult16, AUser, AUserInv, testConfig, dataGenConfig,
            vecTestInfos, refIdx);
    TestCase<AN_scalar_16_32_s_divmod, UNROLL_LO, UNROLL_HI>("AN_scalar_16_32_s_divmod", "AN Scalar S DivMod", bufRawdata16, bufEncoded16, bufResult16, AUser, AUserInv, testConfig, dataGenConfig,
            vecTestInfos, refIdx);
    TestCase<AN_scalar_16_32_u_inv, UNROLL_LO, UNROLL_HI>("AN_scalar_16_32_u_inv", "AN Scalar U Inv", bufRawdata16, bufEncoded16, bufResult16, AUser, AUserInv, testConfig, dataGenConfig, vecTestInfos,
            refIdx);
    TestCase<AN_scalar_16_32_s_inv, UNROLL_LO, UNROLL_HI>("AN_scalar_16_32_s_inv", "AN Scalar S Inv", bufRawdata16, bufEncoded16, bufResult16, AUser, AUserInv, testConfig, dataGenConfig, vecTestInfos,
            refIdx);
    TestCase<Hamming_scalar_16, UNROLL_LO, UNROLL_HI>("Hamming_scalar_16", "Hamming Scalar", bufRawdata16, bufEncoded16, bufResult16, testConfig, dataGenConfig, vecTestInfos, refIdx);

#ifdef __SSE4_2__
    // 16-bit data vectorized tests
    std::clog << "# 16-bit SSE4.2 tests:" << std::endl;
    TestCase<XOR_sse42_8x16_8x16, UNROLL_LO, UNROLL_HI>("XOR_sse42_8x16_8x16", "XOR SSE4.2", bufRawdata16, bufEncoded16, bufResult16, testConfig, dataGenConfig, vecTestInfos, refIdx);
    TestCase<AN_sse42_8x16_8x32_u_divmod, UNROLL_LO, UNROLL_HI>("AN_sse42_8x16_8x32_u_divmod", "AN SSE4.2 U DivMod", bufRawdata16, bufEncoded16, bufResult16, AUser, AUserInv, testConfig,
            dataGenConfig, vecTestInfos, refIdx);
    TestCase<AN_sse42_8x16_8x32_s_divmod, UNROLL_LO, UNROLL_HI>("AN_sse42_8x16_8x32_s_divmod", "AN SSE4.2 S DivMod", bufRawdata16, bufEncoded16, bufResult16, AUser, AUserInv, testConfig,
            dataGenConfig, vecTestInfos, refIdx);
    TestCase<AN_sse42_8x16_8x32_u_inv, UNROLL_LO, UNROLL_HI>("AN_sse42_8x16_8x32_u_inv", "AN SSE4.2 U Inv", bufRawdata16, bufEncoded16, bufResult16, AUser, AUserInv, testConfig, dataGenConfig,
            vecTestInfos, refIdx);
    TestCase<AN_sse42_8x16_8x32_s_inv, UNROLL_LO, UNROLL_HI>("AN_sse42_8x16_8x32_s_inv", "AN SSE4.2 S Inv", bufRawdata16, bufEncoded16, bufResult16, AUser, AUserInv, testConfig, dataGenConfig,
            vecTestInfos, refIdx);
    TestCase<Hamming_sse42_16, UNROLL_LO, UNROLL_HI>("Hamming_sse42_16", "Hamming SSE4.2", bufRawdata16, bufEncoded16, bufResult16, testConfig, dataGenConfig, vecTestInfos, refIdx);
#endif

#ifdef __AVX2__
    std::clog << "# 16-bit AVX2 tests:" << std::endl;
    TestCase<XOR_avx2_16x16_16x16, UNROLL_LO, UNROLL_HI>("XOR_avx2_16x16_16x16", "XOR AVX2", bufRawdata16, bufEncoded16, bufResult16, testConfig, dataGenConfig, vecTestInfos, refIdx);
    TestCase<AN_avx2_16x16_16x32_u_divmod, UNROLL_LO, UNROLL_HI>("AN_avx2_16x16_16x32_u_divmod", "AN AVX2 U DivMod", bufRawdata16, bufEncoded16, bufResult16, AUser, AUserInv, testConfig,
            dataGenConfig, vecTestInfos, refIdx);
    TestCase<AN_avx2_16x16_16x32_s_divmod, UNROLL_LO, UNROLL_HI>("AN_avx2_16x16_16x32_s_divmod", "AN AVX2 S DivMod", bufRawdata16, bufEncoded16, bufResult16, AUser, AUserInv, testConfig,
            dataGenConfig, vecTestInfos, refIdx);
    TestCase<AN_avx2_16x16_16x32_u_inv, UNROLL_LO, UNROLL_HI>("AN_avx2_16x16_16x32_u_inv", "AN AVX2 U Inv", bufRawdata16, bufEncoded16, bufResult16, AUser, AUserInv, testConfig, dataGenConfig,
            vecTestInfos, refIdx);
    TestCase<AN_avx2_16x16_16x32_s_inv, UNROLL_LO, UNROLL_HI>("AN_avx2_16x16_16x32_s_inv", "AN AVX2 S Inv", bufRawdata16, bufEncoded16, bufResult16, AUser, AUserInv, testConfig, dataGenConfig,
            vecTestInfos, refIdx);
    TestCase<Hamming_avx2_16, UNROLL_LO, UNROLL_HI>("Hamming_avx2_16", "Hamming AVX2", bufRawdata16, bufEncoded16, bufResult16, testConfig, dataGenConfig, vecTestInfos, refIdx);
#endif

#ifdef TEST32
    std::clog << "# 32-bit Baseline (memcpy / memcmp) test:" << std::endl;
    TestCase<CopyTest32, UNROLL_LO, UNROLL_HI>("CopyTest32", "Copy", bufRawdata32, bufEncoded32, bufResult32, testConfig, dataGenConfig, vecTestInfos, refIdx);
    refIdx = vecTestInfos.size() - 1;

    std::clog << "# 32-bit Scalar tests:" << std::endl;
    // 32-bit data sequential tests
    TestCase<XOR_seq_32_32, UNROLL_LO, UNROLL_HI>("XOR_seq_32_32", "XOR Scalar", bufRawdata32, bufEncoded32, bufResult32, testConfig, dataGenConfig, vecTestInfos, refIdx);
    TestCase<Hamming_seq_32, UNROLL_LO, UNROLL_HI>("Hamming_seq_32", "Hamming Scalar", bufRawdata32, bufEncoded32, bufResult32, testConfig, dataGenConfig, vecTestInfos, refIdx);

#ifdef __SSE4_2__
    std::clog << "# 32-bit SSE4.2 tests:" << std::endl;
    // 32-bit data vectorized tests
    TestCase<XOR_sse42_4x32_4x32, UNROLL_LO, UNROLL_HI>("XOR_sse42_4x32_4x32", "XOR SSE4.2", bufRawdata32, bufEncoded32, bufResult32, testConfig, dataGenConfig, vecTestInfos, refIdx);
    TestCase<Hamming_sse42_32, UNROLL_LO, UNROLL_HI>("Hamming_sse42_32", "Hamming SSE4.2", bufRawdata32, bufEncoded32, bufResult32, testConfig, dataGenConfig, vecTestInfos, refIdx);
#endif

#ifdef __AVX2__
    std::clog << "# 32-bit AVX2 tests:" << std::endl;
    TestCase<XOR_avx2_8x32_8x32, UNROLL_LO, UNROLL_HI>("XOR_avx2_8x32_8x32", "XOR AVX2", bufRawdata32, bufEncoded32, bufResult32, testConfig, dataGenConfig, vecTestInfos, refIdx);
    TestCase<Hamming_avx2_32, UNROLL_LO, UNROLL_HI>("Hamming_avx2_32", "Hamming AVX2", bufRawdata32, bufEncoded32, bufResult32, testConfig, dataGenConfig, vecTestInfos, refIdx);
#endif
#endif

    printResults<false>(vecTestInfos);
    std::cout << "\n\n";
    printResults<true>(vecTestInfos);
}
