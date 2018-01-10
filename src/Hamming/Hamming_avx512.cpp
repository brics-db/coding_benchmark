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
 * File:   Hamming_avx512.cpp
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 15-08-2017 16:16
 */

#ifdef __AVX512F__

#include <Hamming/Hamming_simd.hpp>
#include <Util/Functors.hpp>

/*
 * For the following algorithms, see
 * https://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel
 * Credit also goes to the book "Hacker's Delight" 2nd Edition, by Henry S. Warren, Jr., published at Addison-Wesley
 */

namespace coding_benchmark {

#ifdef __AVX512BW__
    template struct hamming_t<uint16_t, __m512i >;
#endif
    template struct hamming_t<uint32_t, __m512i >;

#ifdef __AVX512BW__
    template<>
    __m128i hamming_t<uint16_t, __m512i >::computeHamming(
            __m512i data) {
        typedef typename mm<__m512i, uint16_t>::popcnt_t popcnt_t;
        auto pattern1 = _mm512_set1_epi16(static_cast<int16_t>(0xAD5B));
        auto pattern2 = _mm512_set1_epi16(static_cast<int16_t>(0x366D));
        auto pattern3 = _mm512_set1_epi16(static_cast<int16_t>(0xC78E));
        auto pattern4 = _mm512_set1_epi16(static_cast<int16_t>(0x07F0));
        auto pattern5 = _mm512_set1_epi16(static_cast<int16_t>(0xF800));
        static auto pattern_and = _mm256_set1_epi8(0x01);
        __m256i hamming = _mm256_setzero_si256();
        __m256i tmp1 = _mm256_setzero_si256();
        __m256i tmp2 = _mm256_setzero_si256();
        tmp2 = _mm256_and_si256(mm<__m512i, uint16_t>::popcount(_mm512_and_si512(data, pattern1)), pattern_and);
        hamming = _mm256_or_si256(hamming, _mm256_slli_epi16(tmp2, 1));
        tmp1 = _mm256_and_si256(mm<__m512i, uint16_t>::popcount(_mm512_and_si512(data, pattern2)), pattern_and);
        tmp2 = _mm256_xor_si256(tmp2, tmp1);
        hamming = _mm256_or_si256(hamming, _mm256_slli_epi16(tmp2, 2));
        tmp1 = _mm256_and_si256(mm<__m512i, uint16_t>::popcount(_mm512_and_si512(data, pattern3)), pattern_and);
        tmp2 = _mm256_xor_si256(tmp2, tmp1);
        hamming = _mm256_or_si256(hamming, mm256_slli_epi16(tmp2, 3));
        tmp1 = _mm256_and_si256(mm<__m512i, uint16_t>::popcount(_mm512_and_si512(data, pattern4)), pattern_and);
        tmp2 = _mm256_xor_si256(tmp2, tmp1);
        hamming = _mm256_or_si256(hamming, mm256_slli_epi16(tmp2, 4));
        tmp1 = _mm256_and_si256(mm<__m512i, uint16_t>::popcount(_mm512_and_si512(data, pattern5)), pattern_and);
        tmp2 = _mm256_xor_si256(tmp2, tmp1);
        hamming = _mm256_or_si256(hamming, mm256_slli_epi16(tmp2, 5));
        tmp1 = _mm256_and_si256(mm<__m512i, uint16_t>::popcount(_mm512_and_si512(data, pattern6)), pattern_and);
        tmp2 = _mm256_xor_si256(tmp2, tmp1);
        hamming = _mm256_or_si256(hamming, mm256_slli_epi16(tmp2, 6));
        hamming = _mm256_or_si256(hamming, _mm256_and_si256(mm256_add_epi8(mm<__m512i, uint16_t>::popcount(data), tmp2), pattern_and));
        return hamming;
    }

