// Copyright 2017 Till Kolditz
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
 * AVX2_uint08.tcc
 *
 *  Created on: 27.09.2017
 *      Author: Till Kolditz - Till.Kolditz@gmail.com
 */

#pragma once

#ifndef LIB_COLUMN_OPERATORS_SIMD_AVX2_HPP_
#error "This file must only be included by AVX2.hpp !"
#endif

namespace coding_benchmark {
    namespace simd {
        namespace avx2 {

            namespace Private {
                template<size_t current = 0>
                inline void pack_right2_uint8(
                        uint8_t * & result,
                        __m256i & a,
                        uint32_t mask) {
                    *result = reinterpret_cast<uint8_t*>(&a)[current];
                    result += (mask >> current) & 0x1;
                    pack_right2_uint8<current + 1>(result, a, mask);
                }

                template<>
                inline void pack_right2_uint8<31>(
                        uint8_t * & result,
                        __m256i & a,
                        uint32_t mask) {
                    *result = reinterpret_cast<uint8_t*>(&a)[31];
                    result += (mask >> 31) & 0x1;
                }
            }

            template<>
            struct mm256<uint8_t> {

                typedef uint32_t mask_t;
                typedef __m256i popcnt_t;

                static inline __m256i set1(
                        uint8_t value) {
                    return _mm256_set1_epi8(value);
                }

                static inline __m256i set(
                        uint8_t v31,
                        uint8_t v30,
                        uint8_t v29,
                        uint8_t v28,
                        uint8_t v27,
                        uint8_t v26,
                        uint8_t v25,
                        uint8_t v24,
                        uint8_t v23,
                        uint8_t v22,
                        uint8_t v21,
                        uint8_t v20,
                        uint8_t v19,
                        uint8_t v18,
                        uint8_t v17,
                        uint8_t v16,
                        uint8_t v15,
                        uint8_t v14,
                        uint8_t v13,
                        uint8_t v12,
                        uint8_t v11,
                        uint8_t v10,
                        uint8_t v9,
                        uint8_t v8,
                        uint8_t v7,
                        uint8_t v6,
                        uint8_t v5,
                        uint8_t v4,
                        uint8_t v3,
                        uint8_t v2,
                        uint8_t v1,
                        uint8_t v0) {
                    return _mm256_set_epi8(v31, v30, v29, v28, v27, v26, v25, v24, v23, v22, v21, v20, v19, v18, v17, v16, v15, v14, v13, v12, v11, v10, v9, v8, v7, v6, v5, v4, v3, v2, v1, v0);
                }

                static inline __m256i set_inc(
                        uint8_t v0) {
                    return _mm256_set_epi8(v0 + 31, v0 + 30, v0 + 29, v0 + 28, v0 + 27, v0 + 26, v0 + 25, v0 + 24, v0 + 23, v0 + 22, v0 + 21, v0 + 20, v0 + 19, v0 + 18, v0 + 17, v0 + 16, v0 + 15,
                            v0 + 14, v0 + 13, v0 + 12, v0 + 11, v0 + 10, v0 + 9, v0 + 8, v0 + 7, v0 + 6, v0 + 5, v0 + 4, v0 + 3, v0 + 2, v0 + 1, v0);
                }

                static inline __m256i set_inc(
                        uint8_t v0,
                        uint16_t inc) {
                    return _mm256_set_epi8(v0 + 31 * inc, v0 + 30 * inc, v0 + 29 * inc, v0 + 28 * inc, v0 + 27 * inc, v0 + 26 * inc, v0 + 25 * inc, v0 + 24 * inc, v0 + 23 * inc, v0 + 22 * inc,
                            v0 + 21 * inc, v0 + 20 * inc, v0 + 19 * inc, v0 + 18 * inc, v0 + 17 * inc, v0 + 16 * inc, v0 + 15 * inc, v0 + 14 * inc, v0 + 13 * inc, v0 + 12 * inc, v0 + 11 * inc,
                            v0 + 10 * inc, v0 + 9 * inc, v0 + 8 * inc, v0 + 7 * inc, v0 + 6 * inc, v0 + 5 * inc, v0 + 4 * inc, v0 + 3 * inc, v0 + 2 * inc, v0 + inc, v0);
                }

                static inline __m256i min(
                        __m256i a,
                        __m256i b) {
                    return _mm256_min_epu8(a, b);
                }

