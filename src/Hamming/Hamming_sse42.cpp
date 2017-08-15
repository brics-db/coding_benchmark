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

#include <Hamming/Hamming_sse42.hpp>

/*
 * For the following algorithms, see
 * https://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel
 * Credit also goes to the book "Hacker's Delight" 2nd Edition, by Henry S. Warren, Jr., published at Addison-Wesley
 */

__m128i _mm_popcount_epi8(
        __m128i data) {
    static auto pattern1 = _mm_set1_epi8(0x55);
    static auto pattern2 = _mm_set1_epi8(0x33);
    static auto pattern3 = _mm_set1_epi8(0x0F);
    auto temp = _mm_sub_epi8(data, _mm_and_si128(_mm_srli_epi16(data, 1), pattern1));
    temp = _mm_add_epi8(_mm_and_si128(temp, pattern2), _mm_and_si128(_mm_srli_epi16(temp, 2), pattern2));
    temp = _mm_add_epi8(_mm_and_si128(temp, pattern3), _mm_and_si128(_mm_srli_epi16(temp, 4), pattern3));
    return temp;
}

__m128i _mm_popcount_epi8_2(
        __m128i data) {
    static auto lookup = _mm_set_epi8(4, 3, 3, 2, 3, 2, 2, 1, 3, 2, 2, 1, 2, 1, 1, 0);
    auto low_mask = _mm_set1_epi8(0x0f);
    auto lo = _mm_and_si128(data, low_mask);
    auto hi = _mm_and_si128(_mm_srli_epi16(data, 4), low_mask);
    auto cnt_lo = _mm_shuffle_epi8(lookup, lo);
    auto cnt_hi = _mm_shuffle_epi8(lookup, hi);
    return _mm_add_epi8(cnt_lo, cnt_hi);
}

uint64_t _mm_popcount_epi16(
        __m128i data) {
    static auto pattern1 = _mm_set1_epi16(0x5555);
    static auto pattern2 = _mm_set1_epi16(0x3333);
    static auto pattern3 = _mm_set1_epi16(0x0F0F);
    static auto pattern4 = _mm_set1_epi16(0x0101);
    static auto shuffle = _mm_set_epi64x(0xFFFFFFFFFFFFFFFF, 0x0E0C0A0806040200);
    auto temp = _mm_sub_epi16(data, _mm_and_si128(_mm_srli_epi16(data, 1), pattern1));
    temp = _mm_add_epi16(_mm_and_si128(temp, pattern2), _mm_and_si128(_mm_srli_epi16(temp, 2), pattern2));
    temp = _mm_and_si128(_mm_add_epi16(temp, _mm_srli_epi16(temp, 4)), pattern3);
    temp = _mm_srli_epi16(_mm_mullo_epi16(temp, pattern4), 8);
    return static_cast<uint64_t>(_mm_extract_epi64(_mm_shuffle_epi8(temp, shuffle), 0));
}

uint64_t _mm_popcount_epi16_2(
        __m128i data) {
    static auto mask = _mm_set1_epi16(0x0101);
    static auto shuffle = _mm_set_epi64x(0xFFFFFFFFFFFFFFFF, 0x0F0D0B0907050301);
    auto popcount8 = _mm_popcount_epi8_2(std::forward<__m128i >(data));
    return _mm_extract_epi64(_mm_shuffle_epi8(_mm_mullo_epi16(popcount8, mask), shuffle), 0);
}

uint32_t _mm_popcount_epi32(
        __m128i data) {
    static auto pattern1 = _mm_set1_epi32(0x55555555);
    static auto pattern2 = _mm_set1_epi32(0x33333333);
    static auto pattern3 = _mm_set1_epi32(0x0F0F0F0F);
    static auto pattern4 = _mm_set1_epi32(0x01010101);
    static auto shuffle = _mm_set_epi64x(0xFFFFFFFFFFFFFFFF, 0xFFFFFFFF0C080400);
    auto temp = _mm_sub_epi32(data, _mm_and_si128(_mm_srli_epi32(data, 1), pattern1));
    temp = _mm_add_epi32(_mm_and_si128(temp, pattern2), _mm_and_si128(_mm_srli_epi32(temp, 2), pattern2));
    temp = _mm_and_si128(_mm_add_epi32(temp, _mm_srli_epi32(temp, 4)), pattern3);
    temp = _mm_srli_epi32(_mm_mullo_epi32(temp, pattern4), 24);
    return static_cast<uint32_t>(_mm_extract_epi32(_mm_shuffle_epi8(temp, shuffle), 0));
}

