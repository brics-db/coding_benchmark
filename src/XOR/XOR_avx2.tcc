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
            __m256i cs1,
            __m256i cs2);
};

template<typename DATA, typename CS, size_t BLOCKSIZE>
struct XOR_avx2 :
        public Test<DATA, CS> {

    using Test<DATA, CS>::Test;

    virtual ~XOR_avx2() {
    }

    void RunEncode(
            const EncodeConfiguration & config) override {
        for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
            _ReadWriteBarrier();
            auto dataIn = this->bufRaw.template begin<__m256i >();
            auto dataInEnd = this->bufRaw.template end<__m256i >();
            auto dataOut = this->bufEncoded.template begin<CS>();
            while (dataIn <= (dataInEnd - BLOCKSIZE)) {
                __m256i checksum = _mm256_setzero_si256();
                auto dataOut2 = reinterpret_cast<__m256i *>(dataOut);
                for (size_t k = 0; k < BLOCKSIZE; ++k) {
                    auto tmp = _mm256_lddqu_si256(dataIn++);
                    _mm256_storeu_si256(dataOut2++, tmp);
                    checksum = _mm256_xor_si256(checksum, tmp);
                }
                dataOut = reinterpret_cast<CS*>(dataOut2);
                *dataOut++ = XOR<__m256i, CS>::computeFinalChecksum(checksum);
            }
            // checksum remaining values which do not fit in the block size
            if (dataIn <= (dataInEnd - 1)) {
                __m256i checksum = _mm256_setzero_si256();
                auto dataOut2 = reinterpret_cast<__m256i *>(dataOut);
                do {
                    auto tmp = _mm256_lddqu_si256(dataIn++);
                    _mm256_storeu_si256(dataOut2++, tmp);
                    checksum = _mm256_xor_si256(checksum, tmp);
                } while (dataIn <= (dataInEnd - 1));
                dataOut = reinterpret_cast<CS*>(dataOut2);
                *dataOut++ = XOR<__m256i, CS>::computeFinalChecksum(checksum);
            }
            // checksum remaining integers which do not fit in the SIMD register
            if (dataIn < dataInEnd) {
                DATA checksum = 0;
                auto dataEnd = reinterpret_cast<DATA*>(dataInEnd);
                auto dataOut2 = reinterpret_cast<DATA*>(dataOut);
                for (auto data = reinterpret_cast<DATA*>(dataIn); data < dataEnd; ++data) {
                    auto & tmp = *data;
                    *dataOut2++ = tmp;
                    checksum ^= tmp;
                }
                *dataOut2 = XOR<DATA, DATA>::computeFinalChecksum(checksum);
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
            const size_t VALUES_PER_SIMDREG = sizeof(__m256i) / sizeof (DATA);
            const size_t VALUES_PER_BLOCK = BLOCKSIZE * VALUES_PER_SIMDREG;
            size_t numValues = this->bufRaw.template end<DATA>() - this->bufRaw.template begin<DATA>();
            size_t i = 0;
            auto data256 = this->bufEncoded.template begin<__m256i >();
            while (i <= (numValues - VALUES_PER_BLOCK)) {
                __m256i checksum = _mm256_setzero_si256();
                for (size_t k = 0; k < BLOCKSIZE; ++k) {
                    checksum = _mm256_xor_si256(checksum, _mm256_lddqu_si256(data256++));
                }
                i += VALUES_PER_BLOCK;
                auto data = reinterpret_cast<CS*>(data256);
                if (XORdiff<CS>::checksumsDiffer(*data, XOR<__m256i, CS>::computeFinalChecksum(checksum))) // third, test checksum
                        {
                    throw ErrorInfo(__FILE__, __LINE__, data - this->bufEncoded.template begin<CS>(), iteration); // this is not completely accurate, but not SO necessary for our �-Benchmark
                }
                ++data; // fourth, advance after the checksum to the next block of values
                data256 = reinterpret_cast<__m256i *>(data);
            }
            // checksum remaining values which do not fit in the block size
            if (i <= (numValues - VALUES_PER_SIMDREG)) {
                __m256i checksum = _mm256_setzero_si256();
                for (; i <= (numValues - 1); i += VALUES_PER_SIMDREG, ++data256) {
                    checksum = _mm256_xor_si256(checksum, _mm256_lddqu_si256(data256));
                }
                auto data = reinterpret_cast<CS*>(data256);
                if (XORdiff<CS>::checksumsDiffer(*data, XOR<__m256i, CS>::computeFinalChecksum(checksum))) // third, test checksum
                        {
                    throw ErrorInfo(__FILE__, __LINE__, data - this->bufEncoded.template begin<CS>(), iteration); // this is not completely accurate, but not SO necessary for our �-Benchmark
                }
                ++data; // fourth, advance after the checksum to the next block of values
                data256 = reinterpret_cast<__m256i *>(data);
            }
            // checksum remaining integers which do not fit in the SIMD register, so we do it on the actual data width denoted by template parameter IN
            if (i < numValues) {
                DATA checksum = 0;
                auto data = reinterpret_cast<DATA*>(data256);
                for (; i < numValues; ++i, ++data) {
                    checksum ^= *data++;
                }
                auto dataOut = reinterpret_cast<DATA*>(data);
                if (XORdiff<DATA>::checksumsDiffer(*dataOut, XOR<DATA, DATA>::computeFinalChecksum(checksum))) // third, test checksum
                        {
                    throw ErrorInfo(__FILE__, __LINE__, dataOut - this->bufEncoded.template begin<DATA>(), iteration); // this is not completely accurate, but not SO necessary for our �-Benchmark
                }
            }
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
