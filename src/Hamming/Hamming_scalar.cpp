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

namespace coding_benchmark {

    template struct hamming_t<uint16_t, uint16_t> ;
    template struct hamming_t<uint32_t, uint32_t> ;
    template struct hamming_t<uint64_t, uint64_t> ;

    template<>
    uint8_t hamming_t<uint16_t, uint16_t>::computeHamming(
            uint16_t data) {
        uint8_t hamming = 0;
        hamming |= (_mm_popcnt_u32(data & 0xAD5B) & 0x1) << 1;
        hamming |= (_mm_popcnt_u32(data & 0x366D) & 0x1) << 2;
        hamming |= (_mm_popcnt_u32(data & 0xC78E) & 0x1) << 3;
        hamming |= (_mm_popcnt_u32(data & 0x07F0) & 0x1) << 4;
        hamming |= (_mm_popcnt_u32(data & 0xF800) & 0x1) << 5;
        hamming |= (_mm_popcnt_u32(data) + _mm_popcnt_u32(hamming)) & 0x1;
        return hamming;
    }

    template<>
    uint8_t hamming_t<uint16_t, uint16_t>::computeHamming2(
            uint16_t data) {
        return computeHamming(data);
    }

    template<>
    uint8_t hamming_t<uint16_t, uint16_t>::computeHamming3(
            uint16_t data) {
        return computeHamming(data);
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
    void hamming_t<uint16_t, uint16_t>::store2(
            uint16_t data) {
        this->data = data;
        this->code = computeHamming2(data);
    }

    template<>
    void hamming_t<uint16_t, uint16_t>::store3(
            uint16_t data) {
        this->data = data;
        this->code = computeHamming3(data);
    }

    template<>
    uint8_t hamming_t<uint32_t, uint32_t>::computeHamming(
            uint32_t data) {
        uint8_t hamming = 0;
        hamming |= (_mm_popcnt_u32(data & 0x56AAAD5B) & 0x1) << 1;
        hamming |= (_mm_popcnt_u32(data & 0x9B33366D) & 0x1) << 2;
        hamming |= (_mm_popcnt_u32(data & 0xE3C3C78E) & 0x1) << 3;
        hamming |= (_mm_popcnt_u32(data & 0x03FC07F0) & 0x1) << 4;
        hamming |= (_mm_popcnt_u32(data & 0x03FFF800) & 0x1) << 5;
        hamming |= (_mm_popcnt_u32(data & 0xFC000000) & 0x1) << 6;
        hamming |= (_mm_popcnt_u32(data) + _mm_popcnt_u32(hamming)) & 0x1;
        return hamming;
    }

    template<>
    uint8_t hamming_t<uint32_t, uint32_t>::computeHamming2(
            uint32_t data) {
        return computeHamming(data);
    }

    template<>
    uint8_t hamming_t<uint32_t, uint32_t>::computeHamming3(
            uint32_t data) {
        return computeHamming(data);
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
    void hamming_t<uint32_t, uint32_t>::store2(
            uint32_t data) {
        this->data = data;
        this->code = computeHamming2(data);
    }

    template<>
    void hamming_t<uint32_t, uint32_t>::store3(
            uint32_t data) {
        this->data = data;
        this->code = computeHamming3(data);
    }

    template<>
    uint8_t hamming_t<uint64_t, uint64_t>::computeHamming(
            uint64_t data) {
        uint8_t hamming = 0;
        hamming |= (_mm_popcnt_u64(data & 0xAB55555556AAAD5B) & 0x1) << 1;
        hamming |= (_mm_popcnt_u64(data & 0xCD9999999B33366D) & 0x1) << 2;
        hamming |= (_mm_popcnt_u64(data & 0x78F1E1E1E3C3C78E) & 0x1) << 3;
        hamming |= (_mm_popcnt_u64(data & 0x01FE01FE03FC07F0) & 0x1) << 4;
        hamming |= (_mm_popcnt_u64(data & 0x01FFFE0003FFF800) & 0x1) << 5;
        hamming |= (_mm_popcnt_u64(data & 0x01FFFFFFFC000000) & 0x1) << 6;
        hamming |= (_mm_popcnt_u64(data & 0xFE00000000000000) & 0x1) << 7;
        hamming |= (_mm_popcnt_u64(data) + _mm_popcnt_u64(hamming)) & 0x1;
        return hamming;
    }

    template<>
    uint8_t hamming_t<uint64_t, uint64_t>::computeHamming2(
            uint64_t data) {
        return computeHamming(data);
    }

    template<>
    uint8_t hamming_t<uint64_t, uint64_t>::computeHamming3(
            uint64_t data) {
        return computeHamming(data);
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

    template<>
    void hamming_t<uint64_t, uint64_t>::store2(
            uint64_t data) {
        this->data = data;
        this->code = computeHamming2(data);
    }

    template<>
    void hamming_t<uint64_t, uint64_t>::store3(
            uint64_t data) {
        this->data = data;
        this->code = computeHamming3(data);
    }

}
