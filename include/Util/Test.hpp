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
#include <random>

#ifdef OMP
#include <omp.h>
#endif

#include <Util/AlignedBlock.hpp>
#include <Util/TestInfo.hpp>
#include <Util/Helpers.hpp>
#include <Util/Functors.hpp>
#include <Util/ErrorInfo.hpp>
#include <Util/Intrinsics.hpp>
#include <Util/TestConfiguration.hpp>

class Randomizer {
    static Randomizer instance;

    std::random_device rDev;
    std::default_random_engine rEng;
    std::uniform_int_distribution<int> rDist;

    Randomizer();

public:
    static int getUniformInt();
};

class DataGenerationConfiguration {

public:
    const std::optional<size_t> numEffectiveBitsData;
    const std::optional<size_t> numIneffectiveLSBsData;
    const std::optional<size_t> numEffectiveBitsArithOperand;
    const std::optional<size_t> numIneffectiveLSBsArithOperand;

    DataGenerationConfiguration();

    DataGenerationConfiguration(
            const size_t numEffectiveLSBs);

    DataGenerationConfiguration(
            const size_t numEffectiveLSBs,
            const size_t numIneffectiveLSBsData);

    DataGenerationConfiguration(
            const size_t numEffectiveLSBs,
            const size_t numIneffectiveLSBsData,
            const size_t numEffectiveBitsArithOperand);

    DataGenerationConfiguration(
            const size_t numEffectiveLSBs,
            const size_t numIneffectiveLSBsData,
            const size_t numEffectiveBitsArithOperand,
            const size_t numIneffectiveLSBsArithOperand);

    int getUniformData() const;

    int getUniformArithOperand() const;
};

struct TestBase0 {

    virtual ~TestBase0() {
    }

    const virtual std::string & getSIMDtypeName() = 0;

    virtual bool HasCapabilities() = 0;
};

// Forward Declaration needed in TestBase
template<typename DATARAW, typename DATAENC>
class Test;

class TestBase :
        virtual public TestBase0 {

    template<typename, typename >
    friend class Test;

    bool internalPreEncodeCalled;
    bool internalPreCheckCalled;
    bool internalPreFilterCalled;
    bool internalPreFilterCheckedCalled;
    bool internalPreArithmeticCalled;
    bool internalPreArithmeticCheckedCalled;
    bool internalPreAggregateCalled;
    bool internalPreAggregateCheckedCalled;
    bool internalPreReencodeCheckedCalled;
    bool internalPreDecodeCalled;
    bool internalPreDecodeCheckedCalled;

protected:
    size_t datawidth;
    std::string name;
    AlignedBlock bufRaw;
    AlignedBlock bufScratchPad;
    AlignedBlock bufEncoded;
    AlignedBlock bufResult;

private:
    // internal buffers hidden on purpose from client code!
    AlignedBlock bufArith; // store results for UNENCODED arithmetic and aggregate operations, done on bufRaw (for these we use the simple implementation from CopyTest) to allow to compare the result against the encoded version
    AlignedBlock bufDecoded; // buffer for the unencoding of results. This is used to decode the results from e.g. arithmetic and aggregate operations and then compare against bufArith

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

    virtual size_t getRawDataTypeSize() = 0;

    virtual size_t getEncodedDataTypeSize() = 0;

    // Encoding
    virtual void PreEncode(
            const EncodeConfiguration & config) = 0;

    virtual void RunEncode(
            const EncodeConfiguration & config) = 0;

    // Check-Only
    virtual bool DoCheck();

    virtual void PreCheck(
            const CheckConfiguration & config) = 0;

    virtual void RunCheck(
            const CheckConfiguration & config);

    // Filter
    virtual bool DoFilter();

    virtual void PreFilter(
            const FilterConfiguration & config) = 0;

    virtual void RunFilter(
            const FilterConfiguration & config);

    // Filter Checked
    virtual bool DoFilterChecked();

    virtual void PreFilterChecked(
            const FilterConfiguration & config) = 0;

    virtual void RunFilterChecked(
            const FilterConfiguration & config);

    // Arithmetic
    virtual bool DoArithmetic(
            const ArithmeticConfiguration & config);

    virtual void PreArithmetic(
            const ArithmeticConfiguration & config) = 0;

    virtual void RunArithmetic(
            const ArithmeticConfiguration & config);

    // Arithmetic Checked
    virtual bool DoArithmeticChecked(
            const ArithmeticConfiguration & config);

    virtual void PreArithmeticChecked(
            const ArithmeticConfiguration & config) = 0;

    virtual void RunArithmeticChecked(
            const ArithmeticConfiguration & config);

    // Aggregate
    virtual bool DoAggregate(
            const AggregateConfiguration & config);

    virtual void PreAggregate(
            const AggregateConfiguration & config) = 0;

    virtual void RunAggregate(
            const AggregateConfiguration & config);

    // Aggregate Checked
    virtual bool DoAggregateChecked(
            const AggregateConfiguration & config);

    virtual void PreAggregateChecked(
            const AggregateConfiguration & config) = 0;

    virtual void RunAggregateChecked(
            const AggregateConfiguration & config);

    // Reencode (Checked)
    virtual bool DoReencodeChecked();

    virtual void PreReencodeChecked(
            const ReencodeConfiguration & config) = 0;

    virtual void RunReencodeChecked(
            const ReencodeConfiguration & config);

    // Decoding-Only
    virtual bool DoDecode();

    virtual void PreDecode(
            const DecodeConfiguration & config) = 0;

    virtual void RunDecode(
            const DecodeConfiguration & config);

    // Check-And-Decode
    virtual bool DoDecodeChecked();

    virtual void PreDecodeChecked(
            const DecodeConfiguration & config) = 0;

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

    const virtual std::string & getSIMDtypeName() override;

    virtual bool HasCapabilities() override;
};

