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
#include <Util/SIMD.hpp>

/*
 * For the following algorithms, see
 * https://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel
 * Credit also goes to the book "Hacker's Delight" 2nd Edition, by Henry S. Warren, Jr., published at Addison-Wesley
 */

uint64_t hamming_t<uint16_t, __m128i >::computeHamming(
        __m128i data) {
    static auto pattern1 = _mm_set1_epi16(static_cast<int16_t>(0xAD5B));
    static auto pattern2 = _mm_set1_epi16(static_cast<int16_t>(0x366D));
    static auto pattern3 = _mm_set1_epi16(static_cast<int16_t>(0xC78E));
    static auto pattern4 = _mm_set1_epi16(static_cast<int16_t>(0x07F0));
    static auto pattern5 = _mm_set1_epi16(static_cast<int16_t>(0xF800));
    uint64_t hamming = 0;
    uint64_t tmp1(0), tmp2(0);
    tmp2 = SIMD<__m128i, uint16_t>::popcount(_mm_and_si128(data, pattern1)) & 0x0101010101010101;
    hamming |= tmp2 << 1;
    tmp1 = SIMD<__m128i, uint16_t>::popcount(_mm_and_si128(data, pattern2)) & 0x0101010101010101;
    tmp2 ^= tmp1;
    hamming |= tmp1 << 2;
    tmp1 = SIMD<__m128i, uint16_t>::popcount(_mm_and_si128(data, pattern3)) & 0x0101010101010101;
    tmp2 ^= tmp1;
    hamming |= tmp1 << 3;
    tmp1 = SIMD<__m128i, uint16_t>::popcount(_mm_and_si128(data, pattern4)) & 0x0101010101010101;
    tmp2 ^= tmp1;
    hamming |= tmp1 << 4;
    tmp1 = SIMD<__m128i, uint16_t>::popcount(_mm_and_si128(data, pattern5)) & 0x0101010101010101;
    tmp2 ^= tmp1;
    hamming |= tmp1 << 5;
    hamming |= (SIMD<__m128i, uint16_t>::popcount(data) + tmp2) & 0x0101010101010101;
    return hamming;
}

uint64_t hamming_t<uint16_t, __m128i >::computeHamming2(
        __m128i data) {
    static auto pattern1 = _mm_set1_epi16(static_cast<int16_t>(0xAD5B));
    static auto pattern2 = _mm_set1_epi16(static_cast<int16_t>(0x366D));
    static auto pattern3 = _mm_set1_epi16(static_cast<int16_t>(0xC78E));
    static auto pattern4 = _mm_set1_epi16(static_cast<int16_t>(0x07F0));
    static auto pattern5 = _mm_set1_epi16(static_cast<int16_t>(0xF800));
    uint64_t hamming = 0;
    uint64_t tmp1(0), tmp2(0);
    tmp2 = SIMD<__m128i, uint16_t>::popcount2(_mm_and_si128(data, pattern1)) & 0x0101010101010101;
    hamming |= tmp2 << 1;
    tmp1 = SIMD<__m128i, uint16_t>::popcount2(_mm_and_si128(data, pattern2)) & 0x0101010101010101;
    tmp2 ^= tmp1;
    hamming |= tmp1 << 2;
    tmp1 = SIMD<__m128i, uint16_t>::popcount2(_mm_and_si128(data, pattern3)) & 0x0101010101010101;
    tmp2 ^= tmp1;
    hamming |= tmp1 << 3;
    tmp1 = SIMD<__m128i, uint16_t>::popcount2(_mm_and_si128(data, pattern4)) & 0x0101010101010101;
    tmp2 ^= tmp1;
    hamming |= tmp1 << 4;
    tmp1 = SIMD<__m128i, uint16_t>::popcount2(_mm_and_si128(data, pattern5)) & 0x0101010101010101;
    tmp2 ^= tmp1;
    hamming |= tmp1 << 5;
    hamming |= (SIMD<__m128i, uint16_t>::popcount2(data) + tmp2) & 0x0101010101010101;
    return hamming;
}

