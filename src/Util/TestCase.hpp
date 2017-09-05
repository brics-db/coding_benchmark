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
 * TestCase.hpp
 *
 *  Created on: 05.09.2017
 *      Author: tk4
 */

#pragma once

#include <cstdint>
#include <vector>
#include <iomanip>

#include <Test.hpp>
#include <Util/ComputeNumRuns.hpp>
#include <Util/ExpandTest.hpp>

template<template<size_t BlockSize> class TestType, size_t UNROLL_LO, size_t UNROLL_HI>
void WarmUp(
        const char* const pzsType,
        const char* const pzsName,
        AlignedBlock & bufRawdata,
        AlignedBlock & bufEncoded,
        AlignedBlock & bufResult,
        TestConfiguration & testConfig,
        DataGenerationConfiguration & dataGenConfig) {
    std::clog << "# WarmUp " << pzsType << std::endl;
    ExpandTest<TestType, UNROLL_LO, UNROLL_HI>::WarmUp(pzsName, testConfig, dataGenConfig, bufRawdata, bufEncoded, bufResult);
    std::clog << '#' << std::endl;
}

template<template<size_t BlockSize> class TestType, size_t UNROLL_LO, size_t UNROLL_HI>
void TestCase(
        const char* const pzsType,
        const char* const pzsName,
        AlignedBlock & bufRawdata,
        AlignedBlock & bufEncoded,
        AlignedBlock & bufResult,
        TestConfiguration & testConfig,
        DataGenerationConfiguration & dataGenConfig,
        std::vector<std::vector<TestInfos>> & vecTestInfos) {
    std::clog << "# " << std::setw(4) << (vecTestInfos.size() + 2) << ": Testing " << pzsType << " (" << pzsName << ")" << std::endl;
    vecTestInfos.emplace_back();
    auto & vec = *vecTestInfos.rbegin();
    vec.reserve(ComputeNumRuns<UNROLL_LO, UNROLL_HI>::value);
    ExpandTest<TestType, UNROLL_LO, UNROLL_HI>::Execute(vec, pzsName, testConfig, dataGenConfig, bufRawdata, bufEncoded, bufResult);
    std::clog << '#' << std::endl;
}

template<template<size_t BlockSize> class TestType, size_t UNROLL_LO, size_t UNROLL_HI>
void TestCase(
        const char* const pzsType,
        const char* const pzsName,
        AlignedBlock & bufRawdata,
        AlignedBlock & bufEncoded,
        AlignedBlock & bufResult,
        TestConfiguration & testConfig,
        DataGenerationConfiguration & dataGenConfig,
        std::vector<std::vector<TestInfos>> & vecTestInfos,
        const size_t refInfosIndex) {
    TestCase<TestType, UNROLL_LO, UNROLL_HI>(pzsType, pzsName, bufRawdata, bufEncoded, bufResult, testConfig, dataGenConfig, vecTestInfos);
    setTestInfosReference(*vecTestInfos.rbegin(), vecTestInfos.at(refInfosIndex));
}

template<template<size_t BlockSize> class TestType, size_t UNROLL_LO, size_t UNROLL_HI>
void TestCase(
        const char* const pzsType,
        const char* const pzsName,
        AlignedBlock & bufRawdata,
        AlignedBlock & bufEncoded,
        AlignedBlock & bufResult,
        size_t A,
        size_t AInv,
        TestConfiguration & testConfig,
        DataGenerationConfiguration & dataGenConfig,
        std::vector<std::vector<TestInfos>> & vecTestInfos) {
    std::clog << "# " << std::setw(4) << (vecTestInfos.size() + 2) << ": Testing " << pzsType << " (" << pzsName << ")" << std::endl;
    vecTestInfos.emplace_back();
    auto & vec = *vecTestInfos.rbegin();
    vec.reserve(ComputeNumRuns<UNROLL_LO, UNROLL_HI>::value);
    ExpandTest<TestType, UNROLL_LO, UNROLL_HI>::Execute(vec, pzsName, testConfig, dataGenConfig, bufRawdata, bufEncoded, bufResult, A, AInv);
    std::clog << '#' << std::endl;
}

template<template<size_t BlockSize> class TestType, size_t UNROLL_LO, size_t UNROLL_HI>
void TestCase(
        const char* const pzsType,
        const char* const pzsName,
        AlignedBlock & bufRawdata,
        AlignedBlock & bufEncoded,
        AlignedBlock & bufResult,
        size_t A,
        size_t AInv,
        TestConfiguration & testConfig,
        DataGenerationConfiguration & dataGenConfig,
        std::vector<std::vector<TestInfos>> & vecTestInfos,
        const size_t refInfosIndex) {
    TestCase<TestType, UNROLL_LO, UNROLL_HI>(pzsType, pzsName, bufRawdata, bufEncoded, bufResult, A, AInv, testConfig, dataGenConfig, vecTestInfos);
    setTestInfosReference(*vecTestInfos.rbegin(), vecTestInfos.at(refInfosIndex));
}
