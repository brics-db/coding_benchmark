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
                inline void pack_right2_uint16(
                        uint16_t * & result,
                        __m128i & a,
                        uint8_t mask) {
                    *result = reinterpret_cast<uint16_t*>(&a)[current];
                    result += (mask >> current) & 0x1;
                    pack_right2_uint16<current + 1>(result, a, mask);
                }

                template<>
                inline void pack_right2_uint16<7>(
                        uint16_t * & result,
                        __m128i & a,
                        uint8_t mask) {
                    *result = reinterpret_cast<uint16_t*>(&a)[7];
                    result += (mask >> 7) & 0x1;
                }

            }

            template<>
            struct mm128<uint16_t> {

                typedef uint8_t mask_t;
                typedef uint64_t popcnt_t;

                static inline __m128i set1(
                        uint16_t value) {
                    return _mm_set1_epi16(value);
                }

                static inline __m128i set(
                        uint16_t v7,
                        uint16_t v6,
                        uint16_t v5,
                        uint16_t v4,
                        uint16_t v3,
                        uint16_t v2,
                        uint16_t v1,
                        uint16_t v0) {
                    return _mm_set_epi16(v7, v6, v5, v4, v3, v2, v1, v0);
                }

                static inline __m128i set_inc(
                        uint16_t v0) {
                    return _mm_set_epi16(v0 + 7, v0 + 6, v0 + 5, v0 + 4, v0 + 3, v0 + 2, v0 + 1, v0);
                }

                static inline __m128i set_inc(
                        uint16_t v0,
                        uint16_t inc) {
                    return _mm_set_epi16(v0 + 7 * inc, v0 + 6 * inc, v0 + 5 * inc, v0 + 4 * inc, v0 + 3 * inc, v0 + 2 * inc, v0 + inc, v0);
                }

                static inline __m128i min(
                        __m128i a,
                        __m128i b) {
                    return _mm_min_epu16(a, b);
                }

                static inline __m128i max(
                        __m128i a,
                        __m128i b) {
                    return _mm_max_epu16(a, b);
                }

                static inline __m128i add(
                        __m128i a,
                        __m128i b) {
                    return _mm_add_epi16(a, b);
                }

                static inline uint16_t sum(
                        __m128i a) {
                    auto mm = _mm_add_epi16(a, _mm_srli_si128(a, 8));
                    mm = _mm_add_epi16(mm, _mm_srli_si128(mm, 4));
                    mm = _mm_add_epi16(mm, _mm_srli_si128(mm, 2));
                    return static_cast<uint16_t>(_mm_extract_epi16(mm, 0));
                }

                static inline __m128i mullo(
                        __m128i a,
                        __m128i b) {
                    return _mm_mullo_epi16(a, b);
                }

                static inline __m128i pack_right(
                        __m128i a,
                        mask_t mask) {
                    return _mm_shuffle_epi8(a, SHUFFLE_TABLE[mask]);
                }

                static inline void pack_right2(
                        uint16_t * & result,
                        __m128i a,
                        mask_t mask) {
                    Private::pack_right2_uint16(result, a, mask);
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

        namespace Private {

            inline uint8_t v2_mm128_compact_mask_uint16_t(
                    __m128i mask) {
                return static_cast<uint8_t>(_mm_movemask_epi8(_mm_shuffle_epi8(mask, _mm_set_epi64x(0xFFFFFFFFFFFFFFFFull, 0x0E0C0A0806040200ull))));
            }

        }

        template<>
        struct mm_op<__m128i, uint16_t, std::greater> {

            typedef uint8_t mask_t;

            static inline __m128i cmp(
                    __m128i a,
                    __m128i b) {
                return _mm_cmpgt_epi16(a, b);
            }

            static inline mask_t cmp_mask(
                    __m128i a,
                    __m128i b) {
                return Private::v2_mm128_compact_mask_uint16_t(cmp(a, b));
            }
        };

        template<>
        struct mm_op<__m128i, uint16_t, std::greater_equal> {

            typedef uint8_t mask_t;

            static inline __m128i cmp(
                    __m128i a,
                    __m128i b) {
                auto mm = sse::mm128<uint16_t>::max(a, b);
                return _mm_cmpeq_epi16(a, mm);
            }

            static inline mask_t cmp_mask(
                    __m128i a,
                    __m128i b) {
                return Private::v2_mm128_compact_mask_uint16_t(cmp(a, b));
            }
        };

        template<>
        struct mm_op<__m128i, uint16_t, std::less> {

            typedef uint8_t mask_t;

            static inline __m128i cmp(
                    __m128i a,
                    __m128i b) {
                return _mm_cmplt_epi16(a, b);
            }

            static inline mask_t cmp_mask(
                    __m128i a,
                    __m128i b) {
                return Private::v2_mm128_compact_mask_uint16_t(cmp(a, b));
            }
        };

        template<>
        struct mm_op<__m128i, uint16_t, std::less_equal> {

            typedef uint8_t mask_t;

            static inline __m128i cmp(
                    __m128i a,
                    __m128i b) {
                auto mm = sse::mm128<uint16_t>::min(a, b);
                return _mm_cmpeq_epi16(a, mm);
            }

            static inline mask_t cmp_mask(
                    __m128i a,
                    __m128i b) {
                return Private::v2_mm128_compact_mask_uint16_t(cmp(a, b));
            }
        };

        template<>
        struct mm_op<__m128i, uint16_t, std::equal_to> {

            typedef uint8_t mask_t;

            static inline __m128i cmp(
                    __m128i a,
                    __m128i b) {
                return _mm_cmpeq_epi16(a, b);
            }

            static inline mask_t cmp_mask(
                    __m128i a,
                    __m128i b) {
                return Private::v2_mm128_compact_mask_uint16_t(cmp(a, b));
            }
        };

        template<>
        struct mm_op<__m128i, uint16_t, std::not_equal_to> {

            typedef uint8_t mask_t;

            static inline __m128i cmp(
                    __m128i a,
                    __m128i b) {
                return _mm_or_si128(_mm_cmplt_epi16(a, b), _mm_cmpgt_epi16(a, b));
            }

            static inline mask_t cmp_mask(
                    __m128i a,
                    __m128i b) {
                return Private::v2_mm128_compact_mask_uint16_t(cmp(a, b));
            }
        };

        template<>
        struct mm_op<__m128i, uint16_t, coding_benchmark::and_is> {

            typedef uint8_t mask_t;

            static inline __m128i cmp(
                    __m128i a,
                    __m128i b) {
                return _mm_and_si128(a, b);
            }

            static inline mask_t cmp_mask(
                    __m128i a,
                    __m128i b) {
                return Private::v2_mm128_compact_mask_uint16_t(cmp(a, b));
            }
        };

        template<>
        struct mm_op<__m128i, uint16_t, coding_benchmark::or_is> {

            typedef uint8_t mask_t;

            static inline __m128i cmp(
                    __m128i a,
                    __m128i b) {
                return _mm_or_si128(a, b);
            }

            static inline mask_t cmp_mask(
                    __m128i a,
                    __m128i b) {
                return Private::v2_mm128_compact_mask_uint16_t(cmp(a, b));
            }
        };

        template<>
        struct mm_op<__m128i, uint16_t, coding_benchmark::add> {

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

        template<>
        struct mm_op<__m128i, uint16_t, coding_benchmark::sub> {

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

        template<>
        struct mm_op<__m128i, uint16_t, coding_benchmark::mul> {

            static inline __m128i compute(
                    __m128i a,
                    __m128i b) {
                return mullo(a, b);
            }

            static inline __m128i mullo(
                    __m128i a,
                    __m128i b) {
                return sse::mm128<uint16_t>::mullo(a, b);
            }
        };

        template<>
        struct mm_op<__m128i, uint16_t, coding_benchmark::div> {

            static inline __m128i compute(
                    __m128i a,
                    __m128i b) {
                return div(a, b);
            }

            static inline __m128i div(
                    __m128i a,
                    __m128i b) {
                auto mm1 = _mm_div_ps(_mm_cvtepi32_ps(_mm_cvtepi16_epi32(a)), _mm_cvtepi32_ps(_mm_cvtepi16_epi32(b)));
                auto mm2 = _mm_div_ps(_mm_cvtepi32_ps(_mm_cvtepi16_epi32(_mm_srli_si128(a, 8))), _mm_cvtepi32_ps(_mm_cvtepi16_epi32(_mm_srli_si128(b, 8))));
                auto mx1 = _mm_cvtepi32_epi8(_mm_cvtps_epi32(mm1));
                auto mx2 = _mm_shuffle_epi8(_mm_cvtepi32_epi8(_mm_cvtps_epi32(mm2)), _mm_set_epi64x(0x0D0C090805040100, 0xFFFFFFFFFFFFFFFF));
                return _mm_or_si128(mx1, mx2);
            }
        };

    }
}
