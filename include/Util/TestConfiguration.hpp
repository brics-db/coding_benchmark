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
 * File:   TestConfiguration.hpp
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 21-08-2017 10:36
 */

#pragma once

#include <variant>

/**
 * This benchmark suite has a structure where there are individual ``tests'' which
 * contain several ``subtests''.
 * A ``Testcase'' runs each test on a sequence of unroll or block sizes. This is
 * included, because some tests work on an adjustable block size. Other tests do
 * not support block-based operation and there we test how ``loop unrolling''
 * affects the runtimes.
 */

/**
 * Abstract test configuration which keeps those settings which are
 * common / useful / needed across all types of test configurations.
 */
struct BasicTestConfiguration {
    const size_t numIterations;
    const size_t numValues;

    BasicTestConfiguration(
            const size_t numIterations,
            const size_t numValues)
            : numIterations(numIterations),
              numValues(numValues) {
    }
};

/**
 * The TestConfiguration contains the settings
 */
struct TestConfiguration :
        public BasicTestConfiguration {
    bool enableCheck;
    bool enableArithmetic;
    bool enableArithmeticChk;
    bool enableAggregate;
    bool enableAggregateChk;
    bool enableReencodeChk;
    bool enableDecode;
    bool enableDecodeChk;

    TestConfiguration(
            const size_t numIterations,
            const size_t numValues)
            : BasicTestConfiguration(numIterations, numValues),
              enableCheck(true),
              enableArithmetic(true),
              enableArithmeticChk(true),
              enableAggregate(true),
              enableAggregateChk(true),
              enableReencodeChk(true),
              enableDecode(true),
              enableDecodeChk(true) {
    }

    void disableAll() {
        enableCheck = false;
        enableArithmetic = false;
        enableArithmeticChk = false;
        enableAggregate = false;
        enableAggregateChk = false;
        enableReencodeChk = false;
        enableDecode = false;
        enableDecodeChk = false;
    }
};

struct SubTestConfiguration :
        public BasicTestConfiguration {
    const AlignedBlock & source;
    const AlignedBlock & target;
    SubTestConfiguration(
            const BasicTestConfiguration & config,
            const AlignedBlock & source,
            const AlignedBlock & target)
            : BasicTestConfiguration(config),
              source(source),
              target(target) {
    }
    SubTestConfiguration(
            const size_t numIterations,
            const size_t numValues,
            const AlignedBlock & source,
            const AlignedBlock & target)
            : BasicTestConfiguration(numIterations, numValues),
              source(source),
              target(target) {
    }
    SubTestConfiguration(
            const SubTestConfiguration &) = default;
};

struct EncodeConfiguration :
        public SubTestConfiguration {
    using SubTestConfiguration::SubTestConfiguration;
};

struct CheckConfiguration :
        public SubTestConfiguration {
    using SubTestConfiguration::SubTestConfiguration;
};

struct FilterConfiguration :
        public SubTestConfiguration {
    struct None {
    };
    struct LT {
    };
    struct LE {
    };
    struct EQ {
    };
    struct NE {
    };
    struct GE {
    };
    struct GT {
    };
    typedef std::variant<LT, LE, EQ, NE, GE, GT> Mode1;
    typedef std::variant<None, LT, LE, EQ, NE, GE, GT> Mode2;
    Mode1 mode1;
    size_t predicate1;
    Mode2 mode2;
    size_t predicate2;
    FilterConfiguration(
            const SubTestConfiguration & config,
            Mode1 mode,
            size_t predicate)
            : SubTestConfiguration(config),
              mode1(mode),
              predicate1(predicate),
              mode2(None()),
              predicate2() {
    }
    FilterConfiguration(
            const BasicTestConfiguration & config,
            const AlignedBlock & source,
            const AlignedBlock & target,
            Mode1 mode,
            size_t predicate)
            : SubTestConfiguration(config, source, target),
              mode1(mode),
              predicate1(predicate),
              mode2(None()),
              predicate2() {
    }
    FilterConfiguration(
            const SubTestConfiguration & config,
            Mode1 mode1,
            size_t predicate1,
            Mode2 mode2,
            size_t predicate2)
            : SubTestConfiguration(config),
              mode1(mode1),
              predicate1(predicate1),
              mode2(mode2),
              predicate2(predicate2) {
    }
    FilterConfiguration(
            const BasicTestConfiguration & config,
            const AlignedBlock & source,
            const AlignedBlock & target,
            Mode1 mode1,
            size_t predicate1,
            Mode2 mode2,
            size_t predicate2)
            : SubTestConfiguration(config, source, target),
              mode1(mode1),
              predicate1(predicate1),
              mode2(mode2),
              predicate2(predicate2) {
    }
};

struct ArithmeticConfiguration :
        public SubTestConfiguration {
    struct Add {
    };
    struct Sub {
    };
    struct Mul {
    };
    struct Div {
    };
    typedef std::variant<Add, Sub, Mul, Div> Mode;
    Mode mode;
    size_t operand;
    ArithmeticConfiguration(
            const SubTestConfiguration & config,
            Mode mode,
            const size_t operand)
            : SubTestConfiguration(config),
              mode(mode),
              operand(operand) {
    }
    ArithmeticConfiguration(
            const BasicTestConfiguration & config,
            const AlignedBlock & source,
            const AlignedBlock & target,
            Mode mode,
            const size_t operand)
            : SubTestConfiguration(config, source, target),
              mode(mode),
              operand(operand) {
    }
};

struct ArithmeticConfigurationModeName {
    const char * operator()(
            ArithmeticConfiguration::Add) {
        return "add";
    }
    const char * operator()(
            ArithmeticConfiguration::Sub) {
        return "sub";
    }
    const char * operator()(
            ArithmeticConfiguration::Mul) {
        return "mul";
    }
    const char * operator()(
            ArithmeticConfiguration::Div) {
        return "div";
    }
};

struct AggregateConfiguration :
        public SubTestConfiguration {
    struct Sum {
    };
    struct Min {
    };
    struct Max {
    };
    struct Avg {
    };
    typedef std::variant<Sum, Min, Max, Avg> Mode;
    Mode mode;
    AggregateConfiguration(
            const SubTestConfiguration & config,
            Mode mode)
            : SubTestConfiguration(config),
              mode(mode) {
    }
    AggregateConfiguration(
            const BasicTestConfiguration & config,
            const AlignedBlock & source,
            const AlignedBlock & target,
            Mode mode)
            : SubTestConfiguration(config, source, target),
              mode(mode) {
    }
};

struct AggregateConfigurationModeName {
    const char * operator()(
            AggregateConfiguration::Sum) {
        return "sum";
    }
    const char * operator()(
            AggregateConfiguration::Min) {
        return "min";
    }
    const char * operator()(
            AggregateConfiguration::Max) {
        return "max";
    }
    const char * operator()(
            AggregateConfiguration::Avg) {
        return "avg";
    }
};

struct ReencodeConfiguration :
        public SubTestConfiguration {
    size_t newA;
    ReencodeConfiguration(
            const SubTestConfiguration & config,
            const size_t newA)
            : SubTestConfiguration(config),
              newA(newA) {
    }
    ReencodeConfiguration(
            const BasicTestConfiguration & config,
            const AlignedBlock & source,
            const AlignedBlock & target,
            const size_t newA)
            : SubTestConfiguration(config, source, target),
              newA(newA) {
    }
};

struct DecodeConfiguration :
        public SubTestConfiguration {
    using SubTestConfiguration::SubTestConfiguration;
};
