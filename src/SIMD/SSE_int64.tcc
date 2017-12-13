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

            namespace Private64 {

                template<size_t current = 0>
                inline void pack_right2_int64(
                        int64_t * & result,
                        __m128i & a,
                        uint8_t mask) {
                    *result = reinterpret_cast<int64_t*>(&a)[current];
                    result += (mask >> current) & 0x1;
                    pack_right2_int64<current + 1>(result, a, mask);
                }

                template<>
                inline void pack_right2_int64<1>(
                        int64_t * & result,
                        __m128i & a,
                        uint8_t mask) {
                    *result = reinterpret_cast<int64_t*>(&a)[1];
                    result += (mask >> 1) & 0x1;
                }

                template<size_t current = 0>
                inline void pack_right2_int64(
                        uint64_t * & result,
                        __m128i & a,
                        uint8_t mask) {
                    *result = reinterpret_cast<uint64_t*>(&a)[current];
                    result += (mask >> current) & 0x1;
                    pack_right2_int64<current + 1>(result, a, mask);
                }

                template<>
                inline void pack_right2_int64<1>(
                        uint64_t * & result,
                        __m128i & a,
                        uint8_t mask) {
                    *result = reinterpret_cast<uint64_t*>(&a)[1];
                    result += (mask >> 1) & 0x1;
                }

                template<typename T>
                struct _mm128 {

                    typedef uint8_t mask_t;
                    typedef uint16_t popcnt_t;

                    static inline __m128i set1(
                            T value) {
                        return _mm_set1_epi64x(value);
                    }

                    static inline __m128i set(
                            T v1,
                            T v0) {
                        return _mm_set_epi64x(v1, v0);
                    }

                    static inline __m128i set_inc(
                            T v0) {
                        return _mm_set_epi64x(v0 + 1, v0);
                    }

                    static inline __m128i set_inc(
                            T v0,
                            T inc) {
                        return _mm_set_epi64x(v0 + inc, v0);
                    }

                    static inline __m128i min(
                            __m128i a,
                            __m128i b) {
                        return _mm_set_epi64x(std::min(static_cast<T>(_mm_extract_epi64(a, 1)), static_cast<T>(_mm_extract_epi64(b, 1))),
                                std::min(static_cast<T>(_mm_extract_epi64(a, 0)), static_cast<T>(_mm_extract_epi64(b, 0))));
                    }

                    static inline __m128i max(
                            __m128i a,
                            __m128i b) {
                        return _mm_set_epi64x(std::max(static_cast<T>(_mm_extract_epi64(a, 1)), static_cast<T>(_mm_extract_epi64(b, 1))),
                                std::max(static_cast<T>(_mm_extract_epi64(a, 0)), static_cast<T>(_mm_extract_epi64(b, 0))));
                    }

                    static inline T sum(
                            __m128i a) {
                        return static_cast<T>(_mm_extract_epi64(a, 0)) + static_cast<T>(_mm_extract_epi64(a, 1));
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
                        pack_right2_int64(result, a, mask);
                    }

                    static inline popcnt_t popcount(
                            __m128i a) {
                        uint64_t pattern = 0x0101010101010101ull;
                        auto popcount8 = mm128<uint8_t>::popcount(a);
                        return (static_cast<uint16_t>((_mm_extract_epi64(popcount8, 1) * pattern) >> 57) << 8) | static_cast<uint16_t>((_mm_extract_epi64(popcount8, 0) * pattern) >> 57);
                    }

                    static inline popcnt_t popcount2(
                            __m128i a) {
                        uint64_t pattern = 0x0101010101010101ull;
                        auto popcount8 = mm128<uint8_t>::popcount2(a);
                        return (static_cast<uint16_t>((_mm_extract_epi64(popcount8, 1) * pattern) >> 57) << 8) | static_cast<uint16_t>((_mm_extract_epi64(popcount8, 0) * pattern) >> 57);
                    }

                    static inline popcnt_t popcount3(
                            __m128i a) {
                        return (_mm_popcnt_u64(_mm_extract_epi64(a, 1) << 8) | _mm_popcnt_u64(_mm_extract_epi64(a, 0)));
                    }

                    static inline __m128i cvt_larger_lo(
                            __m128i a) {
                        return _mm_and_si128(a, _mm_set_epi64x(0ull, 0xFFFFFFFFFFFFFFFFull));
                    }

                    static inline __m128i cvt_larger_hi(
                            __m128i a) {
                        return _mm_srli_si128(a, 8);
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
                        return _mm_cmpgt_epi64(a, b);
                    }

                    static inline mask_t cmp_mask(
                            __m128i a,
                            __m128i b) {
                        return static_cast<mask_t>(_mm_movemask_pd(_mm_castsi128_pd(cmp(a, b))));
                    }
                };

                template<typename T>
                struct _mm128op<T, std::greater_equal> {

                    typedef typename _mm128<T>::mask_t mask_t;

                    static inline __m128i cmp(
                            __m128i a,
                            __m128i b) {
                        auto mm = _mm128<T>::max(a, b);
                        return _mm_cmpeq_epi64(a, mm);
                    }

                    static inline mask_t cmp_mask(
                            __m128i a,
                            __m128i b) {
                        return static_cast<mask_t>(_mm_movemask_pd(_mm_castsi128_pd(cmp(a, b))));
                    }
                };

                template<typename T>
                struct _mm128op<T, std::less> {

                    typedef typename _mm128<T>::mask_t mask_t;

                    static inline __m128i cmp(
                            __m128i a,
                            __m128i b) {
                        return _mm_cmpgt_epi64(b, a);
                    }

                    static inline mask_t cmp_mask(
                            __m128i a,
                            __m128i b) {
                        return static_cast<mask_t>(_mm_movemask_pd(_mm_castsi128_pd(cmp(a, b))));
                    }
                };

                template<typename T>
                struct _mm128op<T, std::less_equal> {

                    typedef typename _mm128<T>::mask_t mask_t;

                    static inline __m128i cmp(
                            __m128i a,
                            __m128i b) {
                        auto mm = _mm128<T>::min(a, b);
                        return _mm_cmpeq_epi64(a, mm);
                    }

                    static inline mask_t cmp_mask(
                            __m128i a,
                            __m128i b) {
                        return static_cast<mask_t>(_mm_movemask_pd(_mm_castsi128_pd(cmp(a, b))));
                    }
                };

                template<typename T>
                struct _mm128op<T, std::equal_to> {

                    typedef typename _mm128<T>::mask_t mask_t;

                    static inline __m128i cmp(
                            __m128i a,
                            __m128i b) {
                        return _mm_cmpeq_epi64(a, b);
                    }

                    static inline mask_t cmp_mask(
                            __m128i a,
                            __m128i b) {
                        return static_cast<mask_t>(_mm_movemask_pd(_mm_castsi128_pd(cmp(a, b))));
                    }
                };

                template<typename T>
                struct _mm128op<T, std::not_equal_to> {

                    typedef typename _mm128<T>::mask_t mask_t;

                    static inline __m128i cmp(
                            __m128i a,
                            __m128i b) {
                        return _mm_or_si128(_mm_cmpgt_epi64(a, b), _mm_cmpgt_epi64(b, a));
                    }

                    static inline mask_t cmp_mask(
                            __m128i a,
                            __m128i b) {
                        return static_cast<mask_t>(_mm_movemask_pd(_mm_castsi128_pd(cmp(a, b))));
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
                        return static_cast<mask_t>(_mm_movemask_pd(_mm_castsi128_pd(cmp(a, b))));
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
                        return static_cast<mask_t>(_mm_movemask_pd(_mm_castsi128_pd(cmp(a, b))));
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
                        return _mm_add_epi64(a, b);
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
                        return _mm_sub_epi64(a, b);
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
                        return _mm_set_epi64x(_mm_extract_epi64(a, 1) * _mm_extract_epi64(b, 1), _mm_extract_epi64(a, 0) * _mm_extract_epi64(b, 0));
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
                        return _mm_set_epi64x(_mm_extract_epi64(a, 1) / _mm_extract_epi64(b, 1), _mm_extract_epi64(a, 0) / _mm_extract_epi64(b, 0));
                    }
                };

            } /* Private64 */

            template<>
            struct mm128<int64_t> :
                    public Private64::_mm128<int64_t> {
                typedef Private64::_mm128<int64_t> BASE;
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
            struct mm128op<int64_t, std::greater_equal> :
                    private Private64::_mm128op<int64_t, std::greater_equal> {
                typedef Private64::_mm128op<int64_t, std::greater_equal> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm128op<int64_t, std::greater> :
                    private Private64::_mm128op<int64_t, std::greater> {
                typedef Private64::_mm128op<int64_t, std::greater> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm128op<int64_t, std::less_equal> :
                    private Private64::_mm128op<int64_t, std::less_equal> {
                typedef Private64::_mm128op<int64_t, std::less_equal> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm128op<int64_t, std::less> :
                    private Private64::_mm128op<int64_t, std::less> {
                typedef Private64::_mm128op<int64_t, std::less> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm128op<int64_t, std::equal_to> :
                    private Private64::_mm128op<int64_t, std::equal_to> {
                typedef Private64::_mm128op<int64_t, std::equal_to> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm128op<int64_t, std::not_equal_to> :
                    private Private64::_mm128op<int64_t, std::not_equal_to> {
                typedef Private64::_mm128op<int64_t, std::not_equal_to> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm128op<int64_t, coding_benchmark::and_is> :
                    private Private64::_mm128op<int64_t, coding_benchmark::and_is> {
                typedef Private64::_mm128op<int64_t, coding_benchmark::and_is> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm128op<int64_t, coding_benchmark::or_is> :
                    private Private64::_mm128op<int64_t, coding_benchmark::or_is> {
                typedef Private64::_mm128op<int64_t, coding_benchmark::or_is> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm128op<int64_t, coding_benchmark::add> :
                    private Private64::_mm128op<int64_t, coding_benchmark::add> {
                typedef Private64::_mm128op<int64_t, coding_benchmark::add> BASE;
                using BASE::add;
                using BASE::compute;
            };

            template<>
            struct mm128op<int64_t, coding_benchmark::sub> :
                    private Private64::_mm128op<int64_t, coding_benchmark::sub> {
                typedef Private64::_mm128op<int64_t, coding_benchmark::sub> BASE;
                using BASE::sub;
                using BASE::compute;
            };

            template<>
            struct mm128op<int64_t, coding_benchmark::mul> :
                    private Private64::_mm128op<int64_t, coding_benchmark::mul> {
                typedef Private64::_mm128op<int64_t, coding_benchmark::mul> BASE;
                using BASE::mullo;
                using BASE::compute;
            };

            template<>
            struct mm128op<int64_t, coding_benchmark::div> :
                    private Private64::_mm128op<int64_t, coding_benchmark::div> {
                typedef Private64::_mm128op<int64_t, coding_benchmark::div> BASE;
                using BASE::div;
                using BASE::compute;
            };

            template<>
            struct mm128<uint64_t> :
                    public Private64::_mm128<uint64_t> {
                typedef Private64::_mm128<uint64_t> BASE;
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
            struct mm128op<uint64_t, std::greater_equal> :
                    private Private64::_mm128op<uint64_t, std::greater_equal> {
                typedef Private64::_mm128op<uint64_t, std::greater_equal> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm128op<uint64_t, std::greater> :
                    private Private64::_mm128op<uint64_t, std::greater> {
                typedef Private64::_mm128op<uint64_t, std::greater> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm128op<uint64_t, std::less_equal> :
                    private Private64::_mm128op<uint64_t, std::less_equal> {
                typedef Private64::_mm128op<uint64_t, std::less_equal> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm128op<uint64_t, std::less> :
                    private Private64::_mm128op<uint64_t, std::less> {
                typedef Private64::_mm128op<uint64_t, std::less> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm128op<uint64_t, std::equal_to> :
                    private Private64::_mm128op<uint64_t, std::equal_to> {
                typedef Private64::_mm128op<uint64_t, std::equal_to> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm128op<uint64_t, std::not_equal_to> :
                    private Private64::_mm128op<uint64_t, std::not_equal_to> {
                typedef Private64::_mm128op<uint64_t, std::not_equal_to> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm128op<uint64_t, coding_benchmark::and_is> :
                    private Private64::_mm128op<uint64_t, coding_benchmark::and_is> {
                typedef Private64::_mm128op<uint64_t, coding_benchmark::and_is> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm128op<uint64_t, coding_benchmark::or_is> :
                    private Private64::_mm128op<uint64_t, coding_benchmark::or_is> {
                typedef Private64::_mm128op<uint64_t, coding_benchmark::or_is> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm128op<uint64_t, coding_benchmark::add> :
                    private Private64::_mm128op<uint64_t, coding_benchmark::add> {
                typedef Private64::_mm128op<uint64_t, coding_benchmark::add> BASE;
                using BASE::add;
                using BASE::compute;
            };

            template<>
            struct mm128op<uint64_t, coding_benchmark::sub> :
                    private Private64::_mm128op<uint64_t, coding_benchmark::sub> {
                typedef Private64::_mm128op<uint64_t, coding_benchmark::sub> BASE;
                using BASE::sub;
                using BASE::compute;
            };

            template<>
            struct mm128op<uint64_t, coding_benchmark::mul> :
                    private Private64::_mm128op<uint64_t, coding_benchmark::mul> {
                typedef Private64::_mm128op<uint64_t, coding_benchmark::mul> BASE;
                using BASE::mullo;
                using BASE::compute;
            };

            template<>
            struct mm128op<uint64_t, coding_benchmark::div> :
                    private Private64::_mm128op<uint64_t, coding_benchmark::div> {
                typedef Private64::_mm128op<uint64_t, coding_benchmark::div> BASE;
                using BASE::div;
                using BASE::compute;
            };

            template<>
            struct mm128<uint64_t, uint16_t, uint64_t> {

                static inline __m128i mul_add(
                        __m128i * a,
                        __m128i * b,
                        size_t & incA,
                        size_t & incB) {
                    return mm128<uint16_t, uint64_t, uint64_t>::mul_add(b, a, incB, incA);
                }

                template<size_t firstA, size_t firstB>
                static inline __m128i mullo(
                        __m128i & a,
                        __m128i & b) {
                    return mm128<uint16_t, uint64_t, uint64_t>::mullo<firstB, firstA>(b, a);
                }
            };

            template<>
            struct mm128<uint64_t, uint32_t, uint64_t> {
                template<size_t firstA, size_t firstB>
                static inline __m128i mullo(
                        __m128i & a,
                        __m128i & b) {
                    return mm128<uint32_t, uint64_t, uint64_t>::mullo<firstB, firstA>(b, a);
                }
            };

            template<>
            struct mm128<uint64_t, uint64_t, uint64_t> {

                static inline __m128i mul_add(
                        __m128i * a,
                        __m128i * b,
                        size_t & incA,
                        size_t & incB) {
                    incA = incB = 1;
                    return _mm_mullo_epi64(_mm_lddqu_si128(a), _mm_lddqu_si128(b));
                }
            };

            template<>
            struct mm128<uint64_t, uint32_t> {

                static inline __m128i convert(
                        __m128i & mm) {
                    return _mm_shuffle_epi8(mm, _mm_set_epi64x(0xFFFFFFFFFFFFFFFF, 0x0B0A090803020100));
                }
            };

            template<>
            struct mm128<uint64_t, uint64_t> {

                static inline __m128i convert(
                        __m128i & mm) {
                    return mm;
                }
            };

        }
    }
}
