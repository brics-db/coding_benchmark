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
 * AVX2_uint16.tcc
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

            namespace Private16 {

                template<size_t current = 0>
                inline void pack_right2_int16(
                        int16_t * & result,
                        __m256i & a,
                        uint16_t mask) {
                    *result = reinterpret_cast<int16_t*>(&a)[current];
                    result += (mask >> current) & 0x1;
                    pack_right2_int16<current + 1>(result, a, mask);
                }

                template<>
                inline void pack_right2_int16<15>(
                        int16_t * & result,
                        __m256i & a,
                        uint16_t mask) {
                    *result = reinterpret_cast<int16_t*>(&a)[15];
                    result += (mask >> 15) & 0x1;
                }

                template<size_t current = 0>
                inline void pack_right2_int16(
                        uint16_t * & result,
                        __m256i & a,
                        uint16_t mask) {
                    *result = reinterpret_cast<uint16_t*>(&a)[current];
                    result += (mask >> current) & 0x1;
                    pack_right2_int16<current + 1>(result, a, mask);
                }

                template<>
                inline void pack_right2_int16<15>(
                        uint16_t * & result,
                        __m256i & a,
                        uint16_t mask) {
                    *result = reinterpret_cast<uint16_t*>(&a)[15];
                    result += (mask >> 15) & 0x1;
                }

                template<typename T>
                struct _mm256 {

                    typedef uint16_t mask_t;
                    typedef __m128i popcnt_t;

                    static inline __m256i set1(
                            T value) {
                        return _mm256_set1_epi16(value);
                    }

                    static inline __m256i set(
                            T v15,
                            T v14,
                            T v13,
                            T v12,
                            T v11,
                            T v10,
                            T v9,
                            T v8,
                            T v7,
                            T v6,
                            T v5,
                            T v4,
                            T v3,
                            T v2,
                            T v1,
                            T v0) {
                        return _mm256_set_epi16(v15, v14, v13, v12, v11, v10, v9, v8, v7, v6, v5, v4, v3, v2, v1, v0);
                    }

                    static inline __m256i set_inc(
                            T v0) {
                        return _mm256_set_epi16(v0 + 15, v0 + 14, v0 + 13, v0 + 12, v0 + 11, v0 + 10, v0 + 9, v0 + 8, v0 + 7, v0 + 6, v0 + 5, v0 + 4, v0 + 3, v0 + 2, v0 + 1, v0);
                    }

                    static inline __m256i set_inc(
                            T v0,
                            T inc) {
                        return _mm256_set_epi16(v0 + 15 * inc, v0 + 14 * inc, v0 + 13 * inc, v0 + 12 * inc, v0 + 11 * inc, v0 + 10 * inc, v0 + 9 * inc, v0 + 8 * inc, v0 + 7 * inc, v0 + 6 * inc,
                                v0 + 5 * inc, v0 + 4 * inc, v0 + 3 * inc, v0 + 2 * inc, v0 + inc, v0);
                    }

                    static inline __m256i min(
                            __m256i a,
                            __m256i b) {
                        return _mm256_min_epu16(a, b);
                    }

                    static inline __m256i max(
                            __m256i a,
                            __m256i b) {
                        return _mm256_max_epu16(a, b);
                    }

                    static inline __m256i add(
                            __m256i a,
                            __m256i b) {
                        return _mm256_add_epi16(a, b);
                    }

                    static inline __m256i mullo(
                            __m256i a,
                            __m256i b) {
                        return _mm256_mullo_epi16(a, b);
                    }

                    static inline T sum(
                            __m256i a) {
                        auto mm1 = _mm256_cvtepi16_epi32(_mm256_extractf128_si256(a, 0));
                        auto mm2 = _mm256_cvtepi16_epi32(_mm256_extractf128_si256(a, 1));
                        mm1 = _mm256_add_epi32(mm1, mm2);
                        mm1 = _mm256_add_epi32(mm1, _mm256_srli_si256(mm1, 8));
                        mm1 = _mm256_add_epi32(mm1, _mm256_srli_si256(mm1, 4));
                        auto mm128 = _mm_add_epi32(_mm256_extractf128_si256(mm1, 1), _mm256_extractf128_si256(mm1, 0));
                        return static_cast<T>(_mm_extract_epi32(mm128, 0));
                    }

                    static inline __m256i pack_right(
                            __m256i a,
                            mask_t mask) {
                        static const uint64_t ALL_ONES = 0xFFFFFFFFFFFFFFFFull;
                        int MAX0[2] = {0, 1};
                        int MAX32a[2] = {32, 1}; // used for shifts where we may have to "erase" the argument by shifting it out of the register
                        int MAX32b[2] = {32, 1}; // used for shifts where we may have to "erase" the argument by shifting it out of the register
                        int MAX32c[2] = {32, 1}; // used for shifts where we may have to "erase" the argument by shifting it out of the register
                        auto mmShuffleMaskL = SHUFFLE_TABLE_L[static_cast<uint8_t>(mask)];
                        auto mmShuffleMaskH = SHUFFLE_TABLE_H[static_cast<uint8_t>(mask >> 8)];
                        int64_t shuffleMaskLL = static_cast<int64_t>(_mm_extract_epi64(mmShuffleMaskL, 0));
                        // number of unmatched bytes in bits (if a value matches, the leading bits are zero and the inversion makes it ones, so only full bytes are counted)
                        int clzLL = __builtin_clzll(~shuffleMaskLL);
                        int64_t shuffleMaskLH = static_cast<int64_t>(_mm_extract_epi64(mmShuffleMaskL, 1));
                        int clzLH = __builtin_clzll(~shuffleMaskLH);
                        int64_t shuffleMaskHL = static_cast<int64_t>(_mm_extract_epi64(mmShuffleMaskH, 0));
                        int clzHL = __builtin_clzll(~shuffleMaskHL);
                        int64_t shuffleMaskHH = static_cast<int64_t>(_mm_extract_epi64(mmShuffleMaskH, 1));

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
                        return _mm256_shuffle256_epi8(a, _mm256_set_epi64x(shufMask3, shufMask2, shufMask1, shufMask0));
                    }

                    static inline void pack_right2(
                            T * & result,
                            __m256i a,
                            mask_t mask) {
                        pack_right2_int16(result, a, mask);
                    }

                    static inline void pack_right3(
                            T * & result,
                            __m256i a,
                            mask_t mask) {
                        typedef mm<__m128i, T>::mask_t sse_mask_t;
                        auto maskLow = static_cast<sse_mask_t>(mask);
                        _mm_storeu_si128(reinterpret_cast<__m128i *>(result), mm<__m128i, T>::pack_right(_mm256_extracti128_si256(a, 0), maskLow));
                        result += __builtin_popcount(maskLow);
                        auto maskHigh = static_cast<sse_mask_t>(mask >> 8);
                        _mm_storeu_si128(reinterpret_cast<__m128i *>(result), mm<__m128i, T>::pack_right(_mm256_extracti128_si256(a, 1), maskHigh));
                        result += __builtin_popcount(maskHigh);
                    }

                    static inline popcnt_t popcount(
                            __m256i a) {
                        auto pattern1 = _mm256_set1_epi16(0x5555);
                        auto pattern2 = _mm256_set1_epi16(0x3333);
                        auto pattern3 = _mm256_set1_epi16(0x0F0F);
                        auto pattern4 = _mm256_set1_epi16(0x0101);
                        auto shuffle = _mm256_set_epi64x(0xFFFFFFFFFFFFFFFF, 0x0F0D0B0907050301, 0xFFFFFFFFFFFFFFFF, 0x0F0D0B0907050301);
                        auto temp = _mm256_sub_epi16(a, _mm256_and_si256(_mm256_srli_epi16(a, 1), pattern1));
                        temp = _mm256_add_epi16(_mm256_and_si256(temp, pattern2), _mm256_and_si256(_mm256_srli_epi16(temp, 2), pattern2));
                        temp = _mm256_and_si256(_mm256_add_epi16(temp, _mm256_srli_epi16(temp, 4)), pattern3);
                        temp = _mm256_mullo_epi16(temp, pattern4);
                        temp = _mm256_shuffle_epi8(temp, shuffle);
                        return _mm_set_epi64x(_mm256_extract_epi64(temp, 2), _mm256_extract_epi64(temp, 0));
                    }

                    static inline popcnt_t popcount2(
                            __m256i a) {
                        auto mask = _mm256_set1_epi16(0x0101);
                        auto shuffle = _mm256_set_epi64x(0xFFFFFFFFFFFFFFFF, 0x0F0D0B0907050301, 0xFFFFFFFFFFFFFFFF, 0x0F0D0B0907050301);
                        auto popcount8 = mm<__m256i, uint8_t>::popcount2(a);
                        auto temp = _mm256_shuffle_epi8(_mm256_mullo_epi16(popcount8, mask), shuffle);
                        return _mm_set_epi64x(_mm256_extract_epi64(temp, 2), _mm256_extract_epi64(temp, 0));
                    }

                    static inline popcnt_t popcount3(
                            __m256i a) {
                        return _mm_set_epi8(_mm_popcnt_u32(_mm256_extract_epi16(a, 15)), _mm_popcnt_u32(_mm256_extract_epi16(a, 14)), _mm_popcnt_u32(_mm256_extract_epi16(a, 13)),
                                _mm_popcnt_u32(_mm256_extract_epi16(a, 12)), _mm_popcnt_u32(_mm256_extract_epi16(a, 11)), _mm_popcnt_u32(_mm256_extract_epi16(a, 10)),
                                _mm_popcnt_u32(_mm256_extract_epi16(a, 9)), _mm_popcnt_u32(_mm256_extract_epi16(a, 8)), _mm_popcnt_u32(_mm256_extract_epi16(a, 7)),
                                _mm_popcnt_u32(_mm256_extract_epi16(a, 6)), _mm_popcnt_u32(_mm256_extract_epi16(a, 5)), _mm_popcnt_u32(_mm256_extract_epi16(a, 4)),
                                _mm_popcnt_u32(_mm256_extract_epi16(a, 3)), _mm_popcnt_u32(_mm256_extract_epi16(a, 2)), _mm_popcnt_u32(_mm256_extract_epi16(a, 1)),
                                _mm_popcnt_u32(_mm256_extract_epi16(a, 0)));
                    }

                private:
                    static const __m128i * const SHUFFLE_TABLE_L;
                    static const __m128i * const SHUFFLE_TABLE_H;
                };

            } /* Private16 */

            template<>
            struct mm256<int16_t> :
                    public Private16::_mm256<int16_t> {
                typedef Private16::_mm256<int16_t> BASE;
                using BASE::mask_t;
                using BASE::popcnt_t;
                using BASE::set1;
                using BASE::set;
                using BASE::set_inc;
                using BASE::min;
                using BASE::max;
                using BASE::add;
                using BASE::sum;
                using BASE::mullo;
                using BASE::pack_right;
                using BASE::pack_right2;
                using BASE::popcount;
                using BASE::popcount2;
                using BASE::popcount3;
            };

            template<>
            struct mm256<uint16_t> :
                    public Private16::_mm256<uint16_t> {
                typedef Private16::_mm256<uint16_t> BASE;
                using BASE::mask_t;
                using BASE::popcnt_t;
                using BASE::set1;
                using BASE::set;
                using BASE::set_inc;
                using BASE::min;
                using BASE::max;
                using BASE::add;
                using BASE::sum;
                using BASE::mullo;
                using BASE::pack_right;
                using BASE::pack_right2;
                using BASE::popcount;
                using BASE::popcount2;
                using BASE::popcount3;
            };

        }
    }
}
