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

#include <cstring>
#include <iostream>
#include <random>

#ifdef OMP
#include <omp.h>
#endif

#include "Test.hpp"
#include "Util/CPU.hpp"
#include "Util/ErrorInfo.hpp"
#include "Util/Stopwatch.hpp"

TestBase::TestBase(
        const std::string & name,
        AlignedBlock & in,
        AlignedBlock & out)
        : name(name),
          in(in),
          out(out) {
}

TestBase::TestBase(
        TestBase & other)
        : name(other.name),
          in(other.in),
          out(other.out) {
}

TestBase::~TestBase() {
}

// Encoding
void TestBase::PreEncode(
        const EncodeConfiguration & config) {
}

// Check-Only
bool TestBase::DoCheck() {
    return false;
}

void TestBase::PreCheck(
        const CheckConfiguration & config) {
}

void TestBase::RunCheck(
        const CheckConfiguration & config) {
}

// Arithmetic
bool TestBase::DoArithmetic() {
    return false;
}

void TestBase::PreArithmetic(
        const ArithmeticConfiguration & config) {
}

void TestBase::RunArithmetic(
        const ArithmeticConfiguration & config) {
}

// Arithmetic Checked
bool TestBase::DoArithmeticChecked() {
    return false;
}

void TestBase::PreArithmeticChecked(
        const ArithmeticConfiguration & config) {
}

void TestBase::RunArithmeticChecked(
        const ArithmeticConfiguration & config) {
}

// Aggregate
bool TestBase::DoAggregate(
        const AggregateConfiguration & config) {
    return false;
}

void TestBase::PreAggregate(
        const AggregateConfiguration & config) {
}

void TestBase::RunAggregate(
        const AggregateConfiguration & config) {
}

// Aggregate Checked
bool TestBase::DoAggregateChecked(
        const AggregateConfiguration & config) {
    return false;
}

void TestBase::PreAggregateChecked(
        const AggregateConfiguration & config) {
}

void TestBase::RunAggregateChecked(
        const AggregateConfiguration & config) {
}

// Reencode (Checked)
bool TestBase::DoReencodeChecked() {
    return false;
}

void TestBase::PreReencodeChecked(
        const ReencodeConfiguration & config) {
}

void TestBase::RunReencodeChecked(
        const ReencodeConfiguration & config) {
}

// Decoding-Only
bool TestBase::DoDecode() {
    return false;
}

void TestBase::PreDecode(
        const DecodeConfiguration & config) {
}

void TestBase::RunDecode(
        const DecodeConfiguration & config) {
}

// Check-And-Decode
bool TestBase::DoDecodeChecked() {
    return false;
}

void TestBase::PreDecodeChecked(
        const DecodeConfiguration & config) {
}

void TestBase::RunDecodeChecked(
        const DecodeConfiguration & config) {
}

