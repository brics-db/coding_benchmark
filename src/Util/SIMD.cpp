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
    auto pattern1 = _mm_set1_epi8(0x55);
    auto pattern2 = _mm_set1_epi8(0x33);
    auto pattern3 = _mm_set1_epi8(0x0F);
    auto temp = _mm_sub_epi8(a, _mm_and_si128(_mm_srli_epi16(a, 1), pattern1));
    temp = _mm_add_epi8(_mm_and_si128(temp, pattern2), _mm_and_si128(_mm_srli_epi16(temp, 2), pattern2));
    temp = _mm_add_epi8(_mm_and_si128(temp, pattern3), _mm_and_si128(_mm_srli_epi16(temp, 4), pattern3));
    return temp;
}

__m128i SIMD<__m128i, uint8_t>::popcount2(
        __m128i a) {
    auto lookup = _mm_set_epi8(4, 3, 3, 2, 3, 2, 2, 1, 3, 2, 2, 1, 2, 1, 1, 0);
    auto low_mask = _mm_set1_epi8(0x0f);
    auto lo = _mm_and_si128(a, low_mask);
    auto hi = _mm_and_si128(_mm_srli_epi16(a, 4), low_mask);
    auto cnt_lo = _mm_shuffle_epi8(lookup, lo);
    auto cnt_hi = _mm_shuffle_epi8(lookup, hi);
    return _mm_add_epi8(cnt_lo, cnt_hi);
}

__m128i SIMD<__m128i, uint8_t>::popcount3(
        __m128i a) {
    return _mm_set_epi8(_mm_popcnt_u32(_mm_extract_epi8(a, 15)), _mm_popcnt_u32(_mm_extract_epi8(a, 14)), _mm_popcnt_u32(_mm_extract_epi8(a, 13)), _mm_popcnt_u32(_mm_extract_epi8(a, 12)),
            _mm_popcnt_u32(_mm_extract_epi8(a, 11)), _mm_popcnt_u32(_mm_extract_epi8(a, 10)), _mm_popcnt_u32(_mm_extract_epi8(a, 9)), _mm_popcnt_u32(_mm_extract_epi8(a, 8)),
            _mm_popcnt_u32(_mm_extract_epi8(a, 7)), _mm_popcnt_u32(_mm_extract_epi8(a, 6)), _mm_popcnt_u32(_mm_extract_epi8(a, 5)), _mm_popcnt_u32(_mm_extract_epi8(a, 4)),
            _mm_popcnt_u32(_mm_extract_epi8(a, 3)), _mm_popcnt_u32(_mm_extract_epi8(a, 2)), _mm_popcnt_u32(_mm_extract_epi8(a, 1)), _mm_popcnt_u32(_mm_extract_epi8(a, 0)));
}

__m128i SIMD<__m128i, uint8_t>::cvt_larger_lo(
        __m128i a) {
    return _mm_cvtepi8_epi16(a);
}

__m128i SIMD<__m128i, uint8_t>::cvt_larger_hi(
        __m128i a) {
    return _mm_cvtepi8_epi16(_mm_srli_si128(a, 8));
}

uint8_t SIMD<__m128i, uint8_t>::sum(
        __m128i a) {
    auto tmp = _mm_add_epi8(a, _mm_srli_si128(a, 8));
    tmp = _mm_add_epi8(tmp, _mm_srli_si128(tmp, 4));
    tmp = _mm_add_epi8(tmp, _mm_srli_si128(tmp, 2));
    return _mm_extract_epi8(tmp, 1) + _mm_extract_epi8(tmp, 0);
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
    auto pattern1 = _mm_set1_epi16(0x5555);
    auto pattern2 = _mm_set1_epi16(0x3333);
    auto pattern3 = _mm_set1_epi16(0x0F0F);
    auto pattern4 = _mm_set1_epi16(0x0101);
    auto shuffle = _mm_set_epi64x(0xFFFFFFFFFFFFFFFF, 0x0F0D0B0907050301);
    auto temp = _mm_sub_epi16(a, _mm_and_si128(_mm_srli_epi16(a, 1), pattern1));
    temp = _mm_add_epi16(_mm_and_si128(temp, pattern2), _mm_and_si128(_mm_srli_epi16(temp, 2), pattern2));
    temp = _mm_and_si128(_mm_add_epi16(temp, _mm_srli_epi16(temp, 4)), pattern3);
    temp = _mm_mullo_epi16(temp, pattern4);
    return static_cast<uint64_t>(_mm_extract_epi64(_mm_shuffle_epi8(temp, shuffle), 0));
}

