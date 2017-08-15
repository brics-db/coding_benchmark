// Copyright (c) 2016 Till Kolditz
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
 * File:   ExpandTest.hpp
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 17. Februar 2017, 12:46
 */

#ifndef EXPANDTEST_HPP
#define EXPANDTEST_HPP

#include <iostream>

#include <Test.hpp>
#include <Util/AlignedBlock.hpp>

template<template<size_t BlockSize> class TestType, size_t start, size_t end>
struct ExpandTest {

    static void WarmUp(
            const char* const name,
            const TestConfiguration & testConfig,
            const DataGenerationConfiguration & dataGenConfig,
            AlignedBlock & bufRaw,
            AlignedBlock & bufEncoded,
            AlignedBlock & bufResult) {
        std::clog << "#  * " << start << ": " << std::flush;
        TestType<start>(name, bufRaw, bufEncoded, bufResult).Execute(testConfig, dataGenConfig);
        std::clog << std::endl;
        ExpandTest<TestType, start * 2, end>::WarmUp(name, testConfig, dataGenConfig, bufRaw, bufEncoded, bufResult);
    }

    template<typename ... ArgTypes>
    static void Execute(
            std::vector<TestInfos> & vecTestInfos,
            const char* const name,
            const TestConfiguration & testConfig,
            const DataGenerationConfiguration & dataGenConfig,
            AlignedBlock & bufRaw,
            AlignedBlock & bufEncoded,
            AlignedBlock & bufResult,
            ArgTypes && ... args) {
        std::clog << "#      * " << start << ": " << std::flush;
        vecTestInfos.push_back(TestType<start>(name, bufRaw, bufEncoded, bufResult, std::forward<ArgTypes>(args)...).Execute(testConfig, dataGenConfig));
        std::clog << std::endl;
        ExpandTest<TestType, start * 2, end>::Execute(vecTestInfos, name, testConfig, dataGenConfig, bufRaw, bufEncoded, bufResult, std::forward<ArgTypes>(args)...);
    }
};

template<template<size_t BlockSize> class TestType, size_t start>
struct ExpandTest<TestType, start, start> {

    static void WarmUp(
            const char* const name,
            const TestConfiguration & testConfig,
            const DataGenerationConfiguration & dataGenConfig,
            AlignedBlock & bufRaw,
            AlignedBlock & bufEncoded,
            AlignedBlock & bufResult) {
        std::clog << "#  * " << start << ": " << std::flush;
        TestType<start>(name, bufRaw, bufEncoded, bufResult).Execute(testConfig, dataGenConfig);
        std::clog << std::endl;
    }

    template<typename ... ArgTypes>
    static void Execute(
            std::vector<TestInfos> & vecTestInfos,
            const char* const name,
            const TestConfiguration & testConfig,
            const DataGenerationConfiguration & dataGenConfig,
            AlignedBlock & bufRaw,
            AlignedBlock & bufEncoded,
            AlignedBlock & bufResult,
            ArgTypes && ... args) {
        std::clog << "#      * " << start << ": " << std::flush;
        vecTestInfos.push_back(TestType<start>(name, bufRaw, bufEncoded, bufResult, std::forward<ArgTypes>(args)...).Execute(testConfig, dataGenConfig));
        std::clog << std::endl;
    }
};

#endif /* EXPANDTEST_HPP */
