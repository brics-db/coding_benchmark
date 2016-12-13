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

#include <cstdint>

struct CPU {

    enum Vendor {

        Other,
        AMD,
        Intel
    };

    static CPU&
    Instance () {
        static CPU instance;
        return instance;
    }

    //  Vendor
    Vendor CPUVendor;

    // OS:
    bool OS_X64;
    bool OS_AVX;
    bool OS_AVX512;

    //  Misc.
    bool MMX;
    bool x64;
    bool ABM;
    bool RDRAND;
    bool BMI1;
    bool BMI2;
    bool ADX;
    bool PREFETCHWT1;
    bool MPX;

    // SSE
    bool SSE;
    bool SSE2;
    bool SSE3;
    bool SSSE3;
    bool SSE41;
    bool SSE42;
    bool SSE4a;
    bool AES;
    bool SHA;

    // AVX
    bool AVX;
    bool XOP;
    bool FMA3;
    bool FMA4;
    bool AVX2;

    // AVX-512
    bool AVX512_F;
    bool AVX512_PF;
    bool AVX512_ER;
    bool AVX512_CD;
    bool AVX512_VL;
    bool AVX512_BW;
    bool AVX512_DQ;
    bool AVX512_IFMA;
    bool AVX512_VBMI;

private:
    CPU ();

    bool x64Supported ();
    void CPUID (int32_t out[4], int32_t x);
    uint64_t xgetbv (unsigned int index);
};