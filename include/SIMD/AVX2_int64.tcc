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
 * AVX2_uint64.tcc
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

            namespace Private64 {

                template<size_t current = 0>
                inline void pack_right2_int64(
                        int64_t * & result,
                        __m256i & a,
                        uint8_t mask) {
                    *result = reinterpret_cast<int64_t*>(&a)[current];
                    result += (mask >> current) & 0x1;
                    pack_right2_int64<current + 1>(result, a, mask);
                }

                template<>
                inline void pack_right2_int64<3>(
                        int64_t * & result,
                        __m256i & a,
                        uint8_t mask) {
                    *result = reinterpret_cast<int64_t*>(&a)[3];
                    result += (mask >> 3) & 0x1;
                }

                template<size_t current = 0>
                inline void pack_right2_int64(
                        uint64_t * & result,
                        __m256i & a,
                        uint8_t mask) {
                    *result = reinterpret_cast<uint64_t*>(&a)[current];
                    result += (mask >> current) & 0x1;
                    pack_right2_int64<current + 1>(result, a, mask);
                }

                template<>
                inline void pack_right2_int64<3>(
                        uint64_t * & result,
                        __m256i & a,
                        uint8_t mask) {
                    *result = reinterpret_cast<uint64_t*>(&a)[3];
                    result += (mask >> 3) & 0x1;
                }

                template<typename T>
                struct _mm256 {

                    typedef uint8_t mask_t;
                    typedef uint32_t popcnt_t;

                    static const constexpr mask_t FULL_MASK = 0xFu;

                    static inline __m256i set1(
                            T value) {
                        return _mm256_set1_epi64x(value);
                    }

                    static inline __m256i set(
                            T v3,
                            T v2,
                            T v1,
                            T v0) {
                        return _mm256_set_epi64x(v3, v2, v1, v0);
                    }

                    static inline __m256i set_inc(
                            T v0) {
                        return _mm256_set_epi64x(v0 + 3, v0 + 2, v0 + 1, v0);
                    }

                    static inline __m256i set_inc(
                            T v0,
                            T inc) {
                        return _mm256_set_epi64x(v0 + 3 * inc, v0 + 2 * inc, v0 + inc, v0);
                    }

                    template<int I>
                    static inline T extract(
                            __m256i a) {
                        return _mm256_extract_epi64(a, I);
                    }

                    static inline __m256i min(
                            __m256i a,
                            __m256i b) {
                        auto mm0 = mm<__m128i, T>::min(_mm256_extractf128_si256(a, 0), _mm256_extractf128_si256(b, 0));
                        auto mm1 = mm<__m128i, T>::min(_mm256_extractf128_si256(a, 1), _mm256_extractf128_si256(b, 1));
                        auto mm2 = _mm256_insertf128_si256(_mm256_setzero_si256(), mm0, 0);
                        return _mm256_insertf128_si256(mm2, mm1, 1);
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
                        auto mm0 = mm<__m128i, T>::max(_mm256_extractf128_si256(a, 0), _mm256_extractf128_si256(b, 0));
                        auto mm1 = mm<__m128i, T>::max(_mm256_extractf128_si256(a, 1), _mm256_extractf128_si256(b, 1));
                        auto mm2 = _mm256_insertf128_si256(_mm256_setzero_si256(), mm0, 0);
                        return _mm256_insertf128_si256(mm2, mm1, 1);
                    }

                    static T max(
                            __m256i a) {
                        auto x1 = mm<__m128i, T>::max(_mm256_extractf128_si256(a, 0));
                        auto x2 = mm<__m128i, T>::max(_mm256_extractf128_si256(a, 1));
                        return x1 > x2 ? x1 : x2;
                    }

                    static inline T sum(
                            __m256i a) {
                        auto mm = _mm256_add_epi64(a, _mm256_srli_si256(a, 8));
                        return static_cast<T>(_mm256_extract_epi64(mm, 0)) + static_cast<T>(_mm256_extract_epi64(mm, 2));
                    }

                    static inline __m256i pack_right(
                            __m256i a,
                            mask_t mask) {
                        return _mm256_shuffle256_epi8(a, SHUFFLE_TABLE[mask]);
                    }

                    static inline void pack_right2(
                            T * & result,
                            __m256i a,
                            mask_t mask) {
                        pack_right2_uint64(result, a, mask);
                    }

                    static inline void pack_right3(
                            T * & result,
                            __m256i a,
                            mask_t mask) {
                        typedef typename mm<__m128i, T>::mask_t sse_mask_t;
                        auto maskLow = static_cast<sse_mask_t>(mask & 0x3);
                        _mm_storeu_si128(reinterpret_cast<__m128i *>(result), mm<__m128i, T>::pack_right(_mm256_extracti128_si256(a, 0), maskLow));
                        result += __builtin_popcount(maskLow);
                        auto maskHigh = static_cast<sse_mask_t>(mask >> 2);
                        _mm_storeu_si128(reinterpret_cast<__m128i *>(result), mm<__m128i, T>::pack_right(_mm256_extracti128_si256(a, 1), maskHigh));
                        result += __builtin_popcount(maskHigh);
                    }

                    static inline popcnt_t popcount(
                            __m256i a) {
                        auto pattern1 = _mm256_set1_epi64x(0x5555555555555555ull);
                        auto pattern2 = _mm256_set1_epi64x(0x3333333333333333ull);
                        auto pattern3 = _mm256_set1_epi64x(0x0F0F0F0F0F0F0F0Full);
                        auto pattern4 = _mm256_set1_epi64x(0x0101010101010101ull);
                        auto shuffle = _mm256_set_epi64x(0xFFFFFFFFFFFFFFFFull, 0xFFFFFFFFFFFF0F07ull, 0xFFFFFFFFFFFFFFFFull, 0xFFFFFFFFFFFF0F07ull);
                        auto temp = _mm256_sub_epi32(a, _mm256_and_si256(_mm256_srli_epi32(a, 1), pattern1));
                        temp = _mm256_add_epi32(_mm256_and_si256(temp, pattern2), _mm256_and_si256(_mm256_srli_epi32(temp, 2), pattern2));
                        temp = _mm256_and_si256(_mm256_add_epi32(temp, _mm256_srli_epi32(temp, 4)), pattern3);
                        temp = _mm256_mullo_epi32(temp, pattern4);
                        temp = _mm256_shuffle_epi8(temp, shuffle);
                        return static_cast<uint32_t>(_mm256_extract_epi16(temp, 0)) | (static_cast<uint32_t>(_mm256_extract_epi16(temp, 8)) << 16);
                    }

                    static inline popcnt_t popcount2(
                            __m256i a) {
                        uint64_t mask = 0x0101010101010101;
                        auto shuffle = _mm256_set_epi64x(0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFF0F07, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFF0F07);
                        auto popcount8 = mm256 < uint8_t > ::popcount2(a);
                        auto temp = _mm256_shuffle_epi8(
                                _mm256_set_epi64x(static_cast<uint64_t>(_mm256_extract_epi64(popcount8, 3)) * mask, static_cast<uint64_t>(_mm256_extract_epi64(popcount8, 2)) * mask,
                                        static_cast<uint64_t>(_mm256_extract_epi64(popcount8, 1)) * mask, static_cast<uint64_t>(_mm256_extract_epi64(popcount8, 0)) * mask), shuffle);
                        return static_cast<uint32_t>(_mm256_extract_epi16(temp, 0)) | (static_cast<uint32_t>(_mm256_extract_epi16(temp, 8)) << 16);
                    }

                    static inline popcnt_t popcount3(
                            __m256i a) {
                        return (_mm_popcnt_u64(_mm256_extract_epi64(a, 3)) << 24) | (_mm_popcnt_u64(_mm256_extract_epi64(a, 2)) << 16) | (_mm_popcnt_u64(_mm256_extract_epi64(a, 1)) << 8)
                                | _mm_popcnt_u64(_mm256_extract_epi64(a, 0));
                    }

                    static inline __m256i cvt_larger_lo(
                            __m256i a) {
                        if constexpr (std::is_signed_v<T>) {
                            return _mm256_set_epi64x(0, _mm256_extract_epi64(a, 1), 0, _mm256_extract_epi64(a, 0));
                        } else {
                            int64_t a0 = _mm256_extract_epi64(a, 0);
                            int64_t a1 = _mm256_extract_epi64(a, 1);
                            return _mm256_set_epi64x(a1 < 0 ? -1 : 0, a1, a0 < 0 ? -1 : 0, a0);
                        }
                    }

                    static inline __m256i cvt_larger_hi(
                            __m256i a) {
                        if constexpr (std::is_signed_v<T>) {
                            return _mm256_set_epi64x(0, _mm256_extract_epi64(a, 3), 0, _mm256_extract_epi64(a, 2));
                        } else {
                            int64_t a2 = _mm256_extract_epi64(a, 2);
                            int64_t a3 = _mm256_extract_epi64(a, 3);
                            return _mm256_set_epi64x(a3 < 0 ? -1 : 0, a3, a2 < 0 ? -1 : 0, a2);
                        }
                    }

                private:
                    static const __m256i * const SHUFFLE_TABLE;
                };

                template<typename T, template<typename > class Op>
                struct _mm256op;

                template<typename T>
                struct _mm256op<T, std::greater> {

                    typedef typename _mm256<T>::mask_t mask_t;

                    static inline __m256i cmp(
                            __m256i a,
                            __m256i b) {
                        return _mm256_cmpgt_epi64(a, b);
                    }

                    static inline mask_t cmp_mask(
                            __m256i a,
                            __m256i b) {
                        return static_cast<mask_t>(_mm256_movemask_pd(_mm256_castsi256_pd(cmp(a, b))));
                    }
                };

                template<typename T>
                struct _mm256op<T, std::greater_equal> {

                    typedef typename _mm256<T>::mask_t mask_t;

                    static inline __m256i cmp(
                            __m256i a,
                            __m256i b) {
                        auto mm = _mm256<T>::max(a, b);
                        return _mm256_cmpeq_epi64(a, mm);
                    }

                    static inline mask_t cmp_mask(
                            __m256i a,
                            __m256i b) {
                        return static_cast<mask_t>(_mm256_movemask_pd(_mm256_castsi256_pd(cmp(a, b))));
                    }
                };

                template<typename T>
                struct _mm256op<T, std::less> {

                    typedef typename _mm256<T>::mask_t mask_t;

                    static inline __m256i cmp(
                            __m256i a,
                            __m256i b) {
                        return _mm256_cmpgt_epi64(b, a);
                    }

                    static inline mask_t cmp_mask(
                            __m256i a,
                            __m256i b) {
                        return static_cast<mask_t>(_mm256_movemask_pd(_mm256_castsi256_pd(cmp(a, b))));
                    }
                };

                template<typename T>
                struct _mm256op<T, std::less_equal> {

                    typedef typename _mm256<T>::mask_t mask_t;

                    static inline __m256i cmp(
                            __m256i a,
                            __m256i b) {
                        auto mm = avx2::mm256<T>::min(a, b);
                        return _mm256_cmpeq_epi64(a, mm);
                    }

                    static inline mask_t cmp_mask(
                            __m256i a,
                            __m256i b) {
                        return static_cast<mask_t>(_mm256_movemask_pd(_mm256_castsi256_pd(cmp(a, b))));
                    }
                };

                template<typename T>
                struct _mm256op<T, std::equal_to> {

                    typedef typename _mm256<T>::mask_t mask_t;

                    static inline __m256i cmp(
                            __m256i a,
                            __m256i b) {
                        return _mm256_cmpeq_epi64(a, b);
                    }

                    static inline mask_t cmp_mask(
                            __m256i a,
                            __m256i b) {
                        return static_cast<mask_t>(_mm256_movemask_pd(_mm256_castsi256_pd(cmp(a, b))));
                    }
                };

                template<typename T>
                struct _mm256op<T, std::not_equal_to> {

                    typedef typename _mm256<T>::mask_t mask_t;

                    static inline __m256i cmp(
                            __m256i a,
                            __m256i b) {
                        return _mm256_or_si256(_mm256_cmpgt_epi64(b, a), _mm256_cmpgt_epi64(a, b));
                    }

                    static inline mask_t cmp_mask(
                            __m256i a,
                            __m256i b) {
                        return static_cast<mask_t>(_mm256_movemask_pd(_mm256_castsi256_pd(cmp(a, b))));
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
                        return static_cast<mask_t>(_mm256_movemask_pd(_mm256_castsi256_pd(cmp(a, b))));
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
                        return static_cast<mask_t>(_mm256_movemask_pd(_mm256_castsi256_pd(cmp(a, b))));
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
                        return static_cast<mask_t>(_mm256_movemask_pd(_mm256_castsi256_pd(cmp(a, b))));
                    }
                };

                template<typename T>
                struct _mm256op<T, coding_benchmark::is_not> {

                    typedef typename _mm256<T>::mask_t mask_t;

                    static inline __m256i cmp(
                            __m256i a) {
                        return _mm256_andnot_si256(a, _mm256_set1_epi64x(0xFFFFFFFFFFFFFFFFull));
                    }

                    static inline mask_t cmp_mask(
                            __m256i a) {
                        return static_cast<mask_t>(_mm256_movemask_pd(_mm256_castsi256_pd(cmp(a))));
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
                        return _mm256_add_epi64(a, b);
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
                        return _mm256_sub_epi64(a, b);
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
#if defined(AVX512VL) and defined(AVX512DQ)
                        return _mm256_mullo_epi64(a, b);
#else
                        return _mm256_set_epi64x(_mm256_extract_epi64(a, 3) * _mm256_extract_epi64(b, 3), _mm256_extract_epi64(a, 2) * _mm256_extract_epi64(b, 2),
                                _mm256_extract_epi64(a, 1) * _mm256_extract_epi64(b, 1), _mm256_extract_epi64(a, 0) * _mm256_extract_epi64(b, 0));
#endif
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
                        return _mm256_set_epi64x(_mm256_extract_epi64(a, 3) / _mm256_extract_epi64(b, 3), _mm256_extract_epi64(a, 2) / _mm256_extract_epi64(b, 2),
                                _mm256_extract_epi64(a, 1) / _mm256_extract_epi64(b, 1), _mm256_extract_epi64(a, 0) / _mm256_extract_epi64(b, 0));
                    }
                };

            } /* Private64 */

            template<>
            struct mm256<int64_t> :
                    public Private64::_mm256<int64_t> {
                typedef Private64::_mm256<int64_t> BASE;
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
            };

            template<>
            struct mm256op<int64_t, std::greater_equal> :
                    private Private64::_mm256op<int64_t, std::greater_equal> {
                typedef Private64::_mm256op<int64_t, std::greater_equal> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm256op<int64_t, std::greater> :
                    private Private64::_mm256op<int64_t, std::greater> {
                typedef Private64::_mm256op<int64_t, std::greater> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm256op<int64_t, std::less_equal> :
                    private Private64::_mm256op<int64_t, std::less_equal> {
                typedef Private64::_mm256op<int64_t, std::less_equal> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm256op<int64_t, std::less> :
                    private Private64::_mm256op<int64_t, std::less> {
                typedef Private64::_mm256op<int64_t, std::less> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm256op<int64_t, std::equal_to> :
                    private Private64::_mm256op<int64_t, std::equal_to> {
                typedef Private64::_mm256op<int64_t, std::equal_to> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm256op<int64_t, std::not_equal_to> :
                    private Private64::_mm256op<int64_t, std::not_equal_to> {
                typedef Private64::_mm256op<int64_t, std::not_equal_to> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm256op<int64_t, coding_benchmark::and_is> :
                    private Private64::_mm256op<int64_t, coding_benchmark::and_is> {
                typedef Private64::_mm256op<int64_t, coding_benchmark::and_is> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm256op<int64_t, coding_benchmark::or_is> :
                    private Private64::_mm256op<int64_t, coding_benchmark::or_is> {
                typedef Private64::_mm256op<int64_t, coding_benchmark::or_is> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm256op<int64_t, coding_benchmark::xor_is> :
                    private Private64::_mm256op<int64_t, coding_benchmark::xor_is> {
                typedef Private64::_mm256op<int64_t, coding_benchmark::xor_is> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm256op<int64_t, coding_benchmark::is_not> :
                    private Private64::_mm256op<int64_t, coding_benchmark::is_not> {
                typedef Private64::_mm256op<int64_t, coding_benchmark::is_not> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm256op<int64_t, coding_benchmark::add> :
                    private Private64::_mm256op<int64_t, coding_benchmark::add> {
                typedef Private64::_mm256op<int64_t, coding_benchmark::add> BASE;
                using BASE::add;
                using BASE::compute;
            };

            template<>
            struct mm256op<int64_t, coding_benchmark::sub> :
                    private Private64::_mm256op<int64_t, coding_benchmark::sub> {
                typedef Private64::_mm256op<int64_t, coding_benchmark::sub> BASE;
                using BASE::sub;
                using BASE::compute;
            };

            template<>
            struct mm256op<int64_t, coding_benchmark::mul> :
                    private Private64::_mm256op<int64_t, coding_benchmark::mul> {
                typedef Private64::_mm256op<int64_t, coding_benchmark::mul> BASE;
                using BASE::mullo;
                using BASE::compute;
            };

            template<>
            struct mm256op<int64_t, coding_benchmark::div> :
                    private Private64::_mm256op<int64_t, coding_benchmark::div> {
                typedef Private64::_mm256op<int64_t, coding_benchmark::div> BASE;
                using BASE::div;
                using BASE::compute;
            };

            template<>
            struct mm256<uint64_t> :
                    public Private64::_mm256<uint64_t> {
                typedef Private64::_mm256<uint64_t> BASE;
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
            };

            template<>
            struct mm256op<uint64_t, std::greater_equal> :
                    private Private64::_mm256op<uint64_t, std::greater_equal> {
                typedef Private64::_mm256op<uint64_t, std::greater_equal> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm256op<uint64_t, std::greater> :
                    private Private64::_mm256op<uint64_t, std::greater> {
                typedef Private64::_mm256op<uint64_t, std::greater> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm256op<uint64_t, std::less_equal> :
                    private Private64::_mm256op<uint64_t, std::less_equal> {
                typedef Private64::_mm256op<uint64_t, std::less_equal> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm256op<uint64_t, std::less> :
                    private Private64::_mm256op<uint64_t, std::less> {
                typedef Private64::_mm256op<uint64_t, std::less> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm256op<uint64_t, std::equal_to> :
                    private Private64::_mm256op<uint64_t, std::equal_to> {
                typedef Private64::_mm256op<uint64_t, std::equal_to> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm256op<uint64_t, std::not_equal_to> :
                    private Private64::_mm256op<uint64_t, std::not_equal_to> {
                typedef Private64::_mm256op<uint64_t, std::not_equal_to> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm256op<uint64_t, coding_benchmark::and_is> :
                    private Private64::_mm256op<uint64_t, coding_benchmark::and_is> {
                typedef Private64::_mm256op<uint64_t, coding_benchmark::and_is> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm256op<uint64_t, coding_benchmark::or_is> :
                    private Private64::_mm256op<uint64_t, coding_benchmark::or_is> {
                typedef Private64::_mm256op<uint64_t, coding_benchmark::or_is> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm256op<uint64_t, coding_benchmark::xor_is> :
                    private Private64::_mm256op<uint64_t, coding_benchmark::xor_is> {
                typedef Private64::_mm256op<uint64_t, coding_benchmark::xor_is> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm256op<uint64_t, coding_benchmark::is_not> :
                    private Private64::_mm256op<uint64_t, coding_benchmark::is_not> {
                typedef Private64::_mm256op<uint64_t, coding_benchmark::is_not> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm256op<uint64_t, coding_benchmark::add> :
                    private Private64::_mm256op<uint64_t, coding_benchmark::add> {
                typedef Private64::_mm256op<uint64_t, coding_benchmark::add> BASE;
                using BASE::add;
                using BASE::compute;
            };

            template<>
            struct mm256op<uint64_t, coding_benchmark::sub> :
                    private Private64::_mm256op<uint64_t, coding_benchmark::sub> {
                typedef Private64::_mm256op<uint64_t, coding_benchmark::sub> BASE;
                using BASE::sub;
                using BASE::compute;
            };

            template<>
            struct mm256op<uint64_t, coding_benchmark::mul> :
                    private Private64::_mm256op<uint64_t, coding_benchmark::mul> {
                typedef Private64::_mm256op<uint64_t, coding_benchmark::mul> BASE;
                using BASE::mullo;
                using BASE::compute;
            };

            template<>
            struct mm256op<uint64_t, coding_benchmark::div> :
                    private Private64::_mm256op<uint64_t, coding_benchmark::div> {
                typedef Private64::_mm256op<uint64_t, coding_benchmark::div> BASE;
                using BASE::div;
                using BASE::compute;
            };

        }
    }
}
