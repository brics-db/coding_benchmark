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

            namespace Private32 {

                template<size_t current = 0>
                inline void pack_right2_int32(
                        int32_t * & result,
                        __m128i & a,
                        uint8_t mask) {
                    *result = reinterpret_cast<int32_t*>(&a)[current];
                    result += (mask >> current) & 0x1;
                    pack_right2_int32<current + 1>(result, a, mask);
                }

                template<>
                inline void pack_right2_int32<3>(
                        int32_t * & result,
                        __m128i & a,
                        uint8_t mask) {
                    *result = reinterpret_cast<int32_t*>(&a)[3];
                    result += (mask >> 3) & 0x1;
                }

                template<size_t current = 0>
                inline void pack_right2_int32(
                        uint32_t * & result,
                        __m128i & a,
                        uint8_t mask) {
                    *result = reinterpret_cast<uint32_t*>(&a)[current];
                    result += (mask >> current) & 0x1;
                    pack_right2_int32<current + 1>(result, a, mask);
                }

                template<>
                inline void pack_right2_int32<3>(
                        uint32_t * & result,
                        __m128i & a,
                        uint8_t mask) {
                    *result = reinterpret_cast<uint32_t*>(&a)[3];
                    result += (mask >> 3) & 0x1;
                }

                template<typename T>
                struct _mm128 {

                    typedef uint8_t mask_t;
                    typedef uint32_t popcnt_t;

                    static inline __m128i set1(
                            T value) {
                        return _mm_set1_epi32(value);
                    }

                    static inline __m128i set(
                            T v3,
                            T v2,
                            T v1,
                            T v0) {
                        return _mm_set_epi32(v3, v2, v1, v0);
                    }

                    static inline __m128i set_inc(
                            T v0) {
                        return _mm_set_epi32(v0 + 3, v0 + 2, v0 + 1, v0);
                    }

                    static inline __m128i set_inc(
                            T v0,
                            T inc) {
                        return _mm_set_epi32(v0 + 3 * inc, v0 + 2 * inc, v0 + inc, v0);
                    }

                    static inline __m128i min(
                            __m128i a,
                            __m128i b) {
                        return _mm_min_epu32(a, b);
                    }

                    static inline __m128i max(
                            __m128i a,
                            __m128i b) {
                        return _mm_max_epu32(a, b);
                    }

                    static inline T sum(
                            __m128i a) {
                        auto mm = _mm_add_epi32(a, _mm_srli_si128(a, 8));
                        mm = _mm_add_epi32(mm, _mm_srli_si128(mm, 4));
                        return static_cast<T>(_mm_extract_epi32(mm, 0));
                    }

                    static inline __m128i pack_right(
                            __m128i a,
                            mask_t mask) {
                        return _mm_shuffle_epi8(a, SHUFFLE_TABLE[mask]);
                    }

                    static inline void pack_right2(
                            T * & result,
                            __m128i a,
                            mask_t mask) {
                        pack_right2_int32(result, a, mask);
                    }

                    static inline popcnt_t popcount(
                            __m128i a) {
                        auto mask = _mm_set1_epi32(0x01010101);
                        auto shuffle = _mm_set_epi32(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x0F0B0703);
                        auto popcount8 = mm128<uint8_t>::popcount(a);
                        return _mm_extract_epi32(_mm_shuffle_epi8(_mm_mullo_epi32(popcount8, mask), shuffle), 0);
                    }

                    static inline popcnt_t popcount2(
                            __m128i a) {
                        auto mask = _mm_set1_epi32(0x01010101);
                        auto shuffle = _mm_set_epi32(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x0F0B0703);
                        auto popcount8 = mm128<uint8_t>::popcount2(a);
                        return _mm_extract_epi32(_mm_shuffle_epi8(_mm_mullo_epi32(popcount8, mask), shuffle), 0);
                    }

                    static inline popcnt_t popcount3(
                            __m128i a) {
                        return (_mm_popcnt_u32(_mm_extract_epi32(a, 3) << 24) | (_mm_popcnt_u32(_mm_extract_epi32(a, 2)) << 16) | (_mm_popcnt_u32(_mm_extract_epi32(a, 1)) << 8)
                                | _mm_popcnt_u32(_mm_extract_epi32(a, 0)));
                    }

                    static inline __m128i cvt_larger_lo(
                            __m128i a) {
                        return _mm_cvtepi16_epi32(a);
                    }

                    static inline __m128i cvt_larger_hi(
                            __m128i a) {
                        return _mm_cvtepi16_epi32(_mm_srli_si128(a, 8));
                    }

                private:
                    static const __m128i * const SHUFFLE_TABLE;
                };

                template<typename T, template<typename > class Op>
                struct _mm128op;

                template<typename T>
                struct _mm128op<T, std::greater> {

                    typedef typename _mm128<T>::mask_t mask_t;

                    static inline __m128i cmp(
                            __m128i a,
                            __m128i b) {
                        return _mm_cmpgt_epi32(a, b);
                    }

                    static inline mask_t cmp_mask(
                            __m128i a,
                            __m128i b) {
                        return static_cast<mask_t>(_mm_movemask_ps(_mm_castsi128_ps(cmp(a, b))));
                    }
                };

                template<typename T>
                struct _mm128op<T, std::greater_equal> {

                    typedef typename _mm128<T>::mask_t mask_t;

                    static inline __m128i cmp(
                            __m128i a,
                            __m128i b) {
                        return _mm_cmpeq_epi32(a, _mm128<T>::max(a, b));
                    }

                    static inline mask_t cmp_mask(
                            __m128i a,
                            __m128i b) {
                        return static_cast<mask_t>(_mm_movemask_ps(_mm_castsi128_ps(cmp(a, b))));
                    }
                };

                template<typename T>
                struct _mm128op<T, std::less> {

                    typedef typename _mm128<T>::mask_t mask_t;

                    static inline __m128i cmp(
                            __m128i a,
                            __m128i b) {
                        return _mm_cmplt_epi32(a, b);
                    }

                    static inline mask_t cmp_mask(
                            __m128i a,
                            __m128i b) {
                        return static_cast<mask_t>(_mm_movemask_ps(_mm_castsi128_ps(cmp(a, b))));
                    }
                };

                template<typename T>
                struct _mm128op<T, std::less_equal> {

                    typedef typename _mm128<T>::mask_t mask_t;

                    static inline __m128i cmp(
                            __m128i a,
                            __m128i b) {
                        return _mm_cmpeq_epi32(a, _mm128<T>::min(a, b));
                    }

                    static inline mask_t cmp_mask(
                            __m128i a,
                            __m128i b) {
                        return static_cast<mask_t>(_mm_movemask_ps(_mm_castsi128_ps(cmp(a, b))));
                    }
                };

                template<typename T>
                struct _mm128op<T, std::equal_to> {

                    typedef typename _mm128<T>::mask_t mask_t;

                    static inline __m128i cmp(
                            __m128i a,
                            __m128i b) {
                        return _mm_cmpeq_epi32(a, b);
                    }

                    static inline mask_t cmp_mask(
                            __m128i a,
                            __m128i b) {
                        return static_cast<mask_t>(_mm_movemask_ps(_mm_castsi128_ps(cmp(a, b))));
                    }
                };

                template<typename T>
                struct _mm128op<T, std::not_equal_to> {

                    typedef typename _mm128<T>::mask_t mask_t;

                    static inline __m128i cmp(
                            __m128i a,
                            __m128i b) {
                        return _mm_or_si128(_mm_cmplt_epi32(a, b), _mm_cmpgt_epi32(a, b));
                    }

                    static inline mask_t cmp_mask(
                            __m128i a,
                            __m128i b) {
                        return static_cast<mask_t>(_mm_movemask_ps(_mm_castsi128_ps(cmp(a, b))));
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
                        return static_cast<mask_t>(_mm_movemask_ps(_mm_castsi128_ps(cmp(a, b))));
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
                        return static_cast<mask_t>(_mm_movemask_ps(_mm_castsi128_ps(cmp(a, b))));
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
                        return _mm_add_epi32(a, b);
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
                        return _mm_sub_epi32(a, b);
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
                        return _mm_mullo_epi32(a, b);
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
                        return _mm_cvtps_epi32(_mm_div_ps(_mm_cvtepi32_ps(a), _mm_cvtepi32_ps(b)));
                    }
                };

            } /* Private32 */

            template<>
            struct mm128<int32_t> :
                    public Private32::_mm128<int32_t> {
                typedef Private32::_mm128<int32_t> BASE;
                using BASE::mask_t;
                using BASE::popcnt_t;
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
                using BASE::cvt_larger_hi;
                using BASE::cvt_larger_lo;
            };

            template<>
            struct mm128op<int32_t, std::greater_equal> :
                    private Private32::_mm128op<int32_t, std::greater_equal> {
                typedef Private32::_mm128op<int32_t, std::greater_equal> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm128op<int32_t, std::greater> :
                    private Private32::_mm128op<int32_t, std::greater> {
                typedef Private32::_mm128op<int32_t, std::greater> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm128op<int32_t, std::less_equal> :
                    private Private32::_mm128op<int32_t, std::less_equal> {
                typedef Private32::_mm128op<int32_t, std::less_equal> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm128op<int32_t, std::less> :
                    private Private32::_mm128op<int32_t, std::less> {
                typedef Private32::_mm128op<int32_t, std::less> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm128op<int32_t, std::equal_to> :
                    private Private32::_mm128op<int32_t, std::equal_to> {
                typedef Private32::_mm128op<int32_t, std::equal_to> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm128op<int32_t, std::not_equal_to> :
                    private Private32::_mm128op<int32_t, std::not_equal_to> {
                typedef Private32::_mm128op<int32_t, std::not_equal_to> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm128op<int32_t, coding_benchmark::and_is> :
                    private Private32::_mm128op<int32_t, coding_benchmark::and_is> {
                typedef Private32::_mm128op<int32_t, coding_benchmark::and_is> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm128op<int32_t, coding_benchmark::or_is> :
                    private Private32::_mm128op<int32_t, coding_benchmark::or_is> {
                typedef Private32::_mm128op<int32_t, coding_benchmark::or_is> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm128op<int32_t, coding_benchmark::add> :
                    private Private32::_mm128op<int32_t, coding_benchmark::add> {
                typedef Private32::_mm128op<int32_t, coding_benchmark::add> BASE;
                using BASE::add;
                using BASE::compute;
            };

            template<>
            struct mm128op<int32_t, coding_benchmark::sub> :
                    private Private32::_mm128op<int32_t, coding_benchmark::sub> {
                typedef Private32::_mm128op<int32_t, coding_benchmark::sub> BASE;
                using BASE::sub;
                using BASE::compute;
            };

            template<>
            struct mm128op<int32_t, coding_benchmark::mul> :
                    private Private32::_mm128op<int32_t, coding_benchmark::mul> {
                typedef Private32::_mm128op<int32_t, coding_benchmark::mul> BASE;
                using BASE::mullo;
                using BASE::compute;
            };

            template<>
            struct mm128op<int32_t, coding_benchmark::div> :
                    private Private32::_mm128op<int32_t, coding_benchmark::div> {
                typedef Private32::_mm128op<int32_t, coding_benchmark::div> BASE;
                using BASE::div;
                using BASE::compute;
            };

            template<>
            struct mm128<uint32_t> :
                    public Private32::_mm128<uint32_t> {
                typedef Private32::_mm128<uint32_t> BASE;
                using BASE::mask_t;
                using BASE::popcnt_t;
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
                using BASE::cvt_larger_hi;
                using BASE::cvt_larger_lo;
            };

            template<>
            struct mm128op<uint32_t, std::greater_equal> :
                    private Private32::_mm128op<uint32_t, std::greater_equal> {
                typedef Private32::_mm128op<uint32_t, std::greater_equal> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm128op<uint32_t, std::greater> :
                    private Private32::_mm128op<uint32_t, std::greater> {
                typedef Private32::_mm128op<uint32_t, std::greater> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm128op<uint32_t, std::less_equal> :
                    private Private32::_mm128op<uint32_t, std::less_equal> {
                typedef Private32::_mm128op<uint32_t, std::less_equal> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm128op<uint32_t, std::less> :
                    private Private32::_mm128op<uint32_t, std::less> {
                typedef Private32::_mm128op<uint32_t, std::less> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm128op<uint32_t, std::equal_to> :
                    private Private32::_mm128op<uint32_t, std::equal_to> {
                typedef Private32::_mm128op<uint32_t, std::equal_to> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm128op<uint32_t, std::not_equal_to> :
                    private Private32::_mm128op<uint32_t, std::not_equal_to> {
                typedef Private32::_mm128op<uint32_t, std::not_equal_to> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm128op<uint32_t, coding_benchmark::and_is> :
                    private Private32::_mm128op<uint32_t, coding_benchmark::and_is> {
                typedef Private32::_mm128op<uint32_t, coding_benchmark::and_is> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm128op<uint32_t, coding_benchmark::or_is> :
                    private Private32::_mm128op<uint32_t, coding_benchmark::or_is> {
                typedef Private32::_mm128op<uint32_t, coding_benchmark::or_is> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm128op<uint32_t, coding_benchmark::add> :
                    private Private32::_mm128op<uint32_t, coding_benchmark::add> {
                typedef Private32::_mm128op<uint32_t, coding_benchmark::add> BASE;
                using BASE::add;
                using BASE::compute;
            };

            template<>
            struct mm128op<uint32_t, coding_benchmark::sub> :
                    private Private32::_mm128op<uint32_t, coding_benchmark::sub> {
                typedef Private32::_mm128op<uint32_t, coding_benchmark::sub> BASE;
                using BASE::sub;
                using BASE::compute;
            };

            template<>
            struct mm128op<uint32_t, coding_benchmark::mul> :
                    private Private32::_mm128op<uint32_t, coding_benchmark::mul> {
                typedef Private32::_mm128op<uint32_t, coding_benchmark::mul> BASE;
                using BASE::mullo;
                using BASE::compute;
            };

            template<>
            struct mm128op<uint32_t, coding_benchmark::div> :
                    private Private32::_mm128op<uint32_t, coding_benchmark::div> {
                typedef Private32::_mm128op<uint32_t, coding_benchmark::div> BASE;
                using BASE::div;
                using BASE::compute;
            };

            template<>
            struct mm128<uint32_t, uint16_t, uint32_t> {

                static inline __m128i mul_add(
                        __m128i * a,
                        __m128i * b,
                        size_t & incA,
                        size_t & incB) {
                    return mm128<uint16_t, uint32_t, uint32_t>::mul_add(b, a, incB, incA);
                }
            };

            template<>
            struct mm128<uint32_t, uint16_t, uint64_t> {

                static inline __m128i mul_add(
                        __m128i * a,
                        __m128i * b,
                        size_t & incA,
                        size_t & incB) {
                    return mm128<uint16_t, uint32_t, uint64_t>::mul_add(b, a, incB, incA);
                }
            };

            template<>
            struct mm128<uint32_t, uint8_t, uint64_t> {

                static inline __m128i mul_add(
                        __m128i * a,
                        __m128i * b,
                        size_t & incA,
                        size_t & incB) {
                    incA = 4;
                    incB = 1;

                    auto pA = reinterpret_cast<uint32_t*>(a);
                    auto pB = reinterpret_cast<uint8_t*>(b);
                    uint64_t r1 = 0, r2 = 0;
                    for (size_t i = 0; i < 16; i += 2) {
                        r1 += static_cast<uint64_t>(pA[i]) * static_cast<uint64_t>(pB[i]);
                        r2 += static_cast<uint64_t>(pA[i + 1]) * static_cast<uint64_t>(pB[i + 1]);
                    }
                    __m128i mm = _mm_set_epi64x(r1, r2);

                    return mm;
                }
            };

            template<>
            struct mm128<uint32_t, uint32_t, uint32_t> {

                static inline __m128i mul_add(
                        __m128i * a,
                        __m128i * b,
                        size_t & incA,
                        size_t & incB) {
                    incA = incB = 1;
                    return _mm_mullo_epi32(_mm_lddqu_si128(a), _mm_lddqu_si128(b));
                }
            };

            template<>
            struct mm128<uint32_t, uint32_t, uint64_t> {

                static inline __m128i mul_add(
                        __m128i * a,
                        __m128i * b,
                        size_t & incA,
                        size_t & incB) {
                    incA = incB = 1;
                    auto mm = _mm_mul_epu32(*a, *b);
                    return _mm_add_epi32(mm, _mm_mul_epu32(_mm_srli_si128(_mm_lddqu_si128(a), 8), _mm_srli_si128(_mm_lddqu_si128(b), 8)));
                }
            };

            template<>
            struct mm128<uint32_t, uint8_t> {

                static inline __m128i convert(
                        __m128i & mm) {
                    return _mm_shuffle_epi8(mm, _mm_set_epi64x(0xFFFFFFFFFFFFFFFF, 0xFFFFFFFF0C080400));
                }
            };

            template<>
            struct mm128<uint32_t, uint16_t> {

                static inline __m128i convert(
                        __m128i & mm) {
                    return _mm_shuffle_epi8(mm, _mm_set_epi64x(0xFFFFFFFFFFFFFFFF, 0x0D0C090805040100));
                }
            };

            template<>
            struct mm128<uint32_t, uint32_t> {

                static inline __m128i convert(
                        __m128i & mm) {
                    return mm;
                }
            };

            template<>
            struct mm128<uint32_t, uint64_t, uint64_t> {

                template<size_t firstA, size_t firstB>
                static inline __m128i mullo(
                        __m128i & a,
                        __m128i & b) {
                    auto r0 = static_cast<uint64_t>(_mm_extract_epi32(a, firstA)) * _mm_extract_epi64(b, firstB);
                    auto r1 = static_cast<uint64_t>(_mm_extract_epi32(a, firstA + 1)) * _mm_extract_epi64(b, firstB + 1);
                    return _mm_set_epi64x(r1, r0);
                }
            };

        }
    }
}