                static inline __m256i max(
                        __m256i a,
                        __m256i b) {
                    return _mm256_max_epu8(a, b);
                }

                static inline __m256i add(
                        __m256i a,
                        __m256i b) {
                    return _mm256_add_epi8(a, b);
                }

                static inline __m256i mullo(
                        __m256i a,
                        __m256i b) {
                    auto res1 = _mm256_mullo_epi16(_mm256_cvtepi8_epi16(_mm256_extractf128_si256(a, 0)), _mm256_cvtepi8_epi16(_mm256_extractf128_si256(b, 0)));
                    res1 = _mm256_shuffle_epi8(res1, _mm256_set_epi64x(0xFFFFFFFFFFFFFFFFull, 0x0D0C090805040100ull, 0xFFFFFFFFFFFFFFFFull, 0x0D0C090805040100ull));
                    auto res2 = _mm256_mullo_epi16(_mm256_cvtepi8_epi16(_mm256_extractf128_si256(a, 1)), _mm256_cvtepi8_epi16(_mm256_extractf128_si256(b, 1)));
                    res2 = _mm256_shuffle_epi8(res2, _mm256_set_epi64x(0xFFFFFFFFFFFFFFFFull, 0x0D0C090805040100ull, 0xFFFFFFFFFFFFFFFFull, 0x0D0C090805040100ull));
                    return _mm256_set_epi64x(_mm256_extract_epi64(res2, 2), _mm256_extract_epi64(res2, 0), _mm256_extract_epi64(res1, 2), _mm256_extract_epi64(res1, 0));
                }

                static inline uint8_t sum(
                        __m256i a) {
                    auto mm1 = _mm256_cvtepi8_epi32(_mm256_extractf128_si256(a, 0));
                    auto mm2 = _mm256_cvtepi8_epi32(_mm_srli_si128(_mm256_extractf128_si256(a, 0), 8));
                    auto mm3 = _mm256_cvtepi8_epi32(_mm256_extractf128_si256(a, 1));
                    auto mm4 = _mm256_cvtepi8_epi32(_mm_srli_si128(_mm256_extractf128_si256(a, 1), 8));
                    mm1 = _mm256_add_epi32(mm1, mm2);
                    mm3 = _mm256_add_epi32(mm3, mm4);
                    mm1 = _mm256_add_epi32(mm1, mm3);
                    mm1 = _mm256_add_epi32(mm1, _mm256_srli_si256(mm1, 8));
                    mm1 = _mm256_add_epi32(mm1, _mm256_srli_si256(mm1, 4));
                    auto mm128 = _mm_add_epi32(_mm256_extractf128_si256(mm1, 1), _mm256_extractf128_si256(mm1, 0));
                    return static_cast<uint8_t>(_mm_extract_epi32(mm128, 0));
                }

