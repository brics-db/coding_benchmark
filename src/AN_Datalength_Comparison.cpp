/*
 * AN_Datalength_Comparison.cpp
 *
 *  Created on: 06.07.2017
 *      Author: Till Kolditz <till.kolditz@gmail.com>
 */

#include <string>
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>

#ifdef _MSC_VER
// disable stupid diamond-inheritance warnings (the compiler does not see that there is only a single implementation for each of the functions)
#pragma warning(disable: 4250)
#endif

#include <Util/AlignedBlock.hpp>
#include <Util/TestInfo.hpp>
#include <Util/ErrorInfo.hpp>
#include <Util/VFunc.hpp>
#include <Util/Euclidean.hpp>
#include <Util/Intrinsics.hpp>
#include <Util/ComputeNumRuns.hpp>
#include <Util/ExpandTest.hpp>
#include <Util/TestCase.hpp>
#include <Util/Test.hpp>
#include <Util/Output.hpp>

#include <Copy/CopyTest.hpp>
#include <Copy/CopyTest2.hpp>

#include <AN/AN_scalar.hpp>

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
    const constexpr size_t numElements = 1000001;
    const constexpr size_t iterations = 10;
    const constexpr size_t UNROLL_LO = 1;
    const constexpr size_t UNROLL_HI = 1024;

    std::cout << "# numElements = " << numElements << '\n';
    std::cout << "# iterations = " << iterations << '\n';
    std::cout << "# UNROLL_LO = " << UNROLL_LO << '\n';
    std::cout << "# UNROLL_HI = " << UNROLL_HI << '\n';

    uint32_t AUser = 233ul;
    int result = checkArgs(argc, argv, AUser);
    if (result != 0) {
        return result;
    }
    _ReadWriteBarrier();
    uint64_t AUserInv64 = ext_euclidean(static_cast<uint64_t>(AUser), 64);
    uint16_t AUserInv16 = ext_euclidean(AUser, 16);

    AlignedBlock input8(numElements * sizeof(uint8_t), 64);
    AlignedBlock encoded8(numElements * sizeof(uint16_t), 64);
    AlignedBlock result8(numElements * sizeof(uint16_t), 64);

    AlignedBlock input16(numElements * sizeof(uint16_t), 64);
    AlignedBlock encoded16(numElements * sizeof(uint32_t), 64);
    AlignedBlock result16(numElements * sizeof(uint32_t), 64);

    AlignedBlock input32(numElements * sizeof(uint32_t), 64);
    AlignedBlock encoded32(numElements * sizeof(uint64_t), 64);
    AlignedBlock result32(numElements * sizeof(uint64_t), 64);

    std::vector<std::vector<TestInfos>> vecTestInfos;

    TestConfiguration testConfig(iterations);
    DataGenerationConfiguration dataGenConfig(8);

    WarmUp<CopyTest8, UNROLL_LO, UNROLL_HI>("CopyTest8", "Copy 8>8", input8, encoded8, result8, testConfig, dataGenConfig);
    WarmUp<CopyTest16, UNROLL_LO, UNROLL_HI>("CopyTest16", "Copy 16>16", input16, encoded16, result16, testConfig, dataGenConfig);
    WarmUp<CopyTest32, UNROLL_LO, UNROLL_HI>("CopyTest32", "Copy 32>32", input32, encoded32, result32, testConfig, dataGenConfig);
    WarmUp<CopyTest2_8_16, UNROLL_LO, UNROLL_HI>("CopyTest2_8_16", "Copy2 8>16", input8, encoded8, result8, testConfig, dataGenConfig);
    WarmUp<CopyTest2_32_64, UNROLL_LO, UNROLL_HI>("CopyTest2_32_64", "Copy2 32>64", input32, encoded32, result32, testConfig, dataGenConfig);

    TestCase<CopyTest8, UNROLL_LO, UNROLL_HI>("CopyTest8", "Copy 8>8", input8, encoded8, result8, testConfig, dataGenConfig, vecTestInfos);
    size_t refIdx = vecTestInfos.size() - 1;
    TestCase<CopyTest2_8_16, UNROLL_LO, UNROLL_HI>("CopyTest2_8_16", "Copy2 8>16", input8, encoded8, result8, testConfig, dataGenConfig, vecTestInfos, refIdx);
    TestCase<AN_scalar_8_16_u_inv, UNROLL_LO, UNROLL_HI>("AN_scalar_8_16_u_inv", "AN U 8>16", input8, encoded8, result8, AUser, AUserInv16, testConfig, dataGenConfig, vecTestInfos, refIdx);
    TestCase<AN_scalar_8_16_s_inv, UNROLL_LO, UNROLL_HI>("AN_scalar_8_16_s_inv", "AN S 8>16", input8, encoded8, result8, AUser, AUserInv16, testConfig, dataGenConfig, vecTestInfos, refIdx);

    TestCase<CopyTest32, UNROLL_LO, UNROLL_HI>("CopyTest32", "Copy 32>32", input32, encoded32, result32, testConfig, dataGenConfig, vecTestInfos);
    refIdx = vecTestInfos.size() - 1;
    TestCase<CopyTest2_32_64, UNROLL_LO, UNROLL_HI>("CopyTest2_32_64", "Copy2 32>64", input32, encoded32, result32, testConfig, dataGenConfig, vecTestInfos, refIdx);
    TestCase<AN_scalar_32_64_u_inv, UNROLL_LO, UNROLL_HI>("AN_scalar_32_64_u_inv", "AN U 32>64", input32, encoded32, result32, AUser, AUserInv64, testConfig, dataGenConfig, vecTestInfos, refIdx);
    TestCase<AN_scalar_32_64_s_inv, UNROLL_LO, UNROLL_HI>("AN_scalar_32_64_s_inv", "AN S 32>64", input32, encoded32, result32, AUser, AUserInv64, testConfig, dataGenConfig, vecTestInfos, refIdx);

    printResults<false>(vecTestInfos, OutputConfiguration(false, true));
    std::cout << "\n\n";
    printResults<true>(vecTestInfos, OutputConfiguration(false, true));
}