    template<>
    __m128i hamming_t<uint16_t, __m512i >::computeHamming2(
            __m512i data) {
        typedef typename mm<__m512i, uint16_t>::popcnt_t popcnt_t;
        auto pattern1 = _mm512_set1_epi16(static_cast<int16_t>(0xAD5B));
        auto pattern2 = _mm512_set1_epi16(static_cast<int16_t>(0x366D));
        auto pattern3 = _mm512_set1_epi16(static_cast<int16_t>(0xC78E));
        auto pattern4 = _mm512_set1_epi16(static_cast<int16_t>(0x07F0));
        auto pattern5 = _mm512_set1_epi16(static_cast<int16_t>(0xF800));
        static auto pattern_and = _mm256_set1_epi8(0x01);
        __m256i hamming = _mm256_setzero_si256();
        __m256i tmp1 = _mm256_setzero_si256();
        __m256i tmp2 = _mm256_setzero_si256();
        tmp2 = _mm256_and_si256(mm<__m512i, uint16_t>::popcount2(_mm512_and_si512(data, pattern1)), pattern_and);
        hamming = _mm256_or_si256(hamming, _mm256_slli_epi16(tmp2, 1));
        tmp1 = _mm256_and_si256(mm<__m512i, uint16_t>::popcount2(_mm512_and_si512(data, pattern2)), pattern_and);
        tmp2 = _mm256_xor_si256(tmp2, tmp1);
        hamming = _mm256_or_si256(hamming, _mm256_slli_epi16(tmp2, 2));
        tmp1 = _mm256_and_si256(mm<__m512i, uint16_t>::popcount2(_mm512_and_si512(data, pattern3)), pattern_and);
        tmp2 = _mm256_xor_si256(tmp2, tmp1);
        hamming = _mm256_or_si256(hamming, mm256_slli_epi16(tmp2, 3));
        tmp1 = _mm256_and_si256(mm<__m512i, uint16_t>::popcount2(_mm512_and_si512(data, pattern4)), pattern_and);
        tmp2 = _mm256_xor_si256(tmp2, tmp1);
        hamming = _mm256_or_si256(hamming, mm256_slli_epi16(tmp2, 4));
        tmp1 = _mm256_and_si256(mm<__m512i, uint16_t>::popcount2(_mm512_and_si512(data, pattern5)), pattern_and);
        tmp2 = _mm256_xor_si256(tmp2, tmp1);
        hamming = _mm256_or_si256(hamming, mm256_slli_epi16(tmp2, 5));
        tmp1 = _mm256_and_si256(mm<__m512i, uint16_t>::popcount2(_mm512_and_si512(data, pattern6)), pattern_and);
        tmp2 = _mm256_xor_si256(tmp2, tmp1);
        hamming = _mm256_or_si256(hamming, mm256_slli_epi16(tmp2, 6));
        hamming = _mm256_or_si256(hamming, _mm256_and_si256(mm256_add_epi8(mm<__m512i, uint16_t>::popcount2(data), tmp2), pattern_and));
        return hamming;
    }

    template<>
    __m128i hamming_t<uint16_t, __m512i >::computeHamming3(
            __m512i data) {
        typedef typename mm<__m512i, uint16_t>::popcnt_t popcnt_t;
        auto pattern1 = _mm512_set1_epi16(static_cast<int16_t>(0xAD5B));
        auto pattern2 = _mm512_set1_epi16(static_cast<int16_t>(0x366D));
        auto pattern3 = _mm512_set1_epi16(static_cast<int16_t>(0xC78E));
        auto pattern4 = _mm512_set1_epi16(static_cast<int16_t>(0x07F0));
        auto pattern5 = _mm512_set1_epi16(static_cast<int16_t>(0xF800));
        static auto pattern_and = _mm256_set1_epi8(0x01);
        __m256i hamming = _mm256_setzero_si256();
        __m256i tmp1 = _mm256_setzero_si256();
        __m256i tmp2 = _mm256_setzero_si256();
        tmp2 = _mm256_and_si256(mm<__m512i, uint16_t>::popcount3(_mm512_and_si512(data, pattern1)), pattern_and);
        hamming = _mm256_or_si256(hamming, _mm256_slli_epi16(tmp2, 1));
        tmp1 = _mm256_and_si256(mm<__m512i, uint16_t>::popcount3(_mm512_and_si512(data, pattern2)), pattern_and);
        tmp2 = _mm256_xor_si256(tmp2, tmp1);
        hamming = _mm256_or_si256(hamming, _mm256_slli_epi16(tmp2, 2));
        tmp1 = _mm256_and_si256(mm<__m512i, uint16_t>::popcount3(_mm512_and_si512(data, pattern3)), pattern_and);
        tmp2 = _mm256_xor_si256(tmp2, tmp1);
        hamming = _mm256_or_si256(hamming, mm256_slli_epi16(tmp2, 3));
        tmp1 = _mm256_and_si256(mm<__m512i, uint16_t>::popcount3(_mm512_and_si512(data, pattern4)), pattern_and);
        tmp2 = _mm256_xor_si256(tmp2, tmp1);
        hamming = _mm256_or_si256(hamming, mm256_slli_epi16(tmp2, 4));
        tmp1 = _mm256_and_si256(mm<__m512i, uint16_t>::popcount3(_mm512_and_si512(data, pattern5)), pattern_and);
        tmp2 = _mm256_xor_si256(tmp2, tmp1);
        hamming = _mm256_or_si256(hamming, mm256_slli_epi16(tmp2, 5));
        tmp1 = _mm256_and_si256(mm<__m512i, uint16_t>::popcount3(_mm512_and_si512(data, pattern6)), pattern_and);
        tmp2 = _mm256_xor_si256(tmp2, tmp1);
        hamming = _mm256_or_si256(hamming, mm256_slli_epi16(tmp2, 6));
        hamming = _mm256_or_si256(hamming, _mm256_and_si256(mm256_add_epi8(mm<__m512i, uint16_t>::popcount3(data), tmp2), pattern_and));
        return hamming;
    }

