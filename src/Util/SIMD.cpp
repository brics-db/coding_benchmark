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
 * File:   SIMD.cpp
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 21-08-2017 15:06
 */

#include <Util/SIMD.hpp>

__m128i SIMD<__m128i, uint8_t>::set1(
        uint8_t a) {
    return _mm_set1_epi8(a);
}

__m128i SIMD<__m128i, uint8_t>::add(
        __m128i a,
        __m128i b) {
    return _mm_add_epi8(a, b);
}

__m128i SIMD<__m128i, uint8_t>::popcount(
        __m128i a) {
    static auto pattern1 = _mm_set1_epi8(0x55);
    static auto pattern2 = _mm_set1_epi8(0x33);
    static auto pattern3 = _mm_set1_epi8(0x0F);
    auto temp = _mm_sub_epi8(a, _mm_and_si128(_mm_srli_epi16(a, 1), pattern1));
    temp = _mm_add_epi8(_mm_and_si128(temp, pattern2), _mm_and_si128(_mm_srli_epi16(temp, 2), pattern2));
    temp = _mm_add_epi8(_mm_and_si128(temp, pattern3), _mm_and_si128(_mm_srli_epi16(temp, 4), pattern3));
    return temp;
}

__m128i SIMD<__m128i, uint8_t>::popcount2(
        __m128i a) {
    static auto lookup = _mm_set_epi8(4, 3, 3, 2, 3, 2, 2, 1, 3, 2, 2, 1, 2, 1, 1, 0);
    auto low_mask = _mm_set1_epi8(0x0f);
    auto lo = _mm_and_si128(a, low_mask);
    auto hi = _mm_and_si128(_mm_srli_epi16(a, 4), low_mask);
    auto cnt_lo = _mm_shuffle_epi8(lookup, lo);
    auto cnt_hi = _mm_shuffle_epi8(lookup, hi);
    return _mm_add_epi8(cnt_lo, cnt_hi);
}

__m128i SIMD<__m128i, uint16_t>::set1(
        uint16_t a) {
    return _mm_set1_epi16(a);
}

__m128i SIMD<__m128i, uint16_t>::add(
        __m128i a,
        __m128i b) {
    return _mm_add_epi16(a, b);
}

uint64_t SIMD<__m128i, uint16_t>::popcount(
        __m128i a) {
    static auto pattern1 = _mm_set1_epi16(0x5555);
    static auto pattern2 = _mm_set1_epi16(0x3333);
    static auto pattern3 = _mm_set1_epi16(0x0F0F);
    static auto pattern4 = _mm_set1_epi16(0x0101);
    static auto shuffle = _mm_set_epi64x(0xFFFFFFFFFFFFFFFF, 0x0E0C0A0806040200);
    auto temp = _mm_sub_epi16(a, _mm_and_si128(_mm_srli_epi16(a, 1), pattern1));
    temp = _mm_add_epi16(_mm_and_si128(temp, pattern2), _mm_and_si128(_mm_srli_epi16(temp, 2), pattern2));
    temp = _mm_and_si128(_mm_add_epi16(temp, _mm_srli_epi16(temp, 4)), pattern3);
    temp = _mm_srli_epi16(_mm_mullo_epi16(temp, pattern4), 8);
    return static_cast<uint64_t>(_mm_extract_epi64(_mm_shuffle_epi8(temp, shuffle), 0));
}

uint64_t SIMD<__m128i, uint16_t>::popcount2(
        __m128i a) {
    static auto mask = _mm_set1_epi16(0x0101);
    static auto shuffle = _mm_set_epi64x(0xFFFFFFFFFFFFFFFF, 0x0F0D0B0907050301);
    auto popcount8 = SIMD<__m128i, uint8_t>::popcount2(a);
    return _mm_extract_epi64(_mm_shuffle_epi8(_mm_mullo_epi16(popcount8, mask), shuffle), 0);
}

__m128i SIMD<__m128i, uint32_t>::set1(
        uint32_t a) {
    return _mm_set1_epi32(a);
}

__m128i SIMD<__m128i, uint32_t>::add(
        __m128i a,
        __m128i b) {
    return _mm_add_epi32(a, b);
}

