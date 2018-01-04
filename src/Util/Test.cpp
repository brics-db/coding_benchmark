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
#include <functional>

#ifdef OMP
#include <omp.h>
#endif

#include <Util/Test.hpp>
#include <Util/CPU.hpp>
#include <Util/ErrorInfo.hpp>
#include <Util/Stopwatch.hpp>

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

template<typename Conf, size_t max, size_t num = 1>
struct ConfigurationModeExecutor {
    static void run(
            Conf & conf,
            std::function<void(
                    Conf &)> & lambda) {
        conf.mode = typename std::variant_alternative<num - 1, typename Conf::Mode>::type();
        lambda(conf);
        ConfigurationModeExecutor<Conf, max, num + 1>::run(conf, std::function<void(
                Conf &)>(lambda));
    }

    static void run(
            Conf & conf,
            std::function<void(
                    Conf &)> && lambda) {
        conf.mode = typename std::variant_alternative<num - 1, typename Conf::Mode>::type();
        lambda(conf);
        ConfigurationModeExecutor<Conf, max, num + 1>::run(conf, std::forward<std::function<void(
                Conf &)>>(lambda));
    }
};

template<typename Conf, size_t max>
struct ConfigurationModeExecutor<Conf, max, max> {
    static void run(
            Conf & conf,
            std::function<void(
                    Conf &)> & lambda) {
        conf.mode = typename std::variant_alternative<max - 1, typename Conf::Mode>::type();
        lambda(conf);
    }

