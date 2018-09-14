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
#include <XOR/XOR_simd.hpp>

#include <AN/AN_scalar.hpp>
#include <AN/AN_simd.hpp>

#include <Hamming/Hamming_scalar.hpp>
#include <Hamming/Hamming_simd.hpp>

#ifdef TESTCRC
#include <CRC/CRC_scalar.hpp>
#endif

using namespace coding_benchmark;

#ifdef TEST32
#include <boost/multiprecision/cpp_int.hpp>

using uint128_t = boost::multiprecision::uint128_t;
#endif

int checkArgs(
        int argc,
        char* argv[],
        size_t & AUser1) {
    if (argc == 1) {
        return 0;
    }
    if (argc != 2) {
        printUsage(argv);
        return 1;
    }
    char* endPtr = nullptr;
    AUser1 = strtoull(argv[1], &endPtr, 0);
    if (endPtr == argv[1] || AUser1 == 0 || (AUser1 & 1) == 0) {
        std::cerr << "Error: A is not a valid positive, non-zero, odd integer!\n";
        printUsage(argv);
        return 2;
    }
#if defined(TEST8)
    if (AUser1 >= (1ull << 8)) {
        std::cerr << "Error: Given A is too large! Must be at most 2^8-1\n";
        printUsage(argv);
        return 3;
    }
#elif defined(TEST16)
    if (AUser1 >= (1ull << 16)) {
        std::cerr << "Error: Given A is too large! Must be at most 2^16-1\n";
        printUsage(argv);
        return 3;
    }
#elif defined(TEST32)
    if (AUser1 >= (1ull << 32)) {
        std::cerr << "Error: Given A is too large! Must be at most 2^32-1\n";
        printUsage(argv);
        return 3;
    }
#else
#error "You must define (-D) exactly one of TEST8, TEST16, or TEST32"
#endif
    return 0;
}

