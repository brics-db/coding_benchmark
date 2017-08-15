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
 * File:   Hamming_avx2.cpp
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 15-08-2017 16:16
 */

#ifdef __AVX2__

#include <Hamming/Hamming_avx2.hpp>

/*
 * For the following algorithms, see
 * https://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel
 * Credit also goes to the book "Hacker's Delight" 2nd Edition, by Henry S. Warren, Jr., published at Addison-Wesley
 */

__m256i _mm256_popcount_epi8(
        __m256i data) {
    static auto pattern1 = _mm256_set1_epi8(0x55);
    static auto pattern2 = _mm256_set1_epi8(0x33);
    static auto pattern3 = _mm256_set1_epi8(0x0F);
    auto temp = _mm256_sub_epi8(data, _mm256_and_si256(_mm256_srli_epi16(data, 1), pattern1));
    temp = _mm256_add_epi8(_mm256_and_si256(temp, pattern2), _mm256_and_si256(_mm256_srli_epi16(temp, 2), pattern2));
    temp = _mm256_add_epi8(_mm256_and_si256(temp, pattern3), _mm256_and_si256(_mm256_srli_epi16(temp, 4), pattern3));
    return temp;
}

__m256i _mm256_popcount_epi8_2(
        __m256i data) {
    static auto lookup = _mm256_set_epi8(4, 3, 3, 2, 3, 2, 2, 1, 3, 2, 2, 1, 2, 1, 1, 0, 4, 3, 3, 2, 3, 2, 2, 1, 3, 2, 2, 1, 2, 1, 1, 0);
    auto low_mask = _mm256_set1_epi8(0x0f);
    auto lo = _mm256_and_si256(data, low_mask);
    auto hi = _mm256_and_si256(_mm256_srli_epi16(data, 4), low_mask);
    auto cnt_lo = _mm256_shuffle_epi8(lookup, lo);
    auto cnt_hi = _mm256_shuffle_epi8(lookup, hi);
    return _mm256_add_epi8(cnt_lo, cnt_hi);
}

