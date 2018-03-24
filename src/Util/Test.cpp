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
#include <functional>
#include <cstddef>

#ifdef OMP
#include <omp.h>
#endif

#include <Util/Test.hpp>
#include <Util/CPU.hpp>
#include <Util/ErrorInfo.hpp>
#include <Util/Stopwatch.hpp>

Randomizer Randomizer::instance;

Randomizer::Randomizer()
        : rDev(),
          rEng(rDev()),
          rDist() {
}

int Randomizer::getUniformInt() {
    return instance.rDist(instance.rEng);
}

DataGenerationConfiguration::DataGenerationConfiguration()
        : numEffectiveBitsData(std::nullopt),
          numIneffectiveLSBsData(std::nullopt),
          numEffectiveBitsArithOperand(std::nullopt),
          numIneffectiveLSBsArithOperand(std::nullopt) {
}

DataGenerationConfiguration::DataGenerationConfiguration(
        const size_t numEffectiveLSBs)
        : numEffectiveBitsData(numEffectiveLSBs),
          numIneffectiveLSBsData(std::nullopt),
          numEffectiveBitsArithOperand(std::nullopt),
          numIneffectiveLSBsArithOperand(std::nullopt) {
}

DataGenerationConfiguration::DataGenerationConfiguration(
        const size_t numEffectiveLSBs,
        const size_t numIneffectiveLSBsData)
        : numEffectiveBitsData(numEffectiveLSBs),
          numIneffectiveLSBsData(numIneffectiveLSBsData),
          numEffectiveBitsArithOperand(std::nullopt),
          numIneffectiveLSBsArithOperand(std::nullopt) {
}

DataGenerationConfiguration::DataGenerationConfiguration(
        const size_t numEffectiveLSBs,
        const size_t numIneffectiveLSBsData,
        const size_t numEffectiveBitsArithOperand)
        : numEffectiveBitsData(numEffectiveLSBs),
          numIneffectiveLSBsData(numIneffectiveLSBsData),
          numEffectiveBitsArithOperand(numEffectiveBitsArithOperand),
          numIneffectiveLSBsArithOperand(std::nullopt) {
}

DataGenerationConfiguration::DataGenerationConfiguration(
        const size_t numEffectiveLSBs,
        const size_t numIneffectiveLSBsData,
        const size_t numEffectiveBitsArithOperand,
        const size_t numIneffectiveLSBsArithOperand)
        : numEffectiveBitsData(numEffectiveLSBs),
          numIneffectiveLSBsData(numIneffectiveLSBsData),
          numEffectiveBitsArithOperand(numEffectiveBitsArithOperand),
          numIneffectiveLSBsArithOperand(numIneffectiveLSBsArithOperand) {
}

int DataGenerationConfiguration::getUniformData() const {
    auto x = Randomizer::getUniformInt();
    if (numEffectiveBitsData) {
        x &= ((1ull << numEffectiveBitsData.value()) - 1ull);
    }
    if (numIneffectiveLSBsData) {
        x &= ~((1ull << numIneffectiveLSBsData.value()) - 1ull);
    }
    return x;
}

int DataGenerationConfiguration::getUniformArithOperand() const {
    int x;
    do {
        x = Randomizer::getUniformInt();
        if (numEffectiveBitsArithOperand) {
            x &= ((1ull << numEffectiveBitsArithOperand.value()) - 1ull);
        }
        if (numIneffectiveLSBsArithOperand) {
            x &= ~((1ull << numIneffectiveLSBsArithOperand.value()) - 1ull);
        }
    } while (x == 0); // we do not allow zero-valued operands
    return x;
}

