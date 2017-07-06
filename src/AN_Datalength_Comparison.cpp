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

#include <Test.hpp>
#include <CopyTest.hpp>
#include <Output.hpp>

#include <AN/AN_seq_8_16_s_inv.hpp>
#include <AN/AN_seq_8_16_u_inv.hpp>
#include <AN/AN_seq_32_64_s_inv.hpp>
#include <AN/AN_seq_32_64_u_inv.hpp>

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
    const size_t iterations = 100;

    const size_t UNROLL_LO = 1;
    const size_t UNROLL_HI = 1024;

    uint32_t AUser = 233ul;
    int result = checkArgs(argc, argv, AUser);
    if (result != 0) {
        return result;
    }
    _ReadWriteBarrier();
    uint64_t AUserInv64 = ext_euclidean(static_cast<uint64_t>(AUser), 64);
    uint16_t AUserInv16 = ext_euclidean(AUser, 16);

    AlignedBlock input(rawDataSize, 64);
    AlignedBlock output(4 * rawDataSize, 64);
    std::vector<std::vector<TestInfos>> vecTestInfos;

#define WarmUp(type, name) \
    do { \
        std::cout << "# WarmUp " << #type << std::endl; \
        ExpandTest<type, UNROLL_LO, UNROLL_HI>::WarmUp(name, testConfig, dataGenConfig, input, output); \
    } while (0)

#define TestCase(...) VFUNC(TestCase, __VA_ARGS__)

#define TestCase2(type,name) \
    do { \
        std::cout << "# " << std::setw(4) << (vecTestInfos.size() + 2) <<  ": Testing " << #type << " (" << name << ")" << std::endl; \
        vecTestInfos.emplace_back(); \
        auto & vec = *vecTestInfos.rbegin(); \
        vec.reserve(ComputeNumRuns<UNROLL_LO, UNROLL_HI>()()); \
        ExpandTest<type, UNROLL_LO, UNROLL_HI>::Execute(vec, name, testConfig, dataGenConfig, input, output); \
    } while (0)

#define TestCase4(type,name,A,AInv) \
    do { \
        std::cout << "# " << std::setw(4) << (vecTestInfos.size() + 2) <<  ": Testing " << #type << " (" << name << " " << A << ")" << std::endl; \
        vecTestInfos.emplace_back(); \
        auto & vec = *vecTestInfos.rbegin(); \
        vec.reserve(ComputeNumRuns<UNROLL_LO, UNROLL_HI>()()); \
        ExpandTest<type, UNROLL_LO, UNROLL_HI>::Execute(vec, name, testConfig, dataGenConfig, input, output, A, AInv); \
    } while (0)

    TestConfiguration testConfig(iterations);
    DataGenerationConfiguration dataGenConfig(8);

    WarmUp(CopyTest, "Copy");

    TestCase(CopyTest, "Copy");

    TestCase(AN_seq_8_16_u_inv, "AN Seq U Inv 8>16", AUser, AUserInv16);
    TestCase(AN_seq_8_16_s_inv, "AN Seq S Inv 8>16", static_cast<int16_t>(AUser), static_cast<int16_t>(AUserInv16));
    TestCase(AN_seq_32_64_u_inv, "AN Seq U Inv 32>64", AUser, AUserInv64);
    TestCase(AN_seq_32_64_s_inv, "AN Seq S Inv 32>64", static_cast<int64_t>(AUser), static_cast<int64_t>(AUserInv64));

#undef WarmUp
#undef TestCase
#undef TestCase2
#undef TestCase4

    printResults<false>(vecTestInfos);
    std::cout << "\n\n";
    printResults<true>(vecTestInfos);
}
