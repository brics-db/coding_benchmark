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

#include <Util/Output.hpp>

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
    if (log10NumTests < 5) {
        log10NumTests = 5;
    }
    std::string simd = results[0][0].simd;
    bool isEnc = results[0][0].encode.isExecuted;
    bool isChk = results[0][0].check.isExecuted;
    bool isAdd = results[0][0].add.isExecuted;
    bool isSub = results[0][0].sub.isExecuted;
    bool isMul = results[0][0].mul.isExecuted;
    bool isDiv = results[0][0].div.isExecuted;
    bool isAddChk = results[0][0].addChecked.isExecuted;
    bool isSubChk = results[0][0].subChecked.isExecuted;
    bool isMulChk = results[0][0].mulChecked.isExecuted;
    bool isDivChk = results[0][0].divChecked.isExecuted;
    bool isSum = results[0][0].sum.isExecuted;
    bool isMin = results[0][0].min.isExecuted;
    bool isMax = results[0][0].max.isExecuted;
    bool isAvg = results[0][0].avg.isExecuted;
    bool isSumChk = results[0][0].sumChecked.isExecuted;
    bool isMinChk = results[0][0].minChecked.isExecuted;
    bool isMaxChk = results[0][0].maxChecked.isExecuted;
    bool isAvgChk = results[0][0].avgChecked.isExecuted;
    bool isReencChk = results[0][0].reencodeChecked.isExecuted;
    bool isDec = results[0][0].decode.isExecuted;
    bool isDecChk = results[0][0].decodeChecked.isExecuted;
    size_t numEnc = isEnc;
    size_t numChk = isChk;
    size_t numAdd = isAdd;
    size_t numSub = isSub;
    size_t numMul = isMul;
    size_t numDiv = isDiv;
    size_t numAddChk = isAddChk;
    size_t numSubChk = isSubChk;
    size_t numMulChk = isMulChk;
    size_t numDivChk = isDivChk;
    size_t numSum = isSum;
    size_t numMin = isMin;
    size_t numMax = isMax;
    size_t numAvg = isAvg;
    size_t numSumChk = isSumChk;
    size_t numMinChk = isMinChk;
    size_t numMaxChk = isMaxChk;
    size_t numAvgChk = isAvgChk;
    size_t numReencChk = isReencChk;
    size_t numDec = isDec;
    size_t numDecChk = isDecChk;
    for (size_t i = 1; i < numResults; ++i) {
        auto & ti = results[i][0];
        isEnc |= ti.encode.isExecuted;
        numEnc += ti.encode.isExecuted;
        isChk |= ti.check.isExecuted;
        numChk += ti.check.isExecuted;
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
        isSum |= ti.sum.isExecuted;
        numSum += ti.sum.isExecuted;
        isMin |= ti.min.isExecuted;
        numMin += ti.min.isExecuted;
        isMax |= ti.max.isExecuted;
        numMax += ti.max.isExecuted;
        isAvg |= ti.avg.isExecuted;
        numAvg += ti.avg.isExecuted;
        isSumChk |= ti.sumChecked.isExecuted;
        numSumChk += ti.sumChecked.isExecuted;
        isMinChk |= ti.minChecked.isExecuted;
        numMinChk += ti.minChecked.isExecuted;
        isMaxChk |= ti.maxChecked.isExecuted;
        numMaxChk += ti.maxChecked.isExecuted;
        isAvgChk |= ti.avgChecked.isExecuted;
        numAvgChk += ti.avgChecked.isExecuted;
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
    auto headlineWriter = [&log10NumTests,&num] (bool doIt, const char * const name) {
        if (doIt) {
            ++num;
            std::cout << '|' << std::setw(log10NumTests) << name;
        }
    };
    headlineWriter(isEnc, "enc");
    headlineWriter(isChk, "chk");
    headlineWriter(isAdd, "add");
    headlineWriter(isSub, "sub");
    headlineWriter(isMul, "mul");
    headlineWriter(isDiv, "div");
    headlineWriter(isAddChk, "addC");
    headlineWriter(isSubChk, "subC");
    headlineWriter(isMulChk, "mulC");
    headlineWriter(isDivChk, "divC");
    headlineWriter(isSum, "sum");
    headlineWriter(isMin, "min");
    headlineWriter(isMax, "max");
    headlineWriter(isAvg, "avg");
    headlineWriter(isSumChk, "sumC");
    headlineWriter(isMinChk, "minC");
    headlineWriter(isMaxChk, "maxC");
    headlineWriter(isAvgChk, "avgC");
    headlineWriter(isReencChk, "renC");
    headlineWriter(isDecChk, "dec");
    headlineWriter(isDecChk, "decC");
    std::cout << "|\n#   " << std::setfill('-');
    for (size_t i = 0; i < num; ++i) {
        std::cout << ((i == 0) ? '|' : '+') << std::setw(log10NumTests) << '-';
    }
    std::cout << "|\n" << std::setfill(' ');
    size_t id = 1;
    size_t datawidth = 0;
    size_t incChk = isEnc ? numResults : 0;
    size_t incAdd = incChk + (isAdd ? numResults : 0);
    size_t incSub = incAdd + (isSub ? numResults : 0);
    size_t incMul = incSub + (isMul ? numResults : 0);
    size_t incDiv = incMul + (isDiv ? numResults : 0);
    size_t incAddChk = incDiv + (isAddChk ? numResults : 0);
    size_t incSubChk = incAddChk + (isSubChk ? numResults : 0);
    size_t incMulChk = incSubChk + (isMulChk ? numResults : 0);
    size_t incDivChk = incMulChk + (isDivChk ? numResults : 0);
    size_t incSum = incDivChk + (isSum ? numResults : 0);
    size_t incMin = incSum + (isMin ? numResults : 0);
    size_t incMax = incMin + (isMax ? numResults : 0);
    size_t incAvg = incMax + (isAvg ? numResults : 0);
    size_t incSumChk = incAvg + (isSumChk ? numResults : 0);
    size_t incMinChk = incSumChk + (isMinChk ? numResults : 0);
    size_t incMaxChk = incMinChk + (isMaxChk ? numResults : 0);
    size_t incAvgChk = incMaxChk + (isAvgChk ? numResults : 0);
    size_t incReencChk = incAvgChk + (isReencChk ? numResults : 0);
    size_t incDec = incReencChk + (isDec ? numResults : 0);
    size_t incDecChk = incDec + (isDecChk ? numResults : 0);
    auto idWriter = [&log10NumTests] (bool doIt, bool isExecuted, const size_t id, const size_t idInc = 0) {
        if (doIt) {
            if (isExecuted) {
                std::cout << '|' << std::setw(log10NumTests) << (id + idInc);
            } else {
                std::cout << '|' << std::setw(log10NumTests) << ' ';
            }
        }
    };
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
        idWriter(isEnc, ti.encode.isExecuted, id);
        idWriter(isChk, ti.check.isExecuted, id, incChk);
        idWriter(isAdd, ti.add.isExecuted, id, incAdd);
        idWriter(isSub, ti.sub.isExecuted, id, incSub);
        idWriter(isMul, ti.mul.isExecuted, id, incMul);
        idWriter(isDiv, ti.div.isExecuted, id, incDiv);
        idWriter(isAddChk, ti.addChecked.isExecuted, id, incAddChk);
        idWriter(isSubChk, ti.subChecked.isExecuted, id, incSubChk);
        idWriter(isMulChk, ti.mulChecked.isExecuted, id, incMulChk);
        idWriter(isDivChk, ti.divChecked.isExecuted, id, incDivChk);
        idWriter(isSum, ti.sum.isExecuted, id, incSum);
        idWriter(isMin, ti.min.isExecuted, id, incMin);
        idWriter(isMax, ti.max.isExecuted, id, incMax);
        idWriter(isAvg, ti.avg.isExecuted, id, incAvg);
        idWriter(isSumChk, ti.sumChecked.isExecuted, id, incSumChk);
        idWriter(isMinChk, ti.minChecked.isExecuted, id, incMinChk);
        idWriter(isMaxChk, ti.maxChecked.isExecuted, id, incMaxChk);
        idWriter(isAvgChk, ti.avgChecked.isExecuted, id, incAvgChk);
        idWriter(isReencChk, ti.reencodeChecked.isExecuted, id, incReencChk);
        idWriter(isDec, ti.decode.isExecuted, id, incDec);
        idWriter(isDecChk, ti.decodeChecked.isExecuted, id, incDecChk);
        std::cout << "|   " << v[0].name << '\n';
    }

    // print headline
    std::cout << "unroll/block";
    // first all encode columns, then all check columns etc.size
    size_t i = 0;
    if (isEnc) {
        for (auto & v : results) {
            auto & ti = v[0];
            std::cout << ',' << (((i == 0) && config.doRenameFirst) ? "memcpy" : ti.name) << ((((i == 0) && config.doRenameFirst) || !config.doAppendTestMethod) ? "" : " enc");
            ++i;
        }
        i = 0;
    }
    if (isChk) {
        for (auto & v : results) {
            auto & ti = v[0];
            std::cout << ',' << (((i == 0) && config.doRenameFirst) ? "memcmp" : ti.name) << ((((i == 0) && config.doRenameFirst) || !config.doAppendTestMethod) ? "" : " check");
            ++i;
        }
        i = 0;
    }
    if (isAdd) {
        for (auto & v : results) {
            auto & ti = v[0];
            std::cout << ',' << (((i == 0) && config.doRenameFirst) ? "memcpy" : ti.name) << ((((i == 0) && config.doRenameFirst) || !config.doAppendTestMethod) ? "" : " add");
            ++i;
        }
        i = 0;
    }
    if (isSub) {
        for (auto & v : results) {
            auto & ti = v[0];
            std::cout << ',' << (((i == 0) && config.doRenameFirst) ? "memcpy" : ti.name) << ((((i == 0) && config.doRenameFirst) || !config.doAppendTestMethod) ? "" : " sub");
            ++i;
        }
        i = 0;
    }
    if (isMul) {
        for (auto & v : results) {
            auto & ti = v[0];
            std::cout << ',' << (((i == 0) && config.doRenameFirst) ? "memcpy" : ti.name) << ((((i == 0) && config.doRenameFirst) || !config.doAppendTestMethod) ? "" : " mul");
            ++i;
        }
        i = 0;
    }
    if (isDiv) {
        for (auto & v : results) {
            auto & ti = v[0];
            std::cout << ',' << (((i == 0) && config.doRenameFirst) ? "memcpy" : ti.name) << ((((i == 0) && config.doRenameFirst) || !config.doAppendTestMethod) ? "" : " div");
            ++i;
        }
        i = 0;
    }
    if (isAddChk) {
        for (auto & v : results) {
            auto & ti = v[0];
            std::cout << ',' << (((i == 0) && config.doRenameFirst) ? "memcmp+memcpy" : ti.name) << ((((i == 0) && config.doRenameFirst) || !config.doAppendTestMethod) ? "" : " addChk");
            ++i;
        }
        i = 0;
    }
    if (isSubChk) {
        for (auto & v : results) {
            auto & ti = v[0];
            std::cout << ',' << (((i == 0) && config.doRenameFirst) ? "memcmp+memcpy" : ti.name) << ((((i == 0) && config.doRenameFirst) || !config.doAppendTestMethod) ? "" : " subChk");
            ++i;
        }
        i = 0;
    }
    if (isMulChk) {
        for (auto & v : results) {
            auto & ti = v[0];
            std::cout << ',' << (((i == 0) && config.doRenameFirst) ? "memcmp+memcpy" : ti.name) << ((((i == 0) && config.doRenameFirst) || !config.doAppendTestMethod) ? "" : " mulChk");
            ++i;
        }
        i = 0;
    }
    if (isDivChk) {
        for (auto & v : results) {
            auto & ti = v[0];
            std::cout << ',' << (((i == 0) && config.doRenameFirst) ? "memcmp+memcpy" : ti.name) << ((((i == 0) && config.doRenameFirst) || !config.doAppendTestMethod) ? "" : " divChk");
            ++i;
        }
        i = 0;
    }
    if (isSum) {
        for (auto & v : results) {
            auto & ti = v[0];
            std::cout << ',' << (((i == 0) && config.doRenameFirst) ? "memcpy" : ti.name) << ((((i == 0) && config.doRenameFirst) || !config.doAppendTestMethod) ? "" : " sum");
            ++i;
        }
        i = 0;
    }
    if (isMin) {
        for (auto & v : results) {
            auto & ti = v[0];
            std::cout << ',' << (((i == 0) && config.doRenameFirst) ? "memcpy" : ti.name) << ((((i == 0) && config.doRenameFirst) || !config.doAppendTestMethod) ? "" : " min");
            ++i;
        }
        i = 0;
    }
    if (isMax) {
        for (auto & v : results) {
            auto & ti = v[0];
            std::cout << ',' << (((i == 0) && config.doRenameFirst) ? "memcpy" : ti.name) << ((((i == 0) && config.doRenameFirst) || !config.doAppendTestMethod) ? "" : " max");
            ++i;
        }
        i = 0;
    }
    if (isSum) {
        for (auto & v : results) {
            auto & ti = v[0];
            std::cout << ',' << (((i == 0) && config.doRenameFirst) ? "memcpy" : ti.name) << ((((i == 0) && config.doRenameFirst) || !config.doAppendTestMethod) ? "" : " avg");
            ++i;
        }
        i = 0;
    }
    if (isSumChk) {
        for (auto & v : results) {
            auto & ti = v[0];
            std::cout << ',' << (((i == 0) && config.doRenameFirst) ? "memcmp+memcpy" : ti.name) << ((((i == 0) && config.doRenameFirst) || !config.doAppendTestMethod) ? "" : " sumChk");
            ++i;
        }
        i = 0;
    }
    if (isMinChk) {
        for (auto & v : results) {
            auto & ti = v[0];
            std::cout << ',' << (((i == 0) && config.doRenameFirst) ? "memcmp+memcpy" : ti.name) << ((((i == 0) && config.doRenameFirst) || !config.doAppendTestMethod) ? "" : " minChk");
            ++i;
        }
        i = 0;
    }
    if (isMaxChk) {
        for (auto & v : results) {
            auto & ti = v[0];
            std::cout << ',' << (((i == 0) && config.doRenameFirst) ? "memcmp+memcpy" : ti.name) << ((((i == 0) && config.doRenameFirst) || !config.doAppendTestMethod) ? "" : " maxChk");
            ++i;
        }
        i = 0;
    }
    if (isAvgChk) {
        for (auto & v : results) {
            auto & ti = v[0];
            std::cout << ',' << (((i == 0) && config.doRenameFirst) ? "memcmp+memcpy" : ti.name) << ((((i == 0) && config.doRenameFirst) || !config.doAppendTestMethod) ? "" : " avgChk");
            ++i;
        }
        i = 0;
    }
    if (isReencChk) {
        for (auto & v : results) {
            auto & ti = v[0];
            std::cout << ',' << (((i == 0) && config.doRenameFirst) ? "memcmp+memcpy" : ti.name) << ((((i == 0) && config.doRenameFirst) || !config.doAppendTestMethod) ? "" : " reencChk");
            ++i;
        }
        i = 0;
    }
    if (isDec) {
        for (auto & v : results) {
            auto & ti = v[0];
            std::cout << ',' << (((i == 0) && config.doRenameFirst) ? "memcpy" : ti.name) << ((((i == 0) && config.doRenameFirst) || !config.doAppendTestMethod) ? "" : " dec");
            ++i;
        }
        i = 0;
    }
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
        if (isEnc) {
            for (auto & v : results) {
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
        if (isChk) {
            for (auto & v : results) {
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
        if (isAdd) {
            for (auto & v : results) {
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
        if (isSub) {
            for (auto & v : results) {
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
        if (isMul) {
            for (auto & v : results) {
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
        if (isDiv) {
            for (auto & v : results) {
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
        if (isAddChk) {
            for (auto & v : results) {
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
        if (isSubChk) {
            for (auto & v : results) {
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
        if (isMulChk) {
            for (auto & v : results) {
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
        if (isDivChk) {
            for (auto & v : results) {
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
        if (isSum) {
            for (auto & v : results) {
                std::cout << ',';
                if (pos < v.size() && v[pos].sum.isExecuted && v[pos].sum.error.empty()) {
                    auto & x = v[pos];
                    if (doRelative) {
                        if (x.reference) {
                            std::cout << (static_cast<double>(v[pos].sum.nanos) / x.reference->sum.nanos);
                        } else {
                            std::cout << 1.0L;
                        }
                    } else {
                        std::cout << v[pos].sum.nanos;
                    }
                }
            }
        }
        if (isMin) {
            for (auto & v : results) {
                std::cout << ',';
                if (pos < v.size() && v[pos].min.isExecuted && v[pos].min.error.empty()) {
                    auto & x = v[pos];
                    if (doRelative) {
                        if (x.reference) {
                            std::cout << (static_cast<double>(v[pos].min.nanos) / x.reference->min.nanos);
                        } else {
                            std::cout << 1.0L;
                        }
                    } else {
                        std::cout << v[pos].min.nanos;
                    }
                }
            }
        }
        if (isMax) {
            for (auto & v : results) {
                std::cout << ',';
                if (pos < v.size() && v[pos].max.isExecuted && v[pos].max.error.empty()) {
                    auto & x = v[pos];
                    if (doRelative) {
                        if (x.reference) {
                            std::cout << (static_cast<double>(v[pos].max.nanos) / x.reference->max.nanos);
                        } else {
                            std::cout << 1.0L;
                        }
                    } else {
                        std::cout << v[pos].max.nanos;
                    }
                }
            }
        }
        if (isAvg) {
            for (auto & v : results) {
                std::cout << ',';
                if (pos < v.size() && v[pos].avg.isExecuted && v[pos].avg.error.empty()) {
                    auto & x = v[pos];
                    if (doRelative) {
                        if (x.reference) {
                            std::cout << (static_cast<double>(v[pos].avg.nanos) / x.reference->avg.nanos);
                        } else {
                            std::cout << 1.0L;
                        }
                    } else {
                        std::cout << v[pos].avg.nanos;
                    }
                }
            }
        }
        if (isSumChk) {
            for (auto & v : results) {
                std::cout << ',';
                if (pos < v.size() && v[pos].sumChecked.isExecuted && v[pos].sumChecked.error.empty()) {
                    auto & x = v[pos];
                    if (doRelative) {
                        if (x.reference) {
                            std::cout << (static_cast<double>(v[pos].sumChecked.nanos) / x.reference->sumChecked.nanos);
                        } else {
                            std::cout << 1.0L;
                        }
                    } else {
                        std::cout << v[pos].sumChecked.nanos;
                    }
                }
            }
        }
        if (isMinChk) {
            for (auto & v : results) {
                std::cout << ',';
                if (pos < v.size() && v[pos].minChecked.isExecuted && v[pos].minChecked.error.empty()) {
                    auto & x = v[pos];
                    if (doRelative) {
                        if (x.reference) {
                            std::cout << (static_cast<double>(v[pos].minChecked.nanos) / x.reference->minChecked.nanos);
                        } else {
                            std::cout << 1.0L;
                        }
                    } else {
                        std::cout << v[pos].minChecked.nanos;
                    }
                }
            }
        }
        if (isMaxChk) {
            for (auto & v : results) {
                std::cout << ',';
                if (pos < v.size() && v[pos].maxChecked.isExecuted && v[pos].maxChecked.error.empty()) {
                    auto & x = v[pos];
                    if (doRelative) {
                        if (x.reference) {
                            std::cout << (static_cast<double>(v[pos].maxChecked.nanos) / x.reference->maxChecked.nanos);
                        } else {
                            std::cout << 1.0L;
                        }
                    } else {
                        std::cout << v[pos].maxChecked.nanos;
                    }
                }
            }
        }
        if (isAvgChk) {
            for (auto & v : results) {
                std::cout << ',';
                if (pos < v.size() && v[pos].avgChecked.isExecuted && v[pos].avgChecked.error.empty()) {
                    auto & x = v[pos];
                    if (doRelative) {
                        if (x.reference) {
                            std::cout << (static_cast<double>(v[pos].avgChecked.nanos) / x.reference->avgChecked.nanos);
                        } else {
                            std::cout << 1.0L;
                        }
                    } else {
                        std::cout << v[pos].avgChecked.nanos;
                    }
                }
            }
        }
        if (isReencChk) {
            for (auto & v : results) {
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
        if (isDec) {
            for (auto & v : results) {
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
        if (isDecChk) {
            for (auto & v : results) {
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
