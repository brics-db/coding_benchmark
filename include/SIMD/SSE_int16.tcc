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

            namespace Private16 {

                template<typename T, size_t current = 1>
                struct min_max_helper {
                    static inline void min(
                            T & result,
                            __m128i a) {
                        T x = static_cast<T>(_mm_extract_epi16(a, current));
                        if (x < result) {
                            result = x;
                        }
                        min_max_helper<T, current + 1>::min(result, a);
                    }

                    static inline void max(
                            T & result,
                            __m128i a) {
                        T x = static_cast<T>(_mm_extract_epi16(a, current));
                        if (x > result) {
                            result = x;
                        }
                        min_max_helper<T, current + 1>::max(result, a);
                    }
                };

                template<typename T>
                struct min_max_helper<T, 7> {
                    static inline void min(
                            T & result,
                            __m128i a) {
                        T x = static_cast<T>(_mm_extract_epi16(a, 7));
                        if (x < result) {
                            result = x;
                        }
                    }

                    static inline void max(
                            T & result,
                            __m128i a) {
                        T x = static_cast<T>(_mm_extract_epi16(a, 7));
                        if (x > result) {
                            result = x;
                        }
                    }
                };

                template<typename T>
                inline T get_min_int16(
                        __m128i & a) {
                    T result = static_cast<T>(_mm_extract_epi16(a, 0));
                    min_max_helper<T>::min(result, a);
                    return result;
                }

                template<>
                inline int16_t get_min_int16<int16_t>(
                        __m128i & a) {
                    auto mmMin = _mm_set1_epi16(std::numeric_limits < int16_t > ::min());
                    auto min = _mm_extract_epi16(_mm_minpos_epu16(_mm_add_epi16(a, mmMin)), 0);
                    return min - std::numeric_limits < int16_t > ::min();
                }

                template<>
                inline uint16_t get_min_int16<uint16_t>(
                        __m128i & a) {
                    return _mm_extract_epi16(_mm_minpos_epu16(a), 0);
                }

                template<typename T>
                inline T get_max_int16(
                        __m128i & a) {
                    T result = static_cast<T>(_mm_extract_epi16(a, 0));
                    min_max_helper<T>::max(result, a);
                    return result;
                }

                template<size_t current = 0>
                inline void pack_right2_int16(
                        int16_t * & result,
                        __m128i & a,
                        uint8_t mask) {
                    *result = reinterpret_cast<int16_t*>(&a)[current];
                    result += (mask >> current) & 0x1;
                    pack_right2_int16<current + 1>(result, a, mask);
                }

                template<>
                inline void pack_right2_int16<7>(
                        int16_t * & result,
                        __m128i & a,
                        uint8_t mask) {
                    *result = reinterpret_cast<int16_t*>(&a)[7];
                    result += (mask >> 7) & 0x1;
                }

                template<size_t current = 0>
                inline void pack_right2_int16(
                        uint16_t * & result,
                        __m128i & a,
                        uint8_t mask) {
                    *result = reinterpret_cast<uint16_t*>(&a)[current];
                    result += (mask >> current) & 0x1;
                    pack_right2_int16<current + 1>(result, a, mask);
                }

                template<>
                inline void pack_right2_int16<7>(
                        uint16_t * & result,
                        __m128i & a,
                        uint8_t mask) {
                    *result = reinterpret_cast<uint16_t*>(&a)[7];
                    result += (mask >> 7) & 0x1;
                }

                template<typename T>
                struct _mm128 {

                    typedef uint8_t mask_t;
                    typedef uint64_t popcnt_t;

                    static const constexpr mask_t FULL_MASK = 0xFFu;

                    static inline __m128i set1(
                            T value) {
                        return _mm_set1_epi16(value);
                    }

                    static inline __m128i set(
                            T v7,
                            T v6,
                            T v5,
                            T v4,
                            T v3,
                            T v2,
                            T v1,
                            T v0) {
                        return _mm_set_epi16(v7, v6, v5, v4, v3, v2, v1, v0);
                    }

                    static inline __m128i set_inc(
                            T v0) {
                        return _mm_set_epi16(v0 + 7, v0 + 6, v0 + 5, v0 + 4, v0 + 3, v0 + 2, v0 + 1, v0);
                    }

                    static inline __m128i set_inc(
                            T v0,
                            T inc) {
                        return _mm_set_epi16(v0 + 7 * inc, v0 + 6 * inc, v0 + 5 * inc, v0 + 4 * inc, v0 + 3 * inc, v0 + 2 * inc, v0 + inc, v0);
                    }

                    static inline T min(
                            __m128i a) {
                        return get_min_int16<T>(a);
                    }

                    static inline __m128i min(
                            __m128i a,
                            __m128i b) {
                    if constexpr (std::is_signed_v<T>) {
                        return _mm_min_epi16(a, b);
                    } else {
                        return _mm_min_epu16(a, b);
                    }
                }

                static inline T max(
                        __m128i a) {
                    return get_max_int16<T>(a);
                }

                static inline __m128i max(
                        __m128i a,
                        __m128i b) {
                if constexpr (std::is_signed_v<T>) {
                    return _mm_max_epi16(a, b);
                } else {
                    return _mm_max_epu16(a, b);
                }
            }

            static inline T sum(
                    __m128i a) {
                auto mm = _mm_add_epi16(a, _mm_srli_si128(a, 8));
                mm = _mm_add_epi16(mm, _mm_srli_si128(mm, 4));
                mm = _mm_add_epi16(mm, _mm_srli_si128(mm, 2));
                return static_cast<T>(_mm_extract_epi16(mm, 0));
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
                pack_right2_int16(result, a, mask);
            }

            static inline popcnt_t popcount(
                    __m128i a) {
                auto mask = _mm_set1_epi16(0x0101);
                auto shuffle = _mm_set_epi64x(0xFFFFFFFFFFFFFFFF, 0x0F0D0B0907050301);
                auto popcount8 = mm128 < uint8_t > ::popcount(a);
                return _mm_extract_epi64(_mm_shuffle_epi8(_mm_mullo_epi16(popcount8, mask), shuffle), 0);
            }

            static inline popcnt_t popcount2(
                    __m128i a) {
                auto mask = _mm_set1_epi16(0x0101);
                auto shuffle = _mm_set_epi64x(0xFFFFFFFFFFFFFFFF, 0x0F0D0B0907050301);
                auto popcount8 = mm128 < uint8_t > ::popcount2(a);
                return _mm_extract_epi64(_mm_shuffle_epi8(_mm_mullo_epi16(popcount8, mask), shuffle), 0);
            }

            static inline popcnt_t popcount3(
                    __m128i a) {
                return (static_cast<uint64_t>(_mm_popcnt_u32(_mm_extract_epi16(a, 7))) << 56) | (static_cast<uint64_t>(_mm_popcnt_u32(_mm_extract_epi16(a, 6))) << 48)
                        | (static_cast<uint64_t>(_mm_popcnt_u32(_mm_extract_epi16(a, 5))) << 40) | (static_cast<uint64_t>(_mm_popcnt_u32(_mm_extract_epi16(a, 4))) << 23)
                        | (static_cast<uint64_t>(_mm_popcnt_u32(_mm_extract_epi16(a, 3))) << 24) | (static_cast<uint64_t>(_mm_popcnt_u32(_mm_extract_epi16(a, 2))) << 16)
                        | (static_cast<uint64_t>(_mm_popcnt_u32(_mm_extract_epi16(a, 1))) << 8) | static_cast<uint64_t>(_mm_popcnt_u32(_mm_extract_epi16(a, 0)));
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

            inline uint8_t compact_mask(
                    __m128i mask) {
                return static_cast<uint8_t>(_mm_movemask_epi8(_mm_shuffle_epi8(mask, _mm_set_epi64x(0xFFFFFFFFFFFFFFFFull, 0x0E0C0A0806040200ull))));
            }

            template<typename T>
            struct _mm128op<T, std::greater> {

                typedef typename _mm128<T>::mask_t mask_t;

                static inline __m128i cmp(
                        __m128i a,
                        __m128i b) {
                    return _mm_cmpgt_epi16(a, b);
                }

                static inline mask_t cmp_mask(
                        __m128i a,
                        __m128i b) {
                    return compact_mask(cmp(a, b));
                }
            };

            template<typename T>
            struct _mm128op<T, std::greater_equal> {

                typedef typename _mm128<T>::mask_t mask_t;

                static inline __m128i cmp(
                        __m128i a,
                        __m128i b) {
                    auto mm = sse::mm128 < T > ::max(a, b);
                    return _mm_cmpeq_epi16(a, mm);
                }

                static inline mask_t cmp_mask(
                        __m128i a,
                        __m128i b) {
                    return compact_mask(cmp(a, b));
                }
            };

            template<typename T>
            struct _mm128op<T, std::less> {

                typedef typename _mm128<T>::mask_t mask_t;

                static inline __m128i cmp(
                        __m128i a,
                        __m128i b) {
                    return _mm_cmplt_epi16(a, b);
                }

                static inline mask_t cmp_mask(
                        __m128i a,
                        __m128i b) {
                    return compact_mask(cmp(a, b));
                }
            };

            template<typename T>
            struct _mm128op<T, std::less_equal> {

                typedef typename _mm128<T>::mask_t mask_t;

                static inline __m128i cmp(
                        __m128i a,
                        __m128i b) {
                    auto mm = sse::mm128 < T > ::min(a, b);
                    return _mm_cmpeq_epi16(a, mm);
                }

                static inline mask_t cmp_mask(
                        __m128i a,
                        __m128i b) {
                    return compact_mask(cmp(a, b));
                }
            };

            template<typename T>
            struct _mm128op<T, std::equal_to> {

                typedef typename _mm128<T>::mask_t mask_t;

                static inline __m128i cmp(
                        __m128i a,
                        __m128i b) {
                    return _mm_cmpeq_epi16(a, b);
                }

                static inline mask_t cmp_mask(
                        __m128i a,
                        __m128i b) {
                    return compact_mask(cmp(a, b));
                }
            };

            template<typename T>
            struct _mm128op<T, std::not_equal_to> {

                typedef typename _mm128<T>::mask_t mask_t;

                static inline __m128i cmp(
                        __m128i a,
                        __m128i b) {
                    return _mm_or_si128(_mm_cmplt_epi16(a, b), _mm_cmpgt_epi16(a, b));
                }

                static inline mask_t cmp_mask(
                        __m128i a,
                        __m128i b) {
                    return compact_mask(cmp(a, b));
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
                    return compact_mask(cmp(a, b));
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
                    return compact_mask(cmp(a, b));
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
                    return compact_mask(cmp(a, b));
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
                        __m128i a,
                        __m128i b) {
                    return compact_mask(cmp(a));
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
                    return _mm_add_epi16(a, b);
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
                    return _mm_sub_epi16(a, b);
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
                    return _mm_mullo_epi16(a, b);
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
                    // _mm_div_ps does ROUNDING!
                    auto mmA = _mm_min_epi16(a, _mm_sub_epi16(a, _mm_srai_epi16(b, 1))); // repair the rounding and make sure we dont underflow
                    auto mm0 = _mm_div_ps(_mm_cvtepi32_ps(_mm_cvtepi16_epi32(mmA)), _mm_cvtepi32_ps(_mm_cvtepi16_epi32(b)));
                    auto mm1 = _mm_div_ps(_mm_cvtepi32_ps(_mm_cvtepi16_epi32(_mm_srli_si128(mmA, 8))), _mm_cvtepi32_ps(_mm_cvtepi16_epi32(_mm_srli_si128(b, 8))));
                    auto mx0 = _mm_cvtps_pi16(mm0);
                    auto mx1 = _mm_cvtps_pi16(mm1);
                    return _mm_set_epi64(mx1, mx0);
                }
            };

        } /* Private16 */

        template<>
        struct mm128<int16_t> :
                public Private16::_mm128<int16_t> {
            typedef Private16::_mm128<int16_t> BASE;
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
            using BASE::cvt_larger_hi;
            using BASE::cvt_larger_lo;
        };

        template<>
        struct mm128op<int16_t, std::greater_equal> :
                private Private16::_mm128op<int16_t, std::greater_equal> {
            typedef Private16::_mm128op<int16_t, std::greater_equal> BASE;
            using BASE::mask_t;
            using BASE::cmp;
            using BASE::cmp_mask;
        };

        template<>
        struct mm128op<int16_t, std::greater> :
                private Private16::_mm128op<int16_t, std::greater> {
            typedef Private16::_mm128op<int16_t, std::greater> BASE;
            using BASE::mask_t;
            using BASE::cmp;
            using BASE::cmp_mask;
        };

        template<>
        struct mm128op<int16_t, std::less_equal> :
                private Private16::_mm128op<int16_t, std::less_equal> {
            typedef Private16::_mm128op<int16_t, std::less_equal> BASE;
            using BASE::mask_t;
            using BASE::cmp;
            using BASE::cmp_mask;
        };

        template<>
        struct mm128op<int16_t, std::less> :
                private Private16::_mm128op<int16_t, std::less> {
            typedef Private16::_mm128op<int16_t, std::less> BASE;
            using BASE::mask_t;
            using BASE::cmp;
            using BASE::cmp_mask;
        };

        template<>
        struct mm128op<int16_t, std::equal_to> :
                private Private16::_mm128op<int16_t, std::equal_to> {
            typedef Private16::_mm128op<int16_t, std::equal_to> BASE;
            using BASE::mask_t;
            using BASE::cmp;
            using BASE::cmp_mask;
        };

        template<>
        struct mm128op<int16_t, std::not_equal_to> :
                private Private16::_mm128op<int16_t, std::not_equal_to> {
            typedef Private16::_mm128op<int16_t, std::not_equal_to> BASE;
            using BASE::mask_t;
            using BASE::cmp;
            using BASE::cmp_mask;
        };

        template<>
        struct mm128op<int16_t, coding_benchmark::and_is> :
                private Private16::_mm128op<int16_t, coding_benchmark::and_is> {
            typedef Private16::_mm128op<int16_t, coding_benchmark::and_is> BASE;
            using BASE::mask_t;
            using BASE::cmp;
            using BASE::cmp_mask;
        };

        template<>
        struct mm128op<int16_t, coding_benchmark::or_is> :
                private Private16::_mm128op<int16_t, coding_benchmark::or_is> {
            typedef Private16::_mm128op<int16_t, coding_benchmark::or_is> BASE;
            using BASE::mask_t;
            using BASE::cmp;
            using BASE::cmp_mask;
        };

        template<>
        struct mm128op<int16_t, coding_benchmark::xor_is> :
                private Private16::_mm128op<int16_t, coding_benchmark::xor_is> {
            typedef Private16::_mm128op<int16_t, coding_benchmark::xor_is> BASE;
            using BASE::mask_t;
            using BASE::cmp;
            using BASE::cmp_mask;
        };

        template<>
        struct mm128op<int16_t, coding_benchmark::is_not> :
                private Private16::_mm128op<int16_t, coding_benchmark::is_not> {
            typedef Private16::_mm128op<int16_t, coding_benchmark::is_not> BASE;
            using BASE::mask_t;
            using BASE::cmp;
            using BASE::cmp_mask;
        };

        template<>
        struct mm128op<int16_t, coding_benchmark::add> :
                private Private16::_mm128op<int16_t, coding_benchmark::add> {
            typedef Private16::_mm128op<int16_t, coding_benchmark::add> BASE;
            using BASE::add;
            using BASE::compute;
        };

        template<>
        struct mm128op<int16_t, coding_benchmark::sub> :
                private Private16::_mm128op<int16_t, coding_benchmark::sub> {
            typedef Private16::_mm128op<int16_t, coding_benchmark::sub> BASE;
            using BASE::sub;
            using BASE::compute;
        };

        template<>
        struct mm128op<int16_t, coding_benchmark::mul> :
                private Private16::_mm128op<int16_t, coding_benchmark::mul> {
            typedef Private16::_mm128op<int16_t, coding_benchmark::mul> BASE;
            using BASE::mullo;
            using BASE::compute;
        };

        template<>
        struct mm128op<int16_t, coding_benchmark::div> :
                private Private16::_mm128op<int16_t, coding_benchmark::div> {
            typedef Private16::_mm128op<int16_t, coding_benchmark::div> BASE;
            using BASE::div;
            using BASE::compute;
        };

        template<>
        struct mm128<uint16_t> :
                public Private16::_mm128<uint16_t> {
            typedef Private16::_mm128<uint16_t> BASE;
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
            using BASE::cvt_larger_hi;
            using BASE::cvt_larger_lo;
        };

        template<>
        struct mm128op<uint16_t, std::greater_equal> :
                private Private16::_mm128op<uint16_t, std::greater_equal> {
            typedef Private16::_mm128op<uint16_t, std::greater_equal> BASE;
            using BASE::mask_t;
            using BASE::cmp;
            using BASE::cmp_mask;
        };

        template<>
        struct mm128op<uint16_t, std::greater> :
                private Private16::_mm128op<uint16_t, std::greater> {
            typedef Private16::_mm128op<uint16_t, std::greater> BASE;
            using BASE::mask_t;
            using BASE::cmp;
            using BASE::cmp_mask;
        };

        template<>
        struct mm128op<uint16_t, std::less_equal> :
                private Private16::_mm128op<uint16_t, std::less_equal> {
            typedef Private16::_mm128op<uint16_t, std::less_equal> BASE;
            using BASE::mask_t;
            using BASE::cmp;
            using BASE::cmp_mask;
        };

        template<>
        struct mm128op<uint16_t, std::less> :
                private Private16::_mm128op<uint16_t, std::less> {
            typedef Private16::_mm128op<uint16_t, std::less> BASE;
            using BASE::mask_t;
            using BASE::cmp;
            using BASE::cmp_mask;
        };

        template<>
        struct mm128op<uint16_t, std::equal_to> :
                private Private16::_mm128op<uint16_t, std::equal_to> {
            typedef Private16::_mm128op<uint16_t, std::equal_to> BASE;
            using BASE::mask_t;
            using BASE::cmp;
            using BASE::cmp_mask;
        };

        template<>
        struct mm128op<uint16_t, std::not_equal_to> :
                private Private16::_mm128op<uint16_t, std::not_equal_to> {
            typedef Private16::_mm128op<uint16_t, std::not_equal_to> BASE;
            using BASE::mask_t;
            using BASE::cmp;
            using BASE::cmp_mask;
        };

        template<>
        struct mm128op<uint16_t, coding_benchmark::and_is> :
                private Private16::_mm128op<uint16_t, coding_benchmark::and_is> {
            typedef Private16::_mm128op<uint16_t, coding_benchmark::and_is> BASE;
            using BASE::mask_t;
            using BASE::cmp;
            using BASE::cmp_mask;
        };

        template<>
        struct mm128op<uint16_t, coding_benchmark::or_is> :
                private Private16::_mm128op<uint16_t, coding_benchmark::or_is> {
            typedef Private16::_mm128op<uint16_t, coding_benchmark::or_is> BASE;
            using BASE::mask_t;
            using BASE::cmp;
            using BASE::cmp_mask;
        };

        template<>
        struct mm128op<uint16_t, coding_benchmark::xor_is> :
                private Private16::_mm128op<uint16_t, coding_benchmark::xor_is> {
            typedef Private16::_mm128op<uint16_t, coding_benchmark::xor_is> BASE;
            using BASE::mask_t;
            using BASE::cmp;
            using BASE::cmp_mask;
        };

        template<>
        struct mm128op<uint16_t, coding_benchmark::is_not> :
                private Private16::_mm128op<uint16_t, coding_benchmark::is_not> {
            typedef Private16::_mm128op<uint16_t, coding_benchmark::is_not> BASE;
            using BASE::mask_t;
            using BASE::cmp;
            using BASE::cmp_mask;
        };

        template<>
        struct mm128op<uint16_t, coding_benchmark::add> :
                private Private16::_mm128op<uint16_t, coding_benchmark::add> {
            typedef Private16::_mm128op<uint16_t, coding_benchmark::add> BASE;
            using BASE::add;
            using BASE::compute;
        };

        template<>
        struct mm128op<uint16_t, coding_benchmark::sub> :
                private Private16::_mm128op<uint16_t, coding_benchmark::sub> {
            typedef Private16::_mm128op<uint16_t, coding_benchmark::sub> BASE;
            using BASE::sub;
            using BASE::compute;
        };

        template<>
        struct mm128op<uint16_t, coding_benchmark::mul> :
                private Private16::_mm128op<uint16_t, coding_benchmark::mul> {
            typedef Private16::_mm128op<uint16_t, coding_benchmark::mul> BASE;
            using BASE::mullo;
            using BASE::compute;
        };

        template<>
        struct mm128op<uint16_t, coding_benchmark::div> :
                private Private16::_mm128op<uint16_t, coding_benchmark::div> {
            typedef Private16::_mm128op<uint16_t, coding_benchmark::div> BASE;
            using BASE::div;
            using BASE::compute;
        };

        template<>
        struct mm128<uint16_t, uint16_t, uint16_t> {

            static inline __m128i mul_add(
                    __m128i * a,
                    __m128i * b,
                    size_t & incA,
                    size_t & incB) {
                incA = incB = 1;
                return _mm_mullo_epi16(_mm_lddqu_si128(a), _mm_lddqu_si128(b));
            }
        };

        template<>
        struct mm128<uint16_t, uint16_t, uint32_t> {

            static inline __m128i mul_add(
                    __m128i * a,
                    __m128i * b,
                    size_t & incA,
                    size_t & incB) {
                incA = incB = 1;
                auto mm = _mm_mullo_epi32(_mm_cvtepi16_epi32(_mm_lddqu_si128(a)), _mm_cvtepi16_epi32(_mm_lddqu_si128(b)));
                mm = _mm_add_epi32(mm, _mm_mullo_epi32(_mm_cvtepi16_epi32(_mm_srli_si128(_mm_lddqu_si128(a), 8)), _mm_cvtepi16_epi32(_mm_srli_si128(_mm_lddqu_si128(b), 8))));
                return mm;
            }
        };

        template<>
        struct mm128<uint16_t, uint32_t, uint32_t> {

            static inline __m128i mul_add(
                    __m128i * a,
                    __m128i * b,
                    size_t & incA,
                    size_t & incB) {
                incA = 1;
                incB = 2;
                auto mm = _mm_mullo_epi32(_mm_cvtepi16_epi32(_mm_lddqu_si128(a)), _mm_lddqu_si128(b + 1));
                mm = _mm_add_epi32(mm, _mm_mullo_epi32(_mm_cvtepi16_epi32(_mm_srli_si128(_mm_lddqu_si128(a), 8)), _mm_lddqu_si128(b)));
                return mm;
            }
        };

        template<>
        struct mm128<uint16_t, uint32_t, uint64_t> {

            static inline __m128i mul_add(
                    __m128i * a,
                    __m128i * b,
                    size_t & incA,
                    size_t & incB) {
                incA = 1;
                incB = 2;
                auto mm = _mm_mul_epi32(_mm_cvtepi16_epi32(_mm_lddqu_si128(a)), _mm_lddqu_si128(b + 1));
                mm = _mm_add_epi64(mm, _mm_mul_epi32(_mm_cvtepi16_epi32(_mm_srli_si128(_mm_lddqu_si128(a), 8)), _mm_lddqu_si128(b)));
                return mm;
            }
        };

        template<>
        struct mm128<uint16_t, uint64_t, uint64_t> {

            static inline __m128i mul_add(
                    __m128i * a,
                    __m128i * b,
                    size_t & incA,
                    size_t & incB) {
                incA = 1;
                incB = 4;

                auto mm = _mm_mullo_epi64(_mm_cvtepu16_epi64(_mm_lddqu_si128(a)), _mm_lddqu_si128(b));
                mm = _mm_add_epi64(mm, _mm_mullo_epi64(_mm_cvtepu16_epi64(_mm_srli_si128(_mm_lddqu_si128(a), 4)), _mm_lddqu_si128(b + 1)));
                mm = _mm_add_epi64(mm, _mm_mullo_epi64(_mm_cvtepu16_epi64(_mm_srli_si128(_mm_lddqu_si128(a), 8)), _mm_lddqu_si128(b + 2)));
                mm = _mm_add_epi64(mm, _mm_mullo_epi64(_mm_cvtepu16_epi64(_mm_srli_si128(_mm_lddqu_si128(a), 12)), _mm_lddqu_si128(b + 3)));

                return mm;
            }

            template<size_t firstA, size_t firstB>
            static inline __m128i mullo(
                    __m128i & a,
                    __m128i & b) {
                auto r0 = static_cast<uint64_t>(_mm_extract_epi16(a, firstA)) * _mm_extract_epi64(b, firstB);
                auto r1 = static_cast<uint64_t>(_mm_extract_epi16(a, firstA + 1)) * _mm_extract_epi64(b, firstB + 1);
                return _mm_set_epi64x(r1, r0);
            }
        };

        template<>
        struct mm128<uint16_t, uint8_t> {

            static inline __m128i convert(
                    __m128i & mm) {
                return _mm_shuffle_epi8(mm, _mm_set_epi64x(0xFFFFFFFFFFFFFFFF, 0x0E0C0A0806040200));
            }
        };

        template<>
        struct mm128<uint16_t, uint16_t> {

            static inline __m128i convert(
                    __m128i & mm) {
                return mm;
            }
        };

    } /* sse */
}
}