    template<>
    bool hamming_t<uint16_t, __m512i >::code_cmp_eq(
            __m128i hamming1,
            __m128i hamming2) {
        return _mm_movemask_epi8(_mm_cmpeq_epi8(hamming1, hamming2)) == 0xFFFF; // all equal?
    }

    template<>
    bool hamming_t<uint16_t, __m512i >::isValid() {
        return hamming_t<uint16_t, __m512i >::code_cmp_eq(this->code, hamming_t<uint16_t, __m512i >::computeHamming(this->data));
    }

    template<>
    void hamming_t<uint16_t, __m512i >::store(
            __m512i data) {
        _mm256_storeu_si256(&this->data, data);
        _mm_storeu_si128(&this->code, computeHamming(data));
    }

    template<>
    void hamming_t<uint16_t, __m512i >::store2(
            __m512i data) {
        _mm256_storeu_si256(&this->data, data);
        _mm_storeu_si128(&this->code, computeHamming2(data));
    }

    template<>
    void hamming_t<uint16_t, __m512i >::store3(
            __m512i data) {
        _mm256_storeu_si256(&this->data, data);
        _mm_storeu_si128(&this->code, computeHamming3(data));
    }
#endif

    template<>
    __m128i hamming_t<uint32_t, __m512i >::computeHamming(
            __m512i data) {
        static auto pattern1 = _mm512_set1_epi32(0x56AAAD5B);
        static auto pattern2 = _mm512_set1_epi32(0x9B33366D);
        static auto pattern3 = _mm512_set1_epi32(0xE3C3C78E);
        static auto pattern4 = _mm512_set1_epi32(0x03FC07F0);
        static auto pattern5 = _mm512_set1_epi32(0x03FFF800);
        static auto pattern6 = _mm512_set1_epi32(0xFC000000);
        static auto pattern_and = _mm_set1_epi8(0x01);
        __m128i hamming = _mm_setzero_si128();
        __m128i tmp1 = _mm_setzero_si128();
        __m128i tmp2 = _mm_setzero_si128();
        tmp2 = _mm_and_si128(mm<__m512i, uint32_t>::popcount(_mm512_and_si512(data, pattern1)), pattern_and);
        hamming = _mm_or_si128(hamming, _mm_slli_epi16(tmp2, 1));
        tmp1 = _mm_and_si128(mm<__m512i, uint32_t>::popcount(_mm512_and_si512(data, pattern2)), pattern_and);
        tmp2 = _mm_xor_si128(tmp2, tmp1);
        hamming = _mm_or_si128(hamming, _mm_slli_epi16(tmp2, 2));
        tmp1 = _mm_and_si128(mm<__m512i, uint32_t>::popcount(_mm512_and_si512(data, pattern3)), pattern_and);
        tmp2 = _mm_xor_si128(tmp2, tmp1);
        hamming = _mm_or_si128(hamming, _mm_slli_epi16(tmp2, 3));
        tmp1 = _mm_and_si128(mm<__m512i, uint32_t>::popcount(_mm512_and_si512(data, pattern4)), pattern_and);
        tmp2 = _mm_xor_si128(tmp2, tmp1);
        hamming = _mm_or_si128(hamming, _mm_slli_epi16(tmp2, 4));
        tmp1 = _mm_and_si128(mm<__m512i, uint32_t>::popcount(_mm512_and_si512(data, pattern5)), pattern_and);
        tmp2 = _mm_xor_si128(tmp2, tmp1);
        hamming = _mm_or_si128(hamming, _mm_slli_epi16(tmp2, 5));
        tmp1 = _mm_and_si128(mm<__m512i, uint32_t>::popcount(_mm512_and_si512(data, pattern6)), pattern_and);
        tmp2 = _mm_xor_si128(tmp2, tmp1);
        hamming = _mm_or_si128(hamming, _mm_slli_epi16(tmp2, 6));
        hamming = _mm_or_si128(hamming, _mm_and_si128(_mm_add_epi8(mm<__m512i, uint32_t>::popcount(data), tmp2), pattern_and));
        return hamming;
    }