int main(
        int argc,
        char* argv[]) {
    const constexpr size_t numElements = 1000001ull;
    const constexpr size_t iterations = 10000ull;
    const constexpr size_t UNROLL_LO = 1ull;
    const constexpr size_t UNROLL_HI = 1024ull;

    std::cout << "# numElements = " << numElements << '\n';
    std::cout << "# iterations = " << iterations << '\n';
    std::cout << "# UNROLL_LO = " << UNROLL_LO << '\n';
    std::cout << "# UNROLL_HI = " << UNROLL_HI << '\n';

    size_t AUser = 64311ull;
    int result = checkArgs(argc, argv, AUser);
    if (result != 0) {
        return result;
    }
    _ReadWriteBarrier();

    std::vector<std::vector<TestInfos>> vecTestInfos;

    TestConfiguration testConfig(iterations, numElements);
    // The following is for the reproducibility of the SIGMOD 2018 "AHEAD" paper, to reduce the amount of tests
    testConfig.disableAll();
    testConfig.enableCheck = true;
    testConfig.enableDecode = true;

#ifdef TEST8
    {
        uint16_t AUserInv = static_cast<uint16_t>(ext_euclidean<uint32_t>(static_cast<uint32_t>(AUser), 16));
        std::cout << "# A=" << AUser << " A^-1=" << AUserInv << std::endl;

        DataGenerationConfiguration dataGenConfig8(5, 3, 2, 0); /* TODO we do not check for overflows or underflows ANYWHERE, yet :-(*/
        AlignedBlock bufRawdata8(numElements * sizeof(uint8_t), 64);
        AlignedBlock bufEncoded8(3 * numElements * sizeof(uint8_t), 64); // Coding may generate thrice (since CRC) as much encoded output data as raw input data
        AlignedBlock bufResult8(3 * numElements * sizeof(uint8_t), 64);// Coding may generate thrice (since CRC) as much encoded result data as raw input data (or the same amount as encoded data)

        WarmUp<CopyTest8, UNROLL_LO, UNROLL_HI>("CopyTest8", "Copy", bufRawdata8, bufEncoded8, bufResult8, testConfig, dataGenConfig8);

        std::clog << "# 8-bit Baseline (memcpy / memcmp) test:" << std::endl;
        TestCase<CopyTest8, UNROLL_LO, UNROLL_HI>("CopyTest8", "Copy", bufRawdata8, bufEncoded8, bufResult8, testConfig, dataGenConfig8, vecTestInfos);
        size_t refIdx = vecTestInfos.size() - 1;

        // 8-bit data sequential tests
        std::clog << "# 8-bit scalar tests:" << std::endl;
        TestCase<XOR_scalar_8_8, UNROLL_LO, UNROLL_HI>("XOR_scalar_8_8", "XOR Scalar", bufRawdata8, bufEncoded8, bufResult8, testConfig, dataGenConfig8, vecTestInfos, refIdx);
        TestCase<AN_scalar_8_16_u_divmod, UNROLL_LO, UNROLL_HI>("AN_scalar_8_16_u_divmod", "AN Scalar U DivMod", bufRawdata8, bufEncoded8, bufResult8, AUser, AUserInv, testConfig, dataGenConfig8, vecTestInfos, refIdx);
        TestCase<AN_scalar_8_16_s_divmod, UNROLL_LO, UNROLL_HI>("AN_scalar_8_16_s_divmod", "AN Scalar S DivMod", bufRawdata8, bufEncoded8, bufResult8, AUser, AUserInv, testConfig, dataGenConfig8, vecTestInfos, refIdx);
        TestCase<AN_scalar_8_16_u_inv, UNROLL_LO, UNROLL_HI>("AN_scalar_8_16_u_inv", "AN Scalar U Inv", bufRawdata8, bufEncoded8, bufResult8, AUser, AUserInv, testConfig, dataGenConfig8, vecTestInfos, refIdx);
        TestCase<AN_scalar_8_16_s_inv, UNROLL_LO, UNROLL_HI>("AN_scalar_8_16_s_inv", "AN Scalar S Inv", bufRawdata8, bufEncoded8, bufResult8, AUser, AUserInv, testConfig, dataGenConfig8, vecTestInfos, refIdx);
        TestCase<Hamming_scalar_8, UNROLL_LO, UNROLL_HI>("Hamming_scalar_8", "Hamming Scalar", bufRawdata8, bufEncoded8, bufResult8, testConfig, dataGenConfig8, vecTestInfos, refIdx);
#ifdef TESTCRC
        TestCase<CRC16_scalar_8, UNROLL_LO, UNROLL_HI>("CRC16_scalar_8", "CRC16 Scalar", bufRawdata8, bufEncoded8, bufResult8, testConfig, dataGenConfig8, vecTestInfos, refIdx);
#endif

#ifdef __SSE4_2__
        // 8-bit data vectorized tests
        std::clog << "# 8-bit SSE4.2 tests:" << std::endl;
        TestCase<XOR_sse42_8x8_8x8, UNROLL_LO, UNROLL_HI>("XOR_sse42_16x8_16x8", "XOR SSE4.2", bufRawdata8, bufEncoded8, bufResult8, testConfig, dataGenConfig8, vecTestInfos, refIdx);
        TestCase<AN_sse42_8_16_u_divmod, UNROLL_LO, UNROLL_HI>("AN_sse42_8_16_u_divmod", "AN SSE4.2 U DivMod", bufRawdata8, bufEncoded8, bufResult8, AUser, AUserInv, testConfig, dataGenConfig8, vecTestInfos, refIdx);
        TestCase<AN_sse42_8_16_s_divmod, UNROLL_LO, UNROLL_HI>("AN_sse42_8_16_s_divmod", "AN SSE4.2 S DivMod", bufRawdata8, bufEncoded8, bufResult8, AUser, AUserInv, testConfig, dataGenConfig8, vecTestInfos, refIdx);
        TestCase<AN_sse42_8_16_u_inv, UNROLL_LO, UNROLL_HI>("AN_sse42_8_16_u_inv", "AN SSE4.2 U Inv", bufRawdata8, bufEncoded8, bufResult8, AUser, AUserInv, testConfig, dataGenConfig8, vecTestInfos, refIdx);
        TestCase<AN_sse42_8_16_s_inv, UNROLL_LO, UNROLL_HI>("AN_sse42_8_16_s_inv", "AN SSE4.2 S Inv", bufRawdata8, bufEncoded8, bufResult8, AUser, AUserInv, testConfig, dataGenConfig8, vecTestInfos, refIdx);
        TestCase<Hamming_sse42_8, UNROLL_LO, UNROLL_HI>("Hamming_sse42_8", "Hamming SSE4.2", bufRawdata8, bufEncoded8, bufResult8, testConfig, dataGenConfig8, vecTestInfos, refIdx);
#endif

#ifdef __AVX2__
        std::clog << "# 8-bit AVX2 tests:" << std::endl;
        TestCase<XOR_avx2_8x8_8x8, UNROLL_LO, UNROLL_HI>("XOR_avx2_32x8_32x8", "XOR AVX2", bufRawdata8, bufEncoded8, bufResult8, testConfig, dataGenConfig8, vecTestInfos, refIdx);
        TestCase<AN_avx2_8_16_u_divmod, UNROLL_LO, UNROLL_HI>("AN_avx2_8_16_u_divmod", "AN AVX2 U DivMod", bufRawdata8, bufEncoded8, bufResult8, AUser, AUserInv, testConfig, dataGenConfig8, vecTestInfos, refIdx);
        TestCase<AN_avx2_8_16_s_divmod, UNROLL_LO, UNROLL_HI>("AN_avx2_8_16_s_divmod", "AN AVX2 S DivMod", bufRawdata8, bufEncoded8, bufResult8, AUser, AUserInv, testConfig, dataGenConfig8, vecTestInfos, refIdx);
        TestCase<AN_avx2_8_16_u_inv, UNROLL_LO, UNROLL_HI>("AN_avx2_8_16_u_inv", "AN AVX2 U Inv", bufRawdata8, bufEncoded8, bufResult8, AUser, AUserInv, testConfig, dataGenConfig8, vecTestInfos, refIdx);
        TestCase<AN_avx2_8_16_s_inv, UNROLL_LO, UNROLL_HI>("AN_avx2_8_16_s_inv", "AN AVX2 S Inv", bufRawdata8, bufEncoded8, bufResult8, AUser, AUserInv, testConfig, dataGenConfig8, vecTestInfos, refIdx);
        TestCase<Hamming_avx2_8, UNROLL_LO, UNROLL_HI>("Hamming_avx2_8", "Hamming AVX2", bufRawdata8, bufEncoded8, bufResult8, testConfig, dataGenConfig8, vecTestInfos, refIdx);
#endif
    }
#endif /* TEST8 */

#ifdef TEST16
    {
        uint32_t AUserInv = static_cast<uint32_t>(ext_euclidean<uint64_t>(AUser, 32));
        std::cout << "# A=" << AUser << " A^-1=" << AUserInv << std::endl;

        DataGenerationConfiguration dataGenConfig16(11, 5, 4, 0); /* TODO we do not check for overflows or underflows ANYWHERE, yet :-(*/
        AlignedBlock bufRawdata16(numElements * sizeof(uint16_t), 64);
        AlignedBlock bufEncoded16(3 * numElements * sizeof(uint16_t), 64); // Coding may generate thrice (since CRC) as much encoded output data as raw input data
        AlignedBlock bufResult16(3 * numElements * sizeof(uint16_t), 64);// Coding may generate thrice (since CRC) as much encoded result data as raw input data (or the same amount as encoded data)

        WarmUp<CopyTest16, UNROLL_LO, UNROLL_HI>("CopyTest16", "Copy", bufRawdata16, bufEncoded16, bufResult16, testConfig, dataGenConfig16);

        std::clog << "# 16-bit Baseline (memcpy / memcmp) test:" << std::endl;
        TestCase<CopyTest16, UNROLL_LO, UNROLL_HI>("CopyTest16", "Copy", bufRawdata16, bufEncoded16, bufResult16, testConfig, dataGenConfig16, vecTestInfos);
        size_t refIdx = vecTestInfos.size() - 1;

        // 16-bit data sequential tests
        std::clog << "# 16-bit scalar tests:" << std::endl;
        TestCase<XOR_scalar_16_16, UNROLL_LO, UNROLL_HI>("XOR_scalar_16_16", "XOR Scalar", bufRawdata16, bufEncoded16, bufResult16, testConfig, dataGenConfig16, vecTestInfos, refIdx);
        TestCase<AN_scalar_16_32_u_divmod, UNROLL_LO, UNROLL_HI>("AN_scalar_16_32_u_divmod", "AN Scalar U DivMod", bufRawdata16, bufEncoded16, bufResult16, AUser, AUserInv, testConfig, dataGenConfig16, vecTestInfos, refIdx);
        TestCase<AN_scalar_16_32_s_divmod, UNROLL_LO, UNROLL_HI>("AN_scalar_16_32_s_divmod", "AN Scalar S DivMod", bufRawdata16, bufEncoded16, bufResult16, AUser, AUserInv, testConfig, dataGenConfig16, vecTestInfos, refIdx);
        TestCase<AN_scalar_16_32_u_inv, UNROLL_LO, UNROLL_HI>("AN_scalar_16_32_u_inv", "AN Scalar U Inv", bufRawdata16, bufEncoded16, bufResult16, AUser, AUserInv, testConfig, dataGenConfig16, vecTestInfos, refIdx);
        TestCase<AN_scalar_16_32_s_inv, UNROLL_LO, UNROLL_HI>("AN_scalar_16_32_s_inv", "AN Scalar S Inv", bufRawdata16, bufEncoded16, bufResult16, AUser, AUserInv, testConfig, dataGenConfig16, vecTestInfos, refIdx);
        TestCase<Hamming_scalar_16, UNROLL_LO, UNROLL_HI>("Hamming_scalar_16", "Hamming Scalar", bufRawdata16, bufEncoded16, bufResult16, testConfig, dataGenConfig16, vecTestInfos, refIdx);
#ifdef TESTCRC
        TestCase<CRC32_scalar_16, UNROLL_LO, UNROLL_HI>("CRC32_scalar_16", "CRC32 Scalar", bufRawdata16, bufEncoded16, bufResult16, testConfig, dataGenConfig16, vecTestInfos, refIdx);
#endif

#ifdef __SSE4_2__
        // 16-bit data vectorized tests
        std::clog << "# 16-bit SSE4.2 tests:" << std::endl;
        TestCase<XOR_sse42_8x16_8x16, UNROLL_LO, UNROLL_HI>("XOR_sse42_8x16_8x16", "XOR SSE4.2", bufRawdata16, bufEncoded16, bufResult16, testConfig, dataGenConfig16, vecTestInfos, refIdx);
        TestCase<AN_sse42_16_32_u_divmod, UNROLL_LO, UNROLL_HI>("AN_sse42_16_32_u_divmod", "AN SSE4.2 U DivMod", bufRawdata16, bufEncoded16, bufResult16, AUser, AUserInv, testConfig, dataGenConfig16, vecTestInfos, refIdx);
        TestCase<AN_sse42_16_32_s_divmod, UNROLL_LO, UNROLL_HI>("AN_sse42_16_32_s_divmod", "AN SSE4.2 S DivMod", bufRawdata16, bufEncoded16, bufResult16, AUser, AUserInv, testConfig, dataGenConfig16, vecTestInfos, refIdx);
        TestCase<AN_sse42_16_32_u_inv, UNROLL_LO, UNROLL_HI>("AN_sse42_16_32_u_inv", "AN SSE4.2 U Inv", bufRawdata16, bufEncoded16, bufResult16, AUser, AUserInv, testConfig, dataGenConfig16, vecTestInfos, refIdx);
        TestCase<AN_sse42_16_32_s_inv, UNROLL_LO, UNROLL_HI>("AN_sse42_16_32_s_inv", "AN SSE4.2 S Inv", bufRawdata16, bufEncoded16, bufResult16, AUser, AUserInv, testConfig, dataGenConfig16, vecTestInfos, refIdx);
        TestCase<Hamming_sse42_16, UNROLL_LO, UNROLL_HI>("Hamming_sse42_16", "Hamming SSE4.2", bufRawdata16, bufEncoded16, bufResult16, testConfig, dataGenConfig16, vecTestInfos, refIdx);
#endif

#ifdef __AVX2__
        std::clog << "# 16-bit AVX2 tests:" << std::endl;
        TestCase<XOR_avx2_16x16_16x16, UNROLL_LO, UNROLL_HI>("XOR_avx2_16x16_16x16", "XOR AVX2", bufRawdata16, bufEncoded16, bufResult16, testConfig, dataGenConfig16, vecTestInfos, refIdx);
        TestCase<AN_avx2_16_32_u_divmod, UNROLL_LO, UNROLL_HI>("AN_avx2_16_32_u_divmod", "AN AVX2 U DivMod", bufRawdata16, bufEncoded16, bufResult16, AUser, AUserInv, testConfig, dataGenConfig16, vecTestInfos, refIdx);
        TestCase<AN_avx2_16_32_s_divmod, UNROLL_LO, UNROLL_HI>("AN_avx2_16_32_s_divmod", "AN AVX2 S DivMod", bufRawdata16, bufEncoded16, bufResult16, AUser, AUserInv, testConfig, dataGenConfig16, vecTestInfos, refIdx);
        TestCase<AN_avx2_16_32_u_inv, UNROLL_LO, UNROLL_HI>("AN_avx2_16_32_u_inv", "AN AVX2 U Inv", bufRawdata16, bufEncoded16, bufResult16, AUser, AUserInv, testConfig, dataGenConfig16, vecTestInfos, refIdx);
        TestCase<AN_avx2_16_32_s_inv, UNROLL_LO, UNROLL_HI>("AN_avx2_16_32_s_inv", "AN AVX2 S Inv", bufRawdata16, bufEncoded16, bufResult16, AUser, AUserInv, testConfig, dataGenConfig16, vecTestInfos, refIdx);
        TestCase<Hamming_avx2_16, UNROLL_LO, UNROLL_HI>("Hamming_avx2_16", "Hamming AVX2", bufRawdata16, bufEncoded16, bufResult16, testConfig, dataGenConfig16, vecTestInfos, refIdx);
#endif
    }
#endif /* TEST16 */

#ifdef TEST32
    {
        uint64_t AUserInv = static_cast<uint64_t>(ext_euclidean<uint128_t>(static_cast<uint128_t>(AUser), 64));
        std::cout << "# A=" << AUser << " A^-1=" << AUserInv << std::endl;

        DataGenerationConfiguration dataGenConfig32(26, 6, 5, 0); /* TODO we do not check for overflows or underflows ANYWHERE, yet :-(*/
        AlignedBlock bufRawdata32(numElements * sizeof(uint32_t), 64);
        AlignedBlock bufEncoded32(3 * numElements * sizeof(uint32_t), 64); // Coding may generate twice as much encoded output data as raw input data
        AlignedBlock bufResult32(3 * numElements * sizeof(uint32_t), 64);// Coding may generate twice as much encoded result data as raw input data (or the same amount as encoded data)

        WarmUp<CopyTest32, UNROLL_LO, UNROLL_HI>("CopyTest32", "Copy", bufRawdata32, bufEncoded32, bufResult32, testConfig, dataGenConfig32);

        std::clog << "# 32-bit Baseline (memcpy / memcmp) test:" << std::endl;
        TestCase<CopyTest32, UNROLL_LO, UNROLL_HI>("CopyTest32", "Copy", bufRawdata32, bufEncoded32, bufResult32, testConfig, dataGenConfig32, vecTestInfos);
        size_t refIdx = vecTestInfos.size() - 1;

        std::clog << "# 32-bit Scalar tests:" << std::endl;
        // 32-bit data sequential tests
        TestCase<XOR_scalar_32_32, UNROLL_LO, UNROLL_HI>("XOR_scalar_32_32", "XOR Scalar", bufRawdata32, bufEncoded32, bufResult32, testConfig, dataGenConfig32, vecTestInfos, refIdx);
        TestCase<AN_scalar_32_64_u_divmod, UNROLL_LO, UNROLL_HI>("AN_scalar_32_64_u_divmod", "AN Scalar U DivMod", bufRawdata32, bufEncoded32, bufResult32, AUser, AUserInv, testConfig, dataGenConfig32, vecTestInfos, refIdx);
        TestCase<AN_scalar_32_64_s_divmod, UNROLL_LO, UNROLL_HI>("AN_scalar_32_64_s_divmod", "AN Scalar S DivMod", bufRawdata32, bufEncoded32, bufResult32, AUser, AUserInv, testConfig, dataGenConfig32, vecTestInfos, refIdx);
        TestCase<AN_scalar_32_64_u_inv, UNROLL_LO, UNROLL_HI>("AN_scalar_32_64_u_inv", "AN Scalar U Inv", bufRawdata32, bufEncoded32, bufResult32, AUser, AUserInv, testConfig, dataGenConfig32, vecTestInfos, refIdx);
        TestCase<AN_scalar_32_64_s_inv, UNROLL_LO, UNROLL_HI>("AN_scalar_32_64_s_inv", "AN Scalar S Inv", bufRawdata32, bufEncoded32, bufResult32, AUser, AUserInv, testConfig, dataGenConfig32, vecTestInfos, refIdx);
        TestCase<Hamming_scalar_32, UNROLL_LO, UNROLL_HI>("Hamming_scalar_32", "Hamming Scalar", bufRawdata32, bufEncoded32, bufResult32, testConfig, dataGenConfig32, vecTestInfos, refIdx);
#ifdef TESTCRC
        TestCase<CRC32_scalar_32, UNROLL_LO, UNROLL_HI>("CRC32_scalar_32", "CRC32 Scalar", bufRawdata32, bufEncoded32, bufResult32, testConfig, dataGenConfig32, vecTestInfos, refIdx);
#endif

#ifdef __SSE4_2__
        std::clog << "# 32-bit SSE4.2 tests:" << std::endl;
        // 32-bit data vectorized tests
        TestCase<XOR_sse42_4x32_4x32, UNROLL_LO, UNROLL_HI>("XOR_sse42_4x32_4x32", "XOR SSE4.2", bufRawdata32, bufEncoded32, bufResult32, testConfig, dataGenConfig32, vecTestInfos, refIdx);
        TestCase<AN_sse42_32_64_u_divmod, UNROLL_LO, UNROLL_HI>("AN_sse42_32_64_u_divmod", "AN SSE4.2 U DivMod", bufRawdata32, bufEncoded32, bufResult32, AUser, AUserInv, testConfig, dataGenConfig32, vecTestInfos, refIdx);
        TestCase<AN_sse42_32_64_s_divmod, UNROLL_LO, UNROLL_HI>("AN_sse42_32_64_s_divmod", "AN SSE4.2 S DivMod", bufRawdata32, bufEncoded32, bufResult32, AUser, AUserInv, testConfig, dataGenConfig32, vecTestInfos, refIdx);
        TestCase<AN_sse42_32_64_u_inv, UNROLL_LO, UNROLL_HI>("AN_sse42_32_64_u_inv", "AN SSE4.2 U Inv", bufRawdata32, bufEncoded32, bufResult32, AUser, AUserInv, testConfig, dataGenConfig32, vecTestInfos, refIdx);
        TestCase<AN_sse42_32_64_s_inv, UNROLL_LO, UNROLL_HI>("AN_sse42_32_64_s_inv", "AN SSE4.2 S Inv", bufRawdata32, bufEncoded32, bufResult32, AUser, AUserInv, testConfig, dataGenConfig32, vecTestInfos, refIdx);
        TestCase<Hamming_sse42_32, UNROLL_LO, UNROLL_HI>("Hamming_sse42_32", "Hamming SSE4.2", bufRawdata32, bufEncoded32, bufResult32, testConfig, dataGenConfig32, vecTestInfos, refIdx);
#endif

#ifdef __AVX2__
        std::clog << "# 32-bit AVX2 tests:" << std::endl;
        TestCase<XOR_avx2_8x32_8x32, UNROLL_LO, UNROLL_HI>("XOR_avx2_8x32_8x32", "XOR AVX2", bufRawdata32, bufEncoded32, bufResult32, testConfig, dataGenConfig32, vecTestInfos, refIdx);
        TestCase<AN_avx2_32_64_u_divmod, UNROLL_LO, UNROLL_HI>("AN_avx2_32_64_u_divmod", "AN AVX2 U DivMod", bufRawdata32, bufEncoded32, bufResult32, AUser, AUserInv, testConfig, dataGenConfig32, vecTestInfos, refIdx);
        TestCase<AN_avx2_32_64_s_divmod, UNROLL_LO, UNROLL_HI>("AN_avx2_32_64_s_divmod", "AN AVX2 S DivMod", bufRawdata32, bufEncoded32, bufResult32, AUser, AUserInv, testConfig, dataGenConfig32, vecTestInfos, refIdx);
        TestCase<AN_avx2_32_64_u_inv, UNROLL_LO, UNROLL_HI>("AN_avx2_32_64_u_inv", "AN AVX2 U Inv", bufRawdata32, bufEncoded32, bufResult32, AUser, AUserInv, testConfig, dataGenConfig32, vecTestInfos, refIdx);
        TestCase<AN_avx2_32_64_s_inv, UNROLL_LO, UNROLL_HI>("AN_avx2_32_64_s_inv", "AN AVX2 S Inv", bufRawdata32, bufEncoded32, bufResult32, AUser, AUserInv, testConfig, dataGenConfig32, vecTestInfos, refIdx);
        TestCase<Hamming_avx2_32, UNROLL_LO, UNROLL_HI>("Hamming_avx2_32", "Hamming AVX2", bufRawdata32, bufEncoded32, bufResult32, testConfig, dataGenConfig32, vecTestInfos, refIdx);
#endif
    }
#endif /* TEST32 */

    printResults<false>(testConfig, vecTestInfos);
    std::cout << "\n\n";
    printResults<true>(testConfig, vecTestInfos);
}
