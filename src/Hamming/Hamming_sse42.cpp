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
 * File:   Hamming_sse42.cpp
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 15-08-2017 16:32
 */

#ifdef __SSE4_2__

#include <Hamming/Hamming_simd.hpp>

/*
 * For the following algorithms, see
 * https://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel
 * Credit also goes to the book "Hacker's Delight" 2nd Edition, by Henry S. Warren, Jr., published at Addison-Wesley
 */

namespace coding_benchmark {

    template struct hamming_t<uint16_t, __m128i > ;
    template struct hamming_t<uint32_t, __m128i > ;

    template<>
    uint64_t hamming_t<uint16_t, __m128i >::computeHamming(
            __m128i data) {
        auto pattern1 = _mm_set1_epi16(static_cast<int16_t>(0xAD5B));
        auto pattern2 = _mm_set1_epi16(static_cast<int16_t>(0x366D));
        auto pattern3 = _mm_set1_epi16(static_cast<int16_t>(0xC78E));
        auto pattern4 = _mm_set1_epi16(static_cast<int16_t>(0x07F0));
        auto pattern5 = _mm_set1_epi16(static_cast<int16_t>(0xF800));
        uint64_t hamming = 0;
        uint64_t tmp1(0), tmp2(0);
        tmp2 = mm<__m128i, uint16_t>::popcount(_mm_and_si128(data, pattern1)) & 0x0101010101010101;
        hamming |= tmp2 << 1;
        tmp1 = mm<__m128i, uint16_t>::popcount(_mm_and_si128(data, pattern2)) & 0x0101010101010101;
        tmp2 ^= tmp1;
        hamming |= tmp1 << 2;
        tmp1 = mm<__m128i, uint16_t>::popcount(_mm_and_si128(data, pattern3)) & 0x0101010101010101;
        tmp2 ^= tmp1;
        hamming |= tmp1 << 3;
        tmp1 = mm<__m128i, uint16_t>::popcount(_mm_and_si128(data, pattern4)) & 0x0101010101010101;
        tmp2 ^= tmp1;
        hamming |= tmp1 << 4;
        tmp1 = mm<__m128i, uint16_t>::popcount(_mm_and_si128(data, pattern5)) & 0x0101010101010101;
        tmp2 ^= tmp1;
        hamming |= tmp1 << 5;
        hamming |= (mm<__m128i, uint16_t>::popcount(data) + tmp2) & 0x0101010101010101;
        return hamming;
    }

    template<>
    uint64_t hamming_t<uint16_t, __m128i >::computeHamming2(
            __m128i data) {
        auto pattern1 = _mm_set1_epi16(static_cast<int16_t>(0xAD5B));
        auto pattern2 = _mm_set1_epi16(static_cast<int16_t>(0x366D));
        auto pattern3 = _mm_set1_epi16(static_cast<int16_t>(0xC78E));
        auto pattern4 = _mm_set1_epi16(static_cast<int16_t>(0x07F0));
        auto pattern5 = _mm_set1_epi16(static_cast<int16_t>(0xF800));
        uint64_t hamming = 0;
        uint64_t tmp1(0), tmp2(0);
        tmp2 = mm<__m128i, uint16_t>::popcount2(_mm_and_si128(data, pattern1)) & 0x0101010101010101;
        hamming |= tmp2 << 1;
        tmp1 = mm<__m128i, uint16_t>::popcount2(_mm_and_si128(data, pattern2)) & 0x0101010101010101;
        tmp2 ^= tmp1;
        hamming |= tmp1 << 2;
        tmp1 = mm<__m128i, uint16_t>::popcount2(_mm_and_si128(data, pattern3)) & 0x0101010101010101;
        tmp2 ^= tmp1;
        hamming |= tmp1 << 3;
        tmp1 = mm<__m128i, uint16_t>::popcount2(_mm_and_si128(data, pattern4)) & 0x0101010101010101;
        tmp2 ^= tmp1;
        hamming |= tmp1 << 4;
        tmp1 = mm<__m128i, uint16_t>::popcount2(_mm_and_si128(data, pattern5)) & 0x0101010101010101;
        tmp2 ^= tmp1;
        hamming |= tmp1 << 5;
        hamming |= (mm<__m128i, uint16_t>::popcount2(data) + tmp2) & 0x0101010101010101;
        return hamming;
    }

