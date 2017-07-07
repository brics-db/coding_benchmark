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
 * File:   Output.cpp
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 06-07-2017 18:34
 */

#include <Output.hpp>

void printUsage(
        char* argv[]) {
    std::cerr << "Usage: " << argv[0] << " <A>\n" << std::setw(6) << "A" << ":   AN coding parameter (positive, odd, non-zero integer, 0 < A < 2^16)" << std::endl;
}

template void printResults<true>(
        std::vector<std::vector<TestInfos>> & results,
        bool renameFirst);

template void printResults<false>(
        std::vector<std::vector<TestInfos>> & results,
        bool renameFirst);