uint32_t _mm_popcount_epi32_2(
        __m128i data) {
    static auto mask = _mm_set1_epi32(0x01010101);
    static auto shuffle = _mm_set_epi32(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x0F0B0703);
    auto popcount8 = _mm_popcount_epi8_2(std::forward<__m128i >(data));
    return _mm_extract_epi32(_mm_shuffle_epi8(_mm_mullo_epi32(popcount8, mask), shuffle), 0);
}

uint64_t HammingSSE42<uint16_t>::computeHamming(
        __m128i data) {
    static auto pattern1 = _mm_set1_epi16(static_cast<int16_t>(0xAD5B));
    static auto pattern2 = _mm_set1_epi16(static_cast<int16_t>(0x366D));
    static auto pattern3 = _mm_set1_epi16(static_cast<int16_t>(0xC78E));
    static auto pattern4 = _mm_set1_epi16(static_cast<int16_t>(0x07F0));
    static auto pattern5 = _mm_set1_epi16(static_cast<int16_t>(0xF800));
    uint64_t hamming = 0;
    uint64_t tmp1(0), tmp2(0);
    tmp2 = _mm_popcount_epi16(_mm_and_si128(data, pattern1)) & 0x0101010101010101;
    hamming |= tmp2 << 1;
    tmp1 = _mm_popcount_epi16(_mm_and_si128(data, pattern2)) & 0x0101010101010101;
    tmp2 ^= tmp1;
    hamming |= tmp1 << 2;
    tmp1 = _mm_popcount_epi16(_mm_and_si128(data, pattern3)) & 0x0101010101010101;
    tmp2 ^= tmp1;
    hamming |= tmp1 << 3;
    tmp1 = _mm_popcount_epi16(_mm_and_si128(data, pattern4)) & 0x0101010101010101;
    tmp2 ^= tmp1;
    hamming |= tmp1 << 4;
    tmp1 = _mm_popcount_epi16(_mm_and_si128(data, pattern5)) & 0x0101010101010101;
    tmp2 ^= tmp1;
    hamming |= tmp1 << 5;
    hamming |= (_mm_popcount_epi16(std::forward<__m128i >(data)) + tmp2) & 0x0101010101010101;
    return hamming;
}

uint64_t HammingSSE42<uint16_t>::computeHamming2(
        __m128i data) {
    static auto pattern1 = _mm_set1_epi16(static_cast<int16_t>(0xAD5B));
    static auto pattern2 = _mm_set1_epi16(static_cast<int16_t>(0x366D));
    static auto pattern3 = _mm_set1_epi16(static_cast<int16_t>(0xC78E));
    static auto pattern4 = _mm_set1_epi16(static_cast<int16_t>(0x07F0));
    static auto pattern5 = _mm_set1_epi16(static_cast<int16_t>(0xF800));
    uint64_t hamming = 0;
    uint64_t tmp1(0), tmp2(0);
    tmp2 = _mm_popcount_epi16_2(_mm_and_si128(data, pattern1)) & 0x0101010101010101;
    hamming |= tmp2 << 1;
    tmp1 = _mm_popcount_epi16_2(_mm_and_si128(data, pattern2)) & 0x0101010101010101;
    tmp2 ^= tmp1;
    hamming |= tmp1 << 2;
    tmp1 = _mm_popcount_epi16_2(_mm_and_si128(data, pattern3)) & 0x0101010101010101;
    tmp2 ^= tmp1;
    hamming |= tmp1 << 3;
    tmp1 = _mm_popcount_epi16_2(_mm_and_si128(data, pattern4)) & 0x0101010101010101;
    tmp2 ^= tmp1;
    hamming |= tmp1 << 4;
    tmp1 = _mm_popcount_epi16_2(_mm_and_si128(data, pattern5)) & 0x0101010101010101;
    tmp2 ^= tmp1;
    hamming |= tmp1 << 5;
    hamming |= (_mm_popcount_epi16_2(std::forward<__m128i >(data)) + tmp2) & 0x0101010101010101;
    return hamming;
}

