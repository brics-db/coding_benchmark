// Copyright 2016,2017 Till Kolditz, Stefan de Bruijn
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

#include <cstdint>
#include <limits>
#include <climits>
#include <string>
#include <cstring>
#include <cstdint>
#include <optional>
#include <variant>
#include <type_traits>
#include <sstream>

#ifdef OMP
#include <omp.h>
#endif

#include <Util/AlignedBlock.hpp>
#include <Util/TestInfo.hpp>
#include <Util/ErrorInfo.hpp>
#include <Util/Intrinsics.hpp>
#include <Util/TestConfiguration.hpp>

struct DataGenerationConfiguration {
    const std::optional<size_t> numEffectiveLSBs;

    DataGenerationConfiguration()
            : numEffectiveLSBs(std::nullopt) {
    }

    DataGenerationConfiguration(
            const size_t numEffectiveLSBs)
            : numEffectiveLSBs(numEffectiveLSBs) {
    }
};

struct TestBase0 {

    virtual ~TestBase0() {
    }

    virtual const std::string & getSIMDtypeName() = 0;

    virtual bool HasCapabilities() = 0;
};

struct TestBase :
        virtual public TestBase0 {

protected:
    size_t datawidth;
    std::string name;
    AlignedBlock bufRaw;
    AlignedBlock bufEncoded;
    AlignedBlock bufResult;

public:
    TestBase(
            size_t datawidth,
            const std::string & name,
            AlignedBlock & bufRaw,
            AlignedBlock & bufEncoded,
            AlignedBlock & bufResult);

    TestBase(
            TestBase & other);

    virtual ~TestBase();

    virtual void ResetBuffers(
            const DataGenerationConfiguration & config) = 0;

    virtual size_t getInputTypeSize() = 0;

    virtual size_t getOutputTypeSize() = 0;

    // Encoding
    virtual void PreEncode(
            const EncodeConfiguration & config);

    virtual void RunEncode(
            const EncodeConfiguration & config) = 0;

    // Check-Only
    virtual bool DoCheck();

    virtual void PreCheck(
            const CheckConfiguration & config);

    virtual void RunCheck(
            const CheckConfiguration & config);

    // Filter
    virtual bool DoFilter();

    virtual void PreFilter(
            const FilterConfiguration & config);

    virtual void RunFilter(
            const FilterConfiguration & config);

    // Filter Checked
    virtual bool DoFilterChecked();

    virtual void PreFilterChecked(
            const FilterConfiguration & config);

    virtual void RunFilterChecked(
            const FilterConfiguration & config);

    // Arithmetic
    virtual bool DoArithmetic(
            const ArithmeticConfiguration & config);

    virtual void PreArithmetic(
            const ArithmeticConfiguration & config);

    virtual void RunArithmetic(
            const ArithmeticConfiguration & config);

    // Arithmetic Checked
    virtual bool DoArithmeticChecked(
            const ArithmeticConfiguration & config);

    virtual void PreArithmeticChecked(
            const ArithmeticConfiguration & config);

    virtual void RunArithmeticChecked(
            const ArithmeticConfiguration & config);

    // Aggregate
    virtual bool DoAggregate(
            const AggregateConfiguration & config);

    virtual void PreAggregate(
            const AggregateConfiguration & config);

    virtual void RunAggregate(
            const AggregateConfiguration & config);

    // Aggregate Checked
    virtual bool DoAggregateChecked(
            const AggregateConfiguration & config);

    virtual void PreAggregateChecked(
            const AggregateConfiguration & config);

    virtual void RunAggregateChecked(
            const AggregateConfiguration & config);

    // Reencode (Checked)
    virtual bool DoReencodeChecked();

    virtual void PreReencodeChecked(
            const ReencodeConfiguration & config);

    virtual void RunReencodeChecked(
            const ReencodeConfiguration & config);

    // Decoding-Only
    virtual bool DoDecode();

    virtual void PreDecode(
            const DecodeConfiguration & config);

    virtual void RunDecode(
            const DecodeConfiguration & config);

    // Check-And-Decode
    virtual bool DoDecodeChecked();

    virtual void PreDecodeChecked(
            const DecodeConfiguration & config);

    virtual void RunDecodeChecked(
            const DecodeConfiguration & config);

    // Execute test:
    virtual TestInfos Execute(
            const TestConfiguration & config,
            const DataGenerationConfiguration & configDataGen);
};

struct ScalarTest :
        virtual public TestBase0 {

    virtual ~ScalarTest();

    virtual const std::string & getSIMDtypeName() override;

    virtual bool HasCapabilities() override;
};

struct SSE42Test :
        virtual public TestBase0 {

    virtual ~SSE42Test();

    virtual const std::string & getSIMDtypeName() override;

    virtual bool HasCapabilities() override;
};

struct AVX2Test :
        virtual public TestBase0 {

    virtual ~AVX2Test();

    virtual const std::string & getSIMDtypeName() override;

    virtual bool HasCapabilities() override;
};

struct AVX512Test :
        virtual public TestBase0 {

    virtual ~AVX512Test();

    virtual const std::string & getSIMDtypeName() override;

    virtual bool HasCapabilities() override;
};

template<typename DATARAW, typename DATAENC>
struct Test :
        public TestBase {

    Test(
            const std::string & name,
            AlignedBlock & bufRaw,
            AlignedBlock & bufEncoded,
            AlignedBlock & bufResult)
            : TestBase(sizeof(DATARAW), name, bufRaw, bufEncoded, bufResult) {
    }

    virtual ~Test() {
    }

    virtual size_t getInputTypeSize() override {
        return sizeof(DATARAW);
    }

    virtual size_t getOutputTypeSize() override {
        return sizeof(DATARAW);
    }

    size_t getNumValues() {
        return this->bufRaw.template end<DATARAW>() - this->bufRaw.template begin<DATARAW>();
    }

    void ResetBuffers(
            const DataGenerationConfiguration & dataGenConfig) override {
        DATARAW mask = static_cast<DATARAW>(-1);
        if (dataGenConfig.numEffectiveLSBs) {
            mask = static_cast<DATARAW>((1ull << *dataGenConfig.numEffectiveLSBs) - 1ull);
        }
        auto pInEnd = bufRaw.template end<DATARAW>();
        DATARAW value = static_cast<DATARAW>(12783);
        for (DATARAW* pIn = bufRaw.template begin<DATARAW>(); pIn < pInEnd; ++pIn) {
            *pIn = mask & value;
            value = value * static_cast<DATARAW>(7577) + static_cast<DATARAW>(10467);
        }
        memset(bufEncoded.begin(), 0, bufEncoded.nBytes);
        memset(bufResult.begin(), 0, bufResult.nBytes);
    }
};
