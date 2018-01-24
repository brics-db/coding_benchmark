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
 * File:   XOR_sse42.cpp
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 15-08-2017 17:40
 */

#include <XOR/XOR_sse42.hpp>

namespace coding_benchmark {

    __m128i XOR<__m128i, __m128i >::computeFinalChecksum(
            __m128i & checksum) {
        return checksum;
    }

    uint32_t XOR<__m128i, uint32_t>::computeFinalChecksum(
            __m128i & checksum) {
        auto pChk = reinterpret_cast<uint32_t*>(&checksum);
        return pChk[0] ^ pChk[1] ^ pChk[2] ^ pChk[3];
    }

    uint16_t XOR<__m128i, uint16_t>::computeFinalChecksum(
            __m128i & checksum) {
        auto pChk = reinterpret_cast<uint16_t*>(&checksum);
        return pChk[0] ^ pChk[1] ^ pChk[2] ^ pChk[3] ^ pChk[4] ^ pChk[5] ^ pChk[6] ^ pChk[7];
    }

    uint8_t XOR<__m128i, uint8_t>::computeFinalChecksum(
            __m128i & checksum) {
        auto pChk = reinterpret_cast<uint16_t*>(&checksum);
        return pChk[0] ^ pChk[1] ^ pChk[2] ^ pChk[3] ^ pChk[4] ^ pChk[5] ^ pChk[6] ^ pChk[7] ^ pChk[8] ^ pChk[9] ^ pChk[10] ^ pChk[11] ^ pChk[12] ^ pChk[13] ^ pChk[14] ^ pChk[15];
    }

    bool XORdiff<__m128i >::checksumsDiffer(
            __m128i checksum1,
            __m128i checksum2) {
        // check if any of the 16 bytes differ
        return 0xFFFF != _mm_movemask_epi8(_mm_cmpeq_epi8(checksum1, checksum2));
    }

}