uint32_t HammingSSE42<uint32_t>::computeHamming(
        __m128i data) {
    static auto pattern1 = _mm_set1_epi32(0x56AAAD5B);
    static auto pattern2 = _mm_set1_epi32(0x9B33366D);
    static auto pattern3 = _mm_set1_epi32(0xE3C3C78E);
    static auto pattern4 = _mm_set1_epi32(0x03FC07F0);
    static auto pattern5 = _mm_set1_epi32(0x03FFF800);
    static auto pattern6 = _mm_set1_epi32(0xFC000000);
    uint32_t hamming = 0;
    uint32_t tmp1(0), tmp2(0);
    tmp2 = _mm_popcount_epi32(_mm_and_si128(data, pattern1)) & 0x01010101;
    hamming |= tmp2 << 1;
    tmp1 = _mm_popcount_epi32(_mm_and_si128(data, pattern2)) & 0x01010101;
    tmp2 ^= tmp1;
    hamming |= tmp1 << 2;
    tmp1 = _mm_popcount_epi32(_mm_and_si128(data, pattern3)) & 0x01010101;
    tmp2 ^= tmp1;
    hamming |= tmp1 << 3;
    tmp1 = _mm_popcount_epi32(_mm_and_si128(data, pattern4)) & 0x01010101;
    tmp2 ^= tmp1;
    hamming |= tmp1 << 4;
    tmp1 = _mm_popcount_epi32(_mm_and_si128(data, pattern5)) & 0x01010101;
    tmp2 ^= tmp1;
    hamming |= tmp1 << 5;
    tmp1 = _mm_popcount_epi32(_mm_and_si128(data, pattern6)) & 0x01010101;
    tmp2 ^= tmp1;
    hamming |= tmp1 << 6;
    hamming |= (_mm_popcount_epi32(std::forward<__m128i >(data)) + tmp2) & 0x01010101;
    return hamming;
}

uint32_t HammingSSE42<uint32_t>::computeHamming2(
        __m128i data) {
    static auto pattern1 = _mm_set1_epi32(0x56AAAD5B);
    static auto pattern2 = _mm_set1_epi32(0x9B33366D);
    static auto pattern3 = _mm_set1_epi32(0xE3C3C78E);
    static auto pattern4 = _mm_set1_epi32(0x03FC07F0);
    static auto pattern5 = _mm_set1_epi32(0x03FFF800);
    static auto pattern6 = _mm_set1_epi32(0xFC000000);
    uint32_t hamming = 0;
    uint32_t tmp1(0), tmp2(0);
    tmp2 = _mm_popcount_epi32_2(_mm_and_si128(data, pattern1)) & 0x01010101;
    hamming |= tmp2 << 1;
    tmp1 = _mm_popcount_epi32_2(_mm_and_si128(data, pattern2)) & 0x01010101;
    tmp2 ^= tmp1;
    hamming |= tmp1 << 2;
    tmp1 = _mm_popcount_epi32_2(_mm_and_si128(data, pattern3)) & 0x01010101;
    tmp2 ^= tmp1;
    hamming |= tmp1 << 3;
    tmp1 = _mm_popcount_epi32_2(_mm_and_si128(data, pattern4)) & 0x01010101;
    tmp2 ^= tmp1;
    hamming |= tmp1 << 4;
    tmp1 = _mm_popcount_epi32_2(_mm_and_si128(data, pattern5)) & 0x01010101;
    tmp2 ^= tmp1;
    hamming |= tmp1 << 5;
    tmp1 = _mm_popcount_epi32_2(_mm_and_si128(data, pattern6)) & 0x01010101;
    tmp2 ^= tmp1;
    hamming |= tmp1 << 6;
    hamming |= (_mm_popcount_epi32_2(std::forward<__m128i >(data)) + tmp2) & 0x01010101;
    return hamming;
}