    template<>
    uint64_t hamming_t<uint16_t, __m128i >::computeHamming3(
            __m128i data) {
        auto pattern1 = _mm_set1_epi16(static_cast<int16_t>(0xAD5B));
        auto pattern2 = _mm_set1_epi16(static_cast<int16_t>(0x366D));
        auto pattern3 = _mm_set1_epi16(static_cast<int16_t>(0xC78E));
        auto pattern4 = _mm_set1_epi16(static_cast<int16_t>(0x07F0));
        auto pattern5 = _mm_set1_epi16(static_cast<int16_t>(0xF800));
        uint64_t hamming = 0;
        uint64_t tmp1(0), tmp2(0);
        tmp2 = mm<__m128i, uint16_t>::popcount3(_mm_and_si128(data, pattern1)) & 0x0101010101010101;
        hamming |= tmp2 << 1;
        tmp1 = mm<__m128i, uint16_t>::popcount3(_mm_and_si128(data, pattern2)) & 0x0101010101010101;
        tmp2 ^= tmp1;
        hamming |= tmp1 << 2;
        tmp1 = mm<__m128i, uint16_t>::popcount3(_mm_and_si128(data, pattern3)) & 0x0101010101010101;
        tmp2 ^= tmp1;
        hamming |= tmp1 << 3;
        tmp1 = mm<__m128i, uint16_t>::popcount3(_mm_and_si128(data, pattern4)) & 0x0101010101010101;
        tmp2 ^= tmp1;
        hamming |= tmp1 << 4;
        tmp1 = mm<__m128i, uint16_t>::popcount3(_mm_and_si128(data, pattern5)) & 0x0101010101010101;
        tmp2 ^= tmp1;
        hamming |= tmp1 << 5;
        hamming |= (mm<__m128i, uint16_t>::popcount3(data) + tmp2) & 0x0101010101010101;
        return hamming;
    }

    template<>
    bool hamming_t<uint16_t, __m128i >::code_cmp_eq(
            uint64_t code1,
            uint64_t code2) {
        return code1 == code2;
    }

    template<>
    bool hamming_t<uint16_t, __m128i >::isValid() {
        return hamming_t<uint16_t, __m128i >::code_cmp_eq(this->code, hamming_t<uint16_t, __m128i >::computeHamming(this->data));
    }

    template<>
    void hamming_t<uint16_t, __m128i >::store(
            __m128i data) {
        _mm_storeu_si128(&this->data, data);
        this->code = computeHamming(data);
    }

    template<>
    void hamming_t<uint16_t, __m128i >::store2(
            __m128i data) {
        _mm_storeu_si128(&this->data, data);
        this->code = computeHamming2(data);
    }

    template<>
    void hamming_t<uint16_t, __m128i >::store3(
            __m128i data) {
        _mm_storeu_si128(&this->data, data);
        this->code = computeHamming3(data);
    }

    template<>
    uint32_t hamming_t<uint32_t, __m128i >::computeHamming(
            __m128i data) {
        auto pattern1 = _mm_set1_epi32(0x56AAAD5B);
        auto pattern2 = _mm_set1_epi32(0x9B33366D);
        auto pattern3 = _mm_set1_epi32(0xE3C3C78E);
        auto pattern4 = _mm_set1_epi32(0x03FC07F0);
        auto pattern5 = _mm_set1_epi32(0x03FFF800);
        auto pattern6 = _mm_set1_epi32(0xFC000000);
        uint32_t hamming = 0;
        uint32_t tmp1(0), tmp2(0);
        tmp2 = mm<__m128i, uint32_t>::popcount(_mm_and_si128(data, pattern1)) & 0x01010101;
        hamming |= tmp2 << 1;
        tmp1 = mm<__m128i, uint32_t>::popcount(_mm_and_si128(data, pattern2)) & 0x01010101;
        tmp2 ^= tmp1;
        hamming |= tmp1 << 2;
        tmp1 = mm<__m128i, uint32_t>::popcount(_mm_and_si128(data, pattern3)) & 0x01010101;
        tmp2 ^= tmp1;
        hamming |= tmp1 << 3;
        tmp1 = mm<__m128i, uint32_t>::popcount(_mm_and_si128(data, pattern4)) & 0x01010101;
        tmp2 ^= tmp1;
        hamming |= tmp1 << 4;
        tmp1 = mm<__m128i, uint32_t>::popcount(_mm_and_si128(data, pattern5)) & 0x01010101;
        tmp2 ^= tmp1;
        hamming |= tmp1 << 5;
        tmp1 = mm<__m128i, uint32_t>::popcount(_mm_and_si128(data, pattern6)) & 0x01010101;
        tmp2 ^= tmp1;
        hamming |= tmp1 << 6;
        hamming |= (mm<__m128i, uint32_t>::popcount(data) + tmp2) & 0x01010101;
        return hamming;
    }