    template<>
    __m128i hamming_t<uint32_t, __m512i >::computeHamming2(
            __m512i data) {
        static auto pattern1 = _mm512_set1_epi32(0x56AAAD5B);
        static auto pattern2 = _mm512_set1_epi32(0x9B33366D);
        static auto pattern3 = _mm512_set1_epi32(0xE3C3C78E);
        static auto pattern4 = _mm512_set1_epi32(0x03FC07F0);
        static auto pattern5 = _mm512_set1_epi32(0x03FFF800);
        static auto pattern6 = _mm512_set1_epi32(0xFC000000);
        static auto pattern_and = _mm_set1_epi8(0x01);
        __m128i hamming = _mm_setzero_si128();
        __m128i tmp1 = _mm_setzero_si128();
        __m128i tmp2 = _mm_setzero_si128();
        tmp2 = _mm_and_si128(mm<__m512i, uint32_t>::popcount2(_mm512_and_si512(data, pattern1)), pattern_and);
        hamming = _mm_or_si128(hamming, _mm_slli_epi16(tmp2, 1));
        tmp1 = _mm_and_si128(mm<__m512i, uint32_t>::popcount2(_mm512_and_si512(data, pattern2)), pattern_and);
        tmp2 = _mm_xor_si128(tmp2, tmp1);
        hamming = _mm_or_si128(hamming, _mm_slli_epi16(tmp2, 2));
        tmp1 = _mm_and_si128(mm<__m512i, uint32_t>::popcount2(_mm512_and_si512(data, pattern3)), pattern_and);
        tmp2 = _mm_xor_si128(tmp2, tmp1);
        hamming = _mm_or_si128(hamming, _mm_slli_epi16(tmp2, 3));
        tmp1 = _mm_and_si128(mm<__m512i, uint32_t>::popcount2(_mm512_and_si512(data, pattern4)), pattern_and);
        tmp2 = _mm_xor_si128(tmp2, tmp1);
        hamming = _mm_or_si128(hamming, _mm_slli_epi16(tmp2, 4));
        tmp1 = _mm_and_si128(mm<__m512i, uint32_t>::popcount2(_mm512_and_si512(data, pattern5)), pattern_and);
        tmp2 = _mm_xor_si128(tmp2, tmp1);
        hamming = _mm_or_si128(hamming, _mm_slli_epi16(tmp2, 5));
        tmp1 = _mm_and_si128(mm<__m512i, uint32_t>::popcount2(_mm512_and_si512(data, pattern6)), pattern_and);
        tmp2 = _mm_xor_si128(tmp2, tmp1);
        hamming = _mm_or_si128(hamming, _mm_slli_epi16(tmp2, 6));
        hamming = _mm_or_si128(hamming, _mm_and_si128(_mm_add_epi8(mm<__m512i, uint32_t>::popcount2(data), tmp2), pattern_and));
        return hamming;
    }