// Execute test:
TestInfos TestBase::Execute(
        const TestConfiguration & configTest,
        const DataGenerationConfiguration & configDataGen) {
    if (!this->HasCapabilities()) {
        return TestInfos(this->name, getSIMDtypeName());
    }

    ResetBuffers(configDataGen);

    std::random_device rDev;
    std::default_random_engine rEng(rDev());
    std::uniform_int_distribution<uint16_t> rDist;

    EncodeConfiguration encConf(configTest);
    CheckConfiguration chkConf(configTest);
    ArithmeticConfiguration arithConf(configTest, rDist(rEng));
    // Following: use an odd A. As the actual A is not important we can choose an arbitrary one here and simply cast it later to the desired width (i.e. select the appropriate LSBs).
    std::size_t newA = rDist(rEng) | 0x1;
#ifdef DEBUG
    std::cerr << "# Reencode uses A=" << newA << " (cast down to the appropriate data length as needed (LSBs extracted)" << std::endl;
#endif
    ReencodeConfiguration reencConf(configTest, newA);
    DecodeConfiguration decConf(configTest);
    AggregateConfiguration aggrSumConf(configTest, AggregateConfiguration::Mode(AggregateConfiguration::Sum()));
    int64_t nanos;
    Stopwatch sw;

    // Start test:
    this->PreEncode(encConf);

    TestInfo tiEnc, tiCheck, tiArith, tiArithChk, tiAggr, tiAggrChk, tiReencChk, tiDec, tiDecChk;

    sw.Reset();
    try {
#ifdef OMP
#ifdef OMPNUMTHREADS
#pragma omp parallel num_threads(OMPNUMTHREADS)
#else
#pragma omp parallel
#endif
#endif
        {
            this->RunEncode(encConf);
        }
        nanos = sw.Current();
        tiEnc.set(nanos);
    } catch (ErrorInfo & ei) {
        auto msg = ei.what();
        std::cerr << msg << std::endl;
        tiEnc.set(msg);
    }

    if (this->DoCheck()) {
        this->PreCheck(chkConf);

        sw.Reset();
        try {
#ifdef OMP
#ifdef OMPNUMTHREADS
#pragma omp parallel num_threads(OMPNUMTHREADS)
#else
#pragma omp parallel
#endif
#endif
            {
                this->RunCheck(chkConf);
            }
            nanos = sw.Current();
            tiCheck.set(nanos);
        } catch (ErrorInfo & ei) {
            auto msg = ei.what();
            std::cerr << msg << std::endl;
            tiCheck.set(msg);
        }
    }

    if (this->DoArithmetic()) {
        this->PreArithmetic(arithConf);
        sw.Reset();
        try {
#ifdef OMP
#ifdef OMPNUMTHREADS
#pragma omp parallel num_threads(OMPNUMTHREADS)
#else
#pragma omp parallel
#endif
#endif
            {
                this->RunArithmetic(arithConf);
            }
            nanos = sw.Current();
            tiArith.set(nanos);
        } catch (ErrorInfo & ei) {
            auto msg = ei.what();
            std::cerr << msg << std::endl;
            tiArith.set(msg);
        }
    }

    if (this->DoArithmeticChecked()) {
        this->PreArithmeticChecked(arithConf);
        sw.Reset();
        try {
#ifdef OMP
#ifdef OMPNUMTHREADS
#pragma omp parallel num_threads(OMPNUMTHREADS)
#else
#pragma omp parallel
#endif
#endif
            {
                this->RunArithmeticChecked(arithConf);
            }
            nanos = sw.Current();
            tiArithChk.set(nanos);
        } catch (ErrorInfo & ei) {
            auto msg = ei.what();
            std::cerr << msg << std::endl;
            tiArithChk.set(msg);
        }
    }

    if (this->DoAggregate(aggrSumConf)) {
        this->PreAggregate(aggrSumConf);
        sw.Reset();
        try {
#ifdef OMP
#ifdef OMPNUMTHREADS
#pragma omp parallel num_threads(OMPNUMTHREADS)
#else
#pragma omp parallel
#endif
#endif
            {
                this->RunAggregate(aggrSumConf);
            }
            nanos = sw.Current();
            tiAggr.set(nanos);
        } catch (ErrorInfo & ei) {
            auto msg = ei.what();
            std::cerr << msg << std::endl;
            tiAggr.set(msg);
        }
    }

    if (this->DoAggregateChecked(aggrSumConf)) {
        this->PreAggregateChecked(aggrSumConf);
        sw.Reset();
        try {
#ifdef OMP
#ifdef OMPNUMTHREADS
#pragma omp parallel num_threads(OMPNUMTHREADS)
#else
#pragma omp parallel
#endif
#endif
            {
                this->RunAggregateChecked(aggrSumConf);
            }
            nanos = sw.Current();
            tiAggrChk.set(nanos);
        } catch (ErrorInfo & ei) {
            auto msg = ei.what();
            std::cerr << msg << std::endl;
            tiAggrChk.set(msg);
        }
    }

    if (this->DoReencodeChecked()) {
        this->PreReencodeChecked(reencConf);
        sw.Reset();
        try {
#ifdef OMP
#ifdef OMPNUMTHREADS
#pragma omp parallel num_threads(OMPNUMTHREADS)
#else
#pragma omp parallel
#endif
#endif
            {
                this->RunReencodeChecked(reencConf);
            }
            nanos = sw.Current();
            tiReencChk.set(nanos);
        } catch (ErrorInfo & ei) {
            auto msg = ei.what();
            std::cerr << msg << std::endl;
            tiReencChk.set(msg);
        }
    }

    if (this->DoDecode()) {
        this->PreDecode(decConf);
        sw.Reset();
        try {
#ifdef OMP
#ifdef OMPNUMTHREADS
#pragma omp parallel num_threads(OMPNUMTHREADS)
#else
#pragma omp parallel
#endif
#endif
            {
                this->RunDecode(decConf);
            }
            nanos = sw.Current();
            tiDec.set(nanos);
        } catch (ErrorInfo & ei) {
            auto msg = ei.what();
            std::cerr << msg << std::endl;
            tiDec.set(msg);
        }
    }

    if (this->DoDecodeChecked()) {
        this->PreDecodeChecked(decConf);
        sw.Reset();
        try {
#ifdef OMP
#ifdef OMPNUMTHREADS
#pragma omp parallel num_threads(OMPNUMTHREADS)
#else
#pragma omp parallel
#endif
#endif
            {
                this->RunDecodeChecked(decConf);
            }
            nanos = sw.Current();
            tiDecChk.set(nanos);
        } catch (ErrorInfo & ei) {
            auto msg = ei.what();
            std::cerr << msg << std::endl;
            tiDecChk.set(msg);
        }
    }

    return TestInfos(this->name, getSIMDtypeName(), tiEnc, tiCheck, tiArith, tiArithChk, tiAggr, tiAggrChk, tiReencChk, tiDec, tiDecChk);
}

SequentialTest::~SequentialTest() {
}

const std::string &
SequentialTest::getSIMDtypeName() {
    static const std::string SIMDtypeName("Seq");
    return SIMDtypeName;
}

bool SequentialTest::HasCapabilities() {
    return true;
}

SSE42Test::~SSE42Test() {
}

const std::string &
SSE42Test::getSIMDtypeName() {
    static const std::string SIMDtypeName("SSE4.2");
    return SIMDtypeName;
}

bool SSE42Test::HasCapabilities() {
    auto& cpu = CPU::Instance();
    return cpu.SSE42 && cpu.OS_X64;
}

AVX2Test::~AVX2Test() {
}

const std::string &
AVX2Test::getSIMDtypeName() {
    static const std::string SIMDtypeName("AVX2");
    return SIMDtypeName;
}

bool AVX2Test::HasCapabilities() {
    auto& cpu = CPU::Instance();
    return cpu.AVX2 && cpu.OS_X64;
}
