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
    bool isAdd = results[0][0].add.isExecuted;
    bool isSub = results[0][0].sub.isExecuted;
    bool isMul = results[0][0].mul.isExecuted;
    bool isDiv = results[0][0].div.isExecuted;
    bool isAddChk = results[0][0].addChecked.isExecuted;
    bool isSubChk = results[0][0].subChecked.isExecuted;
    bool isMulChk = results[0][0].mulChecked.isExecuted;
    bool isDivChk = results[0][0].divChecked.isExecuted;
    bool isAggr = results[0][0].aggregate.isExecuted;
    bool isAggrChk = results[0][0].aggregateChecked.isExecuted;
    bool isReencChk = results[0][0].reencodeChecked.isExecuted;
    bool isDec = results[0][0].decode.isExecuted;
    bool isDecChk = results[0][0].decodeChecked.isExecuted;
    size_t numEnc = isEnc;
    size_t numCheck = isCheck;
    size_t numAdd = isAdd;
    size_t numSub = isSub;
    size_t numMul = isMul;
    size_t numDiv = isDiv;
    size_t numAddChk = isAddChk;
    size_t numSubChk = isSubChk;
    size_t numMulChk = isMulChk;
    size_t numDivChk = isDivChk;
    size_t numAggr = isAggr;
    size_t numAggrChk = isAggrChk;
    size_t numReencChk = isReencChk;
    size_t numDec = isDec;
    size_t numDecChk = isDecChk;
    for (size_t i = 1; i < numResults; ++i) {
        auto & ti = results[i][0];
        isEnc |= ti.encode.isExecuted;
        numEnc += ti.encode.isExecuted;
        isCheck |= ti.check.isExecuted;
        numCheck += ti.check.isExecuted;
        isAdd |= ti.add.isExecuted;
        isSub |= ti.sub.isExecuted;
        isMul |= ti.mul.isExecuted;
        isDiv |= ti.div.isExecuted;
        numAdd += ti.add.isExecuted;
        numSub += ti.sub.isExecuted;
        numMul += ti.mul.isExecuted;
        numDiv += ti.div.isExecuted;
        isAddChk |= ti.addChecked.isExecuted;
        isSubChk |= ti.subChecked.isExecuted;
        isMulChk |= ti.mulChecked.isExecuted;
        isDivChk |= ti.divChecked.isExecuted;
        numAddChk += ti.addChecked.isExecuted;
        numSubChk += ti.subChecked.isExecuted;
        numMulChk += ti.mulChecked.isExecuted;
        numDivChk += ti.divChecked.isExecuted;
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
        std::cout << '|' << std::setw(log10NumTests) << " enc";
    }
    if (isCheck) {
        ++num;
        std::cout << '|' << std::setw(log10NumTests) << " chk";
    }
    if (isAdd) {
        ++num;
        std::cout << '|' << std::setw(log10NumTests) << " add";
    }
    if (isSub) {
        ++num;
        std::cout << '|' << std::setw(log10NumTests) << " sub";
    }
    if (isMul) {
        ++num;
        std::cout << '|' << std::setw(log10NumTests) << " mul";
    }
    if (isDiv) {
        ++num;
        std::cout << '|' << std::setw(log10NumTests) << " div";
    }
    if (isAddChk) {
        ++num;
        std::cout << '|' << std::setw(log10NumTests) << "addC";
    }
    if (isSubChk) {
        ++num;
        std::cout << '|' << std::setw(log10NumTests) << "subC";
    }
    if (isMulChk) {
        ++num;
        std::cout << '|' << std::setw(log10NumTests) << "mulC";
    }
    if (isDivChk) {
        ++num;
        std::cout << '|' << std::setw(log10NumTests) << "divC";
    }
    if (isAggr) {
        ++num;
        std::cout << '|' << std::setw(log10NumTests) << "agg";
    }
    if (isAggrChk) {
        ++num;
        std::cout << '|' << std::setw(log10NumTests) << "aggC";
    }
    if (isReencChk) {
        ++num;
        std::cout << '|' << std::setw(log10NumTests) << "renC";
    }
    if (isDec) {
        ++num;
        std::cout << '|' << std::setw(log10NumTests) << " dec";
    }
    if (isDecChk) {
        ++num;
        std::cout << '|' << std::setw(log10NumTests) << "decC";
    }
    std::cout << "|\n#   " << std::setfill('-');
    for (size_t i = 0; i < num; ++i) {
        std::cout << ((i == 0) ? '|' : '+') << std::setw(log10NumTests) << '-';
    }
    std::cout << "|\n" << std::setfill(' ');
    size_t id = 1;
    size_t datawidth = 0;
    size_t incCheck = isEnc ? numResults : 0;
    size_t incAdd = incCheck + (isAdd ? numResults : 0);
    size_t incSub = incAdd + (isSub ? numResults : 0);
    size_t incMul = incSub + (isMul ? numResults : 0);
    size_t incDiv = incMul + (isDiv ? numResults : 0);
    size_t incAddChk = incDiv + (isAddChk ? numResults : 0);
    size_t incSubChk = incAddChk + (isSubChk ? numResults : 0);
    size_t incMulChk = incSubChk + (isMulChk ? numResults : 0);
    size_t incDivChk = incMulChk + (isDivChk ? numResults : 0);
    size_t incAggr = incDivChk + (isAggr ? numResults : 0);
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
        if (isAdd) {
            if (ti.add.isExecuted) {
                std::cout << '|' << std::setw(log10NumTests) << (id + incAdd);
            } else {
                std::cout << '|' << std::setw(log10NumTests) << ' ';
            }
        }
        if (isSub) {
            if (ti.sub.isExecuted) {
                std::cout << '|' << std::setw(log10NumTests) << (id + incSub);
            } else {
                std::cout << '|' << std::setw(log10NumTests) << ' ';
            }
        }
        if (isMul) {
            if (ti.mul.isExecuted) {
                std::cout << '|' << std::setw(log10NumTests) << (id + incMul);
            } else {
                std::cout << '|' << std::setw(log10NumTests) << ' ';
            }
        }
        if (isDiv) {
            if (ti.div.isExecuted) {
                std::cout << '|' << std::setw(log10NumTests) << (id + incDiv);
            } else {
                std::cout << '|' << std::setw(log10NumTests) << ' ';
            }
        }
        if (isAddChk) {
            if (ti.addChecked.isExecuted) {
                std::cout << '|' << std::setw(log10NumTests) << (id + incAddChk);
            } else {
                std::cout << '|' << std::setw(log10NumTests) << ' ';
            }
        }
        if (isSubChk) {
            if (ti.subChecked.isExecuted) {
                std::cout << '|' << std::setw(log10NumTests) << (id + incSubChk);
            } else {
                std::cout << '|' << std::setw(log10NumTests) << ' ';
            }
        }
        if (isMulChk) {
            if (ti.mulChecked.isExecuted) {
                std::cout << '|' << std::setw(log10NumTests) << (id + incMulChk);
            } else {
                std::cout << '|' << std::setw(log10NumTests) << ' ';
            }
        }
        if (isDivChk) {
            if (ti.divChecked.isExecuted) {
                std::cout << '|' << std::setw(log10NumTests) << (id + incDivChk);
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
        if (isAdd) {
            std::cout << ',' << (((i == 0) && config.doRenameFirst) ? "memcpy" : ti.name) << ((((i == 0) && config.doRenameFirst) || !config.doAppendTestMethod) ? "" : " add");
        }
        ++i;
    }
    i = 0;
    for (auto & v : results) {
        auto & ti = v[0];
        if (isSub) {
            std::cout << ',' << (((i == 0) && config.doRenameFirst) ? "memcpy" : ti.name) << ((((i == 0) && config.doRenameFirst) || !config.doAppendTestMethod) ? "" : " sub");
        }
        ++i;
    }
    i = 0;
    for (auto & v : results) {
        auto & ti = v[0];
        if (isMul) {
            std::cout << ',' << (((i == 0) && config.doRenameFirst) ? "memcpy" : ti.name) << ((((i == 0) && config.doRenameFirst) || !config.doAppendTestMethod) ? "" : " mul");
        }
        ++i;
    }
    i = 0;
    for (auto & v : results) {
        auto & ti = v[0];
        if (isDiv) {
            std::cout << ',' << (((i == 0) && config.doRenameFirst) ? "memcpy" : ti.name) << ((((i == 0) && config.doRenameFirst) || !config.doAppendTestMethod) ? "" : " div");
        }
        ++i;
    }
    i = 0;
    for (auto & v : results) {
        auto & ti = v[0];
        if (isAddChk) {
            std::cout << ',' << (((i == 0) && config.doRenameFirst) ? "memcmp+memcpy" : ti.name) << ((((i == 0) && config.doRenameFirst) || !config.doAppendTestMethod) ? "" : " addChk");
        }
        ++i;
    }
    i = 0;
    for (auto & v : results) {
        auto & ti = v[0];
        if (isSubChk) {
            std::cout << ',' << (((i == 0) && config.doRenameFirst) ? "memcmp+memcpy" : ti.name) << ((((i == 0) && config.doRenameFirst) || !config.doAppendTestMethod) ? "" : " subChk");
        }
        ++i;
    }
    i = 0;
    for (auto & v : results) {
        auto & ti = v[0];
        if (isMulChk) {
            std::cout << ',' << (((i == 0) && config.doRenameFirst) ? "memcmp+memcpy" : ti.name) << ((((i == 0) && config.doRenameFirst) || !config.doAppendTestMethod) ? "" : " mulChk");
        }
        ++i;
    }
    i = 0;
    for (auto & v : results) {
        auto & ti = v[0];
        if (isDivChk) {
            std::cout << ',' << (((i == 0) && config.doRenameFirst) ? "memcmp+memcpy" : ti.name) << ((((i == 0) && config.doRenameFirst) || !config.doAppendTestMethod) ? "" : " divChk");
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
                    auto & x = v[pos];
                    if (doRelative) {
                        if (x.reference) {
                            std::cout << (static_cast<double>(v[pos].encode.nanos) / x.reference->encode.nanos);
                        } else {
                            std::cout << 1.0L;
                        }
                    } else {
                        std::cout << v[pos].encode.nanos;
                    }
                }
            }
        }
        for (auto & v : results) {
            if (isCheck) {
                std::cout << ',';
                if (pos < v.size() && v[pos].check.isExecuted && v[pos].check.error.empty()) {
                    auto & x = v[pos];
                    if (doRelative) {
                        if (x.reference) {
                            std::cout << (static_cast<double>(v[pos].check.nanos) / x.reference->check.nanos);
                        } else {
                            std::cout << 1.0L;
                        }
                    } else {
                        std::cout << v[pos].check.nanos;
                    }
                }
            }
        }
        for (auto & v : results) {
            if (isAdd) {
                std::cout << ',';
                if (pos < v.size() && v[pos].add.isExecuted && v[pos].add.error.empty()) {
                    auto & x = v[pos];
                    if (doRelative) {
                        if (x.reference) {
                            std::cout << (static_cast<double>(v[pos].add.nanos) / x.reference->add.nanos);
                        } else {
                            std::cout << 1.0L;
                        }
                    } else {
                        std::cout << v[pos].add.nanos;
                    }
                }
            }
        }
        for (auto & v : results) {
            if (isSub) {
                std::cout << ',';
                if (pos < v.size() && v[pos].sub.isExecuted && v[pos].sub.error.empty()) {
                    auto & x = v[pos];
                    if (doRelative) {
                        if (x.reference) {
                            std::cout << (static_cast<double>(v[pos].sub.nanos) / x.reference->sub.nanos);
                        } else {
                            std::cout << 1.0L;
                        }
                    } else {
                        std::cout << v[pos].sub.nanos;
                    }
                }
            }
        }
        for (auto & v : results) {
            if (isMul) {
                std::cout << ',';
                if (pos < v.size() && v[pos].mul.isExecuted && v[pos].mul.error.empty()) {
                    auto & x = v[pos];
                    if (doRelative) {
                        if (x.reference) {
                            std::cout << (static_cast<double>(v[pos].mul.nanos) / x.reference->mul.nanos);
                        } else {
                            std::cout << 1.0L;
                        }
                    } else {
                        std::cout << v[pos].mul.nanos;
                    }
                }
            }
        }
        for (auto & v : results) {
            if (isDiv) {
                std::cout << ',';
                if (pos < v.size() && v[pos].div.isExecuted && v[pos].div.error.empty()) {
                    auto & x = v[pos];
                    if (doRelative) {
                        if (x.reference) {
                            std::cout << (static_cast<double>(v[pos].div.nanos) / x.reference->div.nanos);
                        } else {
                            std::cout << 1.0L;
                        }
                    } else {
                        std::cout << v[pos].div.nanos;
                    }
                }
            }
        }
        for (auto & v : results) {
            if (isAddChk) {
                std::cout << ',';
                if (pos < v.size() && v[pos].addChecked.isExecuted && v[pos].addChecked.error.empty()) {
                    auto & x = v[pos];
                    if (doRelative) {
                        if (x.reference) {
                            std::cout << (static_cast<double>(v[pos].addChecked.nanos) / x.reference->addChecked.nanos);
                        } else {
                            std::cout << 1.0L;
                        }
                    } else {
                        std::cout << v[pos].addChecked.nanos;
                    }
                }
            }
        }
        for (auto & v : results) {
            if (isSubChk) {
                std::cout << ',';
                if (pos < v.size() && v[pos].subChecked.isExecuted && v[pos].subChecked.error.empty()) {
                    auto & x = v[pos];
                    if (doRelative) {
                        if (x.reference) {
                            std::cout << (static_cast<double>(v[pos].subChecked.nanos) / x.reference->subChecked.nanos);
                        } else {
                            std::cout << 1.0L;
                        }
                    } else {
                        std::cout << v[pos].subChecked.nanos;
                    }
                }
            }
        }
        for (auto & v : results) {
            if (isMulChk) {
                std::cout << ',';
                if (pos < v.size() && v[pos].mulChecked.isExecuted && v[pos].mulChecked.error.empty()) {
                    auto & x = v[pos];
                    if (doRelative) {
                        if (x.reference) {
                            std::cout << (static_cast<double>(v[pos].mulChecked.nanos) / x.reference->mulChecked.nanos);
                        } else {
                            std::cout << 1.0L;
                        }
                    } else {
                        std::cout << v[pos].mulChecked.nanos;
                    }
                }
            }
        }
        for (auto & v : results) {
            if (isDivChk) {
                std::cout << ',';
                if (pos < v.size() && v[pos].divChecked.isExecuted && v[pos].divChecked.error.empty()) {
                    auto & x = v[pos];
                    if (doRelative) {
                        if (x.reference) {
                            std::cout << (static_cast<double>(v[pos].divChecked.nanos) / x.reference->divChecked.nanos);
                        } else {
                            std::cout << 1.0L;
                        }
                    } else {
                        std::cout << v[pos].divChecked.nanos;
                    }
                }
            }
        }
        for (auto & v : results) {
            if (isAggr) {
                std::cout << ',';
                if (pos < v.size() && v[pos].aggregate.isExecuted && v[pos].aggregate.error.empty()) {
                    auto & x = v[pos];
                    if (doRelative) {
                        if (x.reference) {
                            std::cout << (static_cast<double>(v[pos].aggregate.nanos) / x.reference->aggregate.nanos);
                        } else {
                            std::cout << 1.0L;
                        }
                    } else {
                        std::cout << v[pos].aggregate.nanos;
                    }
                }
            }
        }
        for (auto & v : results) {
            if (isAggrChk) {
                std::cout << ',';
                if (pos < v.size() && v[pos].aggregateChecked.isExecuted && v[pos].aggregateChecked.error.empty()) {
                    auto & x = v[pos];
                    if (doRelative) {
                        if (x.reference) {
                            std::cout << (static_cast<double>(v[pos].aggregateChecked.nanos) / x.reference->aggregateChecked.nanos);
                        } else {
                            std::cout << 1.0L;
                        }
                    } else {
                        std::cout << v[pos].aggregateChecked.nanos;
                    }
                }
            }
        }
        for (auto & v : results) {
            if (isReencChk) {
                std::cout << ',';
                if (pos < v.size() && v[pos].reencodeChecked.isExecuted && v[pos].reencodeChecked.error.empty()) {
                    auto & x = v[pos];
                    if (doRelative) {
                        if (x.reference) {
                            std::cout << (static_cast<double>(v[pos].reencodeChecked.nanos) / x.reference->reencodeChecked.nanos);
                        } else {
                            std::cout << 1.0L;
                        }
                    } else {
                        std::cout << v[pos].reencodeChecked.nanos;
                    }
                }
            }
        }
        for (auto & v : results) {
            if (isDec) {
                std::cout << ',';
                if (pos < v.size() && v[pos].decode.isExecuted && v[pos].decode.error.empty()) {
                    auto & x = v[pos];
                    if (doRelative) {
                        if (x.reference) {
                            std::cout << (static_cast<double>(v[pos].decode.nanos) / x.reference->decode.nanos);
                        } else {
                            std::cout << 1.0L;
                        }
                    } else {
                        std::cout << v[pos].decode.nanos;
                    }
                }
            }
        }
        for (auto & v : results) {
            if (isDecChk) {
                std::cout << ',';
                if (pos < v.size() && v[pos].decodeChecked.isExecuted && v[pos].decodeChecked.error.empty()) {
                    auto & x = v[pos];
                    if (doRelative) {
                        if (x.reference) {
                            std::cout << (static_cast<double>(v[pos].decodeChecked.nanos) / x.reference->decodeChecked.nanos);
                        } else {
                            std::cout << 1.0L;
                        }
                    } else {
                        std::cout << v[pos].decodeChecked.nanos;
                    }
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
