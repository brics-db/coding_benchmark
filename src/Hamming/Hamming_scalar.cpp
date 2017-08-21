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
 * File:   Hamming_scalar.cpp
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 15-08-2017 16:28
 */

#include <Hamming/Hamming_scalar.hpp>

template struct hamming_t<uint16_t, uint16_t> ;
template struct hamming_t<uint32_t, uint32_t> ;
template struct hamming_t<uint64_t, uint64_t> ;

template<>
uint8_t hamming_t<uint16_t, uint16_t>::computeHamming(
        uint16_t data) {
    uint8_t hamming = 0;
    hamming |= (__builtin_popcount(data & 0xAD5B) & 0x1) << 1;
    hamming |= (__builtin_popcount(data & 0x366D) & 0x1) << 2;
    hamming |= (__builtin_popcount(data & 0xC78E) & 0x1) << 3;
    hamming |= (__builtin_popcount(data & 0x07F0) & 0x1) << 4;
    hamming |= (__builtin_popcount(data & 0xF800) & 0x1) << 5;
    hamming |= (__builtin_popcount(data) + __builtin_popcount(hamming)) & 0x1;
    return hamming;
}

template<>
bool hamming_t<uint16_t, uint16_t>::code_cmp_eq(
        uint8_t code1,
        uint8_t code2) {
    return code1 == code2;
}

template<>
bool hamming_t<uint16_t, uint16_t>::isValid() {
    return hamming_t<uint16_t, uint16_t>::code_cmp_eq(this->code, hamming_t<uint16_t, uint16_t>::computeHamming(this->data));
}

template<>
void hamming_t<uint16_t, uint16_t>::store(
        uint16_t data) {
    this->data = data;
    this->code = computeHamming(data);
}

template<>
uint8_t hamming_t<uint32_t, uint32_t>::computeHamming(
        uint32_t data) {
    uint8_t hamming = 0;
    hamming |= (__builtin_popcount(data & 0x56AAAD5B) & 0x1) << 1;
    hamming |= (__builtin_popcount(data & 0x9B33366D) & 0x1) << 2;
    hamming |= (__builtin_popcount(data & 0xE3C3C78E) & 0x1) << 3;
    hamming |= (__builtin_popcount(data & 0x03FC07F0) & 0x1) << 4;
    hamming |= (__builtin_popcount(data & 0x03FFF800) & 0x1) << 5;
    hamming |= (__builtin_popcount(data & 0xFC000000) & 0x1) << 6;
    hamming |= (__builtin_popcount(data) + __builtin_popcount(hamming)) & 0x1;
    return hamming;
}

template<>
bool hamming_t<uint32_t, uint32_t>::code_cmp_eq(
        uint8_t code1,
        uint8_t code2) {
    return code1 == code2;
}

template<>
bool hamming_t<uint32_t, uint32_t>::isValid() {
    return hamming_t<uint32_t, uint32_t>::code_cmp_eq(this->code, hamming_t<uint32_t, uint32_t>::computeHamming(this->data));
}

template<>
void hamming_t<uint32_t, uint32_t>::store(
        uint32_t data) {
    this->data = data;
    this->code = computeHamming(data);
}

template<>
uint8_t hamming_t<uint64_t, uint64_t>::computeHamming(
        uint64_t data) {
    uint8_t hamming = 0;
    hamming |= (__builtin_popcount(data & 0xAB55555556AAAD5B) & 0x1) << 1;
    hamming |= (__builtin_popcount(data & 0xCD9999999B33366D) & 0x1) << 2;
    hamming |= (__builtin_popcount(data & 0x78F1E1E1E3C3C78E) & 0x1) << 3;
    hamming |= (__builtin_popcount(data & 0x01FE01FE03FC07F0) & 0x1) << 4;
    hamming |= (__builtin_popcount(data & 0x01FFFE0003FFF800) & 0x1) << 5;
    hamming |= (__builtin_popcount(data & 0x01FFFFFFFC000000) & 0x1) << 6;
    hamming |= (__builtin_popcount(data & 0xFE00000000000000) & 0x1) << 7;
    hamming |= (__builtin_popcount(data) + __builtin_popcount(hamming)) & 0x1;
    return hamming;
}

template<>
bool hamming_t<uint64_t, uint64_t>::code_cmp_eq(
        uint8_t code1,
        uint8_t code2) {
    return code1 == code2;
}

template<>
bool hamming_t<uint64_t, uint64_t>::isValid() {
    return hamming_t<uint64_t, uint64_t>::code_cmp_eq(this->code, hamming_t<uint64_t, uint64_t>::computeHamming(this->data));
}

template<>
void hamming_t<uint64_t, uint64_t>::store(
        uint64_t data) {
    this->data = data;
    this->code = computeHamming(data);
}

template
struct Hamming_scalar_16<1> ;
template
struct Hamming_scalar_16<2> ;
template
struct Hamming_scalar_16<4> ;
template
struct Hamming_scalar_16<8> ;
template
struct Hamming_scalar_16<16> ;
template
struct Hamming_scalar_16<32> ;
template
struct Hamming_scalar_16<64> ;
template
struct Hamming_scalar_16<128> ;
template
struct Hamming_scalar_16<256> ;
template
struct Hamming_scalar_16<512> ;
template
struct Hamming_scalar_16<1024> ;

template
struct Hamming_scalar_32<1> ;
template
struct Hamming_scalar_32<2> ;
template
struct Hamming_scalar_32<4> ;
template
struct Hamming_scalar_32<8> ;
template
struct Hamming_scalar_32<16> ;
template
struct Hamming_scalar_32<32> ;
template
struct Hamming_scalar_32<64> ;
template
struct Hamming_scalar_32<128> ;
template
struct Hamming_scalar_32<256> ;
template
struct Hamming_scalar_32<512> ;
template
struct Hamming_scalar_32<1024> ;
