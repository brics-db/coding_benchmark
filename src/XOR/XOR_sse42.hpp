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

#include "XOR_base.hpp"
#include "../Util/Intrinsics.hpp"

template<>
__m128i computeFinalChecksum<__m128i, __m128i >(
        __m128i & checksum) {
    return checksum;
}

template<>
uint32_t computeFinalChecksum<__m128i, uint32_t>(
        __m128i & checksum) {
    auto pChk = reinterpret_cast<uint32_t*>(&checksum);
    return pChk[0] ^ pChk[1] ^ pChk[2] ^ pChk[3];
}

template<>
uint16_t computeFinalChecksum<__m128i, uint16_t>(
        __m128i & checksum) {
    auto pChk = reinterpret_cast<uint16_t*>(&checksum);
    return pChk[0] ^ pChk[1] ^ pChk[2] ^ pChk[3] ^ pChk[4] ^ pChk[5] ^ pChk[6] ^ pChk[7];
}

template<>
uint8_t computeFinalChecksum<__m128i, uint8_t>(
        __m128i & checksum) {
    auto pChk = reinterpret_cast<uint8_t*>(&checksum);
    return pChk[0] ^ pChk[1] ^ pChk[2] ^ pChk[3] ^ pChk[4] ^ pChk[5] ^ pChk[6] ^ pChk[7] ^ pChk[8] ^ pChk[9] ^ pChk[10] ^ pChk[11] ^ pChk[12] ^ pChk[13] ^ pChk[14] ^ pChk[15];
}

template<>
bool checksumsDiffer<__m128i >(
        __m128i checksum1,
        __m128i checksum2) {
    // check if any of the 16 bytes differ
    return 0xFFFF != _mm_movemask_epi8(_mm_cmpeq_epi8(checksum1, checksum2));
}

