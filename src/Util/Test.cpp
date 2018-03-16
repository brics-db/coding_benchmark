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

template<typename Conf, size_t max = std::variant_size_v<typename Conf::Mode>, size_t num = 1>
struct ConfigurationModeExecutor {
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
                    Conf &)> && lambda) {
        conf.mode = typename std::variant_alternative<max - 1, typename Conf::Mode>::type();
        lambda(conf);
    }
};

template<typename PreFunc, typename RunFunc>
void InternalExecute(
        TestBase & test,
        const CheckConfiguration & cc,
        Stopwatch & sw,
        TestInfo & ti,
        PreFunc preFunc,
        RunFunc runFunc) {
    try {
        preFunc();
        sw.Reset();
#ifdef OMP
#ifdef OMPNUMTHREADS
#pragma omp parallel num_threads(OMPNUMTHREADS)
        {
            runFunc();
        }
#else
#pragma omp parallel
        {
            runFunc();
        }
#endif
#else
        runFunc();
#endif
        ti.set(sw.Current());
        test.RunCheck(cc);
    } catch (ErrorInfo & ei) {
        auto msg = ei.what();
        std::cerr << msg << std::endl;
        ti.set(msg);
    }
}

template<typename PreFunc, typename RunFunc, typename SetTimeInfoFunc, typename CatchErrorInfoFunc>
void InternalExecuteMode(
        TestBase & test,
        const CheckConfiguration & cc,
        Stopwatch & sw,
        PreFunc preFunc,
        RunFunc runFunc,
        SetTimeInfoFunc setTimeInfoFunc,
        CatchErrorInfoFunc catchErrorInfoFunc) {
    try {
        preFunc();
        sw.Reset();
#ifdef OMP
#ifdef OMPNUMTHREADS
#pragma omp parallel num_threads(OMPNUMTHREADS)
        {
            runFunc();
        }
#else
#pragma omp parallel
        {
            runFunc();
        }
#endif
#else
        runFunc();
#endif
        auto nanos = sw.Current();
        setTimeInfoFunc(nanos);
        test.RunCheck(cc);
    } catch (ErrorInfo & ei) {
        auto msg = ei.what();
        std::cerr << msg << std::endl;
        catchErrorInfoFunc(msg);
    }
}

template<typename Conf, typename T, size_t max = std::variant_size_v<typename Conf::Mode>, size_t num = 1>
struct SetForMode {
    static void run(
            Conf & conf,
            T data,
            std::array<TestInfo*, max> && arrayTI) {
        try {
            std::get<std::variant_alternative_t<num - 1, typename Conf::Mode>>(conf.mode);
            std::get<num - 1>(arrayTI)->set(data);
        } catch (const std::bad_variant_access&) {
            SetForMode<Conf, T, max, num + 1>::run(conf, data, std::forward<std::array<TestInfo*, max>>(arrayTI));
        }
    }
};