    static void run(
            Conf & conf,
            std::function<void(
                    Conf &)> && lambda) {
        conf.mode = typename std::variant_alternative<max - 1, typename Conf::Mode>::type();
        lambda(conf);
    }
};

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
    AggregateConfiguration aggrConf(configTest, AggregateConfiguration::Mode(AggregateConfiguration::Sum()));
    Stopwatch sw;

    // Start test:
    this->PreEncode(encConf);

    TestInfo tiEnc, tiCheck, tiAdd, tiSub, tiMul, tiDiv, tiAddChk, tiSubChk, tiMulChk, tiDivChk, tiSum, tiMin, tiMax, tiAvg, tiSumChk, tiMinChk, tiMaxChk, tiAvgChk, tiReencChk, tiDec, tiDecChk;

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
        tiEnc.set(sw.Current());
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
            tiCheck.set(sw.Current());
        } catch (ErrorInfo & ei) {
            auto msg = ei.what();
            std::cerr << msg << std::endl;
            tiCheck.set(msg);
        }
    }

    if (configTest.enableArithmetic) {
        auto func = [this,&sw,&tiAdd,&tiSub,&tiMul,&tiDiv] (ArithmeticConfiguration & conf) {
            if (DoArithmetic(conf)) {
                PreArithmetic(conf);
                std::clog << ", " << std::visit(ArithmeticConfigurationModeName(), conf.mode);
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
                    RunArithmetic(conf);
                }
                auto nanos = sw.Current();
                try {
                    std::get<ArithmeticConfiguration::Add>(conf.mode);
                    tiAdd.set(nanos);
                } catch (const std::bad_variant_access&) {
                    try {
                        std::get<ArithmeticConfiguration::Sub>(conf.mode);
                        tiSub.set(nanos);
                    } catch (const std::bad_variant_access&) {
                        try {
                            std::get<ArithmeticConfiguration::Mul>(conf.mode);
                            tiMul.set(nanos);
                        } catch (const std::bad_variant_access&) {
                            std::get<ArithmeticConfiguration::Div>(conf.mode);
                            tiDiv.set(nanos);
                        }
                    }
                };
            } catch (ErrorInfo & ei) {
                auto msg = ei.what();
                std::cerr << msg << std::endl;
                try {
                    std::get<ArithmeticConfiguration::Add>(conf.mode);
                    tiAdd.set(msg);
                } catch (const std::bad_variant_access&) {
                    try {
                        std::get<ArithmeticConfiguration::Sub>(conf.mode);
                        tiSub.set(msg);
                    } catch (const std::bad_variant_access&) {
                        try {
                            std::get<ArithmeticConfiguration::Mul>(conf.mode);
                            tiMul.set(msg);
                        } catch (const std::bad_variant_access&) {
                            std::get<ArithmeticConfiguration::Div>(conf.mode);
                            tiDiv.set(msg);
                        }
                    }
                };
            }
        }
    }   ;
        ConfigurationModeExecutor<ArithmeticConfiguration, std::variant_size_v<typename ArithmeticConfiguration::Mode>>::run(arithConf, func);
    }

    if (configTest.enableArithmeticChk) {
        auto func = [this,&sw,&tiAddChk,&tiSubChk,&tiMulChk,&tiDivChk] (ArithmeticConfiguration & conf) {
            if (DoArithmeticChecked(conf)) {
                PreArithmeticChecked(conf);
                std::clog << ", " << std::visit(ArithmeticConfigurationModeName(), conf.mode) << " checked";
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
                    RunArithmeticChecked(conf);
                }
                auto nanos = sw.Current();
                try {
                    std::get<ArithmeticConfiguration::Add>(conf.mode);
                    tiAddChk.set(nanos);
                } catch (const std::bad_variant_access&) {
                    try {
                        std::get<ArithmeticConfiguration::Sub>(conf.mode);
                        tiSubChk.set(nanos);
                    } catch (const std::bad_variant_access&) {
                        try {
                            std::get<ArithmeticConfiguration::Mul>(conf.mode);
                            tiMulChk.set(nanos);
                        } catch (const std::bad_variant_access&) {
                            std::get<ArithmeticConfiguration::Div>(conf.mode);
                            tiDivChk.set(nanos);
                        }
                    }
                };
            } catch (ErrorInfo & ei) {
                auto msg = ei.what();
                std::cerr << msg << std::endl;
                try {
                    std::get<ArithmeticConfiguration::Add>(conf.mode);
                    tiAddChk.set(msg);
                } catch (const std::bad_variant_access&) {
                    try {
                        std::get<ArithmeticConfiguration::Sub>(conf.mode);
                        tiSubChk.set(msg);
                    } catch (const std::bad_variant_access&) {
                        try {
                            std::get<ArithmeticConfiguration::Mul>(conf.mode);
                            tiMulChk.set(msg);
                        } catch (const std::bad_variant_access&) {
                            std::get<ArithmeticConfiguration::Div>(conf.mode);
                            tiDivChk.set(msg);
                        }
                    }
                };
            }
        }
    }   ;
        ConfigurationModeExecutor<ArithmeticConfiguration, std::variant_size_v<typename ArithmeticConfiguration::Mode>>::run(arithConf, func);
    }

    if (configTest.enableAggregate) {
        auto func = [this,&sw,&tiSum,&tiMin,&tiMax,&tiAvg] (AggregateConfiguration & conf) {
            if (DoAggregate(conf)) {
                PreAggregate(conf);
                std::clog << ", " << std::visit(AggregateConfigurationModeName(), conf.mode);
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
                    RunAggregate(conf);
                }
                auto nanos = sw.Current();
                try {
                    std::get<AggregateConfiguration::Sum>(conf.mode);
                    tiSum.set(nanos);
                } catch (const std::bad_variant_access&) {
                    try {
                        std::get<AggregateConfiguration::Min>(conf.mode);
                        tiMin.set(nanos);
                    } catch (const std::bad_variant_access&) {
                        try {
                            std::get<AggregateConfiguration::Max>(conf.mode);
                            tiMax.set(nanos);
                        } catch (const std::bad_variant_access&) {
                            std::get<AggregateConfiguration::Avg>(conf.mode);
                            tiAvg.set(nanos);
                        }
                    }
                };
            } catch (ErrorInfo & ei) {
                auto msg = ei.what();
                std::cerr << msg << std::endl;
                try {
                    std::get<AggregateConfiguration::Sum>(conf.mode);
                    tiSum.set(msg);
                } catch (const std::bad_variant_access&) {
                    try {
                        std::get<AggregateConfiguration::Min>(conf.mode);
                        tiMin.set(msg);
                    } catch (const std::bad_variant_access&) {
                        try {
                            std::get<AggregateConfiguration::Max>(conf.mode);
                            tiMax.set(msg);
                        } catch (const std::bad_variant_access&) {
                            std::get<AggregateConfiguration::Avg>(conf.mode);
                            tiAvg.set(msg);
                        }
                    }
                };
            }
        }
    }   ;
        ConfigurationModeExecutor<AggregateConfiguration, std::variant_size_v<typename AggregateConfiguration::Mode>>::run(aggrConf, func);
    }

    if (configTest.enableAggregateChk) {
        auto func = [this,&sw,&tiSumChk,&tiMinChk,&tiMaxChk,&tiAvgChk] (AggregateConfiguration & conf) {
            if (DoAggregateChecked(conf)) {
                PreAggregateChecked(conf);
                std::clog << ", " << std::visit(AggregateConfigurationModeName(conf), conf.mode) << " checked";
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
                    RunAggregateChecked(conf);
                }
                auto nanos = sw.Current();
                try {
                    std::get<AggregateConfiguration::Sum>(conf.mode);
                    tiSumChk.set(nanos);
                } catch (const std::bad_variant_access&) {
                    try {
                        std::get<AggregateConfiguration::Min>(conf.mode);
                        tiMinChk.set(nanos);
                    } catch (const std::bad_variant_access&) {
                        try {
                            std::get<AggregateConfiguration::Max>(conf.mode);
                            tiMaxChk.set(nanos);
                        } catch (const std::bad_variant_access&) {
                            std::get<AggregateConfiguration::Avg>(conf.mode);
                            tiAvgChk.set(nanos);
                        }
                    }
                };
            } catch (ErrorInfo & ei) {
                auto msg = ei.what();
                std::cerr << msg << std::endl;
                try {
                    std::get<AggregateConfiguration::Sum>(conf.mode);
                    tiSumChk.set(msg);
                } catch (const std::bad_variant_access&) {
                    try {
                        std::get<AggregateConfiguration::Min>(conf.mode);
                        tiMinChk.set(msg);
                    } catch (const std::bad_variant_access&) {
                        try {
                            std::get<AggregateConfiguration::Max>(conf.mode);
                            tiMaxChk.set(msg);
                        } catch (const std::bad_variant_access&) {
                            std::get<AggregateConfiguration::Avg>(conf.mode);
                            tiAvgChk.set(msg);
                        }
                    }
                };
            }
        }
    }   ;
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
            tiReencChk.set(sw.Current());
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
            tiDec.set(sw.Current());
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
            tiDecChk.set(sw.Current());
        } catch (ErrorInfo & ei) {
            auto msg = ei.what();
            std::cerr << msg << std::endl;
            tiDecChk.set(msg);
        }
    }

    return TestInfos(datawidth, this->name, getSIMDtypeName(), tiEnc, tiCheck, tiAdd, tiSub, tiMul, tiDiv, tiAddChk, tiSubChk, tiMulChk, tiDivChk, tiSum, tiMin, tiMax, tiAvg, tiSumChk, tiMinChk,
            tiMaxChk, tiAvgChk, tiReencChk, tiDec, tiDecChk);
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
