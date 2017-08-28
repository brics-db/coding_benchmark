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

#include "TestInfo.hpp"

#include <exception>
#include <sstream>

TestInfo::TestInfo()
        : isExecuted(false),
          nanos(0),
          error() {
}

TestInfo::TestInfo(
        int64_t nanos)
        : TestInfo(true, nanos, nullptr) {
}

TestInfo::TestInfo(
        const std::string & error)
        : TestInfo(true, 0, error) {
}

TestInfo::TestInfo(
        bool isExecuted,
        int64_t nanos,
        const std::string & error)
        : isExecuted(isExecuted),
          nanos(nanos),
          error(error) {
}

TestInfo::TestInfo(
        const TestInfo & other)
        : isExecuted(other.isExecuted),
          nanos(other.nanos),
          error(other.error) {
}

TestInfo::~TestInfo() {
}

void TestInfo::set(
        int64_t nanos) {
    this->isExecuted = true;
    this->nanos = nanos;
    this->error.clear();
}

void TestInfo::set(
        const std::string & error) {
    this->isExecuted = true;
    this->nanos = 0;
    this->error = error;
}

TestInfo & TestInfo::operator=(
        const TestInfo & other) {
    this->~TestInfo();
    new (this) TestInfo(other);
    return *this;
}

TestInfos::TestInfos(
        const size_t datawidth,
        const std::string & name,
        const std::string & simd)
        : datawidth(datawidth),
          name(name),
          simd(simd),
          encode(),
          check(),
          arithmetic(),
          arithmeticChecked(),
          aggregate(),
          aggregateChecked(),
          reencodeChecked(),
          decode(),
          decodeChecked(),
          reference(nullptr) {
}

TestInfos::TestInfos(
        const size_t datawidth,
        const std::string & name,
        const std::string & simd,
        TestInfo & encode,
        TestInfo & check,
        TestInfo & arithmetic,
        TestInfo & arithmeticChecked,
        TestInfo & aggregate,
        TestInfo & aggregateChecked,
        TestInfo & reencodeChecked,
        TestInfo & decode,
        TestInfo & checkAndDecode)
        : datawidth(datawidth),
          name(name),
          simd(simd),
          encode(encode),
          check(check),
          arithmetic(arithmetic),
          arithmeticChecked(arithmeticChecked),
          aggregate(aggregate),
          aggregateChecked(aggregateChecked),
          reencodeChecked(reencodeChecked),
          decode(decode),
          decodeChecked(checkAndDecode),
          reference(nullptr) {
}

TestInfos::TestInfos(
        const size_t datawidth,
        const std::string & name,
        const std::string & simd,
        TestInfo & encode,
        TestInfo & check,
        TestInfo & arithmetic,
        TestInfo & arithmeticChecked,
        TestInfo & aggregate,
        TestInfo & aggregateChecked,
        TestInfo & reencodeChecked,
        TestInfo & decode,
        TestInfo & checkAndDecode,
        TestInfos * reference)
        : datawidth(datawidth),
          name(name),
          simd(simd),
          encode(encode),
          check(check),
          arithmetic(arithmetic),
          arithmeticChecked(arithmeticChecked),
          aggregate(aggregate),
          aggregateChecked(aggregateChecked),
          reencodeChecked(reencodeChecked),
          decode(decode),
          decodeChecked(checkAndDecode),
          reference(reference) {
}

TestInfos::~TestInfos() {
}

void setTestInfosReference(
        std::vector<TestInfos> & vecTarget,
        std::vector<TestInfos> & vecReferences) {
    if (vecTarget.size() != vecReferences.size()) {
        std::stringstream ss;
        ss << "[ſetTestInfosReference] Sizes don't match! " << vecTarget.size() << " != " << vecReferences.size();
        throw std::runtime_error(ss.str().c_str());
    }
    for (size_t i = 0; i < vecTarget.size(); ++i) {
        vecTarget[i].reference = &vecReferences[i];
    }
}
