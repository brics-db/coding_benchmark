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

#if __has_include(<variant>)
#include <variant>
#define nspvar std
#elif __has_include(<experimental/variant>)
#include <experimental/variant>
#define nspvar std::experimental
#elif __has_include(<boost/variant.hpp>)
#include <boost/variant.hpp>
#define nspvar boost
#else
#error "Neither of the includes <variant>, <experimental/variant>, nor <boost/variant> was found!"
#endif

struct BasicTestConfiguration {
    const size_t numIterations;

    BasicTestConfiguration(
            const size_t numIterations)
            : numIterations(numIterations) {
    }
};

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
            const size_t numIterations)
            : BasicTestConfiguration(numIterations),
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

struct EncodeConfiguration :
        public BasicTestConfiguration {
    EncodeConfiguration(
            const BasicTestConfiguration & config)
            : BasicTestConfiguration(config) {
    }
};

struct CheckConfiguration :
        public BasicTestConfiguration {
    CheckConfiguration(
            const BasicTestConfiguration & config)
            : BasicTestConfiguration(config) {
    }
};

struct FilterConfiguration :
        public BasicTestConfiguration {
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
    typedef nspvar::variant<LT, LE, EQ, NE, GE, GT> Mode1;
    typedef nspvar::variant<None, LT, LE, EQ, NE, GE, GT> Mode2;
    Mode1 mode1;
    size_t predicate1;
    Mode2 mode2;
    size_t predicate2;
    FilterConfiguration(
            const BasicTestConfiguration & config,
            Mode1 mode,
            size_t predicate)
            : BasicTestConfiguration(config),
              mode1(mode),
              predicate1(predicate),
              mode2(None()),
              predicate2() {
    }
    FilterConfiguration(
            const BasicTestConfiguration & config,
            Mode1 mode1,
            size_t predicate1,
            Mode2 mode2,
            size_t predicate2)
            : BasicTestConfiguration(config),
              mode1(mode1),
              predicate1(predicate1),
              mode2(mode2),
              predicate2(predicate2) {
    }
};

struct ArithmeticConfiguration :
        public BasicTestConfiguration {
    struct Add {
    };
    struct Sub {
    };
    struct Mul {
    };
    struct Div {
    };
    typedef nspvar::variant<Add, Sub, Mul, Div> Mode;
    Mode mode;
    size_t operand;
    ArithmeticConfiguration(
            const BasicTestConfiguration & config,
            Mode mode,
            const size_t operand)
            : BasicTestConfiguration(config),
              mode(mode),
              operand(operand) {
    }
};

struct AggregateConfiguration :
        public BasicTestConfiguration {
    struct Sum {
    };
    struct Min {
    };
    struct Max {
    };
    struct Avg {
    };
    typedef nspvar::variant<Sum, Min, Max, Avg> Mode;
    Mode mode;
    AggregateConfiguration(
            const BasicTestConfiguration & config,
            Mode mode)
            : BasicTestConfiguration(config),
              mode(mode) {
    }
};

struct ReencodeConfiguration :
        public BasicTestConfiguration {
    size_t newA;
    ReencodeConfiguration(
            const BasicTestConfiguration & config,
            const size_t newA)
            : BasicTestConfiguration(config),
              newA(newA) {
    }
};

struct DecodeConfiguration :
        public BasicTestConfiguration {
    DecodeConfiguration(
            const BasicTestConfiguration & config)
            : BasicTestConfiguration(config) {
    }
};