TestBase::TestBase(
        size_t datawidth,
        const std::string & name,
        AlignedBlock & bufRaw,
        AlignedBlock & bufEncoded,
        AlignedBlock & bufResult)
        : internalPreEncodeCalled(false),
          internalPreCheckCalled(false),
          internalPreFilterCalled(false),
          internalPreFilterCheckedCalled(false),
          internalPreArithmeticCalled(false),
          internalPreArithmeticCheckedCalled(false),
          internalPreAggregateCalled(false),
          internalPreAggregateCheckedCalled(false),
          internalPreReencodeCheckedCalled(false),
          internalPreDecodeCalled(false),
          internalPreDecodeCheckedCalled(false),
          datawidth(datawidth),
          name(name),
          bufRaw(bufRaw),
          bufScratchPad(bufResult.nBytes, bufResult.alignment),
          bufEncoded(bufEncoded),
          bufResult(bufResult),
          bufArith(bufResult.nBytes, bufResult.alignment),
          bufDecoded(bufResult.nBytes, bufResult.alignment) {
}

TestBase::TestBase(
        TestBase & other)
        : internalPreEncodeCalled(false),
          internalPreCheckCalled(false),
          internalPreFilterCalled(false),
          internalPreFilterCheckedCalled(false),
          internalPreArithmeticCalled(false),
          internalPreArithmeticCheckedCalled(false),
          internalPreAggregateCalled(false),
          internalPreAggregateCheckedCalled(false),
          internalPreReencodeCheckedCalled(false),
          internalPreDecodeCalled(false),
          internalPreDecodeCheckedCalled(false),
          datawidth(other.datawidth),
          name(other.name),
          bufRaw(other.bufRaw),
          bufScratchPad(other.bufScratchPad),
          bufEncoded(other.bufEncoded),
          bufResult(other.bufResult),
          bufArith(other.bufArith),
          bufDecoded(other.bufDecoded) {
}

TestBase::~TestBase() {
}

// Check-Only
bool TestBase::DoCheck() {
    return false;
}

void TestBase::RunCheck(
        const CheckConfiguration & config) {
}

// Filter
bool TestBase::DoFilter() {
    return false;
}

void TestBase::RunFilter(
        const FilterConfiguration & config) {
}

// Filter Checked
bool TestBase::DoFilterChecked() {
    return false;
}

void TestBase::RunFilterChecked(
        const FilterConfiguration & config) {
}

// Arithmetic
bool TestBase::DoArithmetic(
        const ArithmeticConfiguration & config) {
    return false;
}

void TestBase::RunArithmetic(
        const ArithmeticConfiguration & config) {
}

// Arithmetic Checked
bool TestBase::DoArithmeticChecked(
        const ArithmeticConfiguration & config) {
    return false;
}

void TestBase::RunArithmeticChecked(
        const ArithmeticConfiguration & config) {
}

// Aggregate
bool TestBase::DoAggregate(
        const AggregateConfiguration & config) {
    return false;
}

void TestBase::RunAggregate(
        const AggregateConfiguration & config) {
}

// Aggregate Checked
bool TestBase::DoAggregateChecked(
        const AggregateConfiguration & config) {
    return false;
}

void TestBase::RunAggregateChecked(
        const AggregateConfiguration & config) {
}

// Reencode (Checked)
bool TestBase::DoReencodeChecked() {
    return false;
}

void TestBase::RunReencodeChecked(
        const ReencodeConfiguration & config) {
}

// Decoding-Only
bool TestBase::DoDecode() {
    return false;
}

void TestBase::RunDecode(
        const DecodeConfiguration & config) {
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

#ifdef __SSE4_2__

template
struct SIMDTestBase<__m128i > ;

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

#endif /* __SSE4_2__ */

#ifdef __AVX2__

template
struct SIMDTestBase<__m256i > ;

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

#endif /* __AVX2__ */

#ifdef __AVX512F__

template
struct SIMDTestBase<__m512i > ;

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

#endif /* __AVX512F__ */

template<typename Conf, typename Func, size_t max = std::variant_size_v<typename Conf::Mode>, size_t num = 1>
struct ConfigurationModeExecutor {
    static void run(
            Conf & conf,
            Func lambda) {
        conf.mode = typename std::variant_alternative<num - 1, typename Conf::Mode>::type();
        lambda(conf);
        ConfigurationModeExecutor<Conf, Func, max, num + 1>::run(conf, lambda);
    }
};

template<typename Conf, typename Func, size_t max>
struct ConfigurationModeExecutor<Conf, Func, max, max> {
    static void run(
            Conf & conf,
            Func lambda) {
        conf.mode = typename std::variant_alternative<max - 1, typename Conf::Mode>::type();
        lambda(conf);
    }
};

template<typename PreFunc, typename RunFunc, typename PostFunc>
void InternalExecute(
        TestBase & test,
        Stopwatch & sw,
        TestInfo & ti,
        PreFunc preFunc,
        RunFunc runFunc,
        PostFunc postFunc) {
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
        postFunc();
    } catch (ErrorInfo & ei) {
        auto msg = ei.what();
        std::cerr << msg << std::endl;
        ti.set(msg);
    }
}