                static inline __m256i pack_right(
                        __m256i a,
                        mask_t mask) {
                    const uint64_t ALL_ONES = 0xFFFFFFFFFFFFFFFFull;
                    int MAX0[2] = {0, 1};
                    int MAX32a[2] = {32, 1}; // used for shifts where we may have to "erase" the argument by shifting it out of the register
                    int MAX32b[2] = {32, 1}; // used for shifts where we may have to "erase" the argument by shifting it out of the register
                    int MAX32c[2] = {32, 1}; // used for shifts where we may have to "erase" the argument by shifting it out of the register
                    int64_t shuffleMaskLL = SHUFFLE_TABLE_LL[static_cast<uint8_t>(mask)];
                    // number of unmatched bytes in bits (if a value matches, the leading bits are zero and the inversion makes it ones, so only full bytes are counted)
                    int clzLL = __builtin_clzll(~shuffleMaskLL);
                    int64_t shuffleMaskLH = SHUFFLE_TABLE_LH[static_cast<uint8_t>(mask >> 8)];
                    int clzLH = __builtin_clzll(~shuffleMaskLH);
                    int64_t shuffleMaskHL = SHUFFLE_TABLE_HL[static_cast<uint8_t>(mask >> 16)];
                    int clzHL = __builtin_clzll(~shuffleMaskHL);
                    int64_t shuffleMaskHH = SHUFFLE_TABLE_HH[static_cast<uint8_t>(mask >> 24)];

                    int nSelLL = 64 - clzLL;
                    int nSelLH = 64 - clzLH;
                    int nSelHL = 64 - clzHL;
                    int nSelLLLH = nSelLL + nSelLH;
                    MAX0[1] = 64 - nSelLLLH;
                    int clzLLLH = MAX0[MAX0[1] > 0];
                    int nSelLLLHHL = nSelLL + nSelLH + nSelHL;
                    MAX0[1] = 64 - nSelLLLHHL;
                    int clzLLLHHL = MAX0[MAX0[1] > 0];
                    int64_t shufMask0_sub0 = shuffleMaskLL;
                    int64_t shufMask0_sub1 = ((shuffleMaskLH << nSelLL) | (ALL_ONES >> clzLL));
                    int64_t shufMask0_sub2 = ((shuffleMaskHL << nSelLLLH) | (ALL_ONES >> clzLLLH));
                    int64_t shufMask0_sub3 = ((shuffleMaskHH << nSelLLLHHL) | (ALL_ONES >> clzLLLHHL));
                    int64_t shufMask0 = shufMask0_sub0 & shufMask0_sub1 & shufMask0_sub2 & shufMask0_sub3;

                    int64_t shufMask1_sub0 = shuffleMaskLH >> clzLL;
                    MAX32a[1] = clzLL - nSelLH; // amount of right-shift
                    MAX32b[1] = nSelLLLH - 64; // amount of left-shift
                    MAX32c[1] = 128 - nSelLLLH;
                    int64_t shufMask1_sub1 = (shuffleMaskHL >> MAX32a[MAX32a[1] > 0]) | (shuffleMaskHL << MAX32b[MAX32b[1] > 0]) | (ALL_ONES >> MAX32c[MAX32c[1] > 0]);
                    MAX32a[1] = clzLL - (nSelLH + nSelHL);
                    MAX32b[1] = nSelLLLHHL - 64;
                    MAX32c[1] = 128 - nSelLLLHHL;
                    int64_t shufMask1_sub2 = (shuffleMaskHH >> MAX32a[MAX32a[1] > 0]) | (shuffleMaskHH << MAX32b[MAX32b[1] > 0]) | (ALL_ONES >> MAX32c[MAX32c[1] > 0]);
                    int64_t shufMask1 = shufMask1_sub0 & shufMask1_sub1 & shufMask1_sub2;

                    int64_t shufMask2_sub0 = shuffleMaskHL >> (clzLL + clzLH);
                    MAX32a[1] = (clzLL + clzLH) - (nSelLH + nSelHL);
                    MAX32b[1] = nSelLLLHHL - 128;
                    MAX32c[1] = 192 - nSelLLLHHL;
                    int64_t shufMask2_sub1 = (shuffleMaskHH >> MAX32a[MAX32a[1] > 0]) | (shuffleMaskHH << MAX32b[MAX32b[1] > 0]) | (ALL_ONES >> MAX32c[MAX32c[1] > 0]);
                    int64_t shufMask2 = shufMask2_sub0 & shufMask2_sub1;

                    int64_t shufMask3 = shuffleMaskHH >> (clzLL + clzLH + clzHL);
                    return Private::_mm256_shuffle256_epi8(a, _mm256_set_epi64x(shufMask3, shufMask2, shufMask1, shufMask0));
                }

                static inline void pack_right2(
                        uint8_t * & result,
                        __m256i a,
                        mask_t mask) {
                    Private::pack_right2_uint8(result, a, mask);
                }

                static inline void pack_right3(
                        uint8_t * & result,
                        __m256i a,
                        mask_t mask) {
                    typedef mm<__m128i, uint8_t>::mask_t sse_mask_t;
                    auto maskLow = static_cast<sse_mask_t>(mask);
                    _mm_storeu_si128(reinterpret_cast<__m128i *>(result), mm<__m128i, uint8_t>::pack_right(_mm256_extracti128_si256(a, 0), maskLow));
                    result += __builtin_popcount(maskLow);
                    auto maskHigh = static_cast<sse_mask_t>(mask >> 16);
                    _mm_storeu_si128(reinterpret_cast<__m128i *>(result), mm<__m128i, uint8_t>::pack_right(_mm256_extracti128_si256(a, 1), maskHigh));
                    result += __builtin_popcount(maskHigh);
                }