    template<>
    uint32_t hamming_t<uint32_t, __m128i >::computeHamming2(
            __m128i data) {
        auto pattern1 = _mm_set1_epi32(0x56AAAD5B);
        auto pattern2 = _mm_set1_epi32(0x9B33366D);
        auto pattern3 = _mm_set1_epi32(0xE3C3C78E);
        auto pattern4 = _mm_set1_epi32(0x03FC07F0);
        auto pattern5 = _mm_set1_epi32(0x03FFF800);
        auto pattern6 = _mm_set1_epi32(0xFC000000);
        uint32_t hamming = 0;
        uint32_t tmp1(0), tmp2(0);
        tmp2 = mm<__m128i, uint32_t>::popcount2(_mm_and_si128(data, pattern1)) & 0x01010101;
        hamming |= tmp2 << 1;
        tmp1 = mm<__m128i, uint32_t>::popcount2(_mm_and_si128(data, pattern2)) & 0x01010101;
        tmp2 ^= tmp1;
        hamming |= tmp1 << 2;
        tmp1 = mm<__m128i, uint32_t>::popcount2(_mm_and_si128(data, pattern3)) & 0x01010101;
        tmp2 ^= tmp1;
        hamming |= tmp1 << 3;
        tmp1 = mm<__m128i, uint32_t>::popcount2(_mm_and_si128(data, pattern4)) & 0x01010101;
        tmp2 ^= tmp1;
        hamming |= tmp1 << 4;
        tmp1 = mm<__m128i, uint32_t>::popcount2(_mm_and_si128(data, pattern5)) & 0x01010101;
        tmp2 ^= tmp1;
        hamming |= tmp1 << 5;
        tmp1 = mm<__m128i, uint32_t>::popcount2(_mm_and_si128(data, pattern6)) & 0x01010101;
        tmp2 ^= tmp1;
        hamming |= tmp1 << 6;
        hamming |= (mm<__m128i, uint32_t>::popcount2(data) + tmp2) & 0x01010101;
        return hamming;
    }

    template<>
    uint32_t hamming_t<uint32_t, __m128i >::computeHamming3(
            __m128i data) {
        auto pattern1 = _mm_set1_epi32(0x56AAAD5B);
        auto pattern2 = _mm_set1_epi32(0x9B33366D);
        auto pattern3 = _mm_set1_epi32(0xE3C3C78E);
        auto pattern4 = _mm_set1_epi32(0x03FC07F0);
        auto pattern5 = _mm_set1_epi32(0x03FFF800);
        auto pattern6 = _mm_set1_epi32(0xFC000000);
        uint32_t hamming = 0;
        uint32_t tmp1(0), tmp2(0);
        tmp2 = mm<__m128i, uint32_t>::popcount3(_mm_and_si128(data, pattern1)) & 0x01010101;
        hamming |= tmp2 << 1;
        tmp1 = mm<__m128i, uint32_t>::popcount3(_mm_and_si128(data, pattern2)) & 0x01010101;
        tmp2 ^= tmp1;
        hamming |= tmp1 << 2;
        tmp1 = mm<__m128i, uint32_t>::popcount3(_mm_and_si128(data, pattern3)) & 0x01010101;
        tmp2 ^= tmp1;
        hamming |= tmp1 << 3;
        tmp1 = mm<__m128i, uint32_t>::popcount3(_mm_and_si128(data, pattern4)) & 0x01010101;
        tmp2 ^= tmp1;
        hamming |= tmp1 << 4;
        tmp1 = mm<__m128i, uint32_t>::popcount3(_mm_and_si128(data, pattern5)) & 0x01010101;
        tmp2 ^= tmp1;
        hamming |= tmp1 << 5;
        tmp1 = mm<__m128i, uint32_t>::popcount3(_mm_and_si128(data, pattern6)) & 0x01010101;
        tmp2 ^= tmp1;
        hamming |= tmp1 << 6;
        hamming |= (mm<__m128i, uint32_t>::popcount3(data) + tmp2) & 0x01010101;
        return hamming;
    }

    template<>
    bool hamming_t<uint32_t, __m128i >::code_cmp_eq(
            uint32_t code1,
            uint32_t code2) {
        return code1 == code2;
    }

    template<>
    bool hamming_t<uint32_t, __m128i >::isValid() {
        return hamming_t<uint32_t, __m128i >::code_cmp_eq(this->code, hamming_t<uint32_t, __m128i >::computeHamming(this->data));
    }

    template<>
    void hamming_t<uint32_t, __m128i >::store(
            __m128i data) {
        _mm_storeu_si128(&this->data, data);
        this->code = computeHamming(data);
    }

    template<>
    void hamming_t<uint32_t, __m128i >::store2(
            __m128i data) {
        _mm_storeu_si128(&this->data, data);
        this->code = computeHamming2(data);
    }

    template<>
    void hamming_t<uint32_t, __m128i >::store3(
            __m128i data) {
        _mm_storeu_si128(&this->data, data);
        this->code = computeHamming3(data);
    }

}

#endif
