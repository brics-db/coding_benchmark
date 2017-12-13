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
 * AVX2_uint32.tcc
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

            namespace Private32 {

                template<size_t current = 0>
                inline void pack_right2_int32(
                        int32_t * & result,
                        __m256i & a,
                        uint8_t mask) {
                    *result = reinterpret_cast<int32_t*>(&a)[current];
                    result += (mask >> current) & 0x1;
                    pack_right2_int32<current + 1>(result, a, mask);
                }

                template<>
                inline void pack_right2_int32<7>(
                        int32_t * & result,
                        __m256i & a,
                        uint8_t mask) {
                    *result = reinterpret_cast<int32_t*>(&a)[7];
                    result += (mask >> 7) & 0x1;
                }

                template<size_t current = 0>
                inline void pack_right2_int32(
                        uint32_t * & result,
                        __m256i & a,
                        uint8_t mask) {
                    *result = reinterpret_cast<uint32_t*>(&a)[current];
                    result += (mask >> current) & 0x1;
                    pack_right2_int32<current + 1>(result, a, mask);
                }

                template<>
                inline void pack_right2_int32<7>(
                        uint32_t * & result,
                        __m256i & a,
                        uint8_t mask) {
                    *result = reinterpret_cast<uint32_t*>(&a)[7];
                    result += (mask >> 7) & 0x1;
                }

                template<typename T>
                struct _mm256 {

                    typedef uint8_t mask_t;
                    typedef uint64_t popcnt_t;

                    static inline __m256i set1(
                            T value) {
                        return _mm256_set1_epi32(value);
                    }

                    static inline __m256i set(
                            T v7,
                            T v6,
                            T v5,
                            T v4,
                            T v3,
                            T v2,
                            T v1,
                            T v0) {
                        return _mm256_set_epi32(v7, v6, v5, v4, v3, v2, v1, v0);
                    }

                    static inline __m256i set_inc(
                            T v0) {
                        return _mm256_set_epi32(v0 + 7, v0 + 6, v0 + 5, v0 + 4, v0 + 3, v0 + 2, v0 + 1, v0);
                    }

                    static inline __m256i set_inc(
                            T v0,
                            T inc) {
                        return _mm256_set_epi32(v0 + 7 * inc, v0 + 6 * inc, v0 + 5 * inc, v0 + 4 * inc, v0 + 3 * inc, v0 + 2 * inc, v0 + inc, v0);
                    }

                    static inline __m256i min(
                            __m256i a,
                            __m256i b) {
                        return _mm256_min_epu32(a, b);
                    }

                    static inline __m256i max(
                            __m256i a,
                            __m256i b) {
                        return _mm256_max_epu32(a, b);
                    }

                    static inline __m256i add(
                            __m256i a,
                            __m256i b) {
                        return _mm256_add_epi32(a, b);
                    }

                    static inline __m256i mullo(
                            __m256i a,
                            __m256i b) {
                        return _mm256_mullo_epi32(a, b);
                    }

                    static inline __m256i geq(
                            __m256i a,
                            __m256i b) {
                        auto mm = max(a, b);
                        return _mm256_cmpeq_epi32(a, mm);
                    }

                    static inline uint8_t geq_mask(
                            __m256i a,
                            __m256i b) {
                        return static_cast<uint8_t>(_mm256_movemask_ps(_mm256_castsi256_ps(geq(a, b))));
                    }

                    static inline T sum(
                            __m256i a) {
                        auto mm = _mm256_add_epi32(a, _mm256_srli_si256(a, 8));
                        mm = _mm256_add_epi32(mm, _mm256_srli_si256(mm, 4));
                        auto mm128 = _mm_add_epi32(_mm256_extractf128_si256(mm, 1), _mm256_extractf128_si256(mm, 0));
                        return static_cast<T>(_mm_extract_epi32(mm128, 0));
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
                        pack_right2_int32(result, a, mask);
                    }

                    static inline void pack_right3(
                            T * & result,
                            __m256i a,
                            mask_t mask) {
                        typedef typename mm<__m128i, T>::mask_t sse_mask_t;
                        auto maskLow = static_cast<sse_mask_t>(mask & 0xF);
                        _mm_storeu_si128(reinterpret_cast<__m128i *>(result), mm<__m128i, T>::pack_right(_mm256_extracti128_si256(a, 0), maskLow));
                        result += __builtin_popcount(maskLow);
                        auto maskHigh = static_cast<sse_mask_t>(mask >> 4);
                        _mm_storeu_si128(reinterpret_cast<__m128i *>(result), mm<__m128i, T>::pack_right(_mm256_extracti128_si256(a, 1), maskHigh));
                        result += __builtin_popcount(maskHigh);
                    }

                    static inline popcnt_t popcount(
                            __m256i a) {
                        auto pattern1 = _mm256_set1_epi32(0x55555555);
                        auto pattern2 = _mm256_set1_epi32(0x33333333);
                        auto pattern3 = _mm256_set1_epi32(0x0F0F0F0F);
                        auto pattern4 = _mm256_set1_epi32(0x01010101);
                        auto shuffle = _mm256_set_epi64x(0xFFFFFFFFFFFFFFFF, 0xFFFFFFFF0F0B0703, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFF0F0B0703);
                        auto temp = _mm256_sub_epi32(a, _mm256_and_si256(_mm256_srli_epi32(a, 1), pattern1));
                        temp = _mm256_add_epi32(_mm256_and_si256(temp, pattern2), _mm256_and_si256(_mm256_srli_epi32(temp, 2), pattern2));
                        temp = _mm256_and_si256(_mm256_add_epi32(temp, _mm256_srli_epi32(temp, 4)), pattern3);
                        temp = _mm256_mullo_epi32(temp, pattern4);
                        temp = _mm256_shuffle_epi8(temp, shuffle);
                        return static_cast<uint64_t>(_mm256_extract_epi32(temp, 0)) | (static_cast<uint64_t>(_mm256_extract_epi32(temp, 4)) << 32);
                    }

                    static inline popcnt_t popcount2(
                            __m256i a) {
                        auto mask = _mm256_set1_epi32(0x01010101);
                        auto shuffle = _mm256_set_epi32(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x0F0B0703, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x0F0B0703);
                        auto popcount8 = mm256<uint8_t>::popcount2(a);
                        auto temp = _mm256_shuffle_epi8(_mm256_mullo_epi32(popcount8, mask), shuffle);
                        return static_cast<uint64_t>(_mm256_extract_epi32(temp, 0)) | (static_cast<uint64_t>(_mm256_extract_epi32(temp, 4)) << 32);
                    }

                    static inline popcnt_t popcount3(
                            __m256i a) {
                        return (static_cast<uint64_t>(_mm_popcnt_u32(_mm256_extract_epi32(a, 7))) << 56) | (static_cast<uint64_t>(_mm_popcnt_u32(_mm256_extract_epi32(a, 6))) << 48)
                                | (static_cast<uint64_t>(_mm_popcnt_u32(_mm256_extract_epi32(a, 5))) << 40) | (static_cast<uint64_t>(_mm_popcnt_u32(_mm256_extract_epi32(a, 4))) << 32)
                                | (static_cast<uint64_t>(_mm_popcnt_u32(_mm256_extract_epi32(a, 3))) << 24) | (static_cast<uint64_t>(_mm_popcnt_u32(_mm256_extract_epi32(a, 2))) << 16)
                                | (static_cast<uint64_t>(_mm_popcnt_u32(_mm256_extract_epi32(a, 1))) << 8) | static_cast<uint64_t>(_mm_popcnt_u32(_mm256_extract_epi32(a, 0)));
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
                        return _mm256_cmpgt_epi32(a, b);
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
                        return _mm256_cmpeq_epi32(a, mm);
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
                        return _mm256_cmpgt_epi32(b, a);
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
                        return _mm256_cmpeq_epi32(a, mm);
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
                        return _mm256_cmpeq_epi32(a, b);
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
                        return _mm256_or_si256(_mm256_cmpgt_epi32(b, a), _mm256_cmpgt_epi32(a, b));
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
                struct _mm256op<T, coding_benchmark::add> {

                    static inline __m256i compute(
                            __m256i a,
                            __m256i b) {
                        return add(a, b);
                    }

                    static inline __m256i add(
                            __m256i a,
                            __m256i b) {
                        return _mm256_add_epi32(a, b);
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
                        return _mm256_sub_epi32(a, b);
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
                        return _mm256_mullo_epi32(a, b);
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

            } /* Private32 */

            template<>
            struct mm256<int32_t> :
                    public Private32::_mm256<int32_t> {
                typedef Private32::_mm256<int32_t> BASE;
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
            struct mm256op<int32_t, std::greater_equal> :
                    private Private32::_mm256op<int32_t, std::greater_equal> {
                typedef Private32::_mm256op<int32_t, std::greater_equal> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm256op<int32_t, std::greater> :
                    private Private32::_mm256op<int32_t, std::greater> {
                typedef Private32::_mm256op<int32_t, std::greater> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm256op<int32_t, std::less_equal> :
                    private Private32::_mm256op<int32_t, std::less_equal> {
                typedef Private32::_mm256op<int32_t, std::less_equal> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm256op<int32_t, std::less> :
                    private Private32::_mm256op<int32_t, std::less> {
                typedef Private32::_mm256op<int32_t, std::less> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm256op<int32_t, std::equal_to> :
                    private Private32::_mm256op<int32_t, std::equal_to> {
                typedef Private32::_mm256op<int32_t, std::equal_to> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm256op<int32_t, std::not_equal_to> :
                    private Private32::_mm256op<int32_t, std::not_equal_to> {
                typedef Private32::_mm256op<int32_t, std::not_equal_to> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm256op<int32_t, coding_benchmark::and_is> :
                    private Private32::_mm256op<int32_t, coding_benchmark::and_is> {
                typedef Private32::_mm256op<int32_t, coding_benchmark::and_is> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm256op<int32_t, coding_benchmark::or_is> :
                    private Private32::_mm256op<int32_t, coding_benchmark::or_is> {
                typedef Private32::_mm256op<int32_t, coding_benchmark::or_is> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm256op<int32_t, coding_benchmark::add> :
                    private Private32::_mm256op<int32_t, coding_benchmark::add> {
                typedef Private32::_mm256op<int32_t, coding_benchmark::add> BASE;
                using BASE::add;
                using BASE::compute;
            };

            template<>
            struct mm256op<int32_t, coding_benchmark::sub> :
                    private Private32::_mm256op<int32_t, coding_benchmark::sub> {
                typedef Private32::_mm256op<int32_t, coding_benchmark::sub> BASE;
                using BASE::sub;
                using BASE::compute;
            };

            template<>
            struct mm256op<int32_t, coding_benchmark::mul> :
                    private Private32::_mm256op<int32_t, coding_benchmark::mul> {
                typedef Private32::_mm256op<int32_t, coding_benchmark::mul> BASE;
                using BASE::mullo;
                using BASE::compute;
            };

            template<>
            struct mm256op<int32_t, coding_benchmark::div> :
                    private Private32::_mm256op<int32_t, coding_benchmark::div> {
                typedef Private32::_mm256op<int32_t, coding_benchmark::div> BASE;
                using BASE::div;
                using BASE::compute;
            };

            template<>
            struct mm256<uint32_t> :
                    public Private32::_mm256<uint32_t> {
                typedef Private32::_mm256<uint32_t> BASE;
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
            struct mm256op<uint32_t, std::greater_equal> :
                    private Private32::_mm256op<uint32_t, std::greater_equal> {
                typedef Private32::_mm256op<uint32_t, std::greater_equal> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm256op<uint32_t, std::greater> :
                    private Private32::_mm256op<uint32_t, std::greater> {
                typedef Private32::_mm256op<uint32_t, std::greater> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm256op<uint32_t, std::less_equal> :
                    private Private32::_mm256op<uint32_t, std::less_equal> {
                typedef Private32::_mm256op<uint32_t, std::less_equal> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm256op<uint32_t, std::less> :
                    private Private32::_mm256op<uint32_t, std::less> {
                typedef Private32::_mm256op<uint32_t, std::less> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm256op<uint32_t, std::equal_to> :
                    private Private32::_mm256op<uint32_t, std::equal_to> {
                typedef Private32::_mm256op<uint32_t, std::equal_to> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm256op<uint32_t, std::not_equal_to> :
                    private Private32::_mm256op<uint32_t, std::not_equal_to> {
                typedef Private32::_mm256op<uint32_t, std::not_equal_to> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm256op<uint32_t, coding_benchmark::and_is> :
                    private Private32::_mm256op<uint32_t, coding_benchmark::and_is> {
                typedef Private32::_mm256op<uint32_t, coding_benchmark::and_is> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm256op<uint32_t, coding_benchmark::or_is> :
                    private Private32::_mm256op<uint32_t, coding_benchmark::or_is> {
                typedef Private32::_mm256op<uint32_t, coding_benchmark::or_is> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm256op<uint32_t, coding_benchmark::add> :
                    private Private32::_mm256op<uint32_t, coding_benchmark::add> {
                typedef Private32::_mm256op<uint32_t, coding_benchmark::add> BASE;
                using BASE::add;
                using BASE::compute;
            };

            template<>
            struct mm256op<uint32_t, coding_benchmark::sub> :
                    private Private32::_mm256op<uint32_t, coding_benchmark::sub> {
                typedef Private32::_mm256op<uint32_t, coding_benchmark::sub> BASE;
                using BASE::sub;
                using BASE::compute;
            };

            template<>
            struct mm256op<uint32_t, coding_benchmark::mul> :
                    private Private32::_mm256op<uint32_t, coding_benchmark::mul> {
                typedef Private32::_mm256op<uint32_t, coding_benchmark::mul> BASE;
                using BASE::mullo;
                using BASE::compute;
            };

            template<>
            struct mm256op<uint32_t, coding_benchmark::div> :
                    private Private32::_mm256op<uint32_t, coding_benchmark::div> {
                typedef Private32::_mm256op<uint32_t, coding_benchmark::div> BASE;
                using BASE::div;
                using BASE::compute;
            };

        }
    }
}