uint64_t SIMD<__m128i, uint16_t>::popcount2(
        __m128i a) {
    auto mask = _mm_set1_epi16(0x0101);
    auto shuffle = _mm_set_epi64x(0xFFFFFFFFFFFFFFFF, 0x0F0D0B0907050301);
    auto popcount8 = SIMD<__m128i, uint8_t>::popcount2(a);
    return _mm_extract_epi64(_mm_shuffle_epi8(_mm_mullo_epi16(popcount8, mask), shuffle), 0);
}

uint64_t SIMD<__m128i, uint16_t>::popcount3(
        __m128i a) {
    return (static_cast<uint64_t>(_mm_popcnt_u32(_mm_extract_epi16(a, 7))) << 56) | (static_cast<uint64_t>(_mm_popcnt_u32(_mm_extract_epi16(a, 6))) << 48)
            | (static_cast<uint64_t>(_mm_popcnt_u32(_mm_extract_epi16(a, 5))) << 40) | (static_cast<uint64_t>(_mm_popcnt_u32(_mm_extract_epi16(a, 4))) << 23)
            | (static_cast<uint64_t>(_mm_popcnt_u32(_mm_extract_epi16(a, 3))) << 24) | (static_cast<uint64_t>(_mm_popcnt_u32(_mm_extract_epi16(a, 2))) << 16)
            | (static_cast<uint64_t>(_mm_popcnt_u32(_mm_extract_epi16(a, 1))) << 8) | static_cast<uint64_t>(_mm_popcnt_u32(_mm_extract_epi16(a, 0)));
}

__m128i SIMD<__m128i, uint16_t>::cvt_larger_lo(
        __m128i a) {
    return _mm_cvtepi16_epi32(a);
}

__m128i SIMD<__m128i, uint16_t>::cvt_larger_hi(
        __m128i a) {
    return _mm_cvtepi16_epi32(_mm_srli_si128(a, 8));
}

uint16_t SIMD<__m128i, uint16_t>::sum(
        __m128i a) {
    auto tmp = _mm_add_epi16(a, _mm_srli_si128(a, 8));
    tmp = _mm_add_epi16(tmp, _mm_srli_si128(tmp, 4));
    return _mm_extract_epi16(tmp, 1) + _mm_extract_epi16(tmp, 0);
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
    auto pattern1 = _mm_set1_epi32(0x55555555);
    auto pattern2 = _mm_set1_epi32(0x33333333);
    auto pattern3 = _mm_set1_epi32(0x0F0F0F0F);
    auto pattern4 = _mm_set1_epi32(0x01010101);
    auto shuffle = _mm_set_epi32(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x0F0B0703);
    auto temp = _mm_sub_epi32(a, _mm_and_si128(_mm_srli_epi32(a, 1), pattern1));
    temp = _mm_add_epi32(_mm_and_si128(temp, pattern2), _mm_and_si128(_mm_srli_epi32(temp, 2), pattern2));
    temp = _mm_and_si128(_mm_add_epi32(temp, _mm_srli_epi32(temp, 4)), pattern3);
    temp = _mm_mullo_epi32(temp, pattern4);
    return static_cast<uint32_t>(_mm_extract_epi32(_mm_shuffle_epi8(temp, shuffle), 0));
}

uint32_t SIMD<__m128i, uint32_t>::popcount2(
        __m128i a) {
    auto mask = _mm_set1_epi32(0x01010101);
    auto shuffle = _mm_set_epi32(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x0F0B0703);
    auto popcount8 = SIMD<__m128i, uint8_t>::popcount2(a);
    return _mm_extract_epi32(_mm_shuffle_epi8(_mm_mullo_epi32(popcount8, mask), shuffle), 0);
}