uint32_t SIMD<__m128i, uint32_t>::popcount(
        __m128i a) {
    static auto pattern1 = _mm_set1_epi32(0x55555555);
    static auto pattern2 = _mm_set1_epi32(0x33333333);
    static auto pattern3 = _mm_set1_epi32(0x0F0F0F0F);
    static auto pattern4 = _mm_set1_epi32(0x01010101);
    static auto shuffle = _mm_set_epi64x(0xFFFFFFFFFFFFFFFF, 0xFFFFFFFF0C080400);
    auto temp = _mm_sub_epi32(a, _mm_and_si128(_mm_srli_epi32(a, 1), pattern1));
    temp = _mm_add_epi32(_mm_and_si128(temp, pattern2), _mm_and_si128(_mm_srli_epi32(temp, 2), pattern2));
    temp = _mm_and_si128(_mm_add_epi32(temp, _mm_srli_epi32(temp, 4)), pattern3);
    temp = _mm_srli_epi32(_mm_mullo_epi32(temp, pattern4), 24);
    return static_cast<uint32_t>(_mm_extract_epi32(_mm_shuffle_epi8(temp, shuffle), 0));
}

uint32_t SIMD<__m128i, uint32_t>::popcount2(
        __m128i a) {
    static auto mask = _mm_set1_epi32(0x01010101);
    static auto shuffle = _mm_set_epi32(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x0F0B0703);
    auto popcount8 = SIMD<__m128i, uint8_t>::popcount2(a);
    return _mm_extract_epi32(_mm_shuffle_epi8(_mm_mullo_epi32(popcount8, mask), shuffle), 0);
}

#ifdef __AVX2__
__m256i SIMD<__m256i, uint8_t>::set1(
        uint8_t a) {
    return _mm256_set1_epi8(a);
}

__m256i SIMD<__m256i, uint8_t>::add(
        __m256i a,
        __m256i b) {
    return _mm256_add_epi8(a, b);
}

__m256i SIMD<__m256i, uint8_t>::popcount(
        __m256i a) {
    static auto pattern1 = _mm256_set1_epi8(0x55);
    static auto pattern2 = _mm256_set1_epi8(0x33);
    static auto pattern3 = _mm256_set1_epi8(0x0F);
    auto temp = _mm256_sub_epi8(a, _mm256_and_si256(_mm256_srli_epi16(a, 1), pattern1));
    temp = _mm256_add_epi8(_mm256_and_si256(temp, pattern2), _mm256_and_si256(_mm256_srli_epi16(temp, 2), pattern2));
    temp = _mm256_add_epi8(_mm256_and_si256(temp, pattern3), _mm256_and_si256(_mm256_srli_epi16(temp, 4), pattern3));
    return temp;
}

__m256i SIMD<__m256i, uint8_t>::popcount2(
        __m256i a) {
    static auto lookup = _mm256_set_epi8(4, 3, 3, 2, 3, 2, 2, 1, 3, 2, 2, 1, 2, 1, 1, 0, 4, 3, 3, 2, 3, 2, 2, 1, 3, 2, 2, 1, 2, 1, 1, 0);
    auto low_mask = _mm256_set1_epi8(0x0f);
    auto lo = _mm256_and_si256(a, low_mask);
    auto hi = _mm256_and_si256(_mm256_srli_epi16(a, 4), low_mask);
    auto cnt_lo = _mm256_shuffle_epi8(lookup, lo);
    auto cnt_hi = _mm256_shuffle_epi8(lookup, hi);
    return _mm256_add_epi8(cnt_lo, cnt_hi);
}

__m256i SIMD<__m256i, uint16_t>::set1(
        uint16_t a) {
    return _mm256_set1_epi16(a);
}

__m256i SIMD<__m256i, uint16_t>::add(
        __m256i a,
        __m256i b) {
    return _mm256_add_epi16(a, b);
}

