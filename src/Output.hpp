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

#include "Util/TestInfo.hpp"

void printUsage(
        char* argv[]);

template<bool doRelative>
void printResults(
        std::vector<std::vector<TestInfos>> & results,
        bool renameFirst = true) {
    size_t maxPos = 0;
    for (auto & v : results) {
        maxPos = std::max(maxPos, v.size());
    }
    std::vector<double> baseEncode(maxPos);
    std::vector<double> baseCheck(maxPos);
    std::vector<double> baseArith(maxPos);
    std::vector<double> baseReencode(maxPos);
    std::vector<double> baseDecode(maxPos);
    std::vector<double> baseCheckAndDecode(maxPos);

    if (doRelative) {
        // copy results
        for (size_t i = 0; i < maxPos; ++i) {
            auto & r = results[0][i];
            baseEncode[i] = static_cast<double>(r.encode.nanos);
            baseCheck[i] = static_cast<double>(r.check.nanos);
            baseArith[i] = static_cast<double>(r.arithmetic.nanos);
            baseReencode[i] = static_cast<double>(r.reencode.nanos);
            baseDecode[i] = static_cast<double>(r.decode.nanos);
            baseCheckAndDecode[i] = static_cast<double>(r.checkAndDecode.nanos);
        }
    }

    // The following does pretty-print everything so that it can be easily used as input for gnuplot & co.

    // print headline
    std::cout << "unroll/block";
    // first all encode columns, then all check columns etc.size
    size_t i = 0;
    for (auto & v : results) {
        TestInfos &ti = v[0];
        if (ti.encode.isExecuted) {
            std::cout << ',' << (((i == 0) && renameFirst) ? "memcpy" : ti.name) << (((i == 0) && renameFirst) ? "" : " enc");
        }
        ++i;
    }
    i = 0;
    for (auto & v : results) {
        TestInfos &ti = v[0];
        if (ti.check.isExecuted) {
            std::cout << ',' << (((i == 0) && renameFirst) ? "memcmp" : ti.name) << (((i == 0) && renameFirst) ? "" : " check");
        }
        ++i;
    }
    i = 0;
    for (auto & v : results) {
        TestInfos &ti = v[0];
        if (ti.arithmetic.isExecuted) {
            std::cout << ',' << (((i == 0) && renameFirst) ? "memcpy" : ti.name) << (((i == 0) && renameFirst) ? "" : " arith");
        }
        ++i;
    }
    i = 0;
    for (auto & v : results) {
        TestInfos &ti = v[0];
        if (ti.reencode.isExecuted) {
            std::cout << ',' << (((i == 0) && renameFirst) ? "memcpy" : ti.name) << (((i == 0) && renameFirst) ? "" : " reenc");
        }
        ++i;
    }
    i = 0;
    for (auto & v : results) {
        TestInfos &ti = v[0];
        if (ti.decode.isExecuted) {
            std::cout << ',' << (((i == 0) && renameFirst) ? "memcpy" : ti.name) << (((i == 0) && renameFirst) ? "" : " dec");
        }
        ++i;
    }
    i = 0;
    for (auto & v : results) {
        TestInfos &ti = v[0];
        if (ti.checkAndDecode.isExecuted) {
            std::cout << ',' << (((i == 0) && renameFirst) ? "memcpy" : ti.name) << (((i == 0) && renameFirst) ? "" : " chkDec");
        }
        ++i;
    }
    std::cout << '\n';

    // print values, again first all encode columns, ...
    if (doRelative) {
        std::cout << std::fixed << std::setprecision(4);
    }
    for (size_t pos = 0, blocksize = 1; pos < maxPos; ++pos, blocksize *= 2) {
        std::cout << blocksize;
        for (auto & v : results) {
            TestInfos &ti = v[0];
            if (ti.encode.isExecuted) {
                std::cout << ',';
                if (pos < v.size() && v[pos].encode.error.empty()) {
                    if (doRelative)
                        std::cout << (static_cast<double>(v[pos].encode.nanos) / baseEncode[pos]);
                    else
                        std::cout << v[pos].encode.nanos;
                }
            }
        }
        for (auto & v : results) {
            TestInfos &ti = v[0];
            if (ti.check.isExecuted) {
                std::cout << ',';
                if (pos < v.size() && v[pos].check.error.empty()) {
                    if (doRelative)
                        std::cout << (static_cast<double>(v[pos].check.nanos) / baseCheck[pos]);
                    else
                        std::cout << v[pos].check.nanos;
                }
            }
        }
        for (auto & v : results) {
            TestInfos &ti = v[0];
            if (ti.arithmetic.isExecuted) {
                std::cout << ',';
                if (pos < v.size() && v[pos].arithmetic.error.empty()) {
                    if (doRelative)
                        std::cout << (static_cast<double>(v[pos].arithmetic.nanos) / baseArith[pos]);
                    else
                        std::cout << v[pos].arithmetic.nanos;
                }
            }
        }
        for (auto & v : results) {
            TestInfos &ti = v[0];
            if (ti.reencode.isExecuted) {
                std::cout << ',';
                if (pos < v.size() && v[pos].reencode.error.empty()) {
                    if (doRelative)
                        std::cout << (static_cast<double>(v[pos].reencode.nanos) / baseReencode[pos]);
                    else
                        std::cout << v[pos].reencode.nanos;
                }
            }
        }
        for (auto & v : results) {
            TestInfos &ti = v[0];
            if (ti.decode.isExecuted) {
                std::cout << ',';
                if (pos < v.size() && v[pos].decode.error.empty()) {
                    if (doRelative)
                        std::cout << (static_cast<double>(v[pos].decode.nanos) / baseDecode[pos]);
                    else
                        std::cout << v[pos].decode.nanos;
                }
            }
        }
        for (auto & v : results) {
            TestInfos &ti = v[0];
            if (ti.checkAndDecode.isExecuted) {
                std::cout << ',';
                if (pos < v.size() && v[pos].checkAndDecode.error.empty()) {
                    if (doRelative)
                        std::cout << (static_cast<double>(v[pos].checkAndDecode.nanos) / baseCheckAndDecode[pos]);
                    else
                        std::cout << v[pos].checkAndDecode.nanos;
                }
            }
        }
        std::cout << std::endl;
    }
}

extern template void printResults<true>(
        std::vector<std::vector<TestInfos>> & results,
        bool renameFirst);
extern template void printResults<false>(
        std::vector<std::vector<TestInfos>> & results,
        bool renameFirst);

#endif /* OUTPUT_HPP */