    template<>
    __m128i hamming_t<uint32_t, __m512i >::computeHamming3(
            __m512i data) {
        static auto pattern1 = _mm512_set1_epi32(0x56AAAD5B);
        static auto pattern2 = _mm512_set1_epi32(0x9B33366D);
        static auto pattern3 = _mm512_set1_epi32(0xE3C3C78E);
        static auto pattern4 = _mm512_set1_epi32(0x03FC07F0);
        static auto pattern5 = _mm512_set1_epi32(0x03FFF800);
        static auto pattern6 = _mm512_set1_epi32(0xFC000000);
        static auto pattern_and = _mm_set1_epi8(0x01);
        __m128i hamming = _mm_setzero_si128();
        __m128i tmp1 = _mm_setzero_si128();
        __m128i tmp2 = _mm_setzero_si128();
        tmp2 = _mm_and_si128(mm<__m512i, uint32_t>::popcount3(_mm512_and_si512(data, pattern1)), pattern_and);
        hamming = _mm_or_si128(hamming, _mm_slli_epi16(tmp2, 1));
        tmp1 = _mm_and_si128(mm<__m512i, uint32_t>::popcount3(_mm512_and_si512(data, pattern2)), pattern_and);
        tmp2 = _mm_xor_si128(tmp2, tmp1);
        hamming = _mm_or_si128(hamming, _mm_slli_epi16(tmp2, 2));
        tmp1 = _mm_and_si128(mm<__m512i, uint32_t>::popcount3(_mm512_and_si512(data, pattern3)), pattern_and);
        tmp2 = _mm_xor_si128(tmp2, tmp1);
        hamming = _mm_or_si128(hamming, _mm_slli_epi16(tmp2, 3));
        tmp1 = _mm_and_si128(mm<__m512i, uint32_t>::popcount3(_mm512_and_si512(data, pattern4)), pattern_and);
        tmp2 = _mm_xor_si128(tmp2, tmp1);
        hamming = _mm_or_si128(hamming, _mm_slli_epi16(tmp2, 4));
        tmp1 = _mm_and_si128(mm<__m512i, uint32_t>::popcount3(_mm512_and_si512(data, pattern5)), pattern_and);
        tmp2 = _mm_xor_si128(tmp2, tmp1);
        hamming = _mm_or_si128(hamming, _mm_slli_epi16(tmp2, 5));
        tmp1 = _mm_and_si128(mm<__m512i, uint32_t>::popcount3(_mm512_and_si512(data, pattern6)), pattern_and);
        tmp2 = _mm_xor_si128(tmp2, tmp1);
        hamming = _mm_or_si128(hamming, _mm_slli_epi16(tmp2, 6));
        hamming = _mm_or_si128(hamming, _mm_and_si128(_mm_add_epi8(mm<__m512i, uint32_t>::popcount3(data), tmp2), pattern_and));
        return hamming;
    }

    template<>
    bool hamming_t<uint32_t, __m512i >::code_cmp_eq(
            __m128i hamming1,
            __m128i hamming2) {
#ifdef __AVX512VL__
        return 0x3 == _mm_cmpeq_epi64_mask(hamming1, hamming2); // all equal?
#else
        return mm<__m128i, uint8_t>::FULL_MASK == mm_op<__m128i, uint8_t, std::equal_to>::cmp_mask(hamming1, hamming2);
#endif
    }

    template<>
    bool hamming_t<uint32_t, __m512i >::isValid() {
        return hamming_t<uint32_t, __m512i >::code_cmp_eq(this->code, hamming_t<uint32_t, __m512i >::computeHamming(this->data));
    }

    template<>
    void hamming_t<uint32_t, __m512i >::store(
            __m512i data) {
        _mm512_storeu_si512(&this->data, data);
        this->code = computeHamming(data);
    }

    template<>
    void hamming_t<uint32_t, __m512i >::store2(
            __m512i data) {
        _mm512_storeu_si512(&this->data, data);
        this->code = computeHamming2(data);
    }

    template<>
    void hamming_t<uint32_t, __m512i >::store3(
            __m512i data) {
        _mm512_storeu_si512(&this->data, data);
        this->code = computeHamming3(data);
    }

}

#endif