template<typename PreFunc, typename RunFunc, typename PostFunc, typename SetTimeInfoFunc, typename CatchErrorInfoFunc>
void InternalExecuteMode(
        TestBase & test,
        Stopwatch & sw,
        PreFunc preFunc,
        RunFunc runFunc,
        PostFunc postFunc,
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
        postFunc();
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
        // if we get an std::bad_variant_access error here, don't catch it, because it's a real error now!
    }
};

static void compare(
        const AlignedBlock & block1,
        const AlignedBlock & block2,
        size_t numBytes) {
    auto beg1 = block1.template begin<char>();
    auto end1 = beg1 + numBytes;
    auto beg2 = block2.template begin<char>();
    auto end2 = beg2 + numBytes;
    auto ret = std::mismatch(beg1, end1, beg2, end2);
    if (ret.first != end1) {
        throw ErrorInfo(static_cast<ssize_t>(*ret.first - *ret.second), static_cast<ssize_t>(std::distance(beg1, ret.first)), __FILE__, __LINE__, "1");
    } else if (ret.second != end2) {
        throw ErrorInfo(static_cast<ssize_t>(*ret.first - *ret.second), static_cast<ssize_t>(std::distance(beg2, ret.second)), __FILE__, __LINE__, "2");
    }
}

// Execute test:
TestInfos TestBase::Execute(
        const TestConfiguration & configTest,
        const DataGenerationConfiguration & configDataGen) {
    if (!this->HasCapabilities()) {
        return TestInfos(datawidth, this->name, getSIMDtypeName());
    }

    ResetBuffers(configDataGen);

    uint16_t arithOperand;
    do {
        arithOperand = static_cast<uint16_t>(configDataGen.getUniformArithOperand());
    } while (arithOperand == 0); /* force non-zero operand */

    EncodeConfiguration encConf(configTest, bufRaw, bufEncoded); // encode from raw buffer to encoded buffer
    CheckConfiguration chkConf(configTest, bufRaw, bufEncoded); // check encoded buffer, with the possibility to also use the raw buffer (RUnCheck must actually check the TARGET buffer!)
    ArithmeticConfiguration arithConf(configTest, bufEncoded, bufResult, ArithmeticConfiguration::Mode(ArithmeticConfiguration::Add()), arithOperand);
    // Following: use an odd A. As the actual A is not important we can choose an arbitrary one here and simply cast it later to the desired width (i.e. select the appropriate LSBs).
    std::size_t newA = static_cast<size_t>(configDataGen.getUniformData()) | 0x1;
#ifdef DEBUG
    std::clog << "# Reencode uses A=" << newA << " (cast down to the appropriate data length as needed (LSBs extracted)" << std::endl;
#endif
    ReencodeConfiguration reencConf(configTest, bufEncoded, bufResult, newA);
    DecodeConfiguration decConf(configTest, bufEncoded, bufResult);
    AggregateConfiguration aggrConf(configTest, bufEncoded, bufResult, AggregateConfiguration::Mode(AggregateConfiguration::Sum()));
    Stopwatch sw;

    TestInfo tiEnc, tiCheck, tiAdd, tiSub, tiMul, tiDiv, tiAddChk, tiSubChk, tiMulChk, tiDivChk, tiSum, tiMin, tiMax, tiAvg, tiSumChk, tiMinChk, tiMaxChk, tiAvgChk, tiReencChk, tiDec, tiDecChk;

    TestConfiguration tcSingleIter(1, configTest.numValues); // we need to check the result buffer only once!
    TestConfiguration tcTwoValue(1, 2); // we need to check the result buffer only once and for the aggregates only a single value! We check 2 values, because sum and avg require larger ones. The test must respect this!

    {
        std::clog << "encode" << std::flush;
        auto postFunc = [this,&tcSingleIter] {
            if (!this->internalPreEncodeCalled) {
                throw ErrorInfo(__FILE__, __LINE__, static_cast<size_t>(-1), static_cast<size_t>(-1), "Test::PreEncode() was not called!");
            }
            const DecodeConfiguration ccDec = DecodeConfiguration(tcSingleIter, bufEncoded, bufDecoded);
            this->RunDecodeChecked(ccDec);
            compare(this->bufRaw, this->bufDecoded, this->bufRaw.nBytes);
        };
        InternalExecute(*this, sw, tiEnc, [this,&encConf] {this->PreEncode(encConf);}, [this,&encConf] {this->RunEncode(encConf);}, postFunc);
    }

    if (configTest.enableCheck && this->DoCheck()) {
        std::clog << ", check" << std::flush;
        InternalExecute(*this, sw, tiCheck, [this,&chkConf] {this->PreCheck(chkConf);}, [this,&chkConf] {this->RunCheck(chkConf);}, [this] {}); // no need to check again
    }

    if (configTest.enableArithmetic) {
        auto func = [this,&sw,&tiAdd,&tiSub,&tiMul,&tiDiv,&tcSingleIter] (ArithmeticConfiguration & conf) {
            if (DoArithmetic(conf)) {
                std::clog << ", " << std::visit(ArithmeticConfigurationModeName(), conf.mode);
                auto preFunc = [this,&conf] {
                    this->PreArithmetic(conf);
                };
                auto runFunc = [this,&conf] {
                    this->RunArithmetic(conf);
                };
                auto postFunc = [this,&tcSingleIter] {
                    if (!this->internalPreArithmeticCalled) {
                        throw ErrorInfo(__FILE__, __LINE__, static_cast<size_t>(-1), static_cast<size_t>(-1), "Test::PreArithmetic() was not called!");
                    }
                    const DecodeConfiguration ccDec(tcSingleIter, bufResult, bufDecoded);
                    this->RunDecodeChecked(ccDec);
                    compare(this->bufArith, this->bufDecoded, this->bufArith.nBytes);
                };
                auto setFunc = [&conf,&tiAdd,&tiSub,&tiMul,&tiDiv] (int64_t nanos) {
                    SetForMode<ArithmeticConfiguration, int64_t>::run(conf, nanos, { {&tiAdd,&tiSub,&tiMul,&tiDiv}});
                };
                auto catchFunc = [&conf,&tiAdd,&tiSub,&tiMul,&tiDiv] (const char * msg) {
                    SetForMode<ArithmeticConfiguration, const char *>::run(conf, msg, { {&tiAdd,&tiSub,&tiMul,&tiDiv}});
                };
                InternalExecuteMode(*this, sw, preFunc, runFunc, postFunc, setFunc, catchFunc);
            };
        };
        ConfigurationModeExecutor<ArithmeticConfiguration, typeof(func)>::run(arithConf, func);
    }

    if (configTest.enableArithmeticChk) {
        auto func = [this,&sw,&tiAddChk,&tiSubChk,&tiMulChk,&tiDivChk,&tcSingleIter] (ArithmeticConfiguration & conf) {
            if (DoArithmeticChecked(conf)) {
                std::clog << ", " << std::visit(ArithmeticConfigurationModeName(), conf.mode) << " checked";
                auto preFunc = [this,&conf] {
                    this->PreArithmeticChecked(conf);
                };
                auto runFunc = [this,&conf] {
                    this->RunArithmeticChecked(conf);
                };
                auto postFunc = [this,&tcSingleIter] {
                    if (!this->internalPreArithmeticCheckedCalled) {
                        throw ErrorInfo(__FILE__, __LINE__, static_cast<size_t>(-1), static_cast<size_t>(-1), "Test::PreArithmeticChecked() was not called!");
                    }
                    const DecodeConfiguration ccDec(tcSingleIter, bufResult, bufDecoded);
                    this->RunDecodeChecked(ccDec);
                    compare(this->bufArith, this->bufDecoded, this->bufArith.nBytes);
                };
                auto setFunc = [&conf,&tiAddChk,&tiSubChk,&tiMulChk,&tiDivChk] (int64_t nanos) {
                    SetForMode<ArithmeticConfiguration, int64_t>::run(conf, nanos, { {&tiAddChk,&tiSubChk,&tiMulChk,&tiDivChk}});
                };
                auto catchFunc = [&conf,&tiAddChk,&tiSubChk,&tiMulChk,&tiDivChk] (const char * msg) {
                    SetForMode<ArithmeticConfiguration, const char *>::run(conf, msg, { {&tiAddChk,&tiSubChk,&tiMulChk,&tiDivChk}});
                };
                InternalExecuteMode(*this, sw, preFunc, runFunc, postFunc, setFunc, catchFunc);
            }
        };
        ConfigurationModeExecutor<ArithmeticConfiguration, typeof(func)>::run(arithConf, func);
    }

    if (configTest.enableAggregate) {
        auto func = [this,&sw,&tiSum,&tiMin,&tiMax,&tiAvg,&tcTwoValue] (AggregateConfiguration & conf) {
            if (DoAggregate(conf)) {
                std::clog << ", " << std::visit(AggregateConfigurationModeName(), conf.mode);
                auto preFunc = [this,&conf] {
                    PreAggregate(conf);
                };
                auto runFunc = [this,&conf] {
                    RunAggregate(conf);
                };
                auto postFunc = [this,&tcTwoValue] {
                    if (!this->internalPreAggregateCalled) {
                        throw ErrorInfo(__FILE__, __LINE__, static_cast<size_t>(-1), static_cast<size_t>(-1), "Test::PreAggregate() was not called!");
                    }
                    const DecodeConfiguration ccDec(tcTwoValue, bufResult, bufDecoded);
                    this->RunDecodeChecked(ccDec);
                    compare(this->bufArith, this->bufDecoded, 2 * this->getEncodedDataTypeSize());
                };
                auto setFunc = [&conf,&tiSum,&tiMin,&tiMax,&tiAvg] (int64_t nanos) {
                    SetForMode<AggregateConfiguration, int64_t>::run(conf, nanos, { {&tiSum,&tiMin,&tiMax,&tiAvg}});
                };
                auto catchFunc = [&conf,&tiSum,&tiMin,&tiMax,&tiAvg] (const char * msg) {
                    SetForMode<AggregateConfiguration, const char *>::run(conf, msg, { {&tiSum,&tiMin,&tiMax,&tiAvg}});
                };
                InternalExecuteMode(*this, sw, preFunc, runFunc, postFunc, setFunc, catchFunc);
            }
        };
        ConfigurationModeExecutor<AggregateConfiguration, typeof(func)>::run(aggrConf, func);
    }

    if (configTest.enableAggregateChk) {
        auto func = [this,&sw,&tiSumChk,&tiMinChk,&tiMaxChk,&tiAvgChk,&tcTwoValue] (AggregateConfiguration & conf) {
            if (DoAggregateChecked(conf)) {
                std::clog << ", " << std::visit(AggregateConfigurationModeName(), conf.mode) << " checked";
                auto preFunc = [this,&conf] {
                    PreAggregateChecked(conf);
                };
                auto runFunc = [this,&conf] {
                    RunAggregateChecked(conf);
                };
                auto postFunc = [this,&tcTwoValue] {
                    if (!this->internalPreAggregateCheckedCalled) {
                        throw ErrorInfo(__FILE__, __LINE__, static_cast<size_t>(-1), static_cast<size_t>(-1), "Test::PreAggregateChecked() was not called!");
                    }
                    const DecodeConfiguration ccDec(tcTwoValue, bufResult, bufDecoded);
                    this->RunDecodeChecked(ccDec);
                    compare(this->bufArith, this->bufDecoded, 2 * this->getEncodedDataTypeSize());
                };
                auto setFunc = [&conf,&tiSumChk,&tiMinChk,&tiMaxChk,&tiAvgChk] (int64_t nanos) {
                    SetForMode<AggregateConfiguration, int64_t>::run(conf, nanos, { {&tiSumChk,&tiMinChk,&tiMaxChk,&tiAvgChk}});
                };
                auto catchFunc = [&conf,&tiSumChk,&tiMinChk,&tiMaxChk,&tiAvgChk] (const char * msg) {
                    SetForMode<AggregateConfiguration, const char *>::run(conf, msg, { {&tiSumChk,&tiMinChk,&tiMaxChk,&tiAvgChk}});
                };
                InternalExecuteMode(*this, sw, preFunc, runFunc, postFunc, setFunc, catchFunc);
            }
        };
        ConfigurationModeExecutor<AggregateConfiguration, typeof(func)>::run(aggrConf, func);
    }

    if (configTest.enableReencodeChk && this->DoReencodeChecked()) {
        std::clog << ", reencode checked" << std::flush;
        auto preFunc = [this,&reencConf] {
            this->PreReencodeChecked(reencConf);
        };
        auto runFunc = [this,&reencConf] {
            this->RunReencodeChecked(reencConf);
        };
        auto postFunc = [this,&configTest] {
            if (!this->internalPreReencodeCheckedCalled) {
                throw ErrorInfo(__FILE__, __LINE__, static_cast<size_t>(-1), static_cast<size_t>(-1), "Test::PreReencodeChecked() was not called!");
            }
            const DecodeConfiguration ccDec(configTest, bufResult, bufDecoded);
            this->RunDecodeChecked(ccDec);
            compare(this->bufRaw, this->bufDecoded, configTest.numValues);
        };
        InternalExecute(*this, sw, tiReencChk, preFunc, runFunc, postFunc);
    }

    if (configTest.enableDecode && this->DoDecode()) {
        std::clog << ", decode" << std::flush;
        auto preFunc = [this,&decConf] {
            this->PreDecode(decConf);
        };
        auto runFunc = [this,&decConf] {
            this->RunDecode(decConf);
        };
        auto postFunc = [this,&configTest] {
            if (!this->internalPreDecodeCalled) {
                throw ErrorInfo(__FILE__, __LINE__, static_cast<size_t>(-1), static_cast<size_t>(-1), "Test::PreDecode() was not called!");
            }
            compare(this->bufRaw, this->bufResult, configTest.numValues);
        };
        InternalExecute(*this, sw, tiDec, preFunc, runFunc, postFunc);
    }

    if (configTest.enableDecodeChk) {
        std::clog << ", decode checked" << std::flush;
        auto preFunc = [this,&decConf] {
            this->PreDecodeChecked(decConf);
        };
        auto runFunc = [this,&decConf] {
            this->RunDecodeChecked(decConf);
        };
        auto postFunc = [this,&configTest] {
            if (!this->internalPreDecodeCheckedCalled) {
                throw ErrorInfo(__FILE__, __LINE__, static_cast<size_t>(-1), static_cast<size_t>(-1), "Test::PreDecodeChecked() was not called!");
            }
            compare(this->bufRaw, this->bufResult, configTest.numValues);
        };
        InternalExecute(*this, sw, tiDecChk, preFunc, runFunc, postFunc);
    }

    return TestInfos(datawidth, this->name, getSIMDtypeName(), tiEnc, tiCheck, tiAdd, tiSub, tiMul, tiDiv, tiAddChk, tiSubChk, tiMulChk, tiDivChk, tiSum, tiMin, tiMax, tiAvg, tiSumChk, tiMinChk,
            tiMaxChk, tiAvgChk, tiReencChk, tiDec, tiDecChk);
}