uint32_t SIMD<__m128i, uint32_t>::popcount3(
        __m128i a) {
    return (_mm_popcnt_u32(_mm_extract_epi32(a, 3) << 24) | (_mm_popcnt_u32(_mm_extract_epi32(a, 2)) << 16) | (_mm_popcnt_u32(_mm_extract_epi32(a, 1)) << 8) | _mm_popcnt_u32(_mm_extract_epi32(a, 0)));
}

__m128i SIMD<__m128i, uint32_t>::cvt_larger_lo(
        __m128i a) {
    return _mm_cvtepi16_epi32(a);
}

__m128i SIMD<__m128i, uint32_t>::cvt_larger_hi(
        __m128i a) {
    return _mm_cvtepi16_epi32(_mm_srli_si128(a, 8));
}

uint32_t SIMD<__m128i, uint32_t>::sum(
        __m128i a) {
    auto tmp = _mm_add_epi32(a, _mm_srli_si128(a, 8));
    return _mm_extract_epi32(tmp, 1) + _mm_extract_epi32(tmp, 0);
}

__m128i SIMD<__m128i, uint64_t>::set1(
        uint32_t a) {
    return _mm_set1_epi64x(a);
}

__m128i SIMD<__m128i, uint64_t>::add(
        __m128i a,
        __m128i b) {
    return _mm_add_epi64(a, b);
}

uint16_t SIMD<__m128i, uint64_t>::popcount(
        __m128i a) {
    auto pattern1 = _mm_set1_epi64x(0x5555555555555555);
    auto pattern2 = _mm_set1_epi64x(0x3333333333333333);
    auto pattern3 = _mm_set1_epi64x(0x0F0F0F0F0F0F0F0F);
    auto pattern4 = _mm_set1_epi64x(0x0101010101010101);
    auto shuffle = _mm_set_epi64x(0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFF0F07);
    auto temp = _mm_sub_epi32(a, _mm_and_si128(_mm_srli_epi32(a, 1), pattern1));
    temp = _mm_add_epi32(_mm_and_si128(temp, pattern2), _mm_and_si128(_mm_srli_epi32(temp, 2), pattern2));
    temp = _mm_and_si128(_mm_add_epi32(temp, _mm_srli_epi32(temp, 4)), pattern3);
    temp = _mm_mullo_epi32(temp, pattern4);
    return static_cast<uint16_t>(_mm_extract_epi16(_mm_shuffle_epi8(temp, shuffle), 0));
}

uint16_t SIMD<__m128i, uint64_t>::popcount2(
        __m128i a) {
    return (static_cast<uint16_t>(_mm_popcnt_u64(_mm_extract_epi64(a, 1))) << 8) | static_cast<uint16_t>(_mm_popcnt_u64(_mm_extract_epi64(a, 0)));
}

uint16_t SIMD<__m128i, uint64_t>::popcount3(
        __m128i a) {
    return (_mm_popcnt_u64(_mm_extract_epi64(a, 1) << 8) | _mm_popcnt_u64(_mm_extract_epi64(a, 0)));
}

uint64_t SIMD<__m128i, uint64_t>::sum(
        __m128i a) {
    auto tmp = _mm_add_epi32(a, _mm_srli_si128(a, 8));
    return _mm_extract_epi32(tmp, 1) + _mm_extract_epi32(tmp, 0);
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
    auto pattern1 = _mm256_set1_epi8(0x55);
    auto pattern2 = _mm256_set1_epi8(0x33);
    auto pattern3 = _mm256_set1_epi8(0x0F);
    auto temp = _mm256_sub_epi8(a, _mm256_and_si256(_mm256_srli_epi16(a, 1), pattern1));
    temp = _mm256_add_epi8(_mm256_and_si256(temp, pattern2), _mm256_and_si256(_mm256_srli_epi16(temp, 2), pattern2));
    temp = _mm256_add_epi8(_mm256_and_si256(temp, pattern3), _mm256_and_si256(_mm256_srli_epi16(temp, 4), pattern3));
    return temp;
}

