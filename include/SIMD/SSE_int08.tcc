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

            namespace Private08 {

                template<typename T, size_t current = 1>
                struct min_max_helper {
                    static inline void min(
                            T & result,
                            __m128i a) {
                        T x = static_cast<T>(_mm_extract_epi8(a, current));
                        if (x < result) {
                            result = x;
                        }
                        min_max_helper<T, current + 1>::min(result, a);
                    }

                    static inline void max(
                            T & result,
                            __m128i a) {
                        T x = static_cast<T>(_mm_extract_epi8(a, current));
                        if (x > result) {
                            result = x;
                        }
                        min_max_helper<T, current + 1>::max(result, a);
                    }
                };

                template<typename T>
                struct min_max_helper<T, 15> {
                    static inline void min(
                            T & result,
                            __m128i a) {
                        T x = static_cast<T>(_mm_extract_epi8(a, 15));
                        if (x < result) {
                            result = x;
                        }
                    }

                    static inline void max(
                            T & result,
                            __m128i a) {
                        T x = static_cast<T>(_mm_extract_epi8(a, 15));
                        if (x > result) {
                            result = x;
                        }
                    }
                };

                template<typename T>
                inline T get_min_int8(
                        __m128i & a) {
                    T result = static_cast<T>(_mm_extract_epi8(a, 0));
                    min_max_helper<T>::min(result, a);
                    return result;
                }

                template<>
                inline int8_t get_min_int8(
                        __m128i & a) {
                    auto x1 = _mm_cvtepi8_epi16(a);
                    auto x2 = _mm_cvtepi8_epi16(_mm_srli_si128(a, 8));
                    auto mmMin = _mm_set1_epi16(std::numeric_limits < int16_t > ::min());
                    auto min1 = _mm_extract_epi16(_mm_minpos_epu16(_mm_add_epi16(x1, mmMin)), 0);
                    auto min2 = _mm_extract_epi16(_mm_minpos_epu16(_mm_add_epi16(x2, mmMin)), 0);
                    if (min1 < min2) {
                        return static_cast<int8_t>(min1 - std::numeric_limits < int16_t > ::min());
                    } else {
                        return static_cast<int8_t>(min2 - std::numeric_limits < int16_t > ::min());
                    }
                }

                template<>
                inline uint8_t get_min_int8(
                        __m128i & a) {
                    auto x1 = _mm_cvtepi8_epi16(a);
                    auto x2 = _mm_cvtepi8_epi16(_mm_srli_si128(a, 8));
                    auto min1 = _mm_extract_epi16(_mm_minpos_epu16(x1), 0);
                    auto min2 = _mm_extract_epi16(_mm_minpos_epu16(x2), 0);
                    if (min1 < min2) {
                        return static_cast<uint8_t>(min1);
                    } else {
                        return static_cast<uint8_t>(min2);
                    }
                }

                template<typename T>
                inline T get_max_int8(
                        __m128i & a) {
                    T result = static_cast<T>(_mm_extract_epi8(a, 0));
                    min_max_helper<T>::max(result, a);
                    return result;
                }

                template<>
                inline int8_t get_max_int8(
                        __m128i & a) {
                    auto a2 = _mm_sub_epi8(_mm_setzero_si128(), a);
                    auto x1 = _mm_cvtepi8_epi16(a2);
                    auto x2 = _mm_cvtepi8_epi16(_mm_srli_si128(a2, 8));
                    auto mmMin = _mm_set1_epi16(std::numeric_limits < int16_t > ::min());
                    auto min1 = _mm_extract_epi16(_mm_minpos_epu16(_mm_add_epi16(x1, mmMin)), 0);
                    auto min2 = _mm_extract_epi16(_mm_minpos_epu16(_mm_add_epi16(x2, mmMin)), 0);
                    if (min1 < min2) {
                        return static_cast<int8_t>(-(min1 + std::numeric_limits < int16_t > ::min()));
                    } else {
                        return static_cast<int8_t>(-(min2 + std::numeric_limits < int16_t > ::min()));
                    }
                }

                template<>
                inline uint8_t get_max_int8(
                        __m128i & a) {
                    auto a2 = _mm_sub_epi8(_mm_set1_epi8(std::numeric_limits < uint8_t > ::max()), a);
                    auto x1 = _mm_cvtepi8_epi16(a2);
                    auto x2 = _mm_cvtepi8_epi16(_mm_srli_si128(a2, 8));
                    auto min1 = _mm_extract_epi16(_mm_minpos_epu16(x1), 0);
                    auto min2 = _mm_extract_epi16(_mm_minpos_epu16(x2), 0);
                    if (min1 < min2) {
                        return static_cast<uint8_t>(std::numeric_limits < uint8_t > ::max() - min1);
                    } else {
                        return static_cast<uint8_t>(std::numeric_limits < uint8_t > ::max() - min2);
                    }
                }

                template<size_t current = 0>
                inline void pack_right2_int8(
                        int8_t * & result,
                        __m128i & a,
                        uint16_t mask) {
                    *result = reinterpret_cast<int8_t*>(&a)[current];
                    result += (mask >> current) & 0x1;
                    pack_right2_int8<current + 1>(result, a, mask);
                }

                template<>
                inline void pack_right2_int8<15>(
                        int8_t * & result,
                        __m128i & a,
                        uint16_t mask) {
                    *result = reinterpret_cast<int8_t*>(&a)[15];
                    result += (mask >> 15) & 0x1;
                }

                template<size_t current = 0>
                inline void pack_right2_int8(
                        uint8_t * & result,
                        __m128i & a,
                        uint16_t mask) {
                    *result = reinterpret_cast<uint8_t*>(&a)[current];
                    result += (mask >> current) & 0x1;
                    pack_right2_int8<current + 1>(result, a, mask);
                }

                template<>
                inline void pack_right2_int8<15>(
                        uint8_t * & result,
                        __m128i & a,
                        uint16_t mask) {
                    *result = reinterpret_cast<uint8_t*>(&a)[15];
                    result += (mask >> 15) & 0x1;
                }

                template<typename T>
                struct _mm128 {

                    typedef uint16_t mask_t;
                    typedef __m128i popcnt_t;

                    static const constexpr mask_t FULL_MASK = 0xFFFFu;

                    static inline __m128i set1(
                            T value) {
                        return _mm_set1_epi8(value);
                    }

                    static inline __m128i set(
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
                        return _mm_set_epi8(v15, v14, v13, v12, v11, v10, v9, v8, v7, v6, v5, v4, v3, v2, v1, v0);
                    }

                    static inline __m128i set_inc(
                            T v0) {
                        return _mm_set_epi8(v0 + 15, v0 + 14, v0 + 13, v0 + 12, v0 + 11, v0 + 10, v0 + 9, v0 + 8, v0 + 7, v0 + 6, v0 + 5, v0 + 4, v0 + 3, v0 + 2, v0 + 1, v0);
                    }

                    static inline __m128i set_inc(
                            T v0,
                            T inc) {
                        return _mm_set_epi8(v0 + 15 * inc, v0 + 14 * inc, v0 + 13 * inc, v0 + 12 * inc, v0 + 11 * inc, v0 + 10 * inc, v0 + 9 * inc, v0 + 8 * inc, v0 + 7 * inc, v0 + 6 * inc,
                                v0 + 5 * inc, v0 + 4 * inc, v0 + 3 * inc, v0 + 2 * inc, v0 + inc, v0);
                    }

                    template<int I>
                    static inline T extract(
                            __m128i a) {
                        return _mm_extract_epi8(a, I);
                    }

                    static inline __m128i min(
                            __m128i a) {
                        return get_min_int8<T>(a);
                    }

                    static inline __m128i min(
                            __m128i a,
                            __m128i b) {
                        if constexpr (std::is_signed_v<T>) {
                            return _mm_min_epi8(a, b);
                        } else {
                            return _mm_min_epu8(a, b);
                        }
                    }

                    static inline __m128i max(
                            __m128i a) {
                        return get_max_int8<T>(a);
                    }

                    static inline __m128i max(
                            __m128i a,
                            __m128i b) {
                        if constexpr (std::is_signed_v<T>) {
                            return _mm_max_epi8(a, b);
                        } else {
                            return _mm_max_epu8(a, b);
                        }
                    }

                    static inline T sum(
                            __m128i a) {
                        auto mm = _mm_add_epi8(a, _mm_srli_si128(a, 8));
                        mm = _mm_add_epi8(mm, _mm_srli_si128(mm, 4));
                        mm = _mm_add_epi8(mm, _mm_srli_si128(mm, 2));
                        return static_cast<uint16_t>(_mm_extract_epi8(mm, 0));
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
                            T * & result,
                            __m128i a,
                            mask_t mask) {
                        pack_right2_int8(result, a, mask);
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
                        if constexpr (std::is_signed_v<T>) {
                            return _mm_cvtepi8_epi16(a);
                        } else {
                            return _mm_cvtepu8_epi16(a);
                        }
                    }

                    static inline __m128i cvt_larger_hi(
                            __m128i a) {
                        if constexpr (std::is_signed_v<T>) {
                            return _mm_cvtepi8_epi16(_mm_srli_si128(a, 8));
                        } else {
                            return _mm_cvtepu8_epi16(_mm_srli_si128(a, 8));
                        }
                    }

                private:
                    static const uint64_t * const SHUFFLE_TABLE_L;
                    static const uint64_t * const SHUFFLE_TABLE_H;
                };

                template<typename T, template<typename > class Op>
                struct _mm128op;

                template<typename T>
                struct _mm128op<T, std::greater> {

                    typedef typename _mm128<T>::mask_t mask_t;

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

                template<typename T>
                struct _mm128op<T, std::greater_equal> {

                    typedef typename _mm128<T>::mask_t mask_t;

                    static inline __m128i cmp(
                            __m128i a,
                            __m128i b) {
                        auto mm = mm128 < T > ::max(a, b);
                        return _mm_cmpeq_epi8(a, mm);
                    }

                    static inline mask_t cmp_mask(
                            __m128i a,
                            __m128i b) {
                        return static_cast<mask_t>(_mm_movemask_epi8(cmp(a, b)));
                    }
                };

                template<typename T>
                struct _mm128op<T, std::less> {

                    typedef typename _mm128<T>::mask_t mask_t;

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

                template<typename T>
                struct _mm128op<T, std::less_equal> {

                    typedef typename _mm128<T>::mask_t mask_t;

                    static inline __m128i cmp(
                            __m128i a,
                            __m128i b) {
                        auto mm = sse::mm128 < T > ::min(a, b);
                        return _mm_cmpeq_epi8(a, mm);
                    }

                    static inline mask_t cmp_mask(
                            __m128i a,
                            __m128i b) {
                        return static_cast<mask_t>(_mm_movemask_epi8(cmp(a, b)));
                    }
                };

                template<typename T>
                struct _mm128op<T, std::equal_to> {

                    typedef typename _mm128<T>::mask_t mask_t;

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

                template<typename T>
                struct _mm128op<T, std::not_equal_to> {

                    typedef typename _mm128<T>::mask_t mask_t;

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

                template<typename T>
                struct _mm128op<T, coding_benchmark::and_is> {

                    typedef typename _mm128<T>::mask_t mask_t;

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

                template<typename T>
                struct _mm128op<T, coding_benchmark::or_is> {

                    typedef typename _mm128<T>::mask_t mask_t;

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

                template<typename T>
                struct _mm128op<T, coding_benchmark::xor_is> {

                    typedef typename _mm128<T>::mask_t mask_t;

                    static inline __m128i cmp(
                            __m128i a,
                            __m128i b) {
                        return _mm_xor_si128(a, b);
                    }

                    static inline mask_t cmp_mask(
                            __m128i a,
                            __m128i b) {
                        return static_cast<mask_t>(_mm_movemask_epi8(cmp(a, b)));
                    }
                };

                template<typename T>
                struct _mm128op<T, coding_benchmark::is_not> {

                    typedef typename _mm128<T>::mask_t mask_t;

                    static inline __m128i cmp(
                            __m128i a) {
                        return _mm_andnot_si128(a, _mm_set1_epi64x(0xFFFFFFFFFFFFFFFFull));
                    }

                    static inline mask_t cmp_mask(
                            __m128i a) {
                        return static_cast<mask_t>(_mm_movemask_epi8(cmp(a)));
                    }
                };

                template<typename T>
                struct _mm128op<T, coding_benchmark::add> {

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

                template<typename T>
                struct _mm128op<T, coding_benchmark::sub> {

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

                template<typename T>
                struct _mm128op<T, coding_benchmark::mul> {

                    static inline __m128i compute(
                            __m128i a,
                            __m128i b) {
                        return mullo(a, b);
                    }

                    static inline __m128i mullo(
                            __m128i a,
                            __m128i b) {
                        auto mm1 = _mm_shuffle_epi8(_mm_mullo_epi16(_mm_cvtepi8_epi16(a), _mm_cvtepi8_epi16(b)), _mm_set_epi64x(0xFFFFFFFFFFFFFFFFull, 0x0D0C090805040100ull));
                        auto mm2 = _mm_shuffle_epi8(_mm_mullo_epi16(_mm_cvtepi8_epi16(_mm_srli_si128(a, 8)), _mm_cvtepi8_epi16(_mm_srli_si128(b, 8))),
                                _mm_set_epi64x(0x0D0C090805040100ull, 0xFFFFFFFFFFFFFFFFull));
                        return _mm_and_si128(mm1, mm2);
                    }
                };

                template<typename T>
                struct _mm128op<T, coding_benchmark::div> {

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

            } /* Private08 */

            template<>
            struct mm128<int8_t> :
                    public Private08::_mm128<int8_t> {
                typedef Private08::_mm128<int8_t> BASE;
                using BASE::mask_t;
                using BASE::popcnt_t;
                using BASE::FULL_MASK;
                using BASE::set1;
                using BASE::set;
                using BASE::set_inc;
                using BASE::extract;
                using BASE::min;
                using BASE::max;
                using BASE::sum;
                using BASE::pack_right;
                using BASE::pack_right2;
                using BASE::popcount;
                using BASE::popcount2;
                using BASE::popcount3;
                using BASE::cvt_larger_hi;
                using BASE::cvt_larger_lo;
            };

            template<>
            struct mm128op<int8_t, std::greater_equal> :
                    private Private08::_mm128op<int8_t, std::greater_equal> {
                typedef Private08::_mm128op<int8_t, std::greater_equal> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm128op<int8_t, std::greater> :
                    private Private08::_mm128op<int8_t, std::greater> {
                typedef Private08::_mm128op<int8_t, std::greater> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm128op<int8_t, std::less_equal> :
                    private Private08::_mm128op<int8_t, std::less_equal> {
                typedef Private08::_mm128op<int8_t, std::less_equal> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm128op<int8_t, std::less> :
                    private Private08::_mm128op<int8_t, std::less> {
                typedef Private08::_mm128op<int8_t, std::less> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm128op<int8_t, std::equal_to> :
                    private Private08::_mm128op<int8_t, std::equal_to> {
                typedef Private08::_mm128op<int8_t, std::equal_to> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm128op<int8_t, std::not_equal_to> :
                    private Private08::_mm128op<int8_t, std::not_equal_to> {
                typedef Private08::_mm128op<int8_t, std::not_equal_to> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm128op<int8_t, coding_benchmark::and_is> :
                    private Private08::_mm128op<int8_t, coding_benchmark::and_is> {
                typedef Private08::_mm128op<int8_t, coding_benchmark::and_is> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm128op<int8_t, coding_benchmark::or_is> :
                    private Private08::_mm128op<int8_t, coding_benchmark::or_is> {
                typedef Private08::_mm128op<int8_t, coding_benchmark::or_is> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm128op<int8_t, coding_benchmark::xor_is> :
                    private Private08::_mm128op<int8_t, coding_benchmark::xor_is> {
                typedef Private08::_mm128op<int8_t, coding_benchmark::xor_is> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm128op<int8_t, coding_benchmark::is_not> :
                    private Private08::_mm128op<int8_t, coding_benchmark::is_not> {
                typedef Private08::_mm128op<int8_t, coding_benchmark::is_not> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm128op<int8_t, coding_benchmark::add> :
                    private Private08::_mm128op<int8_t, coding_benchmark::add> {
                typedef Private08::_mm128op<int8_t, coding_benchmark::add> BASE;
                using BASE::add;
                using BASE::compute;
            };

            template<>
            struct mm128op<int8_t, coding_benchmark::sub> :
                    private Private08::_mm128op<int8_t, coding_benchmark::sub> {
                typedef Private08::_mm128op<int8_t, coding_benchmark::sub> BASE;
                using BASE::sub;
                using BASE::compute;
            };

            template<>
            struct mm128op<int8_t, coding_benchmark::mul> :
                    private Private08::_mm128op<int8_t, coding_benchmark::mul> {
                typedef Private08::_mm128op<int8_t, coding_benchmark::mul> BASE;
                using BASE::mullo;
                using BASE::compute;
            };

            template<>
            struct mm128op<int8_t, coding_benchmark::div> :
                    private Private08::_mm128op<int8_t, coding_benchmark::div> {
                typedef Private08::_mm128op<int8_t, coding_benchmark::div> BASE;
                using BASE::div;
                using BASE::compute;
            };

            template<>
            struct mm128<uint8_t> :
                    public Private08::_mm128<uint8_t> {
                typedef Private08::_mm128<uint8_t> BASE;
                using BASE::mask_t;
                using BASE::popcnt_t;
                using BASE::FULL_MASK;
                using BASE::set1;
                using BASE::set;
                using BASE::set_inc;
                using BASE::extract;
                using BASE::min;
                using BASE::max;
                using BASE::sum;
                using BASE::pack_right;
                using BASE::pack_right2;
                using BASE::popcount;
                using BASE::popcount2;
                using BASE::popcount3;
                using BASE::cvt_larger_hi;
                using BASE::cvt_larger_lo;
            };

            template<>
            struct mm128op<uint8_t, std::greater_equal> :
                    private Private08::_mm128op<uint8_t, std::greater_equal> {
                typedef Private08::_mm128op<uint8_t, std::greater_equal> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm128op<uint8_t, std::greater> :
                    private Private08::_mm128op<uint8_t, std::greater> {
                typedef Private08::_mm128op<uint8_t, std::greater> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm128op<uint8_t, std::less_equal> :
                    private Private08::_mm128op<uint8_t, std::less_equal> {
                typedef Private08::_mm128op<uint8_t, std::less_equal> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm128op<uint8_t, std::less> :
                    private Private08::_mm128op<uint8_t, std::less> {
                typedef Private08::_mm128op<uint8_t, std::less> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm128op<uint8_t, std::equal_to> :
                    private Private08::_mm128op<uint8_t, std::equal_to> {
                typedef Private08::_mm128op<uint8_t, std::equal_to> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm128op<uint8_t, std::not_equal_to> :
                    private Private08::_mm128op<uint8_t, std::not_equal_to> {
                typedef Private08::_mm128op<uint8_t, std::not_equal_to> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm128op<uint8_t, coding_benchmark::and_is> :
                    private Private08::_mm128op<uint8_t, coding_benchmark::and_is> {
                typedef Private08::_mm128op<uint8_t, coding_benchmark::and_is> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm128op<uint8_t, coding_benchmark::or_is> :
                    private Private08::_mm128op<uint8_t, coding_benchmark::or_is> {
                typedef Private08::_mm128op<uint8_t, coding_benchmark::or_is> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm128op<uint8_t, coding_benchmark::xor_is> :
                    private Private08::_mm128op<uint8_t, coding_benchmark::xor_is> {
                typedef Private08::_mm128op<uint8_t, coding_benchmark::xor_is> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm128op<uint8_t, coding_benchmark::is_not> :
                    private Private08::_mm128op<uint8_t, coding_benchmark::is_not> {
                typedef Private08::_mm128op<uint8_t, coding_benchmark::is_not> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm128op<uint8_t, coding_benchmark::add> :
                    private Private08::_mm128op<uint8_t, coding_benchmark::add> {
                typedef Private08::_mm128op<uint8_t, coding_benchmark::add> BASE;
                using BASE::add;
                using BASE::compute;
            };

            template<>
            struct mm128op<uint8_t, coding_benchmark::sub> :
                    private Private08::_mm128op<uint8_t, coding_benchmark::sub> {
                typedef Private08::_mm128op<uint8_t, coding_benchmark::sub> BASE;
                using BASE::sub;
                using BASE::compute;
            };

            template<>
            struct mm128op<uint8_t, coding_benchmark::mul> :
                    private Private08::_mm128op<uint8_t, coding_benchmark::mul> {
                typedef Private08::_mm128op<uint8_t, coding_benchmark::mul> BASE;
                using BASE::mullo;
                using BASE::compute;
            };

            template<>
            struct mm128op<uint8_t, coding_benchmark::div> :
                    private Private08::_mm128op<uint8_t, coding_benchmark::div> {
                typedef Private08::_mm128op<uint8_t, coding_benchmark::div> BASE;
                using BASE::div;
                using BASE::compute;
            };

        }
    }
}