                static inline popcnt_t popcount(
                        __m256i a) {
                    auto pattern1 = _mm256_set1_epi8(0x55);
                    auto pattern2 = _mm256_set1_epi8(0x33);
                    auto pattern3 = _mm256_set1_epi8(0x0F);
                    auto temp = _mm256_sub_epi8(a, _mm256_and_si256(_mm256_srli_epi16(a, 1), pattern1));
                    temp = _mm256_add_epi8(_mm256_and_si256(temp, pattern2), _mm256_and_si256(_mm256_srli_epi16(temp, 2), pattern2));
                    temp = _mm256_add_epi8(_mm256_and_si256(temp, pattern3), _mm256_and_si256(_mm256_srli_epi16(temp, 4), pattern3));
                    return temp;
                }

                static inline popcnt_t popcount2(
                        __m256i a) {
                    auto lookup = _mm256_set_epi8(4, 3, 3, 2, 3, 2, 2, 1, 3, 2, 2, 1, 2, 1, 1, 0, 4, 3, 3, 2, 3, 2, 2, 1, 3, 2, 2, 1, 2, 1, 1, 0);
                    auto low_mask = _mm256_set1_epi8(0x0f);
                    auto lo = _mm256_and_si256(a, low_mask);
                    auto hi = _mm256_and_si256(_mm256_srli_epi16(a, 4), low_mask);
                    auto cnt_lo = _mm256_shuffle_epi8(lookup, lo);
                    auto cnt_hi = _mm256_shuffle_epi8(lookup, hi);
                    return _mm256_add_epi8(cnt_lo, cnt_hi);
                }

                static inline popcnt_t popcount3(
                        __m256i a) {
                    return _mm256_set_epi8(_mm_popcnt_u32(_mm256_extract_epi8(a, 31)), _mm_popcnt_u32(_mm256_extract_epi8(a, 30)), _mm_popcnt_u32(_mm256_extract_epi8(a, 29)),
                            _mm_popcnt_u32(_mm256_extract_epi8(a, 28)), _mm_popcnt_u32(_mm256_extract_epi8(a, 27)), _mm_popcnt_u32(_mm256_extract_epi8(a, 26)),
                            _mm_popcnt_u32(_mm256_extract_epi8(a, 25)), _mm_popcnt_u32(_mm256_extract_epi8(a, 24)), _mm_popcnt_u32(_mm256_extract_epi8(a, 23)),
                            _mm_popcnt_u32(_mm256_extract_epi8(a, 22)), _mm_popcnt_u32(_mm256_extract_epi8(a, 21)), _mm_popcnt_u32(_mm256_extract_epi8(a, 20)),
                            _mm_popcnt_u32(_mm256_extract_epi8(a, 19)), _mm_popcnt_u32(_mm256_extract_epi8(a, 18)), _mm_popcnt_u32(_mm256_extract_epi8(a, 17)),
                            _mm_popcnt_u32(_mm256_extract_epi8(a, 16)), _mm_popcnt_u32(_mm256_extract_epi8(a, 15)), _mm_popcnt_u32(_mm256_extract_epi8(a, 14)),
                            _mm_popcnt_u32(_mm256_extract_epi8(a, 13)), _mm_popcnt_u32(_mm256_extract_epi8(a, 12)), _mm_popcnt_u32(_mm256_extract_epi8(a, 11)),
                            _mm_popcnt_u32(_mm256_extract_epi8(a, 10)), _mm_popcnt_u32(_mm256_extract_epi8(a, 9)), _mm_popcnt_u32(_mm256_extract_epi8(a, 8)), _mm_popcnt_u32(_mm256_extract_epi8(a, 7)),
                            _mm_popcnt_u32(_mm256_extract_epi8(a, 6)), _mm_popcnt_u32(_mm256_extract_epi8(a, 5)), _mm_popcnt_u32(_mm256_extract_epi8(a, 4)), _mm_popcnt_u32(_mm256_extract_epi8(a, 3)),
                            _mm_popcnt_u32(_mm256_extract_epi8(a, 2)), _mm_popcnt_u32(_mm256_extract_epi8(a, 1)), _mm_popcnt_u32(_mm256_extract_epi8(a, 0)));
                }

            private:
                static const int64_t * const SHUFFLE_TABLE_LL;
                static const int64_t * const SHUFFLE_TABLE_LH;
                static const int64_t * const SHUFFLE_TABLE_HL;
                static const int64_t * const SHUFFLE_TABLE_HH;
            };

        }
    }
}
