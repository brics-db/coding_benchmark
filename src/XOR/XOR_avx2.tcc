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

#pragma once

#include <XOR/XOR_base.hpp>
#include <Test.hpp>
#include <Util/Intrinsics.hpp>
#include <Util/ErrorInfo.hpp>

template<>
struct XOR<__m256i, __m256i> {
static __m256i
computeFinalChecksum(
        __m256i & checksum);
};

template<>
struct XOR<__m256i, uint32_t> {
    static uint32_t
    computeFinalChecksum(
            __m256i & checksum);
};

template<>
struct XOR<__m256i, uint16_t> {
    static uint16_t
    computeFinalChecksum(
            __m256i & checksum);
};

template<>
struct XOR<__m256i, uint8_t> {
    static uint8_t
    computeFinalChecksum(
            __m256i & checksum);
};

template<>
struct XORdiff<__m256i> {
static bool
checksumsDiffer(
        __m256i checksum1,
        __m256i checksum2);
};

template<typename DATA, typename CS, size_t BLOCKSIZE>
struct XOR_avx2 :
        public Test<DATA, CS> {

    using Test<DATA, CS>::Test;

    virtual ~XOR_avx2() {
    }

    size_t getNumValues() {
        return this->bufRaw.template end<DATA>() - this->bufRaw.template begin<DATA>();
    }

    void RunEncode(
            const EncodeConfiguration & config) override {
        for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
            _ReadWriteBarrier();
            auto dataIn = this->bufRaw.template begin<__m256i >();
            auto dataInEnd = this->bufRaw.template end<__m256i >();
            auto pChkOut = this->bufEncoded.template begin<CS>();
            while (dataIn <= (dataInEnd - BLOCKSIZE)) {
                __m256i checksum = _mm256_setzero_si256();
                auto pDataOut = reinterpret_cast<__m256i *>(pChkOut);
                for (size_t k = 0; k < BLOCKSIZE; ++k) {
                    auto tmp = _mm256_lddqu_si256(dataIn++);
                    _mm256_storeu_si256(pDataOut++, tmp);
                    checksum = _mm256_xor_si256(checksum, tmp);
                }
                pChkOut = reinterpret_cast<CS*>(pDataOut);
                *pChkOut++ = XOR<__m256i, CS>::computeFinalChecksum(checksum);
            }
            // checksum remaining values which do not fit in the block size
            if (dataIn <= (dataInEnd - 1)) {
                __m256i checksum = _mm256_setzero_si256();
                auto dataOut2 = reinterpret_cast<__m256i *>(pChkOut);
                do {
                    auto tmp = _mm256_lddqu_si256(dataIn++);
                    _mm256_storeu_si256(dataOut2++, tmp);
                    checksum = _mm256_xor_si256(checksum, tmp);
                } while (dataIn <= (dataInEnd - 1));
                pChkOut = reinterpret_cast<CS*>(dataOut2);
                *pChkOut++ = XOR<__m256i, CS>::computeFinalChecksum(checksum);
            }
            // checksum remaining integers which do not fit in the SIMD register
            if (dataIn < dataInEnd) {
                DATA checksum = 0;
                auto dataEnd = reinterpret_cast<DATA*>(dataInEnd);
                auto pDataOut = reinterpret_cast<DATA*>(pChkOut);
                for (auto data = reinterpret_cast<DATA*>(dataIn); data < dataEnd; ++data) {
                    auto & tmp = *data;
                    *pDataOut++ = tmp;
                    checksum ^= tmp;
                }
                *pDataOut = XOR<DATA, DATA>::computeFinalChecksum(checksum);
            }
        }
    }

    virtual bool DoCheck() override {
        return true;
    }

    virtual void RunCheck(
            const CheckConfiguration & config) override {
        for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
            _ReadWriteBarrier();
            const size_t NUM_VALUES_PER_VECTOR = sizeof(__m256i) / sizeof (DATA);
            const size_t NUM_VALUES_PER_BLOCK = BLOCKSIZE * NUM_VALUES_PER_VECTOR;
            size_t numValues = getNumValues();
            size_t i = 0;
            auto data256 = this->bufEncoded.template begin<__m256i >();
            while (i <= (numValues - NUM_VALUES_PER_BLOCK)) {
                __m256i checksum = _mm256_setzero_si256();
                for (size_t k = 0; k < BLOCKSIZE; ++k) {
                    checksum = _mm256_xor_si256(checksum, _mm256_lddqu_si256(data256++));
                }
                auto pChksum = reinterpret_cast<CS*>(data256);
                if (XORdiff<CS>::checksumsDiffer(*pChksum, XOR<__m256i, CS>::computeFinalChecksum(checksum))) {
                    throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                }
                ++pChksum; // fourth, advance after the checksum to the next block of values
                data256 = reinterpret_cast<__m256i *>(pChksum);
                i += NUM_VALUES_PER_BLOCK;
            }
            // checksum remaining values which do not fit in the block size
            if (i <= (numValues - NUM_VALUES_PER_VECTOR)) {
                __m256i checksum = _mm256_setzero_si256();
                do {
                    checksum = _mm256_xor_si256(checksum, _mm256_lddqu_si256(data256++));
                    i += NUM_VALUES_PER_VECTOR;
                } while (i <= (numValues - NUM_VALUES_PER_VECTOR));
                auto pChksum = reinterpret_cast<CS*>(data256);
                if (XORdiff<CS>::checksumsDiffer(*pChksum, XOR<__m256i, CS>::computeFinalChecksum(checksum))) {
                    throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                }
                ++pChksum; // fourth, advance after the checksum to the next block of values
                data256 = reinterpret_cast<__m256i *>(pChksum);
            }
            // checksum remaining integers which do not fit in the SIMD register, so we do it on the actual data width denoted by template parameter IN
            if (i < numValues) {
                DATA checksum = 0;
                auto data = reinterpret_cast<DATA*>(data256);
                for (; i < numValues; ++i) {
                    checksum ^= *data++;
                }
                if (XORdiff<DATA>::checksumsDiffer(*data, XOR<DATA, DATA>::computeFinalChecksum(checksum))) {
                    throw ErrorInfo(__FILE__, __LINE__, i, iteration);
                }
            }
        }
    }

    struct ArithmeticSelector {
        DATA operator()(
                ArithmeticConfiguration::Add) {
            return true;
        }
        DATA operator()(
                ArithmeticConfiguration::Sub) {
            return false;
        }
        DATA operator()(
                ArithmeticConfiguration::Mul) {
            return false;
        }
        DATA operator()(
                ArithmeticConfiguration::Div) {
            return false;
        }
    };

    bool DoArithmetic(
            const ArithmeticConfiguration & config) override {
        return std::visit(ArithmeticSelector(), config.mode);
    }

    struct Arithmetor {
        XOR_avx2 & test;
        const ArithmeticConfiguration & config;
        Arithmetor(
                XOR_avx2 & test,
                const ArithmeticConfiguration & config)
                : test(test),
                  config(config) {
        }
        void operator()(
                ArithmeticConfiguration::Add) {
            const size_t NUM_VALUES_PER_VECTOR = sizeof(__m256i) / sizeof (DATA);
            const size_t NUM_VALUES_PER_BLOCK = BLOCKSIZE * NUM_VALUES_PER_VECTOR;
            size_t numValues = test.getNumValues();
            size_t i = 0;
            auto data256In = test.bufEncoded.template begin<__m256i>();
            auto data256Out = test.bufResult.template begin<__m256i>();
            while (i <= (numValues - NUM_VALUES_PER_BLOCK)) {
                __m256i checksum = _mm256_setzero_si256();
                for (size_t k = 0; k < BLOCKSIZE; ++k) {
                    auto mmTmp = _mm256_add_epi16(_mm256_lddqu_si256(data256In++), _mm256_set1_epi16(config.operand)); // TODO this is only for 16-bit values!
                    checksum = _mm256_xor_si256(checksum, mmTmp);
                    _mm256_storeu_si256(data256Out++, mmTmp);
                }
                auto pChkOut = reinterpret_cast<CS*>(data256Out);
                *pChkOut++ = XOR<__m256i, CS>::computeFinalChecksum(checksum);
                data256Out = reinterpret_cast<__m256i*>(pChkOut);
                data256In = reinterpret_cast<__m256i*>(reinterpret_cast<CS*>(data256In) + 1);
                i += NUM_VALUES_PER_BLOCK;
            }
            // checksum remaining values which do not fit in the block size
            if (i <= (numValues - NUM_VALUES_PER_VECTOR)) {
                __m256i checksum = _mm256_setzero_si256();
                do {
                    auto mmTmp = _mm256_add_epi16(_mm256_lddqu_si256(data256In++), _mm256_set1_epi16(config.operand)); // TODO this is only for 16-bit values!
                    checksum = _mm256_xor_si256(checksum, mmTmp);
                    _mm256_storeu_si256(data256Out++, mmTmp);
                    i += NUM_VALUES_PER_VECTOR;
                }while (i <= (numValues - NUM_VALUES_PER_VECTOR));
                auto pChkOut = reinterpret_cast<CS*>(data256Out);
                *pChkOut++ = XOR<__m256i, CS>::computeFinalChecksum(checksum);
                data256Out = reinterpret_cast<__m256i*>(pChkOut);
                data256In = reinterpret_cast<__m256i*>(reinterpret_cast<CS*>(data256In) + 1);
            }
            // checksum remaining integers which do not fit in the SIMD register, so we do it on the actual data width denoted by template parameter IN
            if (i < numValues) {
                DATA checksum = 0;
                auto dataIn = reinterpret_cast<DATA*>(data256In);
                auto dataOut = reinterpret_cast<DATA*>(data256Out);
                for (; i < numValues; ++i) {
                    const auto tmp = *dataIn++ + config.operand;
                    checksum ^= tmp;
                    *dataOut++ = tmp;
                }
                *dataOut = XOR<DATA, DATA>::computeFinalChecksum(checksum);
            }
        }
        void operator()(
                ArithmeticConfiguration::Sub) {
        }
        void operator()(
                ArithmeticConfiguration::Mul) {
        }
        void operator()(
                ArithmeticConfiguration::Div) {
        }
    };

    void RunArithmetic(
            const ArithmeticConfiguration & config) override {
        for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
            _ReadWriteBarrier();
            std::visit(Arithmetor(*this, config), config.mode);
        }
    }

    bool DoArithmeticChecked(
            const ArithmeticConfiguration & config) override {
        return std::visit(ArithmeticSelector(), config.mode);
    }

    struct ArithmetorChecked {
        XOR_avx2 & test;
        const ArithmeticConfiguration & config;
        ArithmetorChecked(
                XOR_avx2 & test,
                const ArithmeticConfiguration & config)
        : test(test),
        config(config) {
        }
        void operator()(
                ArithmeticConfiguration::Add) {
            constexpr const size_t NUM_VALUES_PER_VECTOR = sizeof(__m256i) / sizeof (DATA);
            constexpr const size_t NUM_VALUES_PER_BLOCK = NUM_VALUES_PER_VECTOR * BLOCKSIZE;
            size_t numValues = test.getNumValues();
            size_t i = 0;
            auto data256In = test.bufEncoded.template begin<__m256i>();
            auto data256Out = test.bufResult.template begin<__m256i>();
            while (i <= (numValues - NUM_VALUES_PER_BLOCK)) {
                __m256i oldChecksum = _mm256_setzero_si256();
                __m256i newChecksum = _mm256_setzero_si256();
                for (size_t k = 0; k < BLOCKSIZE; ++k) {
                    auto mmTmp = _mm256_lddqu_si256(data256In++); // TODO this is only for 16-bit values!
                    oldChecksum = _mm256_xor_si256(oldChecksum, mmTmp);
                    mmTmp = _mm256_add_epi16(mmTmp, _mm256_set1_epi16(config.operand));
                    newChecksum = _mm256_xor_si256(newChecksum, mmTmp);
                    _mm256_storeu_si256(data256Out++, mmTmp);
                }
                auto storedChecksum = reinterpret_cast<CS*>(data256In);
                if (XORdiff<DATA>::checksumsDiffer(*storedChecksum, XOR<__m256i, CS>::computeFinalChecksum(oldChecksum))) // test checksum
                {
                    throw ErrorInfo(__FILE__, __LINE__, i, 0);
                }
                data256In = reinterpret_cast<__m256i*>(storedChecksum + 1);
                auto newChecksumOut = reinterpret_cast<CS*>(data256Out);
                *newChecksumOut++ = XOR<__m256i, CS>::computeFinalChecksum(newChecksum);
                data256Out = reinterpret_cast<__m256i*>(newChecksumOut);
                i += NUM_VALUES_PER_BLOCK;
            }
            // checksum remaining values which do not fit in the block size
            if (i <= (numValues - NUM_VALUES_PER_VECTOR)) {
                __m256i oldChecksum = _mm256_setzero_si256();
                __m256i newChecksum = _mm256_setzero_si256();
                do {
                    auto mmTmp = _mm256_lddqu_si256(data256In++); // TODO this is only for 16-bit values!
                    oldChecksum = _mm256_xor_si256(oldChecksum, mmTmp);
                    mmTmp = _mm256_add_epi16(mmTmp, _mm256_set1_epi16(config.operand));
                    newChecksum = _mm256_xor_si256(newChecksum, mmTmp);
                    _mm256_storeu_si256(data256Out++, mmTmp);
                    i += NUM_VALUES_PER_VECTOR;
                }while (i <= (numValues - NUM_VALUES_PER_VECTOR));
                auto storedChecksum = reinterpret_cast<CS*>(data256In);
                if (XORdiff<CS>::checksumsDiffer(*storedChecksum, XOR<__m256i, CS>::computeFinalChecksum(oldChecksum))) // test checksum
                {
                    throw ErrorInfo(__FILE__, __LINE__, i, 0);
                }
                data256In = reinterpret_cast<__m256i*>(storedChecksum + 1);
                auto newChecksumOut = reinterpret_cast<CS*>(data256Out);
                *newChecksumOut++ = XOR<__m256i, CS>::computeFinalChecksum(newChecksum);
                data256Out = reinterpret_cast<__m256i*>(newChecksumOut);
            }
            // checksum remaining integers which do not fit in the SIMD register, so we do it on the actual data width denoted by template parameter IN
            if (i < numValues) {
                DATA oldChecksum = 0;
                DATA newChecksum = 0;
                auto dataIn = reinterpret_cast<DATA*>(data256In);
                auto dataOut = reinterpret_cast<DATA*>(data256Out);
                for (; i < numValues; ++i) {
                    auto tmp = *dataIn++;
                    oldChecksum ^= tmp;
                    tmp += config.operand;
                    newChecksum ^= tmp;
                    *dataOut++ = tmp;
                }
                auto storedChecksum = reinterpret_cast<CS*>(dataIn);
                if (XORdiff<CS>::checksumsDiffer(*storedChecksum, XOR<DATA, CS>::computeFinalChecksum(oldChecksum))) // test checksum
                {
                    throw ErrorInfo(__FILE__, __LINE__, i, 0);
                }
                auto newChecksumOut = reinterpret_cast<CS*>(dataOut);
                *newChecksumOut = XOR<DATA, CS>::computeFinalChecksum(newChecksum);
            }
        }
        void operator()(
                ArithmeticConfiguration::Sub) {
        }
        void operator()(
                ArithmeticConfiguration::Mul) {
        }
        void operator()(
                ArithmeticConfiguration::Div) {
        }
    };

    void RunArithmeticChecked(
            const ArithmeticConfiguration & config) override {
        for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
            _ReadWriteBarrier();
            std::visit(Arithmetor(*this, config), config.mode);
        }
    }

    bool DoDecode() override {
        return true;
    }

    void RunDecode(
            const DecodeConfiguration & config) override {
        for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
            _ReadWriteBarrier();
            const size_t VALUES_PER_SIMDREG = sizeof(__m256i) / sizeof (DATA);
            const size_t VALUES_PER_BLOCK = BLOCKSIZE * VALUES_PER_SIMDREG;
            size_t numValues = this->bufRaw.template end<DATA>() - this->bufRaw.template begin<DATA>();
            size_t i = 0;
            auto dataIn = this->bufEncoded.template begin<CS>();
            auto dataOut = this->bufResult.template begin<__m256i >();
            while (i <= (numValues - VALUES_PER_BLOCK)) {
                auto dataIn2 = reinterpret_cast<__m256i *>(dataIn);
                for (size_t k = 0; k < BLOCKSIZE; ++k) {
                    auto tmp = _mm256_lddqu_si256(dataIn2++);
                    _mm256_storeu_si256(dataOut++, tmp);
                }
                dataIn = reinterpret_cast<CS*>(dataIn2);
                ++dataIn;
                i += VALUES_PER_BLOCK;
            }
            // checksum remaining values which do not fit in the block size
            if (i <= (numValues - VALUES_PER_SIMDREG)) {
                auto dataIn2 = reinterpret_cast<__m256i *>(dataIn);
                for (size_t k = 0; k < BLOCKSIZE; ++k) {
                    auto tmp = _mm256_lddqu_si256(dataIn2++);
                    _mm256_storeu_si256(dataOut++, tmp);
                }
                dataIn = reinterpret_cast<CS*>(dataIn2);
                ++dataIn;
            }
            // checksum remaining integers which do not fit in the SIMD register
            if (i < numValues) {
                auto dataOut2 = reinterpret_cast<DATA*>(dataOut);
                for (auto dataIn2 = reinterpret_cast<DATA*>(dataIn); i < numValues; ++i, ++dataIn2) {
                    *dataOut2++ = *dataIn2;
                }
            }
        }
    }
};
