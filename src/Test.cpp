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
        size_t datawidth,
        const std::string & name,
        AlignedBlock & bufRaw,
        AlignedBlock & bufEncoded,
        AlignedBlock & bufResult)
        : datawidth(datawidth),
          name(name),
          bufRaw(bufRaw),
          bufEncoded(bufEncoded),
          bufResult(bufResult) {
}

TestBase::TestBase(
        TestBase & other)
        : datawidth(other.datawidth),
          name(other.name),
          bufRaw(other.bufRaw),
          bufEncoded(other.bufEncoded),
          bufResult(other.bufResult) {
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

// Filter
bool TestBase::DoFilter() {
    return false;
}

void TestBase::PreFilter(
        const FilterConfiguration & config) {
}

void TestBase::RunFilter(
        const FilterConfiguration & config) {
}

// Filter Checked
bool TestBase::DoFilterChecked() {
    return false;
}

void TestBase::PreFilterChecked(
        const FilterConfiguration & config) {
}

void TestBase::RunFilterChecked(
        const FilterConfiguration & config) {
}

// Arithmetic
bool TestBase::DoArithmetic(
        const ArithmeticConfiguration & config) {
    return false;
}

void TestBase::PreArithmetic(
        const ArithmeticConfiguration & config) {
}

void TestBase::RunArithmetic(
        const ArithmeticConfiguration & config) {
}

// Arithmetic Checked
bool TestBase::DoArithmeticChecked(
        const ArithmeticConfiguration & config) {
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
        return TestInfos(datawidth, this->name, getSIMDtypeName());
    }

    ResetBuffers(configDataGen);

    std::random_device rDev;
    std::default_random_engine rEng(rDev());
    std::uniform_int_distribution<uint16_t> rDist;

    EncodeConfiguration encConf(configTest);
    CheckConfiguration chkConf(configTest);
    ArithmeticConfiguration arithConf(configTest, ArithmeticConfiguration::Mode(ArithmeticConfiguration::Add()), rDist(rEng));
    // Following: use an odd A. As the actual A is not important we can choose an arbitrary one here and simply cast it later to the desired width (i.e. select the appropriate LSBs).
    std::size_t newA = rDist(rEng) | 0x1;
#ifdef DEBUG
    std::clog << "# Reencode uses A=" << newA << " (cast down to the appropriate data length as needed (LSBs extracted)" << std::endl;
#endif
    ReencodeConfiguration reencConf(configTest, newA);
    DecodeConfiguration decConf(configTest);
    AggregateConfiguration aggrSumConf(configTest, AggregateConfiguration::Mode(AggregateConfiguration::Sum()));
    int64_t nanos;
    Stopwatch sw;

    // Start test:
    this->PreEncode(encConf);

    TestInfo tiEnc, tiCheck, tiArith, tiArithChk, tiAggr, tiAggrChk, tiReencChk, tiDec, tiDecChk;

    std::clog << "encode" << std::flush;
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

    if (configTest.enableCheck && this->DoCheck()) {
        this->PreCheck(chkConf);
        std::clog << ", check" << std::flush;
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

    if (configTest.enableArithmetic && this->DoArithmetic(arithConf)) {
        this->PreArithmetic(arithConf);
        std::clog << ", arithmetic" << std::flush;
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

    if (configTest.enableArithmeticChk && this->DoArithmeticChecked(arithConf)) {
        this->PreArithmeticChecked(arithConf);
        std::clog << ", arithmetic checked" << std::flush;
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

    if (configTest.enableAggregate && this->DoAggregate(aggrSumConf)) {
        this->PreAggregate(aggrSumConf);
        std::clog << ", aggregate" << std::flush;
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

    if (configTest.enableAggregateChk && this->DoAggregateChecked(aggrSumConf)) {
        this->PreAggregateChecked(aggrSumConf);
        std::clog << ", aggregate checked" << std::flush;
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

    if (configTest.enableReencodeChk && this->DoReencodeChecked()) {
        this->PreReencodeChecked(reencConf);
        std::clog << ", reencode checked" << std::flush;
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

    if (configTest.enableDecode && this->DoDecode()) {
        this->PreDecode(decConf);
        std::clog << ", decode" << std::flush;
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

    if (configTest.enableDecodeChk && this->DoDecodeChecked()) {
        this->PreDecodeChecked(decConf);
        std::clog << ", decode checked" << std::flush;
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

    return TestInfos(datawidth, this->name, getSIMDtypeName(), tiEnc, tiCheck, tiArith, tiArithChk, tiAggr, tiAggrChk, tiReencChk, tiDec, tiDecChk);
}

ScalarTest::~ScalarTest() {
}

const std::string &
ScalarTest::getSIMDtypeName() {
    static const std::string SIMDtypeName("Scalar");
    return SIMDtypeName;
}

bool ScalarTest::HasCapabilities() {
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
    return cpu.AVX2 && cpu.OS_X64 && cpu.OS_AVX;
}

AVX512Test::~AVX512Test() {
}

const std::string &
AVX512Test::getSIMDtypeName() {
    static const std::string SIMDtypeName("AVX512");
    return SIMDtypeName;
}

bool AVX512Test::HasCapabilities() {
    auto& cpu = CPU::Instance();
    return (cpu.AVX512_BW | cpu.AVX512_CD | cpu.AVX512_DQ | cpu.AVX512_ER | cpu.AVX512_F | cpu.AVX512_IFMA | cpu.AVX512_PF | cpu.AVX512_VBMI | cpu.AVX512_VL) && cpu.OS_X64 && cpu.OS_AVX512;
}
