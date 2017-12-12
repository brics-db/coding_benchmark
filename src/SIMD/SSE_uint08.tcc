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
 * SSE_uint8.tcc
 *
 *  Created on: 25.09.2017
 *      Author: Till Kolditz - Till.Kolditz@gmail.com
 */

#pragma once

#ifndef LIB_COLUMN_OPERATORS_SIMD_SSE_HPP_
#error "This file must only be included by SSE.hpp !"
#endif

namespace coding_benchmark {
    namespace simd {
        namespace sse {

            namespace Private {

                template<size_t current = 0>
                inline void pack_right2_uint8(
                        uint8_t * & result,
                        __m128i & a,
                        uint16_t mask) {
                    *result = reinterpret_cast<uint8_t*>(&a)[current];
                    result += (mask >> current) & 0x1;
                    pack_right2_uint8<current + 1>(result, a, mask);
                }

                template<>
                inline void pack_right2_uint8<15>(
                        uint8_t * & result,
                        __m128i & a,
                        uint16_t mask) {
                    *result = reinterpret_cast<uint8_t*>(&a)[15];
                    result += (mask >> 15) & 0x1;
                }

            }

            template<>
            struct mm128<uint8_t> {

                typedef uint16_t mask_t;
                typedef __m128i popcnt_t;

                static inline __m128i set1(
                        uint8_t value) {
                    return _mm_set1_epi8(value);
                }

                static inline __m128i set(
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
                    return _mm_set_epi8(v15, v14, v13, v12, v11, v10, v9, v8, v7, v6, v5, v4, v3, v2, v1, v0);
                }

                static inline __m128i set_inc(
                        uint8_t v0) {
                    return _mm_set_epi8(v0 + 15, v0 + 14, v0 + 13, v0 + 12, v0 + 11, v0 + 10, v0 + 9, v0 + 8, v0 + 7, v0 + 6, v0 + 5, v0 + 4, v0 + 3, v0 + 2, v0 + 1, v0);
                }

                static inline __m128i set_inc(
                        uint8_t v0,
                        uint8_t inc) {
                    return _mm_set_epi8(v0 + 15 * inc, v0 + 14 * inc, v0 + 13 * inc, v0 + 12 * inc, v0 + 11 * inc, v0 + 10 * inc, v0 + 9 * inc, v0 + 8 * inc, v0 + 7 * inc, v0 + 6 * inc, v0 + 5 * inc,
                            v0 + 4 * inc, v0 + 3 * inc, v0 + 2 * inc, v0 + inc, v0);
                }

                static inline __m128i min(
                        __m128i a,
                        __m128i b) {
                    return _mm_min_epu8(a, b);
                }

                static inline __m128i max(
                        __m128i a,
                        __m128i b) {
                    return _mm_max_epu8(a, b);
                }

                static inline __m128i add(
                        __m128i a,
                        __m128i b) {
                    return _mm_add_epi8(a, b);
                }

                static inline uint8_t sum(
                        __m128i a) {
                    auto mm = _mm_add_epi8(a, _mm_srli_si128(a, 8));
                    mm = _mm_add_epi8(mm, _mm_srli_si128(mm, 4));
                    mm = _mm_add_epi8(mm, _mm_srli_si128(mm, 2));
                    return static_cast<uint16_t>(_mm_extract_epi8(mm, 0));
                }

                static inline __m128i mullo(
                        __m128i a,
                        __m128i b) {
                    auto mm1 = _mm_shuffle_epi8(_mm_mullo_epi16(_mm_cvtepi8_epi16(a), _mm_cvtepi8_epi16(b)), _mm_set_epi64x(0xFFFFFFFFFFFFFFFFull, 0x0D0C090805040100ull));
                    auto mm2 = _mm_shuffle_epi8(_mm_mullo_epi16(_mm_cvtepi8_epi16(_mm_srli_si128(a, 8)), _mm_cvtepi8_epi16(_mm_srli_si128(b, 8))),
                            _mm_set_epi64x(0x0D0C090805040100ull, 0xFFFFFFFFFFFFFFFFull));
                    return _mm_and_si128(mm1, mm2);
                }

