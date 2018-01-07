// Copyright (c) 2017 Till Kolditz
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

/*
 * File:   XOR_avx2.cpp
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 15-08-2017 16:47
 */

#ifdef __AVX2__

#include <XOR/XOR_avx2.hpp>

namespace coding_benchmark {

    __m256i XOR<__m256i, __m256i >::computeFinalChecksum(
            __m256i & checksum) {
        return checksum;
    }

    uint32_t XOR<__m256i, uint32_t>::computeFinalChecksum(
            __m256i & checksum) {
        auto pChk = reinterpret_cast<uint32_t*>(&checksum);
        return pChk[0] ^ pChk[1] ^ pChk[2] ^ pChk[3] ^ pChk[4] ^ pChk[5] ^ pChk[6] ^ pChk[7];
    }

    uint16_t XOR<__m256i, uint16_t>::computeFinalChecksum(
            __m256i & checksum) {
        auto pChk = reinterpret_cast<uint16_t*>(&checksum);
        return pChk[0] ^ pChk[1] ^ pChk[2] ^ pChk[3] ^ pChk[4] ^ pChk[5] ^ pChk[6] ^ pChk[7] ^ pChk[8] ^ pChk[9] ^ pChk[10] ^ pChk[11] ^ pChk[12] ^ pChk[13] ^ pChk[14] ^ pChk[15];
    }

    uint8_t XOR<__m256i, uint8_t>::computeFinalChecksum(
            __m256i & checksum) {
        auto pChk = reinterpret_cast<uint16_t*>(&checksum);
        return pChk[0] ^ pChk[1] ^ pChk[2] ^ pChk[3] ^ pChk[4] ^ pChk[5] ^ pChk[6] ^ pChk[7] ^ pChk[8] ^ pChk[9] ^ pChk[10] ^ pChk[11] ^ pChk[12] ^ pChk[13] ^ pChk[14] ^ pChk[15] ^ pChk[16] ^ pChk[17]
        ^ pChk[18] ^ pChk[19] ^ pChk[20] ^ pChk[21] ^ pChk[22] ^ pChk[23] ^ pChk[24] ^ pChk[25] ^ pChk[26] ^ pChk[27] ^ pChk[28] ^ pChk[29] ^ pChk[30] ^ pChk[31];
    }

    bool XORdiff<__m256i >::checksumsDiffer(
            __m256i checksum1,
            __m256i checksum2) {
        // check if any of the 16 bytes differ
        return static_cast<int>(0xFFFFFFFF) != _mm256_movemask_epi8(_mm256_cmpeq_epi8(checksum1, checksum2));
    }

}

#endif /* __AVX2__ */
