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

void TestBase::PreEnc(
        const EncodeConfiguration & config) {
}

bool TestBase::DoCheck() {
    return false;
}

void TestBase::PreCheck(
        const CheckConfiguration & config) {
}

void TestBase::RunCheck(
        const CheckConfiguration & config) {
}

bool TestBase::DoArith() {
    return false;
}

void TestBase::PreArith(
        const ArithmeticConfiguration & config) {
}

void TestBase::RunArith(
        const ArithmeticConfiguration & config) {
}

bool TestBase::DoReenc() {
    return false;
}

void TestBase::PreReenc(
        const ReencodeConfiguration & config) {
}

void TestBase::RunReenc(
        const ReencodeConfiguration & config) {
}

bool TestBase::DoDec() {
    return false;
}

void TestBase::PreDec(
        const DecodeConfiguration & config) {
}

void TestBase::RunDec(
        const DecodeConfiguration & config) {
}

bool TestBase::DoCheckDec() {
    return false;
}

void TestBase::PreCheckDec(
        const CheckAndDecodeConfiguration & config) {
}

void TestBase::RunCheckDec(
        const CheckAndDecodeConfiguration & config) {
}

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
    CheckAndDecodeConfiguration cadConf(configTest);
    int64_t nanos;
    Stopwatch sw;

    // Start test:
    this->PreEnc(encConf);

    TestInfo tiEnc, tiCheck, tiArith, tiReenc, tiDec, tiCheckDec;

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
            this->RunEnc(encConf);
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

    if (this->DoArith()) {
        this->PreArith(arithConf);
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
                this->RunArith(arithConf);
            }
            nanos = sw.Current();
            tiArith.set(nanos);
        } catch (ErrorInfo & ei) {
            auto msg = ei.what();
            std::cerr << msg << std::endl;
            tiArith.set(msg);
        }
    }

    if (this->DoReenc()) {
        this->PreReenc(reencConf);
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
                this->RunReenc(reencConf);
            }
            nanos = sw.Current();
            tiReenc.set(nanos);
        } catch (ErrorInfo & ei) {
            auto msg = ei.what();
            std::cerr << msg << std::endl;
            tiReenc.set(msg);
        }
    }

    if (this->DoDec()) {
        this->PreDec(decConf);
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
                this->RunDec(decConf);
            }
            nanos = sw.Current();
            tiDec.set(nanos);
        } catch (ErrorInfo & ei) {
            auto msg = ei.what();
            std::cerr << msg << std::endl;
            tiDec.set(msg);
        }
    }

    if (this->DoCheckDec()) {
        this->PreCheckDec(cadConf);
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
                this->RunCheckDec(cadConf);
            }
            nanos = sw.Current();
            tiCheckDec.set(nanos);
        } catch (ErrorInfo & ei) {
            auto msg = ei.what();
            std::cerr << msg << std::endl;
            tiCheckDec.set(msg);
        }
    }

    return TestInfos(this->name, getSIMDtypeName(), tiEnc, tiCheck, tiArith, tiReenc, tiDec, tiCheckDec);
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
