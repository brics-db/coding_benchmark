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
 * File:   Output.hpp
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 17. Februar 2017, 12:48
 */

#ifndef OUTPUT_HPP
#define OUTPUT_HPP

#include <iostream>
#include <iomanip>
#include <vector>

#include <Util/TestInfo.hpp>
#include <Util/TestConfiguration.hpp>

struct OutputConfiguration {
    bool doRenameFirst;
    bool doAppendTestMethod;
    bool doPrintCountNumbersColumn;

    OutputConfiguration()
            : doRenameFirst(true),
              doAppendTestMethod(true),
              doPrintCountNumbersColumn(true) {
    }

    OutputConfiguration(
            bool doRenameFirst,
            bool doAppendTestMethod,
            bool doPrintCountNumbersColumn)
            : doRenameFirst(doRenameFirst),
              doAppendTestMethod(doAppendTestMethod),
              doPrintCountNumbersColumn(doPrintCountNumbersColumn) {
    }
};

void printUsage(
        char* argv[]);

template<bool doRelative>
void printResults(
        TestConfiguration testConfig,
        std::vector<std::vector<TestInfos>> & results,
        OutputConfiguration config = OutputConfiguration());

extern template void printResults<true>(
        TestConfiguration testConfig,
        std::vector<std::vector<TestInfos>> & results,
        OutputConfiguration config);
extern template void printResults<false>(
        TestConfiguration testConfig,
        std::vector<std::vector<TestInfos>> & results,
        OutputConfiguration config);

#endif /* OUTPUT_HPP */