__m128i SIMD<__m256i, uint16_t>::popcount(
        __m256i data) {
    static auto pattern1 = _mm256_set1_epi16(0x5555);
    static auto pattern2 = _mm256_set1_epi16(0x3333);
    static auto pattern3 = _mm256_set1_epi16(0x0F0F);
    static auto pattern4 = _mm256_set1_epi16(0x0101);
    static auto shuffle = _mm256_set_epi64x(0xFFFFFFFFFFFFFFFF, 0x0E0C0A0806040200, 0xFFFFFFFFFFFFFFFF, 0x0E0C0A0806040200);
    auto temp = _mm256_sub_epi16(data, _mm256_and_si256(_mm256_srli_epi16(data, 1), pattern1));
    temp = _mm256_add_epi16(_mm256_and_si256(temp, pattern2), _mm256_and_si256(_mm256_srli_epi16(temp, 2), pattern2));
    temp = _mm256_and_si256(_mm256_add_epi16(temp, _mm256_srli_epi16(temp, 4)), pattern3);
    temp = _mm256_srli_epi16(_mm256_mullo_epi16(temp, pattern4), 8);
    temp = _mm256_shuffle_epi8(temp, shuffle);
    return _mm_set_epi64x(_mm256_extract_epi64(temp, 2), _mm256_extract_epi64(temp, 0));
}

__m128i SIMD<__m256i, uint16_t>::popcount2(
        __m256i data) {
    static auto mask = _mm256_set1_epi16(0x0101);
    static auto shuffle = _mm256_set_epi64x(0xFFFFFFFFFFFFFFFF, 0x0F0D0B0907050301, 0xFFFFFFFFFFFFFFFF, 0x0F0D0B0907050301);
    auto popcount8 = SIMD<__m256i, uint8_t>::popcount2(data);
    auto temp = _mm256_shuffle_epi8(_mm256_mullo_epi16(popcount8, mask), shuffle);
    return _mm_set_epi64x(_mm256_extract_epi64(temp, 2), _mm256_extract_epi64(temp, 0));
}

__m256i SIMD<__m256i, uint32_t>::set1(
        uint32_t a) {
    return _mm256_set1_epi32(a);
}

__m256i SIMD<__m256i, uint32_t>::add(
        __m256i a,
        __m256i b) {
    return _mm256_add_epi32(a, b);
}

uint64_t SIMD<__m256i, uint32_t>::popcount(
        __m256i data) {
    static auto pattern1 = _mm256_set1_epi32(0x55555555);
    static auto pattern2 = _mm256_set1_epi32(0x33333333);
    static auto pattern3 = _mm256_set1_epi32(0x0F0F0F0F);
    static auto pattern4 = _mm256_set1_epi32(0x01010101);
    static auto shuffle = _mm256_set_epi64x(0xFFFFFFFFFFFFFFFF, 0xFFFFFFFF0C080400, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFF0C080400);
    auto temp = _mm256_sub_epi32(data, _mm256_and_si256(_mm256_srli_epi32(data, 1), pattern1));
    temp = _mm256_add_epi32(_mm256_and_si256(temp, pattern2), _mm256_and_si256(_mm256_srli_epi32(temp, 2), pattern2));
    temp = _mm256_and_si256(_mm256_add_epi32(temp, _mm256_srli_epi32(temp, 4)), pattern3);
    temp = _mm256_srli_epi32(_mm256_mullo_epi32(temp, pattern4), 24);
    temp = _mm256_shuffle_epi8(temp, shuffle);
    return static_cast<uint64_t>(_mm256_extract_epi32(temp, 0)) | (static_cast<uint64_t>(_mm256_extract_epi32(temp, 4)) << 32);
}

uint64_t SIMD<__m256i, uint32_t>::popcount2(
        __m256i data) {
    static auto mask = _mm256_set1_epi32(0x01010101);
    static auto shuffle = _mm256_set_epi32(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x0F0B0703, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x0F0B0703);
    auto popcount8 = SIMD<__m256i, uint8_t>::popcount2(data);
    auto temp = _mm256_shuffle_epi8(_mm256_mullo_epi32(popcount8, mask), shuffle);
    return static_cast<uint64_t>(_mm256_extract_epi32(temp, 0)) | (static_cast<uint64_t>(_mm256_extract_epi32(temp, 4)) << 32);
}
#endif
