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
 * AVX512_uint32.tcc
 *
 *  Created on: 20.11.2017
 *      Author: Till Kolditz - Till.Kolditz@gmail.com
 */

#pragma once

#ifndef LIB_COLUMN_OPERATORS_SIMD_AVX512_HPP_
#error "This file must only be included by AVX512.hpp !"
#endif

namespace coding_benchmark {
    namespace simd {
        namespace avx512 {

            namespace Private32 {

                template<size_t current = 0>
                inline void pack_right2_uint32(
                        uint32_t * & result,
                        __m512i & a,
                        uint16_t mask) {
                    *result = reinterpret_cast<uint32_t*>(&a)[current];
                    result += (mask >> current) & 0x1;
                    pack_right2_uint32<current + 1>(result, a, mask);
                }

                template<>
                inline void pack_right2_uint32<15>(
                        uint32_t * & result,
                        __m512i & a,
                        uint16_t mask) {
                    *result = reinterpret_cast<uint32_t*>(&a)[15];
                    result += (mask >> 7) & 0x1;
                }

                template<typename T>
                struct _mm512 {

                    typedef uint16_t mask_t;
                    typedef __m128i popcnt_t;

                    static const constexpr mask_t FULL_MASK = 0xFFFFu;

                    static inline __m512i set1(
                            T value) {
                        return _mm512_set1_epi32(value);
                    }

                    static inline __m512i set(
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
                        return _mm512_set_epi32(v15, v14, v13, v12, v11, v10, v9, v8, v7, v6, v5, v4, v3, v2, v1, v0);
                    }

                    static inline __m512i set_inc(
                            T v0) {
                        return _mm512_set_epi32(v0 + 15, v0 + 14, v0 + 13, v0 + 12, v0 + 11, v0 + 10, v0 + 9, v0 + 8, v0 + 7, v0 + 6, v0 + 5, v0 + 4, v0 + 3, v0 + 2, v0 + 1, v0);
                    }

                    static inline __m512i set_inc(
                            T v0,
                            T inc) {
                        return _mm512_set_epi32(v0 + 15 * inc, v0 + 14 * inc, v0 + 13 * inc, v0 + 12 * inc, v0 + 11 * inc, v0 + 10 * inc, v0 + 9 * inc, v0 + 8 * inc, v0 + 7 * inc, v0 + 6 * inc,
                                v0 + 5 * inc, v0 + 4 * inc, v0 + 3 * inc, v0 + 2 * inc, v0 + inc, v0);
                    }

                    static inline __m512i min(
                            __m512i a,
                            __m512i b) {
                        if constexpr (std::is_signed_v<T>) {
                            return _mm512_min_epi32(a, b);
                        } else {
                            return _mm512_min_epu32(a, b);
                        }
                    }

                    static inline __m512i max(
                            __m512i a,
                            __m512i b) {
                        if constexpr (std::is_signed_v<T>) {
                            return _mm512_max_epi32(a, b);
                        } else {
                            return _mm512_max_epu32(a, b);
                        }
                    }

                    static inline __m512i add(
                            __m512i a,
                            __m512i b) {
                        return _mm512_add_epi32(a, b);
                    }

                    static inline __m512i mullo(
                            __m512i a,
                            __m512i b) {
                        return _mm512_mullo_epi32(a, b);
                    }

                    static inline __m512i geq(
                            __m512i a,
                            __m512i b) {
#define MASK(idx) ((mask & idx) ? 0xFFFFFFFF : 0)
                        auto mask = geq_mask(a, b);
                        return set(MASK(0x8000), MASK(0x4000), MASK(0x2000), MASK(0x1000), MASK(0x0800), MASK(0x0400), MASK(0x0200), MASK(0x0100), MASK(0x0080), MASK(0x0040), MASK(0x0020),
                                MASK(0x0010), MASK(0x0008), MASK(0x0004), MASK(0x0002), MASK(0x0001));
#undef MASK
                    }

                    static inline uint16_t geq_mask(
                            __m512i a,
                            __m512i b) {
                        return _mm512_cmpge_epu32_mask(a, b);
                    }

                    static inline uint8_t sum(
                            __m512i a) {
                        throw std::runtime_error("__m512i sum uint16_t not supported yet");
                    }

                    static inline __m512i pack_right(
                            __m512i a,
                            mask_t mask) {
                        throw std::runtime_error("__m512i pack_right uint16_t not supported yet");
                    }

                    static inline void pack_right2(
                            uint32_t * & result,
                            __m512i a,
                            mask_t mask) {
                        pack_right2_uint32(result, a, mask);
                    }