                static inline __m128i pack_right(
                        __m128i a,
                        mask_t mask) {
                    static const uint64_t ALL_ONES = 0xFFFFFFFFFFFFFFFFull;
                    uint64_t shuffleMaskL = SHUFFLE_TABLE_L[static_cast<uint8_t>(mask)];
                    int clzb = __builtin_clzll(~shuffleMaskL); // number of unmatched bytes (if a value matches, the leading bits are zero and the inversion makes it ones, so only full bytes are counted)
                    uint64_t shuffleMaskH = SHUFFLE_TABLE_H[static_cast<uint8_t>(mask >> 8)];
                    return _mm_shuffle_epi8(a, _mm_set_epi64x(((shuffleMaskH >> clzb) | (ALL_ONES << (64 - clzb))), shuffleMaskL & ((shuffleMaskH << (64 - clzb)) | (ALL_ONES >> clzb))));
                }

                static inline void pack_right2(
                        uint8_t * & result,
                        __m128i a,
                        mask_t mask) {
                    Private::pack_right2_uint8(result, a, mask);
                }

                static inline popcnt_t popcount(
                        __m128i a) {
                    auto pattern1 = _mm_set1_epi8(0x55);
                    auto pattern2 = _mm_set1_epi8(0x33);
                    auto pattern3 = _mm_set1_epi8(0x0F);
                    auto temp = _mm_sub_epi8(a, _mm_and_si128(_mm_srli_epi16(a, 1), pattern1));
                    temp = _mm_add_epi8(_mm_and_si128(temp, pattern2), _mm_and_si128(_mm_srli_epi16(temp, 2), pattern2));
                    temp = _mm_add_epi8(_mm_and_si128(temp, pattern3), _mm_and_si128(_mm_srli_epi16(temp, 4), pattern3));
                    return temp;
                }

                static inline popcnt_t popcount2(
                        __m128i a) {
                    auto lookup = _mm_set_epi8(4, 3, 3, 2, 3, 2, 2, 1, 3, 2, 2, 1, 2, 1, 1, 0);
                    auto low_mask = _mm_set1_epi8(0x0f);
                    auto lo = _mm_and_si128(a, low_mask);
                    auto hi = _mm_and_si128(_mm_srli_epi16(a, 4), low_mask);
                    auto cnt_lo = _mm_shuffle_epi8(lookup, lo);
                    auto cnt_hi = _mm_shuffle_epi8(lookup, hi);
                    return _mm_add_epi8(cnt_lo, cnt_hi);
                }

                static inline popcnt_t popcount3(
                        __m128i a) {
                    return _mm_set_epi8(_mm_popcnt_u32(_mm_extract_epi8(a, 15)), _mm_popcnt_u32(_mm_extract_epi8(a, 14)), _mm_popcnt_u32(_mm_extract_epi8(a, 13)),
                            _mm_popcnt_u32(_mm_extract_epi8(a, 12)), _mm_popcnt_u32(_mm_extract_epi8(a, 11)), _mm_popcnt_u32(_mm_extract_epi8(a, 10)), _mm_popcnt_u32(_mm_extract_epi8(a, 9)),
                            _mm_popcnt_u32(_mm_extract_epi8(a, 8)), _mm_popcnt_u32(_mm_extract_epi8(a, 7)), _mm_popcnt_u32(_mm_extract_epi8(a, 6)), _mm_popcnt_u32(_mm_extract_epi8(a, 5)),
                            _mm_popcnt_u32(_mm_extract_epi8(a, 4)), _mm_popcnt_u32(_mm_extract_epi8(a, 3)), _mm_popcnt_u32(_mm_extract_epi8(a, 2)), _mm_popcnt_u32(_mm_extract_epi8(a, 1)),
                            _mm_popcnt_u32(_mm_extract_epi8(a, 0)));
                }

                static inline __m128i cvt_larger_lo(
                        __m128i a) {
                    return _mm_cvtepi8_epi16(a);
                }

                static inline __m128i cvt_larger_hi(
                        __m128i a) {
                    return _mm_cvtepi8_epi16(_mm_srli_si128(a, 8));
                }

            private:
                static const uint64_t * const SHUFFLE_TABLE_L;
                static const uint64_t * const SHUFFLE_TABLE_H;
            };

