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

template<typename DATA, typename CS, size_t BLOCKSIZE>
struct XOR_scalar :
        public Test<DATA, CS> {

    using Test<DATA, CS>::Test;

    virtual ~XOR_scalar() {
    }

    void RunEncode(
            const EncodeConfiguration & config) override {
        for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
            _ReadWriteBarrier();
            auto dataIn = this->bufRaw.template begin<DATA>();
            auto dataInEnd = this->bufRaw.template end<DATA>();
            auto dataOut = this->bufEncoded.template begin<CS>();
            while (dataIn <= (dataInEnd - BLOCKSIZE)) {
                DATA checksum = 0;
                auto dataOut2 = reinterpret_cast<DATA*>(dataOut);
                for (size_t k = 0; k < BLOCKSIZE; ++k) {
                    auto& tmp = *dataIn++;
                    *dataOut2++ = tmp;
                    checksum ^= tmp;
                }
                dataOut = reinterpret_cast<CS*>(dataOut2);
                *dataOut++ = XOR<DATA, CS>::computeFinalChecksum(checksum);
            }
            // checksum remaining values which do not fit in the block size
            if (dataIn < dataInEnd) {
                DATA checksum = 0;
                auto dataOut2 = reinterpret_cast<DATA*>(dataOut);
                do {
                    auto& tmp = *dataIn++;
                    *dataOut2++ = tmp;
                    checksum ^= tmp;
                } while (dataIn < dataInEnd);
                dataOut = reinterpret_cast<CS*>(dataOut2);
                *dataOut++ = XOR<DATA, CS>::computeFinalChecksum(checksum);
            }
        }
    }

    virtual bool DoCheck() override {
        return true;
    }

    virtual void RunCheck(
            const CheckConfiguration & config) override {
        for (size_t iterations = 0; iterations < config.numIterations; ++iterations) {
            _ReadWriteBarrier();
            size_t numValues = this->bufRaw.template end<DATA>() - this->bufRaw.template begin<DATA>();
            size_t i = 0;
            auto data = this->bufEncoded.template begin<CS>();
            while (i <= (numValues - BLOCKSIZE)) {
                auto data2 = reinterpret_cast<DATA*>(data); // first, iterate over sizeof(IN)-bit values
                DATA checksum = 0;
                for (size_t k = 0; k < BLOCKSIZE; ++k) {
                    checksum ^= *data2++;
                }
                i += BLOCKSIZE;
                data = reinterpret_cast<CS*>(data2); // second, advance data2 up to the checksum
                if (XORdiff<CS>::checksumsDiffer(*data, XOR<DATA, CS>::computeFinalChecksum(checksum))) // third, test checksum
                        {
                    throw ErrorInfo(__FILE__, __LINE__, data - this->bufEncoded.template begin<CS>(), iterations);
                }
                ++data; // fourth, advance after the checksum to the next block of values
            }
            // checksum remaining values which do not fit in the block size
            if (i < numValues) {
                auto data2 = reinterpret_cast<DATA*>(data); // first, iterate over sizeof(IN)-bit values
                DATA checksum = 0;
                do {
                    ++i;
                    checksum ^= *data2++;
                } while (i < numValues);
                data = reinterpret_cast<CS*>(data2); // second, advance data2 up to the checksum
                if (XORdiff<CS>::checksumsDiffer(*data, XOR<DATA, CS>::computeFinalChecksum(checksum))) // third, test checksum
                        {
                    throw ErrorInfo(__FILE__, __LINE__, data - this->bufEncoded.template begin<CS>(), iterations);
                }
            }
        }
    }

    virtual bool DoDecode() override {
        return true;
    }

    void RunDecode(
            const DecodeConfiguration & config) override {
        for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
            _ReadWriteBarrier();
            size_t numValues = this->bufRaw.template end<DATA>() - this->bufRaw.template begin<DATA>();
            size_t i = 0;
            auto dataIn = this->bufEncoded.template begin<CS>();
            auto dataOut = this->bufResult.template begin<DATA>();
            while (i <= (numValues - BLOCKSIZE)) {
                auto dataIn2 = reinterpret_cast<DATA*>(dataIn);
                for (size_t k = 0; k < BLOCKSIZE; ++k) {
                    *dataOut++ = *dataIn++;
                }
                i += BLOCKSIZE;
                dataIn = reinterpret_cast<CS*>(dataIn2);
                dataIn++;
            }
            // checksum remaining values which do not fit in the block size
            if (i < numValues) {
                for (auto dataIn2 = reinterpret_cast<DATA*>(dataIn); i < numValues; ++i) {
                    *dataOut++ = *dataIn2++;
                }
            }
        }
    }
};