__m256i SIMD<__m256i, uint8_t>::popcount2(
        __m256i a) {
    auto lookup = _mm256_set_epi8(4, 3, 3, 2, 3, 2, 2, 1, 3, 2, 2, 1, 2, 1, 1, 0, 4, 3, 3, 2, 3, 2, 2, 1, 3, 2, 2, 1, 2, 1, 1, 0);
    auto low_mask = _mm256_set1_epi8(0x0f);
    auto lo = _mm256_and_si256(a, low_mask);
    auto hi = _mm256_and_si256(_mm256_srli_epi16(a, 4), low_mask);
    auto cnt_lo = _mm256_shuffle_epi8(lookup, lo);
    auto cnt_hi = _mm256_shuffle_epi8(lookup, hi);
    return _mm256_add_epi8(cnt_lo, cnt_hi);
}

__m256i SIMD<__m256i, uint8_t>::popcount3(
        __m256i a) {
    return _mm256_set_epi8(_mm_popcnt_u32(_mm256_extract_epi8(a, 31)), _mm_popcnt_u32(_mm256_extract_epi8(a, 30)), _mm_popcnt_u32(_mm256_extract_epi8(a, 29)),
            _mm_popcnt_u32(_mm256_extract_epi8(a, 28)), _mm_popcnt_u32(_mm256_extract_epi8(a, 27)), _mm_popcnt_u32(_mm256_extract_epi8(a, 26)), _mm_popcnt_u32(_mm256_extract_epi8(a, 25)),
            _mm_popcnt_u32(_mm256_extract_epi8(a, 24)), _mm_popcnt_u32(_mm256_extract_epi8(a, 23)), _mm_popcnt_u32(_mm256_extract_epi8(a, 22)), _mm_popcnt_u32(_mm256_extract_epi8(a, 21)),
            _mm_popcnt_u32(_mm256_extract_epi8(a, 20)), _mm_popcnt_u32(_mm256_extract_epi8(a, 19)), _mm_popcnt_u32(_mm256_extract_epi8(a, 18)), _mm_popcnt_u32(_mm256_extract_epi8(a, 17)),
            _mm_popcnt_u32(_mm256_extract_epi8(a, 16)), _mm_popcnt_u32(_mm256_extract_epi8(a, 15)), _mm_popcnt_u32(_mm256_extract_epi8(a, 14)), _mm_popcnt_u32(_mm256_extract_epi8(a, 13)),
            _mm_popcnt_u32(_mm256_extract_epi8(a, 12)), _mm_popcnt_u32(_mm256_extract_epi8(a, 11)), _mm_popcnt_u32(_mm256_extract_epi8(a, 10)), _mm_popcnt_u32(_mm256_extract_epi8(a, 9)),
            _mm_popcnt_u32(_mm256_extract_epi8(a, 8)), _mm_popcnt_u32(_mm256_extract_epi8(a, 7)), _mm_popcnt_u32(_mm256_extract_epi8(a, 6)), _mm_popcnt_u32(_mm256_extract_epi8(a, 5)),
            _mm_popcnt_u32(_mm256_extract_epi8(a, 4)), _mm_popcnt_u32(_mm256_extract_epi8(a, 3)), _mm_popcnt_u32(_mm256_extract_epi8(a, 2)), _mm_popcnt_u32(_mm256_extract_epi8(a, 1)),
            _mm_popcnt_u32(_mm256_extract_epi8(a, 0)));
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
    auto pattern1 = _mm256_set1_epi16(0x5555);
    auto pattern2 = _mm256_set1_epi16(0x3333);
    auto pattern3 = _mm256_set1_epi16(0x0F0F);
    auto pattern4 = _mm256_set1_epi16(0x0101);
    auto shuffle = _mm256_set_epi64x(0xFFFFFFFFFFFFFFFF, 0x0F0D0B0907050301, 0xFFFFFFFFFFFFFFFF, 0x0F0D0B0907050301);
    auto temp = _mm256_sub_epi16(data, _mm256_and_si256(_mm256_srli_epi16(data, 1), pattern1));
    temp = _mm256_add_epi16(_mm256_and_si256(temp, pattern2), _mm256_and_si256(_mm256_srli_epi16(temp, 2), pattern2));
    temp = _mm256_and_si256(_mm256_add_epi16(temp, _mm256_srli_epi16(temp, 4)), pattern3);
    temp = _mm256_mullo_epi16(temp, pattern4);
    temp = _mm256_shuffle_epi8(temp, shuffle);
    return _mm_set_epi64x(_mm256_extract_epi64(temp, 2), _mm256_extract_epi64(temp, 0));
}