                    static inline void pack_right3(
                            uint32_t * & result,
                            __m512i a,
                            mask_t mask) {
                        typedef typename mm<__m128i, uint32_t>::mask_t sse_mask_t;
                        auto subMask = static_cast<sse_mask_t>(mask) & 0xF;
                        _mm_storeu_si128(reinterpret_cast<__m128i *>(result), mm<__m128i, uint32_t>::pack_right(_mm512_extracti32x4_epi32(a, 0), subMask));
                        result += __builtin_popcount(subMask);
                        subMask = static_cast<sse_mask_t>(mask >> 4) & 0xF;
                        _mm_storeu_si128(reinterpret_cast<__m128i *>(result), mm<__m128i, uint32_t>::pack_right(_mm512_extracti32x4_epi32(a, 1), subMask));
                        result += __builtin_popcount(subMask);
                        subMask = static_cast<sse_mask_t>(mask >> 8) & 0xF;
                        _mm_storeu_si128(reinterpret_cast<__m128i *>(result), mm<__m128i, uint32_t>::pack_right(_mm512_extracti32x4_epi32(a, 2), subMask));
                        result += __builtin_popcount(subMask);
                        subMask = static_cast<sse_mask_t>(mask >> 12) & 0xF;
                        _mm_storeu_si128(reinterpret_cast<__m128i *>(result), mm<__m128i, uint32_t>::pack_right(_mm512_extracti32x4_epi32(a, 3), subMask));
                        result += __builtin_popcount(subMask);
                    }

                    static inline popcnt_t popcount(
                            // TODO
                            __m512i a) {
#ifdef __AVX512BW__
                        auto pattern1 = _mm512_set1_epi16(0x5555);
                        auto pattern2 = _mm512_set1_epi16(0x3333);
                        auto pattern3 = _mm512_set1_epi16(0x0F0F);
                        auto pattern4 = _mm512_set1_epi16(0x0101);
                        auto shuffle = _mm256_set_epi64x(0xFFFFFFFFFFFFFFFF, 0x0F0D0B0907050301, 0xFFFFFFFFFFFFFFFF, 0x0F0D0B0907050301);
                        auto temp = _mm256_sub_epi16(a, _mm256_and_si256(_mm256_srli_epi16(a, 1), pattern1));
                        temp = _mm256_add_epi16(_mm256_and_si256(temp, pattern2), _mm256_and_si256(_mm256_srli_epi16(temp, 2), pattern2));
                        temp = _mm256_and_si256(_mm256_add_epi16(temp, _mm256_srli_epi16(temp, 4)), pattern3);
                        temp = _mm256_mullo_epi16(temp, pattern4);
                        temp = _mm256_shuffle_epi8(temp, shuffle);
                        return _mm_set_epi64x(_mm256_extract_epi64(temp, 2), _mm256_extract_epi64(temp, 0));
#else
                        return 0;
#endif
                    }

                    static inline popcnt_t popcount2(
                            // TODO
                            __m512i a) {
#ifdef __AVX512BW__
                        auto mask = _mm256_set1_epi16(0x0101);
                        auto shuffle = _mm256_set_epi64x(0xFFFFFFFFFFFFFFFF, 0x0F0D0B0907050301, 0xFFFFFFFFFFFFFFFF, 0x0F0D0B0907050301);
                        auto popcount8 = mm256 < uint8_t > ::popcount2(a);
                        auto temp = _mm256_shuffle_epi8(_mm256_mullo_epi16(popcount8, mask), shuffle);
                        return _mm_set_epi64x(_mm256_extract_epi64(temp, 2), _mm256_extract_epi64(temp, 0));
#else
                        return 0;
#endif
                    }

                    static inline popcnt_t popcount3(
                            // TODO
                            __m512i a) {
#ifdef __AVX512BW__
                        return _mm_set_epi8(_mm_popcnt_u32(_mm256_extract_epi16(a, 15)), _mm_popcnt_u32(_mm256_extract_epi16(a, 14)), _mm_popcnt_u32(_mm256_extract_epi16(a, 13)),
                                _mm_popcnt_u32(_mm256_extract_epi16(a, 12)), _mm_popcnt_u32(_mm256_extract_epi16(a, 11)), _mm_popcnt_u32(_mm256_extract_epi16(a, 10)),
                                _mm_popcnt_u32(_mm256_extract_epi16(a, 9)), _mm_popcnt_u32(_mm256_extract_epi16(a, 8)), _mm_popcnt_u32(_mm256_extract_epi16(a, 7)), _mm_popcnt_u32(_mm256_extract_epi16(a, 6)),
                                _mm_popcnt_u32(_mm256_extract_epi16(a, 5)), _mm_popcnt_u32(_mm256_extract_epi16(a, 4)), _mm_popcnt_u32(_mm256_extract_epi16(a, 3)), _mm_popcnt_u32(_mm256_extract_epi16(a, 2)),
                                _mm_popcnt_u32(_mm256_extract_epi16(a, 1)), _mm_popcnt_u32(_mm256_extract_epi16(a, 0)));
#else
                        return 0;
#endif
                    }
                };

            } /* Private32 */

            template<>
            struct mm512<int32_t> :
                    public Private32::_mm512<int32_t> {
                typedef Private32::_mm512<int32_t> BASE;
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
                using BASE::pack_right3;
                using BASE::popcount;
                using BASE::popcount2;
                using BASE::popcount3;
                // TODO using BASE::cvt_larger_lo;
                // TODO using BASE::cvt_larger_hi;
            };

            template<>
            struct mm512<uint32_t> :
                    public Private32::_mm512<uint32_t> {
                typedef Private32::_mm512<uint32_t> BASE;
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
                // TODO using BASE::cvt_larger_lo;
                // TODO using BASE::cvt_larger_hi;
            };

        }
    }
}
