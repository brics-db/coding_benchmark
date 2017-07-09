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

template<bool doRelative>
void printResults(
        std::vector<std::vector<TestInfos>> & results,
        bool renameFirst) {
    size_t maxPos = 0;
    for (auto & v : results) {
        maxPos = std::max(maxPos, v.size());
    }
    std::vector<double> baseEncode(maxPos);
    std::vector<double> baseCheck(maxPos);
    std::vector<double> baseArithmetic(maxPos);
    std::vector<double> baseArithmeticChecked(maxPos);
    std::vector<double> baseAggregate(maxPos);
    std::vector<double> baseAggregateChecked(maxPos);
    std::vector<double> baseReencodeChecked(maxPos);
    std::vector<double> baseDecode(maxPos);
    std::vector<double> baseDecodeChecked(maxPos);

    if (doRelative) {
        // copy results
        for (size_t i = 0; i < maxPos; ++i) {
            auto & r = results[0][i];
            baseEncode[i] = static_cast<double>(r.encode.nanos);
            baseCheck[i] = static_cast<double>(r.check.nanos);
            baseArithmetic[i] = static_cast<double>(r.arithmetic.nanos);
            baseArithmeticChecked[i] = static_cast<double>(r.arithmeticChecked.nanos);
            baseAggregate[i] = static_cast<double>(r.aggregate.nanos);
            baseAggregateChecked[i] = static_cast<double>(r.aggregateChecked.nanos);
            baseReencodeChecked[i] = static_cast<double>(r.reencodeChecked.nanos);
            baseDecode[i] = static_cast<double>(r.decode.nanos);
            baseDecodeChecked[i] = static_cast<double>(r.decodeChecked.nanos);
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
        if (ti.arithmeticChecked.isExecuted) {
            std::cout << ',' << (((i == 0) && renameFirst) ? "memcmp+memcpy" : ti.name) << (((i == 0) && renameFirst) ? "" : " arith");
        }
        ++i;
    }
    i = 0;
    for (auto & v : results) {
        TestInfos &ti = v[0];
        if (ti.aggregate.isExecuted) {
            std::cout << ',' << (((i == 0) && renameFirst) ? "memcpy" : ti.name) << (((i == 0) && renameFirst) ? "" : " arith");
        }
        ++i;
    }
    i = 0;
    for (auto & v : results) {
        TestInfos &ti = v[0];
        if (ti.aggregateChecked.isExecuted) {
            std::cout << ',' << (((i == 0) && renameFirst) ? "memcmp+memcpy" : ti.name) << (((i == 0) && renameFirst) ? "" : " arith");
        }
        ++i;
    }
    i = 0;
    for (auto & v : results) {
        TestInfos &ti = v[0];
        if (ti.reencodeChecked.isExecuted) {
            std::cout << ',' << (((i == 0) && renameFirst) ? "memcmp+memcpy" : ti.name) << (((i == 0) && renameFirst) ? "" : " reenc");
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
        if (ti.decodeChecked.isExecuted) {
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
                        std::cout << (static_cast<double>(v[pos].arithmetic.nanos) / baseArithmetic[pos]);
                    else
                        std::cout << v[pos].arithmetic.nanos;
                }
            }
        }
        for (auto & v : results) {
            TestInfos &ti = v[0];
            if (ti.arithmeticChecked.isExecuted) {
                std::cout << ',';
                if (pos < v.size() && v[pos].arithmeticChecked.error.empty()) {
                    if (doRelative)
                        std::cout << (static_cast<double>(v[pos].arithmetic.nanos) / baseArithmetic[pos]);
                    else
                        std::cout << v[pos].arithmeticChecked.nanos;
                }
            }
        }
        for (auto & v : results) {
            TestInfos &ti = v[0];
            if (ti.aggregate.isExecuted) {
                std::cout << ',';
                if (pos < v.size() && v[pos].aggregate.error.empty()) {
                    if (doRelative)
                        std::cout << (static_cast<double>(v[pos].aggregate.nanos) / baseArithmetic[pos]);
                    else
                        std::cout << v[pos].arithmetic.nanos;
                }
            }
        }
        for (auto & v : results) {
            TestInfos &ti = v[0];
            if (ti.aggregateChecked.isExecuted) {
                std::cout << ',';
                if (pos < v.size() && v[pos].aggregateChecked.error.empty()) {
                    if (doRelative)
                        std::cout << (static_cast<double>(v[pos].arithmetic.nanos) / baseArithmetic[pos]);
                    else
                        std::cout << v[pos].aggregateChecked.nanos;
                }
            }
        }
        for (auto & v : results) {
            TestInfos &ti = v[0];
            if (ti.reencodeChecked.isExecuted) {
                std::cout << ',';
                if (pos < v.size() && v[pos].reencodeChecked.error.empty()) {
                    if (doRelative)
                        std::cout << (static_cast<double>(v[pos].reencodeChecked.nanos) / baseReencodeChecked[pos]);
                    else
                        std::cout << v[pos].reencodeChecked.nanos;
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
            if (ti.decodeChecked.isExecuted) {
                std::cout << ',';
                if (pos < v.size() && v[pos].decodeChecked.error.empty()) {
                    if (doRelative)
                        std::cout << (static_cast<double>(v[pos].decodeChecked.nanos) / baseDecodeChecked[pos]);
                    else
                        std::cout << v[pos].decodeChecked.nanos;
                }
            }
        }
        std::cout << std::endl;
    }
}

template void printResults<true>(
        std::vector<std::vector<TestInfos>> & results,
        bool renameFirst);

template void printResults<false>(
        std::vector<std::vector<TestInfos>> & results,
        bool renameFirst);
