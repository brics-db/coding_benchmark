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
 * AVX512_uint64.tcc
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

            namespace Private64 {

                template<size_t current = 0>
                inline void pack_right2_int64(
                        uint64_t * & result,
                        __m512i & a,
                        uint8_t mask) {
                    *result = reinterpret_cast<uint64_t*>(&a)[current];
                    result += (mask >> current) & 0x1;
                    pack_right2_int64<current + 1>(result, a, mask);
                }

                template<>
                inline void pack_right2_int64<7>(
                        uint64_t * & result,
                        __m512i & a,
                        uint8_t mask) {
                    *result = reinterpret_cast<uint64_t*>(&a)[7];
                    result += (mask >> 7) & 0x1;
                }

                template<size_t current = 0>
                inline void pack_right2_int64(
                        int64_t * & result,
                        __m512i & a,
                        uint8_t mask) {
                    *result = reinterpret_cast<int64_t*>(&a)[current];
                    result += (mask >> current) & 0x1;
                    pack_right2_int64<current + 1>(result, a, mask);
                }

                template<>
                inline void pack_right2_int64<7>(
                        int64_t * & result,
                        __m512i & a,
                        uint8_t mask) {
                    *result = reinterpret_cast<int64_t*>(&a)[7];
                    result += (mask >> 7) & 0x1;
                }

                template<typename T>
                struct _mm512 {

                    typedef uint8_t mask_t;
                    typedef uint64_t popcnt_t;

                    static const constexpr mask_t FULL_MASK = 0xFFu;

                    static inline __m512i set1(
                            T value) {
                        return _mm512_set1_epi64(value);
                    }

                    static inline __m512i set(
                            T v7,
                            T v6,
                            T v5,
                            T v4,
                            T v3,
                            T v2,
                            T v1,
                            T v0) {
                        return _mm512_set_epi64(v7, v6, v5, v4, v3, v2, v1, v0);
                    }

                    static inline __m512i set_inc(
                            T v0) {
                        return _mm512_set_epi64(v0 + 7, v0 + 6, v0 + 5, v0 + 4, v0 + 3, v0 + 2, v0 + 1, v0);
                    }

                    static inline __m512i set_inc(
                            T v0,
                            T inc) {
                        return _mm512_set_epi64(v0 + 7 * inc, v0 + 6 * inc, v0 + 5 * inc, v0 + 4 * inc, v0 + 3 * inc, v0 + 2 * inc, v0 + inc, v0);
                    }

                    static inline __m512i min(
                            __m512i a,
                            __m512i b) {
                        if constexpr (std::is_signed_v<T>) {
                            return _mm512_min_epi64(a, b);
                        } else {
                            return _mm512_min_epu64(a, b);
                        }
                    }

                    static inline __m512i max(
                            __m512i a,
                            __m512i b) {
                        if constexpr (std::is_signed_v<T>) {
                            return _mm512_max_epi64(a, b);
                        } else {
                            return _mm512_max_epu64(a, b);
                        }
                    }

                    static inline uint8_t sum(
                            __m512i a) {
                        throw std::runtime_error("__m512i sum uint64_t not supported yet");
                    }

                    static inline __m512i pack_right(
                            __m512i a,
                            mask_t mask) {
                        throw std::runtime_error("__m512i pack_right uint64_t not supported yet");
                    }

                    static inline void pack_right2(
                            uint64_t * & result,
                            __m512i a,
                            mask_t mask) {
                        pack_right2_int64(result, a, mask);
                    }

                    static inline void pack_right3(
                            uint64_t * & result,
                            __m512i a,
                            mask_t mask) {
                        typedef typename mm<__m128i, uint64_t>::mask_t sse_mask_t;
                        auto subMask = static_cast<sse_mask_t>(mask) & 0xF;
                        _mm_storeu_si128(reinterpret_cast<__m128i *>(result), mm<__m128i, uint64_t>::pack_right(_mm512_extracti32x4_epi32(a, 0), subMask));
                        result += __builtin_popcount(subMask);
                        subMask = static_cast<sse_mask_t>(mask >> 4) & 0xF;
                        _mm_storeu_si128(reinterpret_cast<__m128i *>(result), mm<__m128i, uint64_t>::pack_right(_mm512_extracti32x4_epi32(a, 1), subMask));
                        result += __builtin_popcount(subMask);
                        subMask = static_cast<sse_mask_t>(mask >> 8) & 0xF;
                        _mm_storeu_si128(reinterpret_cast<__m128i *>(result), mm<__m128i, uint64_t>::pack_right(_mm512_extracti32x4_epi32(a, 2), subMask));
                        result += __builtin_popcount(subMask);
                        subMask = static_cast<sse_mask_t>(mask >> 12) & 0xF;
                        _mm_storeu_si128(reinterpret_cast<__m128i *>(result), mm<__m128i, uint64_t>::pack_right(_mm512_extracti32x4_epi32(a, 3), subMask));
                        result += __builtin_popcount(subMask);
                    }

                    static inline popcnt_t popcount(
                            // TODO
                            __m512i a) {
                        auto pattern1 = set1(0x5555555555555555ull);
                        auto pattern2 = set1(0x3333333333333333ull);
                        auto pattern3 = set1(0x0F0F0F0F0F0F0F0Full);
                        auto pattern4 = set1(0x0101010101010101ull);
#ifdef __AVX512BW__
                        auto shuffle = set(0xFFFFFFFFFFFFFFFFull, 0xFFFFFFFFFFFFFFFFull, 0xFFFFFFFFFFFFFFFFull, 0xFFFFFFFFFFFFFFFFull, 0xFFFFFFFFFFFFFFFFull, 0xFFFFFFFFFFFFFFFFull, 0xFFFFFFFFFFFFFFFFull, 0x3F372F271F170F07ull);
#else
                        auto shuffle = _mm256_set_epi64x(0xFFFFFFFFFFFFFFFFull, 0xFFFFFFFFFFFF0F07ull, 0xFFFFFFFFFFFFFFFFull, 0xFFFFFFFFFFFF0F07ull);
#endif
                        auto temp = _mm512_sub_epi64(a, _mm512_and_si512(_mm512_srli_epi64(a, 1), pattern1));
                        temp = _mm512_add_epi16(_mm512_and_si512(temp, pattern2), _mm512_and_si512(_mm512_srli_epi16(temp, 2), pattern2));
                        temp = _mm512_and_si512(_mm512_add_epi64(temp, _mm512_srli_epi64(temp, 4)), pattern3);
                        temp = mm512op<T,coding_benchmark::mul>::mullo(temp, pattern4);
#ifdef __AVX512BW__
                        temp = _mm512_shuffle_epi8(temp, shuffle);
                        return static_cast<uint64_t>(temp);
#else
                        auto mm0 = _mm256_shuffle_epi8(_mm512_extracti64x4_epi64(temp, 0), shuffle);
                        auto mm1 = _mm256_shuffle_epi8(_mm512_extracti64x4_epi64(temp, 1), shuffle);
                        auto mm2 = _mm_set_epi16(0, 0, 0, 0, _mm256_extract_epi16(mm1, 8), _mm256_extract_epi16(mm1, 0), _mm256_extract_epi16(mm0, 8), _mm256_extract_epi16(mm0, 0));
                        return _mm_extract_epi64(mm2, 0);
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
                        auto popcnt0 = mm<__m256i, T>::popcount2(_mm512_extracti64x4_epi64(a, 0));
                        auto popcnt1 = mm<__m256i, T>::popcount2(_mm512_extracti64x4_epi64(a, 1));
                        return (static_cast<uint64_t>(popcnt1) << 32) | static_cast<uint64_t>(popcnt0);
#endif
                    }

                    static inline popcnt_t popcount3(
                            // TODO
                            __m512i a) {
                        auto popcnt0 = mm<__m256i, T>::popcount3(_mm512_extracti64x4_epi64(a, 0));
                        auto popcnt1 = mm<__m256i, T>::popcount3(_mm512_extracti64x4_epi64(a, 1));
                        return (static_cast<uint64_t>(popcnt1) << 32) | static_cast<uint64_t>(popcnt0);
                    }
                };

                template<typename T, template<typename > class Op>
                struct _mm512op;

                template<typename T>
                struct _mm512op<T, std::greater> {

                    typedef typename _mm512<T>::mask_t mask_t;

                    static inline __m512i cmp(
                            __m512i a,
                            __m512i b) {
                        if constexpr (std::is_signed_v<T>) {
                            return _mm512_mask_set1_epi16(_mm512_setzero_epi32(), _mm512_cmpgt_epi64_mask(a, b), static_cast<char>(0xFF));
                        } else {
                            return _mm512_mask_set1_epi16(_mm512_setzero_epi32(), _mm512_cmpgt_epu64_mask(a, b), static_cast<char>(0xFF));
                        }
                    }

                    static inline mask_t cmp_mask(
                            __m512i a,
                            __m512i b) {
                        if  constexpr (std::is_signed_v<T>) {
                            return _mm512_cmpgt_epi64_mask(a, b);
                        } else {
                            return _mm512_cmpgt_epu64_mask(a, b);
                        }
                    }
                };

                template<typename T>
                struct _mm512op<T, std::greater_equal> {

                    typedef typename _mm512<T>::mask_t mask_t;

                    static inline __m512i cmp(
                            __m512i a,
                            __m512i b) {
                        if constexpr (std::is_signed_v<T>) {
                            return _mm512_mask_set1_epi16(_mm512_setzero_epi32(), _mm512_cmpge_epi64_mask(a, b), static_cast<char>(0xFF));
                        } else {
                            return _mm512_mask_set1_epi16(_mm512_setzero_epi32(), _mm512_cmpge_epu64_mask(a, b), static_cast<char>(0xFF));
                        }
                    }

                    static inline mask_t cmp_mask(
                            __m512i a,
                            __m512i b) {
                        if  constexpr (std::is_signed_v<T>) {
                            return _mm512_cmpge_epi64_mask(a, b);
                        } else {
                            return _mm512_cmpge_epu64_mask(a, b);
                        }
                    }
                };

                template<typename T>
                struct _mm512op<T, std::less> {

                    typedef typename _mm512<T>::mask_t mask_t;

                    static inline __m512i cmp(
                            __m512i a,
                            __m512i b) {
                        if constexpr (std::is_signed_v<T>) {
                            return _mm512_mask_set1_epi16(_mm512_setzero_epi32(), _mm512_cmplt_epi64_mask(a, b), static_cast<char>(0xFF));
                        } else {
                            return _mm512_mask_set1_epi16(_mm512_setzero_epi32(), _mm512_cmplt_epu64_mask(a, b), static_cast<char>(0xFF));
                        }
                    }

                    static inline mask_t cmp_mask(
                            __m512i a,
                            __m512i b) {
                        if  constexpr (std::is_signed_v<T>) {
                            return _mm512_cmplt_epi64_mask(a, b);
                        } else {
                            return _mm512_cmplt_epu64_mask(a, b);
                        }
                    }
                };

                template<typename T>
                struct _mm512op<T, std::less_equal> {

                    typedef typename _mm512<T>::mask_t mask_t;

                    static inline __m512i cmp(
                            __m512i a,
                            __m512i b) {
                        if constexpr (std::is_signed_v<T>) {
                            return _mm512_mask_set1_epi16(_mm512_setzero_epi32(), _mm512_cmple_epi64_mask(a, b), static_cast<char>(0xFF));
                        } else {
                            return _mm512_mask_set1_epi16(_mm512_setzero_epi32(), _mm512_cmple_epu64_mask(a, b), static_cast<char>(0xFF));
                        }
                    }

                    static inline mask_t cmp_mask(
                            __m512i a,
                            __m512i b) {
                        if  constexpr (std::is_signed_v<T>) {
                            return _mm512_cmple_epi64_mask(a, b);
                        } else {
                            return _mm512_cmple_epu64_mask(a, b);
                        }
                    }
                };

                template<typename T>
                struct _mm512op<T, std::equal_to> {

                    typedef typename _mm512<T>::mask_t mask_t;

                    static inline __m512i cmp(
                            __m512i a,
                            __m512i b) {
                        if constexpr (std::is_signed_v<T>) {
                            return _mm512_mask_set1_epi16(_mm512_setzero_epi32(), _mm512_cmpeq_epi64_mask(a, b), static_cast<char>(0xFF));
                        } else {
                            return _mm512_mask_set1_epi16(_mm512_setzero_epi32(), _mm512_cmpeq_epu64_mask(a, b), static_cast<char>(0xFF));
                        }
                    }

                    static inline mask_t cmp_mask(
                            __m512i a,
                            __m512i b) {
                        if  constexpr (std::is_signed_v<T>) {
                            return _mm512_cmpeq_epi64_mask(a, b);
                        } else {
                            return _mm512_cmpeq_epu64_mask(a, b);
                        }
                    }
                };

                template<typename T>
                struct _mm512op<T, std::not_equal_to> {

                    typedef typename _mm512<T>::mask_t mask_t;

                    static inline __m512i cmp(
                            __m512i a,
                            __m512i b) {
                        if constexpr (std::is_signed_v<T>) {
                            return _mm512_mask_set1_epi16(_mm512_setzero_epi32(), _mm512_cmpneq_epi64_mask(a, b), static_cast<char>(0xFF));
                        } else {
                            return _mm512_mask_set1_epi16(_mm512_setzero_epi32(), _mm512_cmpneq_epu64_mask(a, b), static_cast<char>(0xFF));
                        }
                    }

                    static inline mask_t cmp_mask(
                            __m512i a,
                            __m512i b) {
                        if  constexpr (std::is_signed_v<T>) {
                            return _mm512_cmpneq_epi64_mask(a, b);
                        } else {
                            return _mm512_cmpneq_epu64_mask(a, b);
                        }
                    }
                };

                template<typename T>
                struct _mm512op<T, coding_benchmark::and_is> {

                    typedef typename _mm512<T>::mask_t mask_t;

                    static inline __m512i cmp(
                            __m512i a,
                            __m512i b) {
                        return _mm512_and_si512(a, b);
                    }

                    static inline mask_t cmp_mask(
                            __m512i a,
                            __m512i b) {
#ifdef __AVX512DQ__
                        return _mm512_movepi64_mask(cmp(a, b));
#else
                        return _mm512op<T, std::not_equal_to>::cmp_mask(_mm512_setzero_si512(), cmp(a, b));
#endif
                    }
                };

                template<typename T>
                struct _mm512op<T, coding_benchmark::or_is> {

                    typedef typename _mm512<T>::mask_t mask_t;

                    static inline __m512i cmp(
                            __m512i a,
                            __m512i b) {
                        return _mm512_or_si512(a, b);
                    }

                    static inline mask_t cmp_mask(
                            __m512i a,
                            __m512i b) {
#ifdef __AVX512DQ__
                        return _mm512_movepi64_mask(cmp(a, b));
#else
                        return _mm512op<T, std::not_equal_to>::cmp_mask(_mm512_setzero_si512(), cmp(a, b));
#endif
                    }
                };

                template<typename T>
                struct _mm512op<T, coding_benchmark::xor_is> {

                    typedef typename _mm512<T>::mask_t mask_t;

                    static inline __m512i cmp(
                            __m512i a,
                            __m512i b) {
                        return _mm512_xor_si512(a, b);
                    }

                    static inline mask_t cmp_mask(
                            __m512i a,
                            __m512i b) {
#ifdef __AVX512DQ__
                        return _mm512_movepi64_mask(cmp(a, b));
#else
                        return _mm512op<T, std::not_equal_to>::cmp_mask(_mm512_setzero_si512(), cmp(a, b));
#endif
                    }
                };

                template<typename T>
                struct _mm512op<T, coding_benchmark::is_not> {

                    typedef typename _mm512<T>::mask_t mask_t;

                    static inline __m512i cmp(
                            __m512i a) {
                        return _mm512_andnot_si512(a, _mm512_set1_epi32(0xFFFFFFFF));
                    }

                    static inline mask_t cmp_mask(
                            __m512i a) {
#ifdef __AVX512DQ__
                        return _mm512_movepi64_mask(cmp(a));
#else
                        return _mm512op<T, std::not_equal_to>::cmp_mask(_mm512_setzero_si512(), cmp(a));
#endif
                    }
                };

                template<typename T>
                struct _mm512op<T, coding_benchmark::add> {

                    static inline __m512i compute(
                            __m512i a,
                            __m512i b) {
                        return add(a, b);
                    }

                    static inline __m512i add(
                            __m512i a,
                            __m512i b) {
                        return _mm512_add_epi64(a, b);
                    }
                };

                template<typename T>
                struct _mm512op<T, coding_benchmark::sub> {

                    static inline __m512i compute(
                            __m512i a,
                            __m512i b) {
                        return sub(a, b);
                    }

                    static inline __m512i sub(
                            __m512i a,
                            __m512i b) {
                        return _mm512_sub_epi64(a, b);
                    }
                };

                template<typename T>
                struct _mm512op<T, coding_benchmark::mul> {

                    static inline __m512i compute(
                            __m512i a,
                            __m512i b) {
                        return mullo(a, b);
                    }

                    static inline __m512i mullo(
                            __m512i a,
                            __m512i b) {
#ifdef __AVX512DQ__
                        return _mm512_mullo_epi64(a, b);
#else
                        auto r0 = mm_op<__m256i, T, coding_benchmark::mul>::mullo(_mm512_extracti64x4_epi64(a, 0), _mm512_extracti64x4_epi64(b, 0));
                        auto r1 = mm_op<__m256i, T, coding_benchmark::mul>::mullo(_mm512_extracti64x4_epi64(a, 1), _mm512_extracti64x4_epi64(b, 1));
                        return _mm512_inserti32x8(_mm512_castsi256_si512(r0), r1, 1);
#endif
                    }
                };

                template<typename T>
                struct _mm512op<T, coding_benchmark::div> {

                    static inline __m512i compute(
                            __m512i a,
                            __m512i b) {
                        return div(a, b);
                    }

                    static inline __m512i div(
                            __m512i a,
                            __m512i b) {
#ifdef __AVX512DQ__
                        if constexpr (std::is_signed_v<T>) {
                            return _mm512_cvtpd_epi64(_mm512_div_pd(_mm512_cvtepi64_pd(a), _mm512_cvtepi64_pd(b)));
                        } else {
                            return _mm512_cvtpd_epu64(_mm512_div_pd(_mm512_cvtepu64_pd(a), _mm512_cvtepu64_pd(b)));
                        }
#else
                        auto r0 = mm_op<__m256i, T, coding_benchmark::div>::div(_mm512_extracti64x4_epi64(a, 0), _mm512_extracti64x4_epi64(b, 0));
                        auto r1 = mm_op<__m256i, T, coding_benchmark::div>::div(_mm512_extracti64x4_epi64(a, 1), _mm512_extracti64x4_epi64(b, 1));
                        return _mm512_inserti32x8(_mm512_castsi256_si512(r0), r1, 1);
#endif
                    }
                };

            } /* Private32 */

            template<>
            struct mm512<int64_t> :
                    public Private32::_mm512<int64_t> {
                typedef Private32::_mm512<int64_t> BASE;
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
            struct mm512op<int64_t, std::greater_equal> :
                    private Private08::_mm512op<int64_t, std::greater_equal> {
                typedef Private08::_mm512op<int64_t, std::greater_equal> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm512op<int64_t, std::greater> :
                    private Private08::_mm512op<int64_t, std::greater> {
                typedef Private08::_mm512op<int64_t, std::greater> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm512op<int64_t, std::less_equal> :
                    private Private08::_mm512op<int64_t, std::less_equal> {
                typedef Private08::_mm512op<int64_t, std::less_equal> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm512op<int64_t, std::less> :
                    private Private08::_mm512op<int64_t, std::less> {
                typedef Private08::_mm512op<int64_t, std::less> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm512op<int64_t, std::equal_to> :
                    private Private08::_mm512op<int64_t, std::equal_to> {
                typedef Private08::_mm512op<int64_t, std::equal_to> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm512op<int64_t, std::not_equal_to> :
                    private Private08::_mm512op<int64_t, std::not_equal_to> {
                typedef Private08::_mm512op<int64_t, std::not_equal_to> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm512op<int64_t, coding_benchmark::and_is> :
                    private Private08::_mm512op<int64_t, coding_benchmark::and_is> {
                typedef Private08::_mm512op<int64_t, coding_benchmark::and_is> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm512op<int64_t, coding_benchmark::or_is> :
                    private Private08::_mm512op<int64_t, coding_benchmark::or_is> {
                typedef Private08::_mm512op<int64_t, coding_benchmark::or_is> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm512op<int64_t, coding_benchmark::add> :
                    private Private08::_mm512op<int64_t, coding_benchmark::add> {
                typedef Private08::_mm512op<int64_t, coding_benchmark::add> BASE;
                using BASE::add;
                using BASE::compute;
            };

            template<>
            struct mm512op<int64_t, coding_benchmark::sub> :
                    private Private08::_mm512op<int64_t, coding_benchmark::sub> {
                typedef Private08::_mm512op<int64_t, coding_benchmark::sub> BASE;
                using BASE::sub;
                using BASE::compute;
            };

            template<>
            struct mm512op<int64_t, coding_benchmark::mul> :
                    private Private08::_mm512op<int64_t, coding_benchmark::mul> {
                typedef Private08::_mm512op<int64_t, coding_benchmark::mul> BASE;
                using BASE::mullo;
                using BASE::compute;
            };

            template<>
            struct mm512op<int64_t, coding_benchmark::div> :
                    private Private08::_mm512op<int64_t, coding_benchmark::div> {
                typedef Private08::_mm512op<int64_t, coding_benchmark::div> BASE;
                using BASE::div;
                using BASE::compute;
            };

            template<>
            struct mm512<uint64_t> :
                    public Private32::_mm512<uint64_t> {
                typedef Private32::_mm512<uint64_t> BASE;
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

            template<>
            struct mm512op<uint64_t, std::greater_equal> :
                    private Private08::_mm512op<uint64_t, std::greater_equal> {
                typedef Private08::_mm512op<uint64_t, std::greater_equal> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm512op<uint64_t, std::greater> :
                    private Private08::_mm512op<uint64_t, std::greater> {
                typedef Private08::_mm512op<uint64_t, std::greater> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm512op<uint64_t, std::less_equal> :
                    private Private08::_mm512op<uint64_t, std::less_equal> {
                typedef Private08::_mm512op<uint64_t, std::less_equal> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm512op<uint64_t, std::less> :
                    private Private08::_mm512op<uint64_t, std::less> {
                typedef Private08::_mm512op<uint64_t, std::less> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm512op<uint64_t, std::equal_to> :
                    private Private08::_mm512op<uint64_t, std::equal_to> {
                typedef Private08::_mm512op<uint64_t, std::equal_to> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm512op<uint64_t, std::not_equal_to> :
                    private Private08::_mm512op<uint64_t, std::not_equal_to> {
                typedef Private08::_mm512op<uint64_t, std::not_equal_to> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm512op<uint64_t, coding_benchmark::and_is> :
                    private Private08::_mm512op<uint64_t, coding_benchmark::and_is> {
                typedef Private08::_mm512op<uint64_t, coding_benchmark::and_is> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm512op<uint64_t, coding_benchmark::or_is> :
                    private Private08::_mm512op<uint64_t, coding_benchmark::or_is> {
                typedef Private08::_mm512op<uint64_t, coding_benchmark::or_is> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm512op<uint64_t, coding_benchmark::add> :
                    private Private08::_mm512op<uint64_t, coding_benchmark::add> {
                typedef Private08::_mm512op<uint64_t, coding_benchmark::add> BASE;
                using BASE::add;
                using BASE::compute;
            };

            template<>
            struct mm512op<uint64_t, coding_benchmark::sub> :
                    private Private08::_mm512op<uint64_t, coding_benchmark::sub> {
                typedef Private08::_mm512op<uint64_t, coding_benchmark::sub> BASE;
                using BASE::sub;
                using BASE::compute;
            };

            template<>
            struct mm512op<uint64_t, coding_benchmark::mul> :
                    private Private08::_mm512op<uint64_t, coding_benchmark::mul> {
                typedef Private08::_mm512op<uint64_t, coding_benchmark::mul> BASE;
                using BASE::mullo;
                using BASE::compute;
            };

            template<>
            struct mm512op<uint64_t, coding_benchmark::div> :
                    private Private08::_mm512op<uint64_t, coding_benchmark::div> {
                typedef Private08::_mm512op<uint64_t, coding_benchmark::div> BASE;
                using BASE::div;
                using BASE::compute;
            };

        }
    }
}
