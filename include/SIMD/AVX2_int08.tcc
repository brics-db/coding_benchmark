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

            namespace Private08 {

                template<size_t current = 0>
                inline void pack_right2_int8(
                        int8_t * & result,
                        __m256i & a,
                        uint32_t mask) {
                    *result = reinterpret_cast<int8_t*>(&a)[current];
                    result += (mask >> current) & 0x1;
                    pack_right2_int8<current + 1>(result, a, mask);
                }

                template<>
                inline void pack_right2_int8<31>(
                        int8_t * & result,
                        __m256i & a,
                        uint32_t mask) {
                    *result = reinterpret_cast<int8_t*>(&a)[31];
                    result += (mask >> 31) & 0x1;
                }

                template<size_t current = 0>
                inline void pack_right2_int8(
                        uint8_t * & result,
                        __m256i & a,
                        uint32_t mask) {
                    *result = reinterpret_cast<uint8_t*>(&a)[current];
                    result += (mask >> current) & 0x1;
                    pack_right2_int8<current + 1>(result, a, mask);
                }

                template<>
                inline void pack_right2_int8<31>(
                        uint8_t * & result,
                        __m256i & a,
                        uint32_t mask) {
                    *result = reinterpret_cast<uint8_t*>(&a)[31];
                    result += (mask >> 31) & 0x1;
                }

                template<typename T>
                struct _mm256 {

                    typedef uint32_t mask_t;
                    typedef __m256i popcnt_t;

                    static const constexpr mask_t FULL_MASK = 0xFFFFFFFFu;

                    static inline __m256i set1(
                            T value) {
                        return _mm256_set1_epi8(value);
                    }

                    static inline __m256i set(
                            T v31,
                            T v30,
                            T v29,
                            T v28,
                            T v27,
                            T v26,
                            T v25,
                            T v24,
                            T v23,
                            T v22,
                            T v21,
                            T v20,
                            T v19,
                            T v18,
                            T v17,
                            T v16,
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
                        return _mm256_set_epi8(v31, v30, v29, v28, v27, v26, v25, v24, v23, v22, v21, v20, v19, v18, v17, v16, v15, v14, v13, v12, v11, v10, v9, v8, v7, v6, v5, v4, v3, v2, v1, v0);
                    }

                    static inline __m256i set_inc(
                            T v0) {
                        return _mm256_set_epi8(v0 + 31, v0 + 30, v0 + 29, v0 + 28, v0 + 27, v0 + 26, v0 + 25, v0 + 24, v0 + 23, v0 + 22, v0 + 21, v0 + 20, v0 + 19, v0 + 18, v0 + 17, v0 + 16, v0 + 15,
                                v0 + 14, v0 + 13, v0 + 12, v0 + 11, v0 + 10, v0 + 9, v0 + 8, v0 + 7, v0 + 6, v0 + 5, v0 + 4, v0 + 3, v0 + 2, v0 + 1, v0);
                    }

                    static inline __m256i set_inc(
                            T v0,
                            T inc) {
                        return _mm256_set_epi8(v0 + 31 * inc, v0 + 30 * inc, v0 + 29 * inc, v0 + 28 * inc, v0 + 27 * inc, v0 + 26 * inc, v0 + 25 * inc, v0 + 24 * inc, v0 + 23 * inc, v0 + 22 * inc,
                                v0 + 21 * inc, v0 + 20 * inc, v0 + 19 * inc, v0 + 18 * inc, v0 + 17 * inc, v0 + 16 * inc, v0 + 15 * inc, v0 + 14 * inc, v0 + 13 * inc, v0 + 12 * inc, v0 + 11 * inc,
                                v0 + 10 * inc, v0 + 9 * inc, v0 + 8 * inc, v0 + 7 * inc, v0 + 6 * inc, v0 + 5 * inc, v0 + 4 * inc, v0 + 3 * inc, v0 + 2 * inc, v0 + inc, v0);
                    }

                    static inline __m256i min(
                            __m256i a,
                            __m256i b) {
                        if constexpr (std::is_signed_v<T>) {
                            return _mm256_min_epi8(a, b);
                        } else {
                            return _mm256_min_epu8(a, b);
                        }
                    }

                    static T min(
                            __m256i a) {
                        auto x1 = mm<__m128i, T>::min(_mm256_extractf128_si256(a, 0));
                        auto x2 = mm<__m128i, T>::min(_mm256_extractf128_si256(a, 1));
                        return x1 < x2 ? x1 : x2;
                    }

                    static inline __m256i max(
                            __m256i a,
                            __m256i b) {
                        if constexpr (std::is_signed_v<T>) {
                            return _mm256_max_epi8(a, b);
                        } else {
                            return _mm256_max_epu8(a, b);
                        }
                    }

                    static T max(
                            __m256i a) {
                        auto x1 = mm<__m128i, T>::max(_mm256_extractf128_si256(a, 0));
                        auto x2 = mm<__m128i, T>::max(_mm256_extractf128_si256(a, 1));
                        return x1 > x2 ? x1 : x2;
                    }

                    static inline T sum(
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
                        return static_cast<T>(_mm_extract_epi32(mm128, 0));
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
                        return _mm256_shuffle256_epi8(a, _mm256_set_epi64x(shufMask3, shufMask2, shufMask1, shufMask0));
                    }

                    static inline void pack_right2(
                            T * & result,
                            __m256i a,
                            mask_t mask) {
                        pack_right2_int8(result, a, mask);
                    }

                    static inline void pack_right3(
                            T * & result,
                            __m256i a,
                            mask_t mask) {
                        typedef typename mm<__m128i, T>::mask_t sse_mask_t;
                        auto maskLow = static_cast<sse_mask_t>(mask);
                        _mm_storeu_si128(reinterpret_cast<__m128i *>(result), mm<__m128i, T>::pack_right(_mm256_extracti128_si256(a, 0), maskLow));
                        result += __builtin_popcount(maskLow);
                        auto maskHigh = static_cast<sse_mask_t>(mask >> 16);
                        _mm_storeu_si128(reinterpret_cast<__m128i *>(result), mm<__m128i, T>::pack_right(_mm256_extracti128_si256(a, 1), maskHigh));
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
                                _mm_popcnt_u32(_mm256_extract_epi8(a, 10)), _mm_popcnt_u32(_mm256_extract_epi8(a, 9)), _mm_popcnt_u32(_mm256_extract_epi8(a, 8)),
                                _mm_popcnt_u32(_mm256_extract_epi8(a, 7)), _mm_popcnt_u32(_mm256_extract_epi8(a, 6)), _mm_popcnt_u32(_mm256_extract_epi8(a, 5)),
                                _mm_popcnt_u32(_mm256_extract_epi8(a, 4)), _mm_popcnt_u32(_mm256_extract_epi8(a, 3)), _mm_popcnt_u32(_mm256_extract_epi8(a, 2)),
                                _mm_popcnt_u32(_mm256_extract_epi8(a, 1)), _mm_popcnt_u32(_mm256_extract_epi8(a, 0)));
                    }

                    static inline __m256i cvt_larger_lo(
                            __m256i a) {
                        return _mm256_cvtepi8_epi16(_mm256_extractf128_si256(a, 0));
                    }

                    static inline __m256i cvt_larger_hi(
                            __m256i a) {
                        return _mm256_cvtepi8_epi16(_mm256_extractf128_si256(a, 1));
                    }

                private:
            static const int64_t * const SHUFFLE_TABLE_LL;
            static const int64_t * const SHUFFLE_TABLE_LH;
            static const int64_t * const SHUFFLE_TABLE_HL;
            static const int64_t * const SHUFFLE_TABLE_HH;
                };

                template<typename T, template<typename > class Op>
                struct _mm256op;

                template<typename T>
                struct _mm256op<T, std::greater> {

                    typedef typename _mm256<T>::mask_t mask_t;

                    static inline __m256i cmp(
                            __m256i a,
                            __m256i b) {
                        return _mm256_cmpgt_epi8(a, b);
                    }

                    static inline mask_t cmp_mask(
                            __m256i a,
                            __m256i b) {
                        return static_cast<mask_t>(_mm256_movemask_epi8(cmp(a, b)));
                    }
                };

                template<typename T>
                struct _mm256op<T, std::greater_equal> {

                    typedef typename _mm256<T>::mask_t mask_t;

                    static inline __m256i cmp(
                            __m256i a,
                            __m256i b) {
                        auto mm = _mm256<T>::max(a, b);
                        return _mm256_cmpeq_epi8(a, mm);
                    }

                    static inline mask_t cmp_mask(
                            __m256i a,
                            __m256i b) {
                        return static_cast<mask_t>(_mm256_movemask_epi8(cmp(a, b)));
                    }
                };

                template<typename T>
                struct _mm256op<T, std::less> {

                    typedef typename _mm256<T>::mask_t mask_t;

                    static inline __m256i cmp(
                            __m256i a,
                            __m256i b) {
                        return _mm256_cmpgt_epi8(b, a);
                    }

                    static inline mask_t cmp_mask(
                            __m256i a,
                            __m256i b) {
                        return static_cast<mask_t>(_mm256_movemask_epi8(cmp(a, b)));
                    }
                };

                template<typename T>
                struct _mm256op<T, std::less_equal> {

                    typedef typename _mm256<T>::mask_t mask_t;

                    static inline __m256i cmp(
                            __m256i a,
                            __m256i b) {
                        auto mm = sse::mm128 < T > ::min(a, b);
                        return _mm256_cmpeq_epi8(a, mm);
                    }

                    static inline mask_t cmp_mask(
                            __m256i a,
                            __m256i b) {
                        return static_cast<mask_t>(_mm256_movemask_epi8(cmp(a, b)));
                    }
                };

                template<typename T>
                struct _mm256op<T, std::equal_to> {

                    typedef typename _mm256<T>::mask_t mask_t;

                    static inline __m256i cmp(
                            __m256i a,
                            __m256i b) {
                        return _mm256_cmpeq_epi8(a, b);
                    }

                    static inline mask_t cmp_mask(
                            __m256i a,
                            __m256i b) {
                        return static_cast<mask_t>(_mm256_movemask_epi8(cmp(a, b)));
                    }
                };

                template<typename T>
                struct _mm256op<T, std::not_equal_to> {

                    typedef typename _mm256<T>::mask_t mask_t;

                    static inline __m256i cmp(
                            __m256i a,
                            __m256i b) {
                        return _mm256_or_si256(_mm256_cmpgt_epi8(b, a), _mm256_cmpgt_epi8(a, b));
                    }

                    static inline mask_t cmp_mask(
                            __m256i a,
                            __m256i b) {
                        return static_cast<mask_t>(_mm256_movemask_epi8(cmp(a, b)));
                    }
                };

                template<typename T>
                struct _mm256op<T, coding_benchmark::and_is> {

                    typedef typename _mm256<T>::mask_t mask_t;

                    static inline __m256i cmp(
                            __m256i a,
                            __m256i b) {
                        return _mm256_and_si256(a, b);
                    }

                    static inline mask_t cmp_mask(
                            __m256i a,
                            __m256i b) {
                        return static_cast<mask_t>(_mm256_movemask_epi8(cmp(a, b)));
                    }
                };

                template<typename T>
                struct _mm256op<T, coding_benchmark::or_is> {

                    typedef typename _mm256<T>::mask_t mask_t;

                    static inline __m256i cmp(
                            __m256i a,
                            __m256i b) {
                        return _mm256_or_si256(a, b);
                    }

                    static inline mask_t cmp_mask(
                            __m256i a,
                            __m256i b) {
                        return static_cast<mask_t>(_mm256_movemask_epi8(cmp(a, b)));
                    }
                };

                template<typename T>
                struct _mm256op<T, coding_benchmark::xor_is> {

                    typedef typename _mm256<T>::mask_t mask_t;

                    static inline __m256i cmp(
                            __m256i a,
                            __m256i b) {
                        return _mm256_xor_si256(a, b);
                    }

                    static inline mask_t cmp_mask(
                            __m256i a,
                            __m256i b) {
                        return static_cast<mask_t>(_mm256_movemask_epi8(cmp(a, b)));
                    }
                };

                template<typename T>
                struct _mm256op<T, coding_benchmark::is_not> {

                    typedef typename _mm256<T>::mask_t mask_t;

                    static inline __m256i cmp(
                            __m256i a,
                            __m256i b) {
                        return _mm256_andnot_si256(a, _mm256_set1_epi64x(0xFFFFFFFFFFFFFFFFull));
                    }

                    static inline mask_t cmp_mask(
                            __m256i a,
                            __m256i b) {
                        return static_cast<mask_t>(_mm256_movemask_epi8(cmp(a, b)));
                    }
                };

                template<typename T>
                struct _mm256op<T, coding_benchmark::add> {

                    static inline __m256i compute(
                            __m256i a,
                            __m256i b) {
                        return add(a, b);
                    }

                    static inline __m256i add(
                            __m256i a,
                            __m256i b) {
                        return _mm256_add_epi8(a, b);
                    }
                };

                template<typename T>
                struct _mm256op<T, coding_benchmark::sub> {

                    static inline __m256i compute(
                            __m256i a,
                            __m256i b) {
                        return sub(a, b);
                    }

                    static inline __m256i sub(
                            __m256i a,
                            __m256i b) {
                        return _mm256_sub_epi8(a, b);
                    }
                };

                template<typename T>
                struct _mm256op<T, coding_benchmark::mul> {

                    static inline __m256i compute(
                            __m256i a,
                            __m256i b) {
                        return mullo(a, b);
                    }

                    static inline __m256i mullo(
                            __m256i a,
                            __m256i b) {
                        auto mmA0 = _mm256_extracti128_si256(a, 0);
                        auto mmA1 = _mm256_extracti128_si256(a, 1);
                        auto mmB0 = _mm256_extracti128_si256(b, 0);
                        auto mmB1 = _mm256_extracti128_si256(b, 1);
                        auto mm0 = mm_op<__m128i, T, coding_benchmark::mul>::mullo(mmA0, mmB0);
                        auto mm1 = mm_op<__m128i, T, coding_benchmark::mul>::mullo(mmA1, mmB1);
                        auto c = _mm256_castsi128_si256(mm0);
                        return _mm256_insertf128_si256(c, mm1, 1);
                    }
                };

                template<typename T>
                struct _mm256op<T, coding_benchmark::div> {

                    static inline __m256i compute(
                            __m256i a,
                            __m256i b) {
                        return div(a, b);
                    }

                    static inline __m256i div(
                            __m256i a,
                            __m256i b) {
                        auto mmA0 = _mm256_extracti128_si256(a, 0);
                        auto mmA1 = _mm256_extracti128_si256(a, 1);
                        auto mmB0 = _mm256_extracti128_si256(b, 0);
                        auto mmB1 = _mm256_extracti128_si256(b, 1);
                        auto mm0 = mm_op<__m128i, T, coding_benchmark::div>::div(mmA0, mmB0);
                        auto mm1 = mm_op<__m128i, T, coding_benchmark::div>::div(mmA1, mmB1);
                        auto c = _mm256_castsi128_si256(mm0);
                        return _mm256_insertf128_si256(c, mm1, 1);
                    }
                };

            } /* Private08 */

            template<>
            struct mm256<int8_t> :
                    public Private08::_mm256<int8_t> {
                typedef Private08::_mm256<int8_t> BASE;
                using BASE::mask_t;
                using BASE::popcnt_t;
                using BASE::FULL_MASK;
                using BASE::set1;
                using BASE::set;
                using BASE::set_inc;
                using BASE::min;
                using BASE::max;
                using BASE::sum;
                using BASE::pack_right;
                using BASE::pack_right2;
                using BASE::popcount;
                using BASE::popcount2;
                using BASE::popcount3;
                using BASE::cvt_larger_lo;
                using BASE::cvt_larger_hi;
            };

            template<>
            struct mm256op<int8_t, std::greater_equal> :
                    private Private08::_mm256op<int8_t, std::greater_equal> {
                typedef Private08::_mm256op<int8_t, std::greater_equal> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm256op<int8_t, std::greater> :
                    private Private08::_mm256op<int8_t, std::greater> {
                typedef Private08::_mm256op<int8_t, std::greater> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm256op<int8_t, std::less_equal> :
                    private Private08::_mm256op<int8_t, std::less_equal> {
                typedef Private08::_mm256op<int8_t, std::less_equal> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm256op<int8_t, std::less> :
                    private Private08::_mm256op<int8_t, std::less> {
                typedef Private08::_mm256op<int8_t, std::less> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm256op<int8_t, std::equal_to> :
                    private Private08::_mm256op<int8_t, std::equal_to> {
                typedef Private08::_mm256op<int8_t, std::equal_to> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm256op<int8_t, std::not_equal_to> :
                    private Private08::_mm256op<int8_t, std::not_equal_to> {
                typedef Private08::_mm256op<int8_t, std::not_equal_to> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm256op<int8_t, coding_benchmark::and_is> :
                    private Private08::_mm256op<int8_t, coding_benchmark::and_is> {
                typedef Private08::_mm256op<int8_t, coding_benchmark::and_is> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm256op<int8_t, coding_benchmark::or_is> :
                    private Private08::_mm256op<int8_t, coding_benchmark::or_is> {
                typedef Private08::_mm256op<int8_t, coding_benchmark::or_is> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm256op<int8_t, coding_benchmark::xor_is> :
                    private Private08::_mm256op<int8_t, coding_benchmark::xor_is> {
                typedef Private08::_mm256op<int8_t, coding_benchmark::xor_is> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm256op<int8_t, coding_benchmark::is_not> :
                    private Private08::_mm256op<int8_t, coding_benchmark::is_not> {
                typedef Private08::_mm256op<int8_t, coding_benchmark::is_not> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm256op<int8_t, coding_benchmark::add> :
                    private Private08::_mm256op<int8_t, coding_benchmark::add> {
                typedef Private08::_mm256op<int8_t, coding_benchmark::add> BASE;
                using BASE::add;
                using BASE::compute;
            };

            template<>
            struct mm256op<int8_t, coding_benchmark::sub> :
                    private Private08::_mm256op<int8_t, coding_benchmark::sub> {
                typedef Private08::_mm256op<int8_t, coding_benchmark::sub> BASE;
                using BASE::sub;
                using BASE::compute;
            };

            template<>
            struct mm256op<int8_t, coding_benchmark::mul> :
                    private Private08::_mm256op<int8_t, coding_benchmark::mul> {
                typedef Private08::_mm256op<int8_t, coding_benchmark::mul> BASE;
                using BASE::mullo;
                using BASE::compute;
            };

            template<>
            struct mm256op<int8_t, coding_benchmark::div> :
                    private Private08::_mm256op<int8_t, coding_benchmark::div> {
                typedef Private08::_mm256op<int8_t, coding_benchmark::div> BASE;
                using BASE::div;
                using BASE::compute;
            };

            template<>
            struct mm256<uint8_t> :
                    public Private08::_mm256<uint8_t> {
                typedef Private08::_mm256<uint8_t> BASE;
                using BASE::mask_t;
                using BASE::popcnt_t;
                using BASE::FULL_MASK;
                using BASE::set1;
                using BASE::set;
                using BASE::set_inc;
                using BASE::min;
                using BASE::max;
                using BASE::sum;
                using BASE::pack_right;
                using BASE::pack_right2;
                using BASE::popcount;
                using BASE::popcount2;
                using BASE::popcount3;
                using BASE::cvt_larger_lo;
                using BASE::cvt_larger_hi;
            };

            template<>
            struct mm256op<uint8_t, std::greater_equal> :
                    private Private08::_mm256op<uint8_t, std::greater_equal> {
                typedef Private08::_mm256op<uint8_t, std::greater_equal> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm256op<uint8_t, std::greater> :
                    private Private08::_mm256op<uint8_t, std::greater> {
                typedef Private08::_mm256op<uint8_t, std::greater> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm256op<uint8_t, std::less_equal> :
                    private Private08::_mm256op<uint8_t, std::less_equal> {
                typedef Private08::_mm256op<uint8_t, std::less_equal> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm256op<uint8_t, std::less> :
                    private Private08::_mm256op<uint8_t, std::less> {
                typedef Private08::_mm256op<uint8_t, std::less> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm256op<uint8_t, std::equal_to> :
                    private Private08::_mm256op<uint8_t, std::equal_to> {
                typedef Private08::_mm256op<uint8_t, std::equal_to> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm256op<uint8_t, std::not_equal_to> :
                    private Private08::_mm256op<uint8_t, std::not_equal_to> {
                typedef Private08::_mm256op<uint8_t, std::not_equal_to> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm256op<uint8_t, coding_benchmark::and_is> :
                    private Private08::_mm256op<uint8_t, coding_benchmark::and_is> {
                typedef Private08::_mm256op<uint8_t, coding_benchmark::and_is> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm256op<uint8_t, coding_benchmark::or_is> :
                    private Private08::_mm256op<uint8_t, coding_benchmark::or_is> {
                typedef Private08::_mm256op<uint8_t, coding_benchmark::or_is> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm256op<uint8_t, coding_benchmark::xor_is> :
                    private Private08::_mm256op<uint8_t, coding_benchmark::xor_is> {
                typedef Private08::_mm256op<uint8_t, coding_benchmark::xor_is> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm256op<uint8_t, coding_benchmark::is_not> :
                    private Private08::_mm256op<uint8_t, coding_benchmark::is_not> {
                typedef Private08::_mm256op<uint8_t, coding_benchmark::is_not> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm256op<uint8_t, coding_benchmark::add> :
                    private Private08::_mm256op<uint8_t, coding_benchmark::add> {
                typedef Private08::_mm256op<uint8_t, coding_benchmark::add> BASE;
                using BASE::add;
                using BASE::compute;
            };

            template<>
            struct mm256op<uint8_t, coding_benchmark::sub> :
                    private Private08::_mm256op<uint8_t, coding_benchmark::sub> {
                typedef Private08::_mm256op<uint8_t, coding_benchmark::sub> BASE;
                using BASE::sub;
                using BASE::compute;
            };

            template<>
            struct mm256op<uint8_t, coding_benchmark::mul> :
                    private Private08::_mm256op<uint8_t, coding_benchmark::mul> {
                typedef Private08::_mm256op<uint8_t, coding_benchmark::mul> BASE;
                using BASE::mullo;
                using BASE::compute;
            };

            template<>
            struct mm256op<uint8_t, coding_benchmark::div> :
                    private Private08::_mm256op<uint8_t, coding_benchmark::div> {
                typedef Private08::_mm256op<uint8_t, coding_benchmark::div> BASE;
                using BASE::div;
                using BASE::compute;
            };

        }
    }
}
