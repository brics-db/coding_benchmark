// Copyright 2016 Till Kolditz, Stefan de Bruijn
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

#pragma once

#include <string>
#include <vector>
#include <optional>

#include <Util/Stopwatch.hpp>

struct TestInfo {

    bool isExecuted;
    int64_t nanos;
    std::string error;

    TestInfo();

    TestInfo(
            int64_t nanos);

    TestInfo(
            const std::string & error);

    TestInfo(
            bool isExecuted,
            int64_t nanos,
            const std::string & error);

    TestInfo(
            const TestInfo & other);

    virtual ~TestInfo();

    void set(
            int64_t nanos);

    void set(
            const std::string & error);

    TestInfo & operator=(
            const TestInfo &);
};

struct TestInfos {

    size_t datawidth;
    std::string name;
    std::string simd;
    TestInfo encode;
    TestInfo check;
    TestInfo add;
    TestInfo sub;
    TestInfo mul;
    TestInfo div;
    TestInfo addChecked;
    TestInfo subChecked;
    TestInfo mulChecked;
    TestInfo divChecked;
    TestInfo sum;
    TestInfo min;
    TestInfo max;
    TestInfo avg;
    TestInfo sumChecked;
    TestInfo minChecked;
    TestInfo maxChecked;
    TestInfo avgChecked;
    TestInfo reencodeChecked;
    TestInfo decode;
    TestInfo decodeChecked;
    const TestInfos * reference;

    TestInfos(
            const size_t datawidth,
            const std::string & name,
            const std::string & simd);

    TestInfos(
            const size_t datawidth,
            const std::string & name,
            const std::string & simd,
            TestInfo & encode,
            TestInfo & check,
            TestInfo & add,
            TestInfo & sub,
            TestInfo & mul,
            TestInfo & div,
            TestInfo & addChecked,
            TestInfo & subChecked,
            TestInfo & mulChecked,
            TestInfo & divChecked,
            TestInfo & sum,
            TestInfo & min,
            TestInfo & max,
            TestInfo & avg,
            TestInfo & sumChecked,
            TestInfo & minChecked,
            TestInfo & maxChecked,
            TestInfo & avgChecked,
            TestInfo & reencodeChecked,
            TestInfo & decode,
            TestInfo & checkAndDecode);

    TestInfos(
            const size_t datawidth,
            const std::string & name,
            const std::string & simd,
            TestInfo & encode,
            TestInfo & check,
            TestInfo & add,
            TestInfo & sub,
            TestInfo & mul,
            TestInfo & div,
            TestInfo & addChecked,
            TestInfo & subChecked,
            TestInfo & mulChecked,
            TestInfo & divChecked,
            TestInfo & sum,
            TestInfo & min,
            TestInfo & max,
            TestInfo & avg,
            TestInfo & sumChecked,
            TestInfo & minChecked,
            TestInfo & maxChecked,
            TestInfo & avgChecked,
            TestInfo & reencodeChecked,
            TestInfo & decode,
            TestInfo & checkAndDecode,
            const TestInfos * const reference);

    TestInfos(
            const TestInfos &) = default;

    virtual ~TestInfos();

    TestInfos & operator=(
            const TestInfos &) = default;
};

void setTestInfosReference(
        std::vector<TestInfos> & vecTarget,
        const std::vector<TestInfos> & vecReferences);