__m128i SIMD<__m256i, uint16_t>::popcount2(
        __m256i data) {
    auto mask = _mm256_set1_epi16(0x0101);
    auto shuffle = _mm256_set_epi64x(0xFFFFFFFFFFFFFFFF, 0x0F0D0B0907050301, 0xFFFFFFFFFFFFFFFF, 0x0F0D0B0907050301);
    auto popcount8 = SIMD<__m256i, uint8_t>::popcount2(data);
    auto temp = _mm256_shuffle_epi8(_mm256_mullo_epi16(popcount8, mask), shuffle);
    return _mm_set_epi64x(_mm256_extract_epi64(temp, 2), _mm256_extract_epi64(temp, 0));
}

__m128i SIMD<__m256i, uint16_t>::popcount3(
        __m256i a) {
    return _mm_set_epi8(_mm_popcnt_u32(_mm256_extract_epi16(a, 15)), _mm_popcnt_u32(_mm256_extract_epi16(a, 14)), _mm_popcnt_u32(_mm256_extract_epi16(a, 13)),
            _mm_popcnt_u32(_mm256_extract_epi16(a, 12)), _mm_popcnt_u32(_mm256_extract_epi16(a, 11)), _mm_popcnt_u32(_mm256_extract_epi16(a, 10)), _mm_popcnt_u32(_mm256_extract_epi16(a, 9)),
            _mm_popcnt_u32(_mm256_extract_epi16(a, 8)), _mm_popcnt_u32(_mm256_extract_epi16(a, 7)), _mm_popcnt_u32(_mm256_extract_epi16(a, 6)), _mm_popcnt_u32(_mm256_extract_epi16(a, 5)),
            _mm_popcnt_u32(_mm256_extract_epi16(a, 4)), _mm_popcnt_u32(_mm256_extract_epi16(a, 3)), _mm_popcnt_u32(_mm256_extract_epi16(a, 2)), _mm_popcnt_u32(_mm256_extract_epi16(a, 1)),
            _mm_popcnt_u32(_mm256_extract_epi16(a, 0)));
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
    auto pattern1 = _mm256_set1_epi32(0x55555555);
    auto pattern2 = _mm256_set1_epi32(0x33333333);
    auto pattern3 = _mm256_set1_epi32(0x0F0F0F0F);
    auto pattern4 = _mm256_set1_epi32(0x01010101);
    auto shuffle = _mm256_set_epi64x(0xFFFFFFFFFFFFFFFF, 0xFFFFFFFF0F0B0703, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFF0F0B0703);
    auto temp = _mm256_sub_epi32(data, _mm256_and_si256(_mm256_srli_epi32(data, 1), pattern1));
    temp = _mm256_add_epi32(_mm256_and_si256(temp, pattern2), _mm256_and_si256(_mm256_srli_epi32(temp, 2), pattern2));
    temp = _mm256_and_si256(_mm256_add_epi32(temp, _mm256_srli_epi32(temp, 4)), pattern3);
    temp = _mm256_mullo_epi32(temp, pattern4);
    temp = _mm256_shuffle_epi8(temp, shuffle);
    return static_cast<uint64_t>(_mm256_extract_epi32(temp, 0)) | (static_cast<uint64_t>(_mm256_extract_epi32(temp, 4)) << 32);
}

uint64_t SIMD<__m256i, uint32_t>::popcount2(
        __m256i data) {
    auto mask = _mm256_set1_epi32(0x01010101);
    auto shuffle = _mm256_set_epi32(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x0F0B0703, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x0F0B0703);
    auto popcount8 = SIMD<__m256i, uint8_t>::popcount2(data);
    auto temp = _mm256_shuffle_epi8(_mm256_mullo_epi32(popcount8, mask), shuffle);
    return static_cast<uint64_t>(_mm256_extract_epi32(temp, 0)) | (static_cast<uint64_t>(_mm256_extract_epi32(temp, 4)) << 32);
}