struct SSE42Test :
        virtual public TestBase0 {

    virtual ~SSE42Test();

    const virtual std::string & getSIMDtypeName() override;

    virtual bool HasCapabilities() override;
};

struct AVX2Test :
        virtual public TestBase0 {

    virtual ~AVX2Test();

    const virtual std::string & getSIMDtypeName() override;

    virtual bool HasCapabilities() override;
};

struct AVX512Test :
        virtual public TestBase0 {

    virtual ~AVX512Test();

    const virtual std::string & getSIMDtypeName() override;

    virtual bool HasCapabilities() override;
};

template<typename V>
struct SIMDTest;

#ifdef __SSE4_2__

template<>
struct SIMDTest<__m128i> : public SSE42Test {

    virtual ~SIMDTest() {};

    using SSE42Test::getSIMDtypeName;
    using SSE42Test::HasCapabilities;
};

#endif

#ifdef __AVX2__

template<>
struct SIMDTest<__m256i> : public AVX2Test {

    virtual ~SIMDTest() {};

    using AVX2Test::getSIMDtypeName;
    using AVX2Test::HasCapabilities;
};

#endif

#ifdef __AVX512F__

template<>
struct SIMDTest<__m512i> : public AVX512Test {

    virtual ~SIMDTest() {};

    using AVX512Test::getSIMDtypeName;
    using AVX512Test::HasCapabilities;
};

#endif

