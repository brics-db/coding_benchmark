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

#include "Main.hpp"
#include "Output.hpp"

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
    const size_t rawDataSize = 1024 * 1024; // size in BYTES
    const size_t iterations = 10;

    const size_t UNROLL_LO = 1;
    const size_t UNROLL_HI = 1024;

    uint32_t AUser = 64311ul;
    int result = checkArgs(argc, argv, AUser);
    if (result != 0) {
        return result;
    }
    _ReadWriteBarrier();
    uint32_t AUserInv = ext_euclidean(AUser, 32);

    AlignedBlock input(rawDataSize, 64);
    AlignedBlock output(2 * rawDataSize, 64); // AN coding generates twice as much output data as input data
    std::vector<std::vector<TestInfos>> vecTestInfos;

#define WarmUp(type, name) \
    do { \
        std::clog << "# WarmUp " << #type << std::endl; \
        ExpandTest<type, UNROLL_LO, UNROLL_HI>::WarmUp(name, testConfig, dataGenConfig, input, output); \
        std::clog << std::endl; \
    } while (0)

#define TestCase(...) VFUNC(TestCase, __VA_ARGS__)

#define TestCase2(type,name) \
    do { \
        std::clog << "# " << std::setw(4) << (vecTestInfos.size() + 2) <<  ": Testing " << #type << " (" << name << ")" << std::endl; \
        vecTestInfos.emplace_back(); \
        auto & vec = *vecTestInfos.rbegin(); \
        vec.reserve(ComputeNumRuns<UNROLL_LO, UNROLL_HI>::value); \
        ExpandTest<type, UNROLL_LO, UNROLL_HI>::Execute(vec, name, testConfig, dataGenConfig, input, output); \
        std::clog << '#' << std::endl; \
    } while (0)

#define TestCase4(type,name,A,AInv) \
    do { \
        std::clog << "# " << std::setw(4) << (vecTestInfos.size() + 2) <<  ": Testing " << #type << " (" << name << " " << A << ")" << std::endl; \
        vecTestInfos.emplace_back(); \
        auto & vec = *vecTestInfos.rbegin(); \
        vec.reserve(ComputeNumRuns<UNROLL_LO, UNROLL_HI>::value); \
        ExpandTest<type, UNROLL_LO, UNROLL_HI>::Execute(vec, name, testConfig, dataGenConfig, input, output, A, AInv); \
        std::clog << '#' << std::endl; \
    } while (0)

    TestConfiguration testConfig(iterations);
    DataGenerationConfiguration dataGenConfig;

    WarmUp(CopyTest, "Copy");

    TestCase(CopyTest, "Copy");

    // 16-bit data sequential tests
    std::cout << "# 16-bit scalar tests:" << std::endl;
    std::clog << "# 16-bit scalar tests:" << std::endl;
    TestCase(XOR_seq_16_16, "XOR Seq");
    TestCase(AN_seq_16_32_u_divmod, "AN Seq U DivMod", AUser, AUserInv);
    TestCase(AN_seq_16_32_s_divmod, "AN Seq S DivMod", (static_cast<int32_t>(AUser)), (static_cast<int32_t>(AUserInv)));
    TestCase(AN_seq_16_32_u_inv, "AN Seq U Inv", AUser, AUserInv);
    TestCase(AN_seq_16_32_s_inv, "AN Seq S Inv", static_cast<int32_t>(AUser), static_cast<int32_t>(AUserInv));
    TestCase(Hamming_seq_16, "Hamming Seq");

    // 16-bit data vectorized tests
    std::cout << "# 16-bit SSE4.2 tests:" << std::endl;
    std::clog << "# 16-bit SSE4.2 tests:" << std::endl;
    TestCase(XOR_sse42_8x16_8x16, "XOR SSE4.2");
    TestCase(AN_sse42_8x16_8x32_u_divmod, "AN SSE4.2 U DivMod", AUser, AUserInv);
    TestCase(AN_sse42_8x16_8x32_s_divmod, "AN SSE4.2 S DivMod", static_cast<int32_t>(AUser), static_cast<int32_t>(AUserInv));
    TestCase(AN_sse42_8x16_8x32_u_inv, "AN SSE4.2 U Inv", AUser, AUserInv);
    TestCase(AN_sse42_8x16_8x32_s_inv, "AN SSE4.2 S Inv", static_cast<int32_t>(AUser), static_cast<int32_t>(AUserInv));
    TestCase(Hamming_sse42_16, "Hamming SSE4.2");

#ifdef __AVX2__
    std::cout << "# 16-bit AVX2 tests:" << std::endl;
    std::clog << "# 16-bit AVX2 tests:" << std::endl;
    TestCase(XOR_avx2_16x16_16x16, "XOR AVX2");
    TestCase(AN_avx2_16x16_16x32_u_divmod, "AN AVX2 U DivMod", AUser, AUserInv);
    TestCase(AN_avx2_16x16_16x32_s_divmod, "AN AVX2 S DivMod", static_cast<int32_t>(AUser), static_cast<int32_t>(AUserInv));
    TestCase(AN_avx2_16x16_16x32_u_inv, "AN AVX2 U Inv", AUser, AUserInv);
    TestCase(AN_avx2_16x16_16x32_s_inv, "AN AVX2 S Inv", static_cast<int32_t>(AUser), static_cast<int32_t>(AUserInv));
    TestCase(Hamming_avx2_16, "Hamming AVX2");
#endif

#ifdef TEST32
    std::cout << "# 32-bit scalar tests:" << std::endl;
    std::clog << "# 32-bit scalar tests:" << std::endl;
    // 32-bit data sequential tests
    TestCase(XOR_seq_32_32, "XOR Seq");
    TestCase(Hamming_seq_32, "Hamming Seq");

    std::cout << "# 32-bit SSE4.2 tests:" << std::endl;
    std::clog << "# 32-bit SSE4.2 tests:" << std::endl;
    // 32-bit data vectorized tests
    TestCase(XOR_sse42_4x32_4x32, "XOR SSE4.2");
    TestCase(Hamming_sse42_32, "Hamming SSE4.2");

#ifdef __AVX2__
    std::cout << "# 32-bit AVX2 tests:" << std::endl;
    std::clog << "# 32-bit AVX2 tests:" << std::endl;
    TestCase(XOR_avx2_8x32_8x32, "XOR AVX2");
    TestCase(Hamming_avx2_32, "Hamming AVX2");
#endif
#endif

#undef WarmUp
#undef TestCase
#undef TestCase2
#undef TestCase4

    printResults<false>(vecTestInfos);
    std::cout << "\n\n";
    printResults<true>(vecTestInfos);
}