__m128i _mm256_popcount_epi16(
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

__m128i _mm256_popcount_epi16_2(
        __m256i data) {
    static auto mask = _mm256_set1_epi16(0x0101);
    static auto shuffle = _mm256_set_epi64x(0xFFFFFFFFFFFFFFFF, 0x0F0D0B0907050301, 0xFFFFFFFFFFFFFFFF, 0x0F0D0B0907050301);
    auto popcount8 = _mm256_popcount_epi8_2(std::forward<__m256i >(data));
    auto temp = _mm256_shuffle_epi8(_mm256_mullo_epi16(popcount8, mask), shuffle);
    return _mm_set_epi64x(_mm256_extract_epi64(temp, 2), _mm256_extract_epi64(temp, 0));
}

uint64_t _mm256_popcount_epi32(
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

uint64_t _mm256_popcount_epi32_2(
        __m256i data) {
    static auto mask = _mm256_set1_epi32(0x01010101);
    static auto shuffle = _mm256_set_epi32(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x0F0B0703, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x0F0B0703);
    auto popcount8 = _mm256_popcount_epi8_2(std::forward<__m256i >(data));
    auto temp = _mm256_shuffle_epi8(_mm256_mullo_epi32(popcount8, mask), shuffle);
    return static_cast<uint64_t>(_mm256_extract_epi32(temp, 0)) | (static_cast<uint64_t>(_mm256_extract_epi32(temp, 4)) << 32);
}

__m128i HammingAVX2<uint16_t>::computeHamming(
        __m256i data) {
    static auto pattern1 = _mm256_set1_epi16(static_cast<int16_t>(0xAD5B));
    static auto pattern2 = _mm256_set1_epi16(static_cast<int16_t>(0x366D));
    static auto pattern3 = _mm256_set1_epi16(static_cast<int16_t>(0xC78E));
    static auto pattern4 = _mm256_set1_epi16(static_cast<int16_t>(0x07F0));
    static auto pattern5 = _mm256_set1_epi16(static_cast<int16_t>(0xF800));
    static auto mask = _mm_set1_epi8(0x01);
    __m128i tmp2 = _mm256_popcount_epi16(_mm256_and_si256(data, pattern1));
    __m128i hamming = _mm_slli_epi16(_mm_and_si128(tmp2, mask), 1);
    __m128i tmp1 = _mm256_popcount_epi16(_mm256_and_si256(data, pattern2));
    tmp1 = _mm_and_si128(tmp1, mask);
    tmp2 = _mm_xor_si128(tmp2, tmp1);
    hamming = _mm_or_si128(hamming, _mm_slli_epi16(tmp1, 2));
    tmp1 = _mm256_popcount_epi16(_mm256_and_si256(data, pattern3));
    tmp1 = _mm_and_si128(tmp1, mask);
    tmp2 = _mm_xor_si128(tmp2, tmp1);
    hamming = _mm_or_si128(hamming, _mm_slli_epi16(tmp1, 3));
    tmp1 = _mm256_popcount_epi16(_mm256_and_si256(data, pattern4));
    tmp1 = _mm_and_si128(tmp1, mask);
    tmp2 = _mm_xor_si128(tmp2, tmp1);
    hamming = _mm_or_si128(hamming, _mm_slli_epi16(tmp1, 4));
    tmp1 = _mm256_popcount_epi16(_mm256_and_si256(data, pattern5));
    tmp1 = _mm_and_si128(tmp1, mask);
    tmp2 = _mm_xor_si128(tmp2, tmp1);
    hamming = _mm_or_si128(hamming, _mm_slli_epi16(tmp1, 5));
    tmp1 = _mm_add_epi8(_mm256_popcount_epi16(std::forward<__m256i >(data)), tmp2);
    tmp1 = _mm_and_si128(tmp1, mask);
    hamming = _mm_or_si128(hamming, tmp1);
    return hamming;
}

__m128i HammingAVX2<uint16_t>::computeHamming2(
        __m256i data) {
    static auto pattern1 = _mm256_set1_epi16(static_cast<int16_t>(0xAD5B));
    static auto pattern2 = _mm256_set1_epi16(static_cast<int16_t>(0x366D));
    static auto pattern3 = _mm256_set1_epi16(static_cast<int16_t>(0xC78E));
    static auto pattern4 = _mm256_set1_epi16(static_cast<int16_t>(0x07F0));
    static auto pattern5 = _mm256_set1_epi16(static_cast<int16_t>(0xF800));
    static auto mask = _mm_set1_epi8(0x01);
    __m128i tmp2 = _mm256_popcount_epi16(_mm256_and_si256(data, pattern1));
    __m128i hamming = _mm_slli_epi16(_mm_and_si128(tmp2, mask), 1);
    __m128i tmp1 = _mm256_popcount_epi16(_mm256_and_si256(data, pattern2));
    tmp1 = _mm_and_si128(tmp1, mask);
    tmp2 = _mm_xor_si128(tmp2, tmp1);
    hamming = _mm_or_si128(hamming, _mm_slli_epi16(tmp1, 2));
    tmp1 = _mm256_popcount_epi16(_mm256_and_si256(data, pattern3));
    tmp1 = _mm_and_si128(tmp1, mask);
    tmp2 = _mm_xor_si128(tmp2, tmp1);
    hamming = _mm_or_si128(hamming, _mm_slli_epi16(tmp1, 3));
    tmp1 = _mm256_popcount_epi16(_mm256_and_si256(data, pattern4));
    tmp1 = _mm_and_si128(tmp1, mask);
    tmp2 = _mm_xor_si128(tmp2, tmp1);
    hamming = _mm_or_si128(hamming, _mm_slli_epi16(tmp1, 4));
    tmp1 = _mm256_popcount_epi16(_mm256_and_si256(data, pattern5));
    tmp1 = _mm_and_si128(tmp1, mask);
    tmp2 = _mm_xor_si128(tmp2, tmp1);
    hamming = _mm_or_si128(hamming, _mm_slli_epi16(tmp1, 5));
    tmp1 = _mm_add_epi8(_mm256_popcount_epi16(std::forward<__m256i >(data)), tmp2);
    tmp1 = _mm_and_si128(tmp1, mask);
    hamming = _mm_or_si128(hamming, tmp1);
    return hamming;
}

bool HammingAVX2<uint16_t>::isHammingEqual(
        __m128i hamming1,
        __m128i hamming2) {
    return _mm_movemask_epi8(_mm_cmpeq_epi8(hamming1, hamming2)) == 0xFFFF; // all equal?
}

uint64_t HammingAVX2<uint32_t>::computeHamming(
        __m256i data) {
    static auto pattern1 = _mm256_set1_epi32(0x56AAAD5B);
    static auto pattern2 = _mm256_set1_epi32(0x9B33366D);
    static auto pattern3 = _mm256_set1_epi32(0xE3C3C78E);
    static auto pattern4 = _mm256_set1_epi32(0x03FC07F0);
    static auto pattern5 = _mm256_set1_epi32(0x03FFF800);
    static auto pattern6 = _mm256_set1_epi32(0xFC000000);
    uint64_t hamming = 0;
    uint64_t tmp1(0), tmp2(0);
    tmp2 = _mm256_popcount_epi32(_mm256_and_si256(data, pattern1)) & 0x0101010101010101;
    hamming |= tmp2 << 1;
    tmp1 = _mm256_popcount_epi32(_mm256_and_si256(data, pattern2)) & 0x0101010101010101;
    tmp2 ^= tmp1;
    hamming |= tmp1 << 2;
    tmp1 = _mm256_popcount_epi32(_mm256_and_si256(data, pattern3)) & 0x0101010101010101;
    tmp2 ^= tmp1;
    hamming |= tmp1 << 3;
    tmp1 = _mm256_popcount_epi32(_mm256_and_si256(data, pattern4)) & 0x0101010101010101;
    tmp2 ^= tmp1;
    hamming |= tmp1 << 4;
    tmp1 = _mm256_popcount_epi32(_mm256_and_si256(data, pattern5)) & 0x0101010101010101;
    tmp2 ^= tmp1;
    hamming |= tmp1 << 5;
    tmp1 = _mm256_popcount_epi32(_mm256_and_si256(data, pattern6)) & 0x0101010101010101;
    tmp2 ^= tmp1;
    hamming |= tmp1 << 6;
    hamming |= (_mm256_popcount_epi32(std::forward<__m256i >(data)) + tmp2) & 0x0101010101010101;
    return hamming;
}

uint64_t HammingAVX2<uint32_t>::computeHamming2(
        __m256i data) {
    static auto pattern1 = _mm256_set1_epi32(0x56AAAD5B);
    static auto pattern2 = _mm256_set1_epi32(0x9B33366D);
    static auto pattern3 = _mm256_set1_epi32(0xE3C3C78E);
    static auto pattern4 = _mm256_set1_epi32(0x03FC07F0);
    static auto pattern5 = _mm256_set1_epi32(0x03FFF800);
    static auto pattern6 = _mm256_set1_epi32(0xFC000000);
    uint64_t hamming = 0;
    uint64_t tmp1(0), tmp2(0);
    tmp2 = _mm256_popcount_epi32_2(_mm256_and_si256(data, pattern1)) & 0x0101010101010101;
    hamming |= tmp2 << 1;
    tmp1 = _mm256_popcount_epi32_2(_mm256_and_si256(data, pattern2)) & 0x0101010101010101;
    tmp2 ^= tmp1;
    hamming |= tmp1 << 2;
    tmp1 = _mm256_popcount_epi32_2(_mm256_and_si256(data, pattern3)) & 0x0101010101010101;
    tmp2 ^= tmp1;
    hamming |= tmp1 << 3;
    tmp1 = _mm256_popcount_epi32_2(_mm256_and_si256(data, pattern4)) & 0x0101010101010101;
    tmp2 ^= tmp1;
    hamming |= tmp1 << 4;
    tmp1 = _mm256_popcount_epi32_2(_mm256_and_si256(data, pattern5)) & 0x0101010101010101;
    tmp2 ^= tmp1;
    hamming |= tmp1 << 5;
    tmp1 = _mm256_popcount_epi32_2(_mm256_and_si256(data, pattern6)) & 0x0101010101010101;
    tmp2 ^= tmp1;
    hamming |= tmp1 << 6;
    hamming |= (_mm256_popcount_epi32_2(std::forward<__m256i >(data)) + tmp2) & 0x0101010101010101;
    return hamming;
}

bool HammingAVX2<uint32_t>::isHammingEqual(
        uint64_t hamming1,
        uint64_t hamming2) {
    return hamming1 == hamming2; // all equal?
}

template
struct Hamming_avx2_16<1>;
template
struct Hamming_avx2_16<2>;
template
struct Hamming_avx2_16<4>;
template
struct Hamming_avx2_16<8>;
template
struct Hamming_avx2_16<16>;
template
struct Hamming_avx2_16<32>;
template
struct Hamming_avx2_16<64>;
template
struct Hamming_avx2_16<128>;
template
struct Hamming_avx2_16<256>;
template
struct Hamming_avx2_16<512>;
template
struct Hamming_avx2_16<1024>;

template
struct Hamming_avx2_32<1>;
template
struct Hamming_avx2_32<2>;
template
struct Hamming_avx2_32<4>;
template
struct Hamming_avx2_32<8>;
template
struct Hamming_avx2_32<16>;
template
struct Hamming_avx2_32<32>;
template
struct Hamming_avx2_32<64>;
template
struct Hamming_avx2_32<128>;
template
struct Hamming_avx2_32<256>;
template
struct Hamming_avx2_32<512>;
template
struct Hamming_avx2_32<1024>;

#endif