            template<>
            struct mm128<int8_t> :
                    public mm128<uint8_t> {

                typedef uint16_t mask_t;
                typedef __m128i popcnt_t;

                static inline __m128i set1(
                        int8_t value) {
                    return _mm_set1_epi8(value);
                }

                static inline __m128i set(
                        int8_t v15,
                        int8_t v14,
                        int8_t v13,
                        int8_t v12,
                        int8_t v11,
                        int8_t v10,
                        int8_t v9,
                        int8_t v8,
                        int8_t v7,
                        int8_t v6,
                        int8_t v5,
                        int8_t v4,
                        int8_t v3,
                        int8_t v2,
                        int8_t v1,
                        int8_t v0) {
                    return _mm_set_epi8(v15, v14, v13, v12, v11, v10, v9, v8, v7, v6, v5, v4, v3, v2, v1, v0);
                }

                static inline __m128i set_inc(
                        int8_t v0) {
                    return _mm_set_epi8(v0 + 15, v0 + 14, v0 + 13, v0 + 12, v0 + 11, v0 + 10, v0 + 9, v0 + 8, v0 + 7, v0 + 6, v0 + 5, v0 + 4, v0 + 3, v0 + 2, v0 + 1, v0);
                }

                static inline __m128i set_inc(
                        int8_t v0,
                        int8_t inc) {
                    return _mm_set_epi8(v0 + 15 * inc, v0 + 14 * inc, v0 + 13 * inc, v0 + 12 * inc, v0 + 11 * inc, v0 + 10 * inc, v0 + 9 * inc, v0 + 8 * inc, v0 + 7 * inc, v0 + 6 * inc, v0 + 5 * inc,
                            v0 + 4 * inc, v0 + 3 * inc, v0 + 2 * inc, v0 + inc, v0);
                }

                using mm128<uint8_t>::min;
                using mm128<uint8_t>::max;
                using mm128<uint8_t>::add;
                using mm128<uint8_t>::sum;
                using mm128<uint8_t>::mullo;
                using mm128<uint8_t>::pack_right;

                static inline void pack_right2(
                        int8_t * & result,
                        __m128i a,
                        mask_t mask) {
                    mm128 < uint8_t > ::pack_right2(reinterpret_cast<uint8_t*&>(result), a, mask);
                }

                using mm128<uint8_t>::popcount;
                using mm128<uint8_t>::popcount2;
                using mm128<uint8_t>::popcount3;
                using mm128<uint8_t>::cvt_larger_hi;
                using mm128<uint8_t>::cvt_larger_lo;

            };

        }

        template<>
        struct mm_op<__m128i, uint8_t, std::greater> {

            typedef uint16_t mask_t;

            static inline __m128i cmp(
                    __m128i a,
                    __m128i b) {
                return _mm_cmpgt_epi8(a, b);
            }

            static inline mask_t cmp_mask(
                    __m128i a,
                    __m128i b) {
                return static_cast<mask_t>(_mm_movemask_epi8(cmp(a, b)));
            }
        };

        template<>
        struct mm_op<__m128i, uint8_t, std::greater_equal> {

            typedef uint16_t mask_t;

            static inline __m128i cmp(
                    __m128i a,
                    __m128i b) {
                auto mm = sse::mm128<uint8_t>::max(a, b);
                return _mm_cmpeq_epi8(a, mm);
            }

            static inline mask_t cmp_mask(
                    __m128i a,
                    __m128i b) {
                return static_cast<mask_t>(_mm_movemask_epi8(cmp(a, b)));
            }
        };

        template<>
        struct mm_op<__m128i, uint8_t, std::less> {

            typedef uint16_t mask_t;

            static inline __m128i cmp(
                    __m128i a,
                    __m128i b) {
                return _mm_cmplt_epi8(a, b);
            }

            static inline mask_t cmp_mask(
                    __m128i a,
                    __m128i b) {
                return static_cast<mask_t>(_mm_movemask_epi8(cmp(a, b)));
            }
        };

        template<>
        struct mm_op<__m128i, uint8_t, std::less_equal> {

            typedef uint16_t mask_t;

            static inline __m128i cmp(
                    __m128i a,
                    __m128i b) {
                auto mm = sse::mm128<uint8_t>::min(a, b);
                return _mm_cmpeq_epi8(a, mm);
            }

            static inline mask_t cmp_mask(
                    __m128i a,
                    __m128i b) {
                return static_cast<mask_t>(_mm_movemask_epi8(cmp(a, b)));
            }
        };

        template<>
        struct mm_op<__m128i, uint8_t, std::equal_to> {

            typedef uint16_t mask_t;

            static inline __m128i cmp(
                    __m128i a,
                    __m128i b) {
                return _mm_cmpeq_epi8(a, b);
            }