uint32_t hamming_t<uint32_t, __m128i >::computeHamming(
        __m128i data) {
    static auto pattern1 = _mm_set1_epi32(0x56AAAD5B);
    static auto pattern2 = _mm_set1_epi32(0x9B33366D);
    static auto pattern3 = _mm_set1_epi32(0xE3C3C78E);
    static auto pattern4 = _mm_set1_epi32(0x03FC07F0);
    static auto pattern5 = _mm_set1_epi32(0x03FFF800);
    static auto pattern6 = _mm_set1_epi32(0xFC000000);
    uint32_t hamming = 0;
    uint32_t tmp1(0), tmp2(0);
    tmp2 = SIMD<__m128i, uint32_t>::popcount(_mm_and_si128(data, pattern1)) & 0x01010101;
    hamming |= tmp2 << 1;
    tmp1 = SIMD<__m128i, uint32_t>::popcount(_mm_and_si128(data, pattern2)) & 0x01010101;
    tmp2 ^= tmp1;
    hamming |= tmp1 << 2;
    tmp1 = SIMD<__m128i, uint32_t>::popcount(_mm_and_si128(data, pattern3)) & 0x01010101;
    tmp2 ^= tmp1;
    hamming |= tmp1 << 3;
    tmp1 = SIMD<__m128i, uint32_t>::popcount(_mm_and_si128(data, pattern4)) & 0x01010101;
    tmp2 ^= tmp1;
    hamming |= tmp1 << 4;
    tmp1 = SIMD<__m128i, uint32_t>::popcount(_mm_and_si128(data, pattern5)) & 0x01010101;
    tmp2 ^= tmp1;
    hamming |= tmp1 << 5;
    tmp1 = SIMD<__m128i, uint32_t>::popcount(_mm_and_si128(data, pattern6)) & 0x01010101;
    tmp2 ^= tmp1;
    hamming |= tmp1 << 6;
    hamming |= (SIMD<__m128i, uint32_t>::popcount(data) + tmp2) & 0x01010101;
    return hamming;
}

uint32_t hamming_t<uint32_t, __m128i >::computeHamming2(
        __m128i data) {
    static auto pattern1 = _mm_set1_epi32(0x56AAAD5B);
    static auto pattern2 = _mm_set1_epi32(0x9B33366D);
    static auto pattern3 = _mm_set1_epi32(0xE3C3C78E);
    static auto pattern4 = _mm_set1_epi32(0x03FC07F0);
    static auto pattern5 = _mm_set1_epi32(0x03FFF800);
    static auto pattern6 = _mm_set1_epi32(0xFC000000);
    uint32_t hamming = 0;
    uint32_t tmp1(0), tmp2(0);
    tmp2 = SIMD<__m128i, uint32_t>::popcount2(_mm_and_si128(data, pattern1)) & 0x01010101;
    hamming |= tmp2 << 1;
    tmp1 = SIMD<__m128i, uint32_t>::popcount2(_mm_and_si128(data, pattern2)) & 0x01010101;
    tmp2 ^= tmp1;
    hamming |= tmp1 << 2;
    tmp1 = SIMD<__m128i, uint32_t>::popcount2(_mm_and_si128(data, pattern3)) & 0x01010101;
    tmp2 ^= tmp1;
    hamming |= tmp1 << 3;
    tmp1 = SIMD<__m128i, uint32_t>::popcount2(_mm_and_si128(data, pattern4)) & 0x01010101;
    tmp2 ^= tmp1;
    hamming |= tmp1 << 4;
    tmp1 = SIMD<__m128i, uint32_t>::popcount2(_mm_and_si128(data, pattern5)) & 0x01010101;
    tmp2 ^= tmp1;
    hamming |= tmp1 << 5;
    tmp1 = SIMD<__m128i, uint32_t>::popcount2(_mm_and_si128(data, pattern6)) & 0x01010101;
    tmp2 ^= tmp1;
    hamming |= tmp1 << 6;
    hamming |= (SIMD<__m128i, uint32_t>::popcount2(data) + tmp2) & 0x01010101;
    return hamming;
}

template
struct Hamming_sse42_16<1> ;
template
struct Hamming_sse42_16<2> ;
template
struct Hamming_sse42_16<4> ;
template
struct Hamming_sse42_16<8> ;
template
struct Hamming_sse42_16<16> ;
template
struct Hamming_sse42_16<32> ;
template
struct Hamming_sse42_16<64> ;
template
struct Hamming_sse42_16<128> ;
template
struct Hamming_sse42_16<256> ;
template
struct Hamming_sse42_16<512> ;
template
struct Hamming_sse42_16<1024> ;

template
struct Hamming_sse42_32<1> ;
template
struct Hamming_sse42_32<2> ;
template
struct Hamming_sse42_32<4> ;
template
struct Hamming_sse42_32<8> ;
template
struct Hamming_sse42_32<16> ;
template
struct Hamming_sse42_32<32> ;
template
struct Hamming_sse42_32<64> ;
template
struct Hamming_sse42_32<128> ;
template
struct Hamming_sse42_32<256> ;
template
struct Hamming_sse42_32<512> ;
template
struct Hamming_sse42_32<1024> ;