template<typename Conf, typename T, size_t max>
struct SetForMode<Conf, T, max, max> {
    static void run(
            Conf & conf,
            T data,
            std::array<TestInfo*, max> && arrayTI) {
        std::get<std::variant_alternative_t<max - 1, typename Conf::Mode>>(conf.mode);
        std::get<max - 1>(arrayTI)->set(data);
        // if we get an std::bad_variant_access error here, don't catch it here!
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
    CheckConfiguration chkConf(configTest, bufEncoded);
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

    TestInfo tiEnc, tiCheck, tiAdd, tiSub, tiMul, tiDiv, tiAddChk, tiSubChk, tiMulChk, tiDivChk, tiSum, tiMin, tiMax, tiAvg, tiSumChk, tiMinChk, tiMaxChk, tiAvgChk, tiReencChk, tiDec, tiDecChk;

    TestConfiguration tc(1); // we need to check the result buffer only once!
    const CheckConfiguration cc1 = CheckConfiguration(tc, bufEncoded);
    const CheckConfiguration cc2 = CheckConfiguration(tc, bufResult);

    std::clog << "encode" << std::flush;
    InternalExecute(*this, cc1, sw, tiEnc, [this,&encConf] {this->PreEncode(encConf);}, [this,&encConf] {this->RunEncode(encConf);});

    if (configTest.enableCheck && this->DoCheck()) {
        std::clog << ", check" << std::flush;
        InternalExecute(*this, cc1, sw, tiCheck, [this,&chkConf] {this->PreCheck(chkConf);}, [this,&chkConf] {this->RunCheck(chkConf);});
    }

    if (configTest.enableArithmetic) {
        auto func = [this,&cc2,&sw,&tiAdd,&tiSub,&tiMul,&tiDiv] (ArithmeticConfiguration & conf) {
            if (DoArithmetic(conf)) {
                std::clog << ", " << std::visit(ArithmeticConfigurationModeName(), conf.mode);
                auto preFunc = [this,&conf] {
                    PreArithmetic(conf);
                };
                auto runFunc = [this,&conf] {
                    RunArithmetic(conf);
                };
                auto setFunc = [&conf,&tiAdd,&tiSub,&tiMul,&tiDiv] (int64_t nanos) {
                    SetForMode<ArithmeticConfiguration, int64_t>::run(conf, nanos, {&tiAdd,&tiSub,&tiMul,&tiDiv});
                };
                auto catchFunc = [&conf,&tiAdd,&tiSub,&tiMul,&tiDiv] (const char * msg) {
                    SetForMode<ArithmeticConfiguration, const char *>::run(conf, msg, {&tiAdd,&tiSub,&tiMul,&tiDiv});
                };
                InternalExecuteMode(*this, cc2, sw, preFunc, runFunc, setFunc, catchFunc);
            };
        };
        ConfigurationModeExecutor<ArithmeticConfiguration>::run(arithConf, func);
    }

    if (configTest.enableArithmeticChk) {
        auto func = [this,&cc2,&sw,&tiAddChk,&tiSubChk,&tiMulChk,&tiDivChk] (ArithmeticConfiguration & conf) {
            if (DoArithmeticChecked(conf)) {
                std::clog << ", " << std::visit(ArithmeticConfigurationModeName(), conf.mode) << " checked";
                auto preFunc = [this,&conf] {
                    PreArithmeticChecked(conf);
                };
                auto runFunc = [this,&conf] {
                    RunArithmeticChecked(conf);
                };
                auto setFunc = [&conf,&tiAddChk,&tiSubChk,&tiMulChk,&tiDivChk] (int64_t nanos) {
                    SetForMode<ArithmeticConfiguration, int64_t>::run(conf, nanos, {&tiAddChk,&tiSubChk,&tiMulChk,&tiDivChk});
                };
                auto catchFunc = [&conf,&tiAddChk,&tiSubChk,&tiMulChk,&tiDivChk] (const char * msg) {
                    SetForMode<ArithmeticConfiguration, const char *>::run(conf, msg, {&tiAddChk,&tiSubChk,&tiMulChk,&tiDivChk});
                };
                InternalExecuteMode(*this, cc2, sw, preFunc, runFunc, setFunc, catchFunc);
            }
        };
        ConfigurationModeExecutor<ArithmeticConfiguration>::run(arithConf, func);
    }

    if (configTest.enableAggregate) {
        auto func = [this,&cc2,&sw,&tiSum,&tiMin,&tiMax,&tiAvg] (AggregateConfiguration & conf) {
            if (DoAggregate(conf)) {
                std::clog << ", " << std::visit(AggregateConfigurationModeName(), conf.mode);
                auto preFunc = [this,&conf] {
                    PreAggregate(conf);
                };
                auto runFunc = [this,&conf] {
                    RunAggregate(conf);
                };
                auto setFunc = [&conf,&tiSum,&tiMin,&tiMax,&tiAvg] (int64_t nanos) {
                    SetForMode<AggregateConfiguration, int64_t>::run(conf, nanos, {&tiSum,&tiMin,&tiMax,&tiAvg});
                };
                auto catchFunc = [&conf,&tiSum,&tiMin,&tiMax,&tiAvg] (const char * msg) {
                    SetForMode<AggregateConfiguration, const char *>::run(conf, msg, {&tiSum,&tiMin,&tiMax,&tiAvg});
                };
                InternalExecuteMode(*this, cc2, sw, preFunc, runFunc, setFunc, catchFunc);
            }
        };
        ConfigurationModeExecutor<AggregateConfiguration>::run(aggrConf, func);
    }

    if (configTest.enableAggregateChk) {
        auto func = [this,&cc2,&sw,&tiSumChk,&tiMinChk,&tiMaxChk,&tiAvgChk] (AggregateConfiguration & conf) {
            if (DoAggregateChecked(conf)) {
                std::clog << ", " << std::visit(AggregateConfigurationModeName(), conf.mode) << " checked";
                auto preFunc = [this,&conf] {
                    PreAggregateChecked(conf);
                };
                auto runFunc = [this,&conf] {
                    RunAggregateChecked(conf);
                };
                auto setFunc = [&conf,&tiSumChk,&tiMinChk,&tiMaxChk,&tiAvgChk] (int64_t nanos) {
                    SetForMode<AggregateConfiguration, int64_t>::run(conf, nanos, {&tiSumChk,&tiMinChk,&tiMaxChk,&tiAvgChk});
                };
                auto catchFunc = [&conf,&tiSumChk,&tiMinChk,&tiMaxChk,&tiAvgChk] (const char * msg) {
                    SetForMode<AggregateConfiguration, const char *>::run(conf, msg, {&tiSumChk,&tiMinChk,&tiMaxChk,&tiAvgChk});
                };
                InternalExecuteMode(*this, cc2, sw, preFunc, runFunc, setFunc, catchFunc);
            }
        };
        ConfigurationModeExecutor<AggregateConfiguration>::run(aggrConf, func);
    }

    if (configTest.enableReencodeChk && this->DoReencodeChecked()) {
        std::clog << ", reencode checked" << std::flush;
        InternalExecute(*this, cc2, sw, tiReencChk, [this,&reencConf] {this->PreReencodeChecked(reencConf);}, [this,&reencConf] {this->RunReencodeChecked(reencConf);});
    }

    if (configTest.enableDecode && this->DoDecode()) {
        std::clog << ", decode" << std::flush;
        InternalExecute(*this, cc2, sw, tiDec, [this,&decConf] {this->PreDecode(decConf);}, [this,&decConf] {this->RunDecode(decConf);});
    }

    if (configTest.enableDecodeChk && this->DoDecodeChecked()) {
        std::clog << ", decode checked" << std::flush;
        InternalExecute(*this, cc2, sw, tiDecChk, [this,&decConf] {this->PreDecodeChecked(decConf);}, [this,&decConf] {this->RunDecodeChecked(decConf);});
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