uint64_t SIMD<__m256i, uint32_t>::popcount3(
        __m256i a) {
    return (static_cast<uint64_t>(_mm_popcnt_u32(_mm256_extract_epi32(a, 7))) << 56) | (static_cast<uint64_t>(_mm_popcnt_u32(_mm256_extract_epi32(a, 6))) << 48)
            | (static_cast<uint64_t>(_mm_popcnt_u32(_mm256_extract_epi32(a, 5))) << 40) | (static_cast<uint64_t>(_mm_popcnt_u32(_mm256_extract_epi32(a, 4))) << 32)
            | (static_cast<uint64_t>(_mm_popcnt_u32(_mm256_extract_epi32(a, 3))) << 24) | (static_cast<uint64_t>(_mm_popcnt_u32(_mm256_extract_epi32(a, 2))) << 16)
            | (static_cast<uint64_t>(_mm_popcnt_u32(_mm256_extract_epi32(a, 1))) << 8) | static_cast<uint64_t>(_mm_popcnt_u32(_mm256_extract_epi32(a, 0)));
}

__m256i SIMD<__m256i, uint64_t>::set1(
        uint64_t a) {
    return _mm256_set1_epi64x(a);
}

__m256i SIMD<__m256i, uint64_t>::add(
        __m256i a,
        __m256i b) {
    return _mm256_add_epi64(a, b);
}

uint32_t SIMD<__m256i, uint64_t>::popcount(
        __m256i data) {
    auto pattern1 = _mm256_set1_epi64x(0x5555555555555555);
    auto pattern2 = _mm256_set1_epi64x(0x3333333333333333);
    auto pattern3 = _mm256_set1_epi64x(0x0F0F0F0F0F0F0F0F);
    auto pattern4 = _mm256_set1_epi64x(0x0101010101010101);
    auto shuffle = _mm256_set_epi64x(0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFF0F07, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFF0F07);
    auto temp = _mm256_sub_epi32(data, _mm256_and_si256(_mm256_srli_epi32(data, 1), pattern1));
    temp = _mm256_add_epi32(_mm256_and_si256(temp, pattern2), _mm256_and_si256(_mm256_srli_epi32(temp, 2), pattern2));
    temp = _mm256_and_si256(_mm256_add_epi32(temp, _mm256_srli_epi32(temp, 4)), pattern3);
    temp = _mm256_mullo_epi32(temp, pattern4);
    temp = _mm256_shuffle_epi8(temp, shuffle);
    return static_cast<uint32_t>(_mm256_extract_epi16(temp, 0)) | (static_cast<uint32_t>(_mm256_extract_epi16(temp, 8)) << 16);
}

uint32_t SIMD<__m256i, uint64_t>::popcount2(
        __m256i data) {
    uint64_t mask = 0x0101010101010101;
    auto shuffle = _mm256_set_epi64x(0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFF0F07, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFF0F07);
    auto popcount8 = SIMD<__m256i, uint8_t>::popcount2(data);
    auto temp = _mm256_shuffle_epi8(
            _mm256_set_epi64x(static_cast<uint64_t>(_mm256_extract_epi64(popcount8, 3)) * mask, static_cast<uint64_t>(_mm256_extract_epi64(popcount8, 2)) * mask,
                    static_cast<uint64_t>(_mm256_extract_epi64(popcount8, 1)) * mask, static_cast<uint64_t>(_mm256_extract_epi64(popcount8, 0)) * mask), shuffle);
    return static_cast<uint32_t>(_mm256_extract_epi16(temp, 0)) | (static_cast<uint32_t>(_mm256_extract_epi16(temp, 8)) << 16);
}

uint32_t SIMD<__m256i, uint64_t>::popcount3(
        __m256i a) {
    return (_mm_popcnt_u64(_mm256_extract_epi64(a, 3)) << 24) | (_mm_popcnt_u64(_mm256_extract_epi64(a, 2)) << 16) | (_mm_popcnt_u64(_mm256_extract_epi64(a, 1)) << 8)
            | _mm_popcnt_u64(_mm256_extract_epi64(a, 0));
}
#endif