template<typename DATARAW, typename DATAENC>
class Test :
        public TestBase {

public:
    Test(
            const std::string & name,
            AlignedBlock & bufRaw,
            AlignedBlock & bufEncoded,
            AlignedBlock & bufResult)
            : TestBase(sizeof(DATARAW), name, bufRaw, bufEncoded, bufResult) {
    }

    virtual ~Test() {
    }

    virtual size_t getRawDataTypeSize() override {
        return sizeof(DATARAW);
    }

    virtual size_t getEncodedDataTypeSize() override {
        return sizeof(DATAENC);
    }

    size_t getNumValues() const {
        return this->bufRaw.template end<DATARAW>() - this->bufRaw.template begin<DATARAW>();
    }

    void ResetBuffers(
            const DataGenerationConfiguration & dataGenConfig) override {
        DATARAW mask = static_cast<DATARAW>(-1);
        if (dataGenConfig.numEffectiveBitsData) {
            mask = static_cast<DATARAW>((1ull << dataGenConfig.numEffectiveBitsData.value()) - 1ull);
        }
        if (dataGenConfig.numIneffectiveLSBsData) {
            mask &= ~static_cast<DATARAW>((1ull << dataGenConfig.numIneffectiveLSBsData.value()) - 1ull);
        }
        auto pInEnd = this->bufRaw.template end<DATARAW>();
        DATARAW value = static_cast<DATARAW>(12783);
        DATARAW* pIn = this->bufRaw.template begin<DATARAW>();
        while (pIn < pInEnd) {
            DATARAW x;
            do {
                x = mask & value;
                value = value * static_cast<DATARAW>(7577) + static_cast<DATARAW>(10467);
            } while (x == 0);
            *pIn++ = x;
        }
        this->bufEncoded.clear();
        this->bufResult.clear();
        this->bufArith.clear();
        this->bufDecoded.clear();
    }

    void PreEncode(
            const EncodeConfiguration & config) {
        config.target.clear();
        this->bufScratchPad.clear();
        this->internalPreEncodeCalled = true;
    }

    void PreCheck(
            const CheckConfiguration & config) {
        this->bufScratchPad.clear();
        this->internalPreCheckCalled = true;
    }

    void PreFilter(
            const FilterConfiguration & config) {
        config.target.clear(); // make sure the target buffer is empty
        this->bufScratchPad.clear();
        this->internalPreFilterCalled = true;
    }

    void PreFilterChecked(
            const FilterConfiguration & config) {
        config.target.clear(); // make sure the target buffer is empty
        this->bufScratchPad.clear();
        this->internalPreFilterCheckedCalled = true;
    }

private:
    struct Arithmetor {
        Test & test;
        const ArithmeticConfiguration & config;
        Arithmetor(
                Test & test,
                const ArithmeticConfiguration & config)
                : test(test),
                  config(config) {
        }
        void operator()(
                ArithmeticConfiguration::Add) {
            auto beg = config.source.template begin<DATARAW>();
            auto end = beg + config.numValues;
            auto out = config.target.template begin<DATARAW>();
            while (beg < end) {
                *out++ = *beg++ + config.operand;
            }
        }
        void operator()(
                ArithmeticConfiguration::Sub) {
            auto beg = config.source.template begin<DATARAW>();
            auto end = beg + config.numValues;
            auto out = config.target.template begin<DATARAW>();
            while (beg < end) {
                *out++ = *beg++ - config.operand;
            }
        }
        void operator()(
                ArithmeticConfiguration::Mul) {
            auto beg = config.source.template begin<DATARAW>();
            auto end = beg + config.numValues;
            auto out = config.target.template begin<DATARAW>();
            while (beg < end) {
                *out++ = *beg++ * config.operand;
            }
        }
        void operator()(
                ArithmeticConfiguration::Div) {
            auto beg = config.source.template begin<DATARAW>();
            auto end = beg + config.numValues;
            auto out = config.target.template begin<DATARAW>();
            while (beg < end) {
                *out++ = *beg++ / config.operand;
            }
        }
    };

public:
    void PreArithmetic(
            const ArithmeticConfiguration & config) {
        config.target.clear(); // make sure the target buffer is empty
        this->bufScratchPad.clear();
        // here we prepare the adapted raw data (with the given arithmetic operation) aginst which
        // the encoded data (modified also by the given arithmetic operation) can be checked.
        // The encoded operation will store its result in bufResult, so we can check bufArith against
        // bufResult then.
        this->bufArith.clear();
        this->bufDecoded.clear();
        ArithmeticConfiguration ac(config, this->bufRaw, this->bufArith, config.mode, config.operand);
        std::visit(Arithmetor(*this, ac), ac.mode);
        this->internalPreArithmeticCalled = true;
    }

    void PreArithmeticChecked(
            const ArithmeticConfiguration & config) {
        config.target.clear(); // make sure the target buffer is empty
        this->bufScratchPad.clear();
        // here we prepare the adapted raw data (with the given arithmetic operation) aginst which
        // the encoded data (modified also by the given arithmetic operation) can be checked.
        // The encoded operation will store its result in bufResult, so we can check bufArith against
        // bufResult then.
        this->bufArith.clear();
        this->bufDecoded.clear();
        ArithmeticConfiguration ac(config, this->bufRaw, this->bufArith, config.mode, config.operand);
        std::visit(Arithmetor(*this, ac), ac.mode);
        this->internalPreArithmeticCheckedCalled = true;
    }

private:

    struct Aggregator {
        typedef typename Larger<DATARAW>::larger_t larger_t;
        const Test & test;
        const AggregateConfiguration & config;
        Aggregator(
                const Test & test,
                const AggregateConfiguration & config)
                : test(test),
                  config(config) {
        }
        void operator()(
                AggregateConfiguration::Sum) {
            larger_t sum = larger_t(0);
            auto beg = config.source.template begin<DATARAW>();
            auto end = beg + config.numValues;
            auto out = config.target.template begin<larger_t>();
            while (beg < end) {
                sum += *beg++;
            }
            *out = sum;
        }
        void operator()(
                AggregateConfiguration::Min) {
            DATARAW min(std::numeric_limits<DATARAW>::max());
            auto beg = config.source.template begin<DATARAW>();
            auto end = beg + config.numValues;
            auto out = config.target.template begin<DATARAW>();
            while (beg < end) {
                min = std::min(min, *beg++);
            }
            *out = min;
        }
        void operator()(
                AggregateConfiguration::Max) {
            DATARAW max(std::numeric_limits<DATARAW>::min());
            auto beg = config.source.template begin<DATARAW>();
            auto end = beg + config.numValues;
            auto out = config.target.template begin<DATARAW>();
            while (beg < end) {
                max = std::max(max, *beg++);
            }
            *out = max;
        }
        void operator()(
                AggregateConfiguration::Avg) {
            larger_t sum = larger_t(0);
            auto beg = config.source.template begin<DATARAW>();
            auto end = beg + config.numValues;
            auto out = config.target.template begin<larger_t>();
            while (beg < end) {
                sum += *beg++;
            }
            *out = sum / config.numValues;
        }
    };

public:

    void PreAggregate(
            const AggregateConfiguration & config) {
        config.target.clear(); // make sure the target buffer is empty
        this->bufScratchPad.clear();
        // here we prepare the adapted raw data (with the given aggregate operation) aginst which
        // the encoded data (modified also by the given aggregate operation) can be checked.
        // The encoded operation will store its result in bufResult, so we can check bufArith against
        // bufResult then.
        this->bufArith.clear();
        this->bufDecoded.clear();
        AggregateConfiguration ac(config, this->bufRaw, this->bufArith, config.mode);
        std::visit(Aggregator(*this, ac), ac.mode);
        this->internalPreAggregateCalled = true;
    }

    void PreAggregateChecked(
            const AggregateConfiguration & config) {
        config.target.clear(); // make sure the target buffer is empty
        this->bufScratchPad.clear();
        // here we prepare the adapted raw data (with the given aggregate operation) aginst which
        // the encoded data (modified also by the given aggregate operation) can be checked.
        // The encoded operation will store its result in bufResult, so we can check bufArith against
        // bufResult then.
        this->bufArith.clear();
        this->bufDecoded.clear();
        AggregateConfiguration ac(config, this->bufRaw, this->bufArith, config.mode);
        std::visit(Aggregator(*this, ac), ac.mode);
        this->internalPreAggregateCheckedCalled = true;
    }

    void PreReencodeChecked(
            const ReencodeConfiguration & config) {
        config.target.clear(); // make sure the target buffer is empty
        this->bufScratchPad.clear();
        // TODO after Run: decode to bufDecoded and compare against bufRaw
        this->internalPreReencodeCheckedCalled = true;
    }

    void PreDecode(
            const DecodeConfiguration & config) {
        config.target.clear(); // make sure the target buffer is empty
        this->bufScratchPad.clear();
        // TODO after Run: compare config.target against bufRaw
        this->internalPreDecodeCalled = true;
    }

    void PreDecodeChecked(
            const DecodeConfiguration & config) {
        config.target.clear(); // make sure the target buffer is empty
        this->bufScratchPad.clear();
        // TODO after Run: compare config.target against bufRaw
        this->internalPreDecodeCheckedCalled = true;
    }
};
