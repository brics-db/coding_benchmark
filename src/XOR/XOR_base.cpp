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
 * File:   XOR_base.cpp
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 15-08-2017 17:08
 */

#include <XOR/XOR_base.hpp>

uint8_t XOR<uint16_t, uint8_t>::computeFinalChecksum(
        uint16_t & checksum) {
    auto pChk = reinterpret_cast<uint8_t*>(&checksum);
    return pChk[0] ^ pChk[1];
}

uint16_t XOR<uint16_t, uint16_t>::computeFinalChecksum(
        uint16_t & checksum) {
    return checksum;
}

uint8_t XOR<uint32_t, uint8_t>::computeFinalChecksum(
        uint32_t & checksum) {
    auto pChk = reinterpret_cast<uint8_t*>(&checksum);
    return pChk[0] ^ pChk[1] ^ pChk[2] ^ pChk[3];
}

uint32_t XOR<uint32_t, uint32_t>::computeFinalChecksum(
        uint32_t & checksum) {
    return checksum;
}

bool XORdiff<uint8_t>::checksumsDiffer(
        uint8_t cs1,
        uint8_t cs2) {
    return cs1 != cs2;
}

bool XORdiff<uint16_t>::checksumsDiffer(
        uint16_t cs1,
        uint16_t cs2) {
    return cs1 != cs2;
}

bool XORdiff<uint32_t>::checksumsDiffer(
        uint32_t cs1,
        uint32_t cs2) {
    return cs1 != cs2;
}

bool XORdiff<uint64_t>::checksumsDiffer(
        uint64_t cs1,
        uint64_t cs2) {
    return cs1 != cs2;
}