template<typename DATA, typename CS, size_t BLOCKSIZE>
struct XOR_sse42 :
        public Test<DATA, CS> {

    XOR_sse42(
            const char* const name,
            AlignedBlock & in,
            AlignedBlock & out)
            : Test<DATA, CS>(name, in, out) {
    }

    virtual ~XOR_sse42() {
    }

    void RunEnc(
            const EncodeConfiguration & config) override {
        for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
            _ReadWriteBarrier();
            auto dataIn = this->in.template begin<__m128i >();
            auto dataInEnd = this->in.template end<__m128i >();
            auto dataOut = this->out.template begin<CS>();
            while (dataIn <= (dataInEnd - BLOCKSIZE)) {
                __m128i checksum = _mm_setzero_si128();
                auto dataOut2 = reinterpret_cast<__m128i *>(dataOut);
                for (size_t k = 0; k < BLOCKSIZE; ++k) {
                    auto tmp = _mm_lddqu_si128(dataIn++);
                    _mm_storeu_si128(dataOut2++, tmp);
                    checksum = _mm_xor_si128(checksum, tmp);
                }
                dataOut = reinterpret_cast<CS*>(dataOut2);
                *dataOut++ = computeFinalChecksum<__m128i, CS>(checksum);
            }
            // checksum remaining values which do not fit in the block size
            if (dataIn <= (dataInEnd - 1)) {
                __m128i checksum = _mm_setzero_si128();
                auto dataOut2 = reinterpret_cast<__m128i *>(dataOut);
                do {
                    auto tmp = _mm_lddqu_si128(dataIn++);
                    _mm_storeu_si128(dataOut2++, tmp);
                    checksum = _mm_xor_si128(checksum, tmp);
                } while (dataIn <= (dataInEnd - 1));
                dataOut = reinterpret_cast<CS*>(dataOut2);
                *dataOut++ = computeFinalChecksum<__m128i, CS>(checksum);
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
                *dataOut2 = computeFinalChecksum<DATA, DATA>(checksum);
            }
        }
    }

    virtual bool DoCheck() override {
        return true;
    }

    virtual void RunCheck(
            const CheckConfiguration & config) override {
        const size_t NUM_VALUES_PER_M128 = sizeof(__m128i) / sizeof (DATA);
        const size_t NUM_VALUES_PER_BLOCK = NUM_VALUES_PER_M128 * BLOCKSIZE;
        for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
            _ReadWriteBarrier();
            size_t numValues = this->in.template end<DATA>() - this->in.template begin<DATA>();
            size_t i = 0;
            auto data128 = this->out.template begin<__m128i>();
            while (i <= (numValues - NUM_VALUES_PER_BLOCK)) {
                __m128i checksum = _mm_setzero_si128();
                for (size_t k = 0; k < BLOCKSIZE; ++k) {
                    checksum = _mm_xor_si128(checksum, _mm_lddqu_si128(data128++));
                }
                auto dataOut = reinterpret_cast<CS*>(data128);
                if (checksumsDiffer<CS>(*dataOut, computeFinalChecksum<__m128i, CS>(checksum))) // third, test checksum
                {
                    throw ErrorInfo(__FILE__, __LINE__, dataOut - this->out.template begin<CS>(), iteration); // this is not completely accurate, but not SO necessary for our µ-Benchmark
                }
                ++dataOut; // fourth, advance after the checksum to the next block of values
                data128 = reinterpret_cast<__m128i*>(dataOut);
                i += NUM_VALUES_PER_BLOCK;
            }
            // checksum remaining values which do not fit in the block size
            if (i <= (numValues - NUM_VALUES_PER_M128)) {
                __m128i checksum = _mm_setzero_si128();
                do {
                    checksum = _mm_xor_si128(checksum, _mm_lddqu_si128(data128++));
                    i += NUM_VALUES_PER_M128;
                }while (i <= (numValues - NUM_VALUES_PER_M128));
                auto dataOut = reinterpret_cast<CS*>(data128);
                if (checksumsDiffer<CS>(*dataOut, computeFinalChecksum<__m128i, CS>(checksum))) // third, test checksum
                {
                    throw ErrorInfo(__FILE__, __LINE__, dataOut - this->out.template begin<CS>(), iteration); // this is not completely accurate, but not SO necessary for our µ-Benchmark
                }
                ++dataOut; // fourth, advance after the checksum to the next block of values
                data128 = reinterpret_cast<__m128i*>(dataOut);
            }
            // checksum remaining integers which do not fit in the SIMD register, so we do it on the actual data width denoted by template parameter IN
            if (i < numValues) {
                DATA checksum = 0;
                auto data = reinterpret_cast<DATA*>(data128);
                for (; i < numValues; ++i) {
                    checksum ^= *data++;
                }
                auto dataOut = reinterpret_cast<DATA*>(data);
                if (checksumsDiffer<DATA>(*dataOut, computeFinalChecksum<DATA, DATA>(checksum))) // third, test checksum
                {
                    throw ErrorInfo(__FILE__, __LINE__, dataOut - this->out.template begin<DATA>(), iteration); // this is not completely accurate, but not SO necessary for our µ-Benchmark
                }
            }
        }
    }

    bool
    DoDec () override {
        return true;
    }

    virtual void RunDec (
            const DecodeConfiguration & config) override {
        for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
            _ReadWriteBarrier();
            const size_t VALUES_PER_SIMDREG = sizeof (__m128i) / sizeof (DATA);
            const size_t VALUES_PER_BLOCK = BLOCKSIZE * VALUES_PER_SIMDREG;
            size_t numValues = this->in.template end<DATA>() - this->in.template begin<DATA>();
            size_t i = 0;
            auto dataIn = this->out.template begin<CS>();
            auto dataOut = this->in.template begin<__m128i>();
            for (; i <= (numValues - VALUES_PER_BLOCK); i += VALUES_PER_BLOCK, dataIn++) {
                auto dataIn2 = reinterpret_cast<__m128i*>(dataIn);
                for (size_t k = 0; k < BLOCKSIZE; ++k) {
                    _mm_storeu_si128(dataOut++, _mm_lddqu_si128(dataIn2++));
                }
                dataIn = reinterpret_cast<CS*>(dataIn2);
            }
            // checksum remaining values which do not fit in the block size
            if (i <= (numValues - VALUES_PER_SIMDREG)) {
                auto dataIn2 = reinterpret_cast<__m128i*>(dataIn);
                for (; i <= (numValues - VALUES_PER_SIMDREG); i += VALUES_PER_SIMDREG) {
                    _mm_storeu_si128(dataOut++, _mm_lddqu_si128(dataIn2++));
                }
                dataIn = reinterpret_cast<CS*>(dataIn2);
                dataOut++;
            }
            // checksum remaining integers which do not fit in the SIMD register
            if (i < numValues) {
                auto dataIn2 = reinterpret_cast<DATA*>(dataIn);
                auto dataOut2 = reinterpret_cast<DATA*>(dataOut);
                for (; i < numValues; ++i) {
                    *dataOut2++ = *dataIn2++;
                }
            }
        }
    }
};
