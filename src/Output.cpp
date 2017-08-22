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
        OutputConfiguration config) {
    size_t numResults = results.size();
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

    // print names and IDs of tests for easier gnuplot script changes
    size_t log10NumTests = 0;
    for (size_t x = numResults; x; ++log10NumTests) {
        x /= 10;
    }
    if (log10NumTests < 3) {
        log10NumTests = 3;
    }
    std::string simd = results[0][0].simd;
    bool isEnc = results[0][0].encode.isExecuted;
    bool isCheck = results[0][0].check.isExecuted;
    bool isArith = results[0][0].arithmetic.isExecuted;
    bool isArithChk = results[0][0].arithmeticChecked.isExecuted;
    bool isAggr = results[0][0].aggregate.isExecuted;
    bool isAggrChk = results[0][0].aggregateChecked.isExecuted;
    bool isReencChk = results[0][0].reencodeChecked.isExecuted;
    bool isDec = results[0][0].decode.isExecuted;
    bool isDecChk = results[0][0].decodeChecked.isExecuted;
    size_t numEnc = isEnc, numCheck = isCheck, numArith = isArith, numArithChk = isArithChk, numAggr = isAggr, numAggrChk = isAggrChk, numReencChk = isReencChk, numDec = isDec, numDecChk = isDecChk;
    for (size_t i = 1; i < numResults; ++i) {
        auto & ti = results[i][0];
        isEnc |= ti.encode.isExecuted;
        numEnc += ti.encode.isExecuted;
        isCheck |= ti.check.isExecuted;
        numCheck += ti.check.isExecuted;
        isArith |= ti.arithmetic.isExecuted;
        numArith += ti.arithmetic.isExecuted;
        isArithChk |= ti.arithmeticChecked.isExecuted;
        numArithChk += ti.arithmeticChecked.isExecuted;
        isAggr |= ti.aggregate.isExecuted;
        numAggr += ti.aggregate.isExecuted;
        isAggrChk |= ti.aggregateChecked.isExecuted;
        numAggrChk += ti.aggregateChecked.isExecuted;
        isReencChk |= ti.reencodeChecked.isExecuted;
        numReencChk += ti.reencodeChecked.isExecuted;
        isDec |= ti.decode.isExecuted;
        numDec += ti.decode.isExecuted;
        isDecChk |= ti.decodeChecked.isExecuted;
        numDecChk += ti.decodeChecked.isExecuted;
    }
    // headline
    std::cout << "#   ";
    size_t num = 0;
    if (isEnc) {
        ++num;
        std::cout << '|' << std::setw(log10NumTests) << "enc";
    }
    if (isCheck) {
        ++num;
        std::cout << '|' << std::setw(log10NumTests) << "chk";
    }
    if (isArith) {
        ++num;
        std::cout << '|' << std::setw(log10NumTests) << "ari";
    }
    if (isArithChk) {
        ++num;
        std::cout << '|' << std::setw(log10NumTests) << "arC";
    }
    if (isAggr) {
        ++num;
        std::cout << '|' << std::setw(log10NumTests) << "agg";
    }
    if (isAggrChk) {
        ++num;
        std::cout << '|' << std::setw(log10NumTests) << "agC";
    }
    if (isReencChk) {
        ++num;
        std::cout << '|' << std::setw(log10NumTests) << "reC";
    }
    if (isDec) {
        ++num;
        std::cout << '|' << std::setw(log10NumTests) << "dec";
    }
    if (isDecChk) {
        ++num;
        std::cout << '|' << std::setw(log10NumTests) << "deC";
    }
    std::cout << "|\n#   " << std::setfill('-');
    for (size_t i = 0; i < num; ++i) {
        std::cout << ((i == 0) ? '|' : '+') << std::setw(log10NumTests) << '-';
    }
    std::cout << "|\n" << std::setfill(' ');
    size_t id = 1;
    size_t datawidth = 0;
    size_t incCheck = isEnc ? numResults : 0;
    size_t incArith = incCheck + (isCheck ? numResults : 0);
    size_t incArithChk = incArith + (isArithChk ? numResults : 0);
    size_t incAggr = incArithChk + (isAggr ? numResults : 0);
    size_t incAggrChk = incAggr + (isAggrChk ? numResults : 0);
    size_t incReencChk = incAggrChk + (isReencChk ? numResults : 0);
    size_t incDec = incReencChk + (isDec ? numResults : 0);
    size_t incDecChk = incDec + (isDecChk ? numResults : 0);
    for (auto & v : results) {
        ++id;
        auto & ti = v[0];
        bool isDifferentDWorSIMD = (datawidth != ti.datawidth) || (simd != ti.simd);
        if (isDifferentDWorSIMD) {
            datawidth = ti.datawidth;
            simd = v[0].simd;
            std::cout << "# " << (v[0].datawidth * 8) << "-bit " << simd << ":\n";
        }
        std::cout << "#   ";
        if (isEnc) {
            if (ti.encode.isExecuted) {
                std::cout << '|' << std::setw(log10NumTests) << id;
            } else {
                std::cout << '|' << std::setw(log10NumTests) << ' ';
            }
        }
        if (isCheck) {
            if (ti.check.isExecuted) {
                std::cout << '|' << std::setw(log10NumTests) << (id + incCheck);
            } else {
                std::cout << '|' << std::setw(log10NumTests) << ' ';
            }
        }
        if (isArith) {
            if (ti.arithmetic.isExecuted) {
                std::cout << '|' << std::setw(log10NumTests) << (id + incArith);
            } else {
                std::cout << '|' << std::setw(log10NumTests) << ' ';
            }
        }
        if (isArithChk) {
            if (ti.arithmeticChecked.isExecuted) {
                std::cout << '|' << std::setw(log10NumTests) << (id + incArithChk);
            } else {
                std::cout << '|' << std::setw(log10NumTests) << ' ';
            }
        }
        if (isAggr) {
            if (ti.aggregate.isExecuted) {
                std::cout << '|' << std::setw(log10NumTests) << (id + incAggr);
            } else {
                std::cout << '|' << std::setw(log10NumTests) << ' ';
            }
        }
        if (isAggrChk) {
            if (ti.aggregateChecked.isExecuted) {
                std::cout << '|' << std::setw(log10NumTests) << (id + incAggrChk);
            } else {
                std::cout << '|' << std::setw(log10NumTests) << ' ';
            }
        }
        if (isReencChk) {
            if (ti.reencodeChecked.isExecuted) {
                std::cout << '|' << std::setw(log10NumTests) << (id + incReencChk);
            } else {
                std::cout << '|' << std::setw(log10NumTests) << ' ';
            }
        }
        if (isDec) {
            if (ti.decode.isExecuted) {
                std::cout << '|' << std::setw(log10NumTests) << (id + incDec);
            } else {
                std::cout << '|' << std::setw(log10NumTests) << ' ';
            }
        }
        if (isDecChk) {
            if (ti.decodeChecked.isExecuted) {
                std::cout << '|' << std::setw(log10NumTests) << (id + incDecChk);
            } else {
                std::cout << '|' << std::setw(log10NumTests) << ' ';
            }
        }
        std::cout << "|   " << v[0].name << '\n';
    }

    // print headline
    std::cout << "unroll/block";
    // first all encode columns, then all check columns etc.size
    size_t i = 0;
    for (auto & v : results) {
        auto & ti = v[0];
        if (isEnc) {
            std::cout << ',' << (((i == 0) && config.doRenameFirst) ? "memcpy" : ti.name) << ((((i == 0) && config.doRenameFirst) || !config.doAppendTestMethod) ? "" : " enc");
        }
        ++i;
    }
    i = 0;
    for (auto & v : results) {
        auto & ti = v[0];
        if (isCheck) {
            std::cout << ',' << (((i == 0) && config.doRenameFirst) ? "memcmp" : ti.name) << ((((i == 0) && config.doRenameFirst) || !config.doAppendTestMethod) ? "" : " check");
        }
        ++i;
    }
    i = 0;
    for (auto & v : results) {
        auto & ti = v[0];
        if (isArith) {
            std::cout << ',' << (((i == 0) && config.doRenameFirst) ? "memcpy" : ti.name) << ((((i == 0) && config.doRenameFirst) || !config.doAppendTestMethod) ? "" : " arith");
        }
        ++i;
    }
    i = 0;
    for (auto & v : results) {
        auto & ti = v[0];
        if (isArithChk) {
            std::cout << ',' << (((i == 0) && config.doRenameFirst) ? "memcmp+memcpy" : ti.name) << ((((i == 0) && config.doRenameFirst) || !config.doAppendTestMethod) ? "" : " arithChk");
        }
        ++i;
    }
    i = 0;
    for (auto & v : results) {
        auto & ti = v[0];
        if (isAggr) {
            std::cout << ',' << (((i == 0) && config.doRenameFirst) ? "memcpy" : ti.name) << ((((i == 0) && config.doRenameFirst) || !config.doAppendTestMethod) ? "" : " aggr");
        }
        ++i;
    }
    i = 0;
    for (auto & v : results) {
        auto & ti = v[0];
        if (isAggrChk) {
            std::cout << ',' << (((i == 0) && config.doRenameFirst) ? "memcmp+memcpy" : ti.name) << ((((i == 0) && config.doRenameFirst) || !config.doAppendTestMethod) ? "" : " aggrChk");
        }
        ++i;
    }
    i = 0;
    for (auto & v : results) {
        auto & ti = v[0];
        if (isReencChk) {
            std::cout << ',' << (((i == 0) && config.doRenameFirst) ? "memcmp+memcpy" : ti.name) << ((((i == 0) && config.doRenameFirst) || !config.doAppendTestMethod) ? "" : " reencChk");
        }
        ++i;
    }
    i = 0;
    for (auto & v : results) {
        auto & ti = v[0];
        if (isDec) {
            std::cout << ',' << (((i == 0) && config.doRenameFirst) ? "memcpy" : ti.name) << ((((i == 0) && config.doRenameFirst) || !config.doAppendTestMethod) ? "" : " dec");
        }
        ++i;
    }
    i = 0;
    for (auto & v : results) {
        auto & ti = v[0];
        if (isDecChk) {
            std::cout << ',' << (((i == 0) && config.doRenameFirst) ? "memcpy" : ti.name) << ((((i == 0) && config.doRenameFirst) || !config.doAppendTestMethod) ? "" : " decChk");
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
            if (isEnc) {
                std::cout << ',';
                if (pos < v.size() && v[pos].encode.isExecuted && v[pos].encode.error.empty()) {
                    if (doRelative)
                        std::cout << (static_cast<double>(v[pos].encode.nanos) / baseEncode[pos]);
                    else
                        std::cout << v[pos].encode.nanos;
                }
            }
        }
        for (auto & v : results) {
            if (isCheck) {
                std::cout << ',';
                if (pos < v.size() && v[pos].check.isExecuted && v[pos].check.error.empty()) {
                    if (doRelative)
                        std::cout << (static_cast<double>(v[pos].check.nanos) / baseCheck[pos]);
                    else
                        std::cout << v[pos].check.nanos;
                }
            }
        }
        for (auto & v : results) {
            if (isArith) {
                std::cout << ',';
                if (pos < v.size() && v[pos].arithmetic.isExecuted && v[pos].arithmetic.error.empty()) {
                    if (doRelative)
                        std::cout << (static_cast<double>(v[pos].arithmetic.nanos) / baseArithmetic[pos]);
                    else
                        std::cout << v[pos].arithmetic.nanos;
                }
            }
        }
        for (auto & v : results) {
            if (isArithChk) {
                std::cout << ',';
                if (pos < v.size() && v[pos].arithmeticChecked.isExecuted && v[pos].arithmeticChecked.error.empty()) {
                    if (doRelative)
                        std::cout << (static_cast<double>(v[pos].arithmeticChecked.nanos) / baseArithmeticChecked[pos]);
                    else
                        std::cout << v[pos].arithmeticChecked.nanos;
                }
            }
        }
        for (auto & v : results) {
            if (isAggr) {
                std::cout << ',';
                if (pos < v.size() && v[pos].aggregate.isExecuted && v[pos].aggregate.error.empty()) {
                    if (doRelative)
                        std::cout << (static_cast<double>(v[pos].aggregate.nanos) / baseAggregate[pos]);
                    else
                        std::cout << v[pos].aggregate.nanos;
                }
            }
        }
        for (auto & v : results) {
            if (isAggrChk) {
                std::cout << ',';
                if (pos < v.size() && v[pos].aggregateChecked.isExecuted && v[pos].aggregateChecked.error.empty()) {
                    if (doRelative)
                        std::cout << (static_cast<double>(v[pos].aggregateChecked.nanos) / baseAggregateChecked[pos]);
                    else
                        std::cout << v[pos].aggregateChecked.nanos;
                }
            }
        }
        for (auto & v : results) {
            if (isReencChk) {
                std::cout << ',';
                if (pos < v.size() && v[pos].reencodeChecked.isExecuted && v[pos].reencodeChecked.error.empty()) {
                    if (doRelative)
                        std::cout << (static_cast<double>(v[pos].reencodeChecked.nanos) / baseReencodeChecked[pos]);
                    else
                        std::cout << v[pos].reencodeChecked.nanos;
                }
            }
        }
        for (auto & v : results) {
            if (isDec) {
                std::cout << ',';
                if (pos < v.size() && v[pos].decode.isExecuted && v[pos].decode.error.empty()) {
                    if (doRelative)
                        std::cout << (static_cast<double>(v[pos].decode.nanos) / baseDecode[pos]);
                    else
                        std::cout << v[pos].decode.nanos;
                }
            }
        }
        for (auto & v : results) {
            if (isDecChk) {
                std::cout << ',';
                if (pos < v.size() && v[pos].decodeChecked.isExecuted && v[pos].decodeChecked.error.empty()) {
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
        OutputConfiguration config);

template void printResults<false>(
        std::vector<std::vector<TestInfos>> & results,
        OutputConfiguration config);