            static inline mask_t cmp_mask(
                    __m128i a,
                    __m128i b) {
                return static_cast<mask_t>(_mm_movemask_epi8(cmp(a, b)));
            }
        };

        template<>
        struct mm_op<__m128i, uint8_t, std::not_equal_to> {

            typedef uint16_t mask_t;

            static inline __m128i cmp(
                    __m128i a,
                    __m128i b) {
                return _mm_or_si128(_mm_cmplt_epi8(a, b), _mm_cmpgt_epi8(a, b));
            }

            static inline mask_t cmp_mask(
                    __m128i a,
                    __m128i b) {
                return static_cast<mask_t>(_mm_movemask_epi8(cmp(a, b)));
            }
        };

        template<>
        struct mm_op<__m128i, uint8_t, coding_benchmark::and_is> {

            typedef uint16_t mask_t;

            static inline __m128i cmp(
                    __m128i a,
                    __m128i b) {
                return _mm_and_si128(a, b);
            }

            static inline mask_t cmp_mask(
                    __m128i a,
                    __m128i b) {
                return static_cast<mask_t>(_mm_movemask_epi8(cmp(a, b)));
            }
        };

        template<>
        struct mm_op<__m128i, uint8_t, coding_benchmark::or_is> {

            typedef uint16_t mask_t;

            static inline __m128i cmp(
                    __m128i a,
                    __m128i b) {
                return _mm_or_si128(a, b);
            }

            static inline mask_t cmp_mask(
                    __m128i a,
                    __m128i b) {
                return static_cast<mask_t>(_mm_movemask_epi8(cmp(a, b)));
            }
        };

        template<>
        struct mm_op<__m128i, uint8_t, coding_benchmark::add> {

            static inline __m128i compute(
                    __m128i a,
                    __m128i b) {
                return add(a, b);
            }

            static inline __m128i add(
                    __m128i a,
                    __m128i b) {
                return _mm_add_epi8(a, b);
            }
        };

        template<>
        struct mm_op<__m128i, uint8_t, coding_benchmark::sub> {

            static inline __m128i compute(
                    __m128i a,
                    __m128i b) {
                return sub(a, b);
            }

            static inline __m128i sub(
                    __m128i a,
                    __m128i b) {
                return _mm_sub_epi8(a, b);
            }
        };

        template<>
        struct mm_op<__m128i, uint8_t, coding_benchmark::mul> {

            static inline __m128i compute(
                    __m128i a,
                    __m128i b) {
                return mullo(a, b);
            }

            static inline __m128i mullo(
                    __m128i a,
                    __m128i b) {
                return sse::mm128<uint8_t>::mullo(a, b);
            }
        };

        template<>
        struct mm_op<__m128i, uint8_t, coding_benchmark::div> {

            static inline __m128i compute(
                    __m128i a,
                    __m128i b) {
                return div(a, b);
            }

            static inline __m128i div(
                    __m128i a,
                    __m128i b) {
                auto mm0 = _mm_div_ps(_mm_cvtepi32_ps(_mm_cvtepi8_epi32(a)), _mm_cvtepi32_ps(_mm_cvtepi8_epi32(b)));
                auto mm1 = _mm_div_ps(_mm_cvtepi32_ps(_mm_cvtepi8_epi32(_mm_srli_si128(a, 4))), _mm_cvtepi32_ps(_mm_cvtepi8_epi32(_mm_srli_si128(b, 4))));
                auto mm2 = _mm_div_ps(_mm_cvtepi32_ps(_mm_cvtepi8_epi32(_mm_srli_si128(a, 8))), _mm_cvtepi32_ps(_mm_cvtepi8_epi32(_mm_srli_si128(b, 8))));
                auto mm3 = _mm_div_ps(_mm_cvtepi32_ps(_mm_cvtepi8_epi32(_mm_srli_si128(a, 12))), _mm_cvtepi32_ps(_mm_cvtepi8_epi32(_mm_srli_si128(b, 12))));
                auto mx0 = _m_to_int(_mm_cvtps_pi8(mm0));
                auto mx1 = _m_to_int(_mm_cvtps_pi8(mm1));
                auto mx2 = _m_to_int(_mm_cvtps_pi8(mm2));
                auto mx3 = _m_to_int(_mm_cvtps_pi8(mm3));
                return _mm_set_epi32(mx3, mx2, mx1, mx0);
            }
        };

    }
}
