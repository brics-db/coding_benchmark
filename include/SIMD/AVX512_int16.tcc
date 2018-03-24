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
 * AVX512_uint16.tcc
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

            namespace Private16 {

                template<size_t current = 0>
                inline void pack_right2_int16(
                        uint16_t * & result,
                        __m512i & a,
                        uint32_t mask) {
                    *result = reinterpret_cast<uint16_t*>(&a)[current];
                    result += (mask >> current) & 0x1;
                    pack_right2_int16<current + 1>(result, a, mask);
                }

                template<>
                inline void pack_right2_int16<31>(
                        uint16_t * & result,
                        __m512i & a,
                        uint32_t mask) {
                    *result = reinterpret_cast<uint16_t*>(&a)[31];
                    result += (mask >> 31) & 0x1;
                }

                template<size_t current = 0>
                inline void pack_right2_int16(
                        int16_t * & result,
                        __m512i & a,
                        uint32_t mask) {
                    *result = reinterpret_cast<int16_t*>(&a)[current];
                    result += (mask >> current) & 0x1;
                    pack_right2_int16<current + 1>(result, a, mask);
                }

                template<>
                inline void pack_right2_int16<31>(
                        int16_t * & result,
                        __m512i & a,
                        uint32_t mask) {
                    *result = reinterpret_cast<int16_t*>(&a)[31];
                    result += (mask >> 31) & 0x1;
                }

                template<typename T>
                struct _mm512 {

                    typedef uint32_t mask_t;
                    typedef __m256i popcnt_t;

                    static const constexpr mask_t FULL_MASK = 0xFFFFFFFFu;

                    static inline __m512i set1(
                            T value) {
                        return _mm512_set1_epi16(value);
                    }

                    static inline __m512i set(
                            T v31,
                            T v30,
                            T v29,
                            T v28,
                            T v27,
                            T v26,
                            T v25,
                            T v24,
                            T v23,
                            T v22,
                            T v21,
                            T v20,
                            T v19,
                            T v18,
                            T v17,
                            T v16,
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
#ifdef __AVX512BW__
                        return _mm512_set_epi16(v31, v30, v29, v28, v27, v26, v25, v24, v23, v22, v21, v20, v19, v18, v17, v16, v15, v14, v13, v12, v11, v10, v9, v8, v7, v6, v5, v4, v3, v2, v1,
                                v0);
#else
                        return _mm512_set_epi32((static_cast<T>(v31) << 16) | static_cast<T>(v30), (static_cast<T>(v29) << 16) | static_cast<T>(v28), (static_cast<T>(v27) << 16) | static_cast<T>(v26),
                                (static_cast<T>(v25) << 16) | static_cast<T>(v24), (static_cast<T>(v23) << 16) | static_cast<T>(v22), (static_cast<T>(v21) << 16) | static_cast<T>(v20),
                                (static_cast<T>(v19) << 16) | static_cast<T>(v18), (static_cast<T>(v17) << 16) | static_cast<T>(v16), (static_cast<T>(v15) << 16) | static_cast<T>(v14),
                                (static_cast<T>(v13) << 16) | static_cast<T>(v12), (static_cast<T>(v11) << 16) | static_cast<T>(v10), (static_cast<T>(v9) << 16) | static_cast<T>(v8),
                                (static_cast<T>(v7) << 16) | static_cast<T>(v6), (static_cast<T>(v5) << 16) | static_cast<T>(v4), (static_cast<T>(v3) << 16) | static_cast<T>(v2),
                                (static_cast<T>(v1) << 16) | static_cast<T>(v0));
#endif
                    }

                    static inline __m512i set_inc(
                            uint16_t v0) {
#ifdef __AVX512BW__
                        return _mm512_set_epi16(v0 + 31, v0 + 30, v0 + 29, v0 + 28, v0 + 27, v0 + 26, v0 + 25, v0 + 24, v0 + 23, v0 + 22, v0 + 21, v0 + 20, v0 + 19, v0 + 18, v0 + 17, v0 + 16,
                                v0 + 15, v0 + 14, v0 + 13, v0 + 12, v0 + 11, v0 + 10, v0 + 9, v0 + 8, v0 + 7, v0 + 6, v0 + 5, v0 + 4, v0 + 3, v0 + 2, v0 + 1, v0);
#else
                        return _mm512_set_epi32(((static_cast<T>(v0) << 16) + 31) | (static_cast<T>(v0) + 30), ((static_cast<T>(v0) << 16) + 29) | (static_cast<T>(v0) + 28),
                                ((static_cast<T>(v0) << 16) + 27) | (static_cast<T>(v0) + 26), ((static_cast<T>(v0) << 16) + 25) | (static_cast<T>(v0) + 24),
                                ((static_cast<T>(v0) << 16) + 23) | (static_cast<T>(v0) + 22), ((static_cast<T>(v0) << 16) + 21) | (static_cast<T>(v0) + 20),
                                ((static_cast<T>(v0) << 16) + 19) | (static_cast<T>(v0) + 18), ((static_cast<T>(v0) << 16) + 17) | (static_cast<T>(v0) + 16),
                                ((static_cast<T>(v0) << 16) + 15) | (static_cast<T>(v0) + 14), ((static_cast<T>(v0) << 16) + 13) | (static_cast<T>(v0) + 12),
                                ((static_cast<T>(v0) << 16) + 11) | (static_cast<T>(v0) + 10), ((static_cast<T>(v0) << 16) + 9) | (static_cast<T>(v0) + 8),
                                ((static_cast<T>(v0) << 16) + 7) | (static_cast<T>(v0) + 6), ((static_cast<T>(v0) << 16) + 5) | (static_cast<T>(v0) + 4),
                                ((static_cast<T>(v0) << 16) + 3) | (static_cast<T>(v0) + 2), ((static_cast<T>(v0) << 16) + 1) | (static_cast<T>(v0)));
#endif
                    }

                    static inline __m512i set_inc(
                            uint16_t v0,
                            uint16_t inc) {
#ifdef __AVX512BW__
                        return _mm512_set_epi16(v0 + 31 * inc, v0 + 30 * inc, v0 + 29 * inc, v0 + 28 * inc, v0 + 27 * inc, v0 + 26 * inc, v0 + 25 * inc, v0 + 24 * inc, v0 + 23 * inc,
                                v0 + 22 * inc, v0 + 21 * inc, v0 + 20 * inc, v0 + 19 * inc, v0 + 18 * inc, v0 + 17 * inc, v0 + 16 * inc, v0 + 15 * inc, v0 + 14 * inc, v0 + 13 * inc, v0 + 12 * inc,
                                v0 + 11 * inc, v0 + 10 * inc, v0 + 9 * inc, v0 + 8 * inc, v0 + 7 * inc, v0 + 6 * inc, v0 + 5 * inc, v0 + 4 * inc, v0 + 3 * inc, v0 + 2 * inc, v0 + inc, v0);
#else
                        return _mm512_set_epi32(((static_cast<T>(v0) << 16) + 31 * inc) | (static_cast<T>(v0) + 30 * inc), ((static_cast<T>(v0) << 16) + 29 * inc) | (static_cast<T>(v0) + 28 * inc),
                                ((static_cast<T>(v0) << 16) + 27 * inc) | (static_cast<T>(v0) + 26 * inc), ((static_cast<T>(v0) << 16) + 25 * inc) | (static_cast<T>(v0) + 24 * inc),
                                ((static_cast<T>(v0) << 16) + 23 * inc) | (static_cast<T>(v0) + 22 * inc), ((static_cast<T>(v0) << 16) + 21 * inc) | (static_cast<T>(v0) + 20 * inc),
                                ((static_cast<T>(v0) << 16) + 19 * inc) | (static_cast<T>(v0) + 18 * inc), ((static_cast<T>(v0) << 16) + 17 * inc) | (static_cast<T>(v0) + 16 * inc),
                                ((static_cast<T>(v0) << 16) + 15 * inc) | (static_cast<T>(v0) + 14 * inc), ((static_cast<T>(v0) << 16) + 13 * inc) | (static_cast<T>(v0) + 12 * inc),
                                ((static_cast<T>(v0) << 16) + 11 * inc) | (static_cast<T>(v0) + 10 * inc), ((static_cast<T>(v0) << 16) + 9 * inc) | (static_cast<T>(v0) + 8 * inc),
                                ((static_cast<T>(v0) << 16) + 7 * inc) | (static_cast<T>(v0) + 6 * inc), ((static_cast<T>(v0) << 16) + 5 * inc) | (static_cast<T>(v0) + 4 * inc),
                                ((static_cast<T>(v0) << 16) + 3 * inc) | (static_cast<T>(v0) + 2 * inc), ((static_cast<T>(v0) << 16) + 1 * inc) | (static_cast<T>(v0) + 0 * inc));
#endif
                    }

                    static inline __m512i min(
                            __m512i a,
                            __m512i b) {
#ifdef __AVX512BW__
                        if constexpr (std::is_signed_v<T>) {
                            return _mm512_min_epi16(a, b);
                        } else {
                            return _mm512_min_epu16(a, b);
                        }
#else
                        auto a0 = _mm512_cvtepi16_epi32(_mm512_extracti32x8_epi32(a, 0));
                        auto a1 = _mm512_cvtepi16_epi32(_mm512_extracti32x8_epi32(a, 1));
                        auto b0 = _mm512_cvtepi16_epi32(_mm512_extracti32x8_epi32(b, 0));
                        auto b1 = _mm512_cvtepi16_epi32(_mm512_extracti32x8_epi32(b, 1));
                        if constexpr (std::is_signed_v<T>) {
                            auto r0 = _mm512_min_epi32(a0, b0);
                            auto r1 = _mm512_min_epi32(a1, b1);
                            return _mm512_inserti32x8(_mm512_castsi256_si512(_mm512_cvtepi32_epi16(r0)), _mm512_cvtepi32_epi16(r1), 1);
                        } else {
                            auto r0 = _mm512_min_epu32(a0, b0);
                            auto r1 = _mm512_min_epu32(a1, b1);
                            return _mm512_inserti32x8(_mm512_castsi256_si512(_mm512_cvtepi32_epi16(r0)), _mm512_cvtepi32_epi16(r1), 1);
                        }
#endif
                    }

                    static inline __m512i max(
                            __m512i a,
                            __m512i b) {
#ifdef __AVX512BW__
                        if constexpr (std::is_signed_v<T>) {
                            return _mm512_max_epi16(a, b);
                        } else {
                            return _mm512_max_epu16(a, b);
                        }
#else
                        auto a0 = _mm512_cvtepi16_epi32(_mm512_extracti32x8_epi32(a, 0));
                        auto a1 = _mm512_cvtepi16_epi32(_mm512_extracti32x8_epi32(a, 1));
                        auto b0 = _mm512_cvtepi16_epi32(_mm512_extracti32x8_epi32(b, 0));
                        auto b1 = _mm512_cvtepi16_epi32(_mm512_extracti32x8_epi32(b, 1));
                        if constexpr (std::is_signed_v<T>) {
                            auto r0 = _mm512_max_epi32(a0, b0);
                            auto r1 = _mm512_max_epi32(a1, b1);
                            return _mm512_inserti32x8(_mm512_castsi256_si512(_mm512_cvtepi32_epi16(r0)), _mm512_cvtepi32_epi16(r1), 1);
                        } else {
                            auto r0 = _mm512_max_epu32(a0, b0);
                            auto r1 = _mm512_max_epu32(a1, b1);
                            return _mm512_inserti32x8(_mm512_castsi256_si512(_mm512_cvtepi32_epi16(r0)), _mm512_cvtepi32_epi16(r1), 1);
                        }
#endif
                    }

                    static inline uint16_t sum(
                            __m512i a) {
                        throw std::runtime_error("__m512i sum int16_t not supported yet");
                    }

                    static inline __m512i pack_right(
                            __m512i a,
                            mask_t mask) {
                        throw std::runtime_error("__m512i pack_right int16_t not supported yet");
                    }

                    static inline void pack_right2(
                            T * & result,
                            __m512i a,
                            mask_t mask) {
                        pack_right2_int16(result, a, mask);
                    }

                    static inline void pack_right3(
                            T * & result,
                            __m512i a,
                            mask_t mask) {
                        typedef typename mm<__m128i, T>::mask_t sse_mask_t;
                        auto subMask = static_cast<sse_mask_t>(mask);
                        _mm_storeu_si128(reinterpret_cast<__m128i *>(result), mm<__m128i, uint16_t>::pack_right(_mm512_extracti32x4_epi32(a, 0), subMask));
                        result += __builtin_popcount(subMask);
                        subMask = static_cast<sse_mask_t>(mask >> 8);
                        _mm_storeu_si128(reinterpret_cast<__m128i *>(result), mm<__m128i, uint16_t>::pack_right(_mm512_extracti32x4_epi32(a, 1), subMask));
                        result += __builtin_popcount(subMask);
                        subMask = static_cast<sse_mask_t>(mask >> 16);
                        _mm_storeu_si128(reinterpret_cast<__m128i *>(result), mm<__m128i, uint16_t>::pack_right(_mm512_extracti32x4_epi32(a, 2), subMask));
                        result += __builtin_popcount(subMask);
                        subMask = static_cast<sse_mask_t>(mask >> 24);
                        _mm_storeu_si128(reinterpret_cast<__m128i *>(result), mm<__m128i, uint16_t>::pack_right(_mm512_extracti32x4_epi32(a, 3), subMask));
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
                        auto popcnt0 = mm<__m256i, T>::popcount(_mm512_extracti64x4_epi64(a, 0));
                        auto popcnt1 = mm<__m256i, T>::popcount(_mm512_extracti64x4_epi64(a, 1));
                        return _mm256_inserti128_si256(_mm256_castsi128_si256(popcnt0), popcnt1, 1);
#endif
                    }

                    static inline popcnt_t popcount2(
                            // TODO
                            __m512i a) {
#ifdef __AVX512BW__
                        auto mask = _mm256_set1_epi16(0x0101);
                        auto shuffle = _mm256_set_epi64x(0xFFFFFFFFFFFFFFFF, 0x0F0D0B0907050301, 0xFFFFFFFFFFFFFFFF, 0x0F0D0B0907050301);
                        auto popcount8 = mm256 < uint16_t > ::popcount2(a);
                        auto temp = _mm256_shuffle_epi8(_mm256_mullo_epi16(popcount8, mask), shuffle);
                        return _mm_set_epi64x(_mm256_extract_epi64(temp, 2), _mm256_extract_epi64(temp, 0));
#else
                        auto popcnt0 = mm<__m256i, T>::popcount2(_mm512_extracti64x4_epi64(a, 0));
                        auto popcnt1 = mm<__m256i, T>::popcount2(_mm512_extracti64x4_epi64(a, 1));
                        return _mm256_inserti128_si256(_mm256_castsi128_si256(popcnt0), popcnt1, 1);
#endif
                    }

                    static inline popcnt_t popcount3(
                            // TODO
                            __m512i a) {
                        auto popcnt0 = mm<__m256i, T>::popcount3(_mm512_extracti64x4_epi64(a, 0));
                        auto popcnt1 = mm<__m256i, T>::popcount3(_mm512_extracti64x4_epi64(a, 1));
                        return _mm256_inserti128_si256(_mm256_castsi128_si256(popcnt0), popcnt1, 1);
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
#ifdef __AVX512BW__
                        if constexpr (std::is_signed_v<T>) {
                            return _mm512_mask_set1_epi16(_mm512_setzero_epi32(), _mm512_cmp_epi16_mask(a, b, _MM_CMPINT_NLE), static_cast<char>(0xFF));
                        } else {
                            return _mm512_mask_set1_epi16(_mm512_setzero_epi32(), _mm512_cmp_epu16_mask(a, b, _MM_CMPINT_NLE), static_cast<char>(0xFF));
                        }
#else
                        auto mm256 = coding_benchmark::simd::mm_op<__m256i, T, std::greater>::cmp(_mm512_extracti64x4_epi64(a, 0), _mm512_extracti64x4_epi64(b, 0));
                        auto mm = _mm512_castps256_ps512(mm256);
                        mm256 = coding_benchmark::simd::mm_op<__m256i, T, std::greater>::cmp(_mm512_extracti64x4_epi64(a, 1), _mm512_extracti64x4_epi64(b, 1));
                        return _mm512_inserti64x4(mm, mm256, 1);
#endif
                    }

                    static inline mask_t cmp_mask(
                            __m512i a,
                            __m512i b) {
#ifdef __AVX512BW__
                        if constexpr (std::is_signed_v<T>) {
                            return _mm512_cmp_epi16_mask(a, b, _MM_CMPINT_NLE);
                        } else {
                            return _mm512_cmp_epu16_mask(a, b, _MM_CMPINT_NLE);
                        }
#else
                        auto mask1 = static_cast<mask_t>(coding_benchmark::simd::mm_op<__m256i, T, std::greater>::cmp_mask(_mm512_extracti64x4_epi64(a, 0), _mm512_extracti64x4_epi64(b, 0)));
                        auto mask2 = static_cast<mask_t>(coding_benchmark::simd::mm_op<__m256i, T, std::greater>::cmp_mask(_mm512_extracti64x4_epi64(a, 1), _mm512_extracti64x4_epi64(b, 1)));
                        return mask1 | (mask2 << (sizeof(mask_t) * 4));
#endif
                    }
                };

                template<typename T>
                struct _mm512op<T, std::greater_equal> {

                    typedef typename _mm512<T>::mask_t mask_t;

                    static inline __m512i cmp(
                            __m512i a,
                            __m512i b) {
#ifdef __AVX512BW__
                        if constexpr (std::is_signed_v<T>) {
                            return _mm512_mask_set1_epi16(_mm512_setzero_epi32(), _mm512_cmp_epi16_mask(a, b, _MM_CMPINT_NLT), static_cast<char>(0xFF));
                        } else {
                            return _mm512_mask_set1_epi16(_mm512_setzero_epi32(), _mm512_cmp_epu16_mask(a, b, _MM_CMPINT_NLT), static_cast<char>(0xFF));
                        }
#else
                        auto mm256 = coding_benchmark::simd::mm_op<__m256i, T, std::greater_equal>::cmp(_mm512_extracti64x4_epi64(a, 0), _mm512_extracti64x4_epi64(b, 0));
                        auto mm = _mm512_castps256_ps512(mm256);
                        mm256 = coding_benchmark::simd::mm_op<__m256i, T, std::greater_equal>::cmp(_mm512_extracti64x4_epi64(a, 1), _mm512_extracti64x4_epi64(b, 1));
                        return _mm512_inserti64x4(mm, mm256, 1);
#endif
                    }

                    static inline mask_t cmp_mask(
                            __m512i a,
                            __m512i b) {
#ifdef __AVX512BW__
                        if constexpr (std::is_signed_v<T>) {
                            return _mm512_cmp_epi16_mask(a, b, _MM_CMPINT_NLT);
                        } else {
                            return _mm512_cmp_epu16_mask(a, b, _MM_CMPINT_NLT);
                        }
#else
                        auto mask1 = static_cast<mask_t>(coding_benchmark::simd::mm_op<__m256i, T, std::greater_equal>::cmp_mask(_mm512_extracti64x4_epi64(a, 0), _mm512_extracti64x4_epi64(b, 0)));
                        auto mask2 = static_cast<mask_t>(coding_benchmark::simd::mm_op<__m256i, T, std::greater_equal>::cmp_mask(_mm512_extracti64x4_epi64(a, 1), _mm512_extracti64x4_epi64(b, 1)));
                        return mask1 | (mask2 << (sizeof(mask_t) * 4));
#endif
                    }
                };

                template<typename T>
                struct _mm512op<T, std::less> {

                    typedef typename _mm512<T>::mask_t mask_t;

                    static inline __m512i cmp(
                            __m512i a,
                            __m512i b) {
#ifdef __AVX512BW__
                        if constexpr (std::is_signed_v<T>) {
                            return _mm512_mask_set1_epi16(_mm512_setzero_epi32(), _mm512_cmp_epi16_mask(a, b, _MM_CMPINT_LT), static_cast<char>(0xFF));
                        } else {
                            return _mm512_mask_set1_epi16(_mm512_setzero_epi32(), _mm512_cmp_epu16_mask(a, b, _MM_CMPINT_LT), static_cast<char>(0xFF));
                        }
#else
                        auto mm256 = coding_benchmark::simd::mm_op<__m256i, T, std::less>::cmp(_mm512_extracti64x4_epi64(a, 0), _mm512_extracti64x4_epi64(b, 0));
                        auto mm = _mm512_castps256_ps512(mm256);
                        mm256 = coding_benchmark::simd::mm_op<__m256i, T, std::less>::cmp(_mm512_extracti64x4_epi64(a, 1), _mm512_extracti64x4_epi64(b, 1));
                        return _mm512_inserti64x4(mm, mm256, 1);
#endif
                    }

                    static inline mask_t cmp_mask(
                            __m512i a,
                            __m512i b) {
#ifdef __AVX512BW__
                        if constexpr (std::is_signed_v<T>) {
                            return _mm512_cmp_epi16_mask(a, b, _MM_CMPINT_LT);
                        } else {
                            return _mm512_cmp_epu16_mask(a, b, _MM_CMPINT_LT);
                        }
#else
                        auto mask1 = static_cast<mask_t>(coding_benchmark::simd::mm_op<__m256i, T, std::less>::cmp_mask(_mm512_extracti64x4_epi64(a, 0), _mm512_extracti64x4_epi64(b, 0)));
                        auto mask2 = static_cast<mask_t>(coding_benchmark::simd::mm_op<__m256i, T, std::less>::cmp_mask(_mm512_extracti64x4_epi64(a, 1), _mm512_extracti64x4_epi64(b, 1)));
                        return mask1 | (mask2 << (sizeof(mask_t) * 4));
#endif
                    }
                };

                template<typename T>
                struct _mm512op<T, std::less_equal> {

                    typedef typename _mm512<T>::mask_t mask_t;

                    static inline __m512i cmp(
                            __m512i a,
                            __m512i b) {
#ifdef __AVX512BW__
                        if constexpr (std::is_signed_v<T>) {
                            return _mm512_mask_set1_epi16(_mm512_setzero_epi32(), _mm512_cmp_epi16_mask(a, b, _MM_CMPINT_LE), static_cast<char>(0xFF));
                        } else {
                            return _mm512_mask_set1_epi16(_mm512_setzero_epi32(), _mm512_cmp_epu16_mask(a, b, _MM_CMPINT_LE), static_cast<char>(0xFF));
                        }
#else
                        auto mm256 = coding_benchmark::simd::mm_op<__m256i, T, std::less_equal>::cmp(_mm512_extracti64x4_epi64(a, 0), _mm512_extracti64x4_epi64(b, 0));
                        auto mm = _mm512_castps256_ps512(mm256);
                        mm256 = coding_benchmark::simd::mm_op<__m256i, T, std::less_equal>::cmp(_mm512_extracti64x4_epi64(a, 1), _mm512_extracti64x4_epi64(b, 1));
                        return _mm512_inserti64x4(mm, mm256, 1);
#endif
                    }

                    static inline mask_t cmp_mask(
                            __m512i a,
                            __m512i b) {
#ifdef __AVX512BW__
                        if constexpr (std::is_signed_v<T>) {
                            return _mm512_cmp_epi16_mask(a, b, _MM_CMPINT_LE);
                        } else {
                            return _mm512_cmp_epu16_mask(a, b, _MM_CMPINT_LE);
                        }
#else
                        auto mask1 = static_cast<mask_t>(coding_benchmark::simd::mm_op<__m256i, T, std::less_equal>::cmp_mask(_mm512_extracti64x4_epi64(a, 0), _mm512_extracti64x4_epi64(b, 0)));
                        auto mask2 = static_cast<mask_t>(coding_benchmark::simd::mm_op<__m256i, T, std::less_equal>::cmp_mask(_mm512_extracti64x4_epi64(a, 1), _mm512_extracti64x4_epi64(b, 1)));
                        return mask1 | (mask2 << (sizeof(mask_t) * 4));
#endif
                    }
                };

                template<typename T>
                struct _mm512op<T, std::equal_to> {

                    typedef typename _mm512<T>::mask_t mask_t;

                    static inline __m512i cmp(
                            __m512i a,
                            __m512i b) {
#ifdef __AVX512BW__
                        if constexpr (std::is_signed_v<T>) {
                            return _mm512_mask_set1_epi16(_mm512_setzero_epi32(), _mm512_cmp_epi16_mask(a, b, _MM_CMPINT_EQ), static_cast<char>(0xFF));
                        } else {
                            return _mm512_mask_set1_epi16(_mm512_setzero_epi32(), _mm512_cmp_epu16_mask(a, b, _MM_CMPINT_EQ), static_cast<char>(0xFF));
                        }
#else
                        auto mm256 = coding_benchmark::simd::mm_op<__m256i, T, std::equal_to>::cmp(_mm512_extracti64x4_epi64(a, 0), _mm512_extracti64x4_epi64(b, 0));
                        auto mm = _mm512_castps256_ps512(mm256);
                        mm256 = coding_benchmark::simd::mm_op<__m256i, T, std::equal_to>::cmp(_mm512_extracti64x4_epi64(a, 1), _mm512_extracti64x4_epi64(b, 1));
                        return _mm512_inserti64x4(mm, mm256, 1);
#endif
                    }

                    static inline mask_t cmp_mask(
                            __m512i a,
                            __m512i b) {
#ifdef __AVX512BW__
                        if constexpr (std::is_signed_v<T>) {
                            return _mm512_cmp_epi16_mask(a, b, _MM_CMPINT_EQ);
                        } else {
                            return _mm512_cmp_epu16_mask(a, b, _MM_CMPINT_EQ);
                        }
#else
                        auto mask1 = static_cast<mask_t>(coding_benchmark::simd::mm_op<__m256i, T, std::equal_to>::cmp_mask(_mm512_extracti64x4_epi64(a, 0), _mm512_extracti64x4_epi64(b, 0)));
                        auto mask2 = static_cast<mask_t>(coding_benchmark::simd::mm_op<__m256i, T, std::equal_to>::cmp_mask(_mm512_extracti64x4_epi64(a, 1), _mm512_extracti64x4_epi64(b, 1)));
                        return mask1 | (mask2 << (sizeof(mask_t) * 4));
#endif
                    }
                };

                template<typename T>
                struct _mm512op<T, std::not_equal_to> {

                    typedef typename _mm512<T>::mask_t mask_t;

                    static inline __m512i cmp(
                            __m512i a,
                            __m512i b) {
#ifdef __AVX512BW__
                        if constexpr (std::is_signed_v<T>) {
                            return _mm512_mask_set1_epi16(_mm512_setzero_epi32(), _mm512_cmp_epi16_mask(a, b, _MM_CMPINT_NE), static_cast<char>(0xFF));
                        } else {
                            return _mm512_mask_set1_epi16(_mm512_setzero_epi32(), _mm512_cmp_epu16_mask(a, b, _MM_CMPINT_NE), static_cast<char>(0xFF));
                        }
#else
                        auto mm256 = coding_benchmark::simd::mm_op<__m256i, T, std::not_equal_to>::cmp(_mm512_extracti64x4_epi64(a, 0), _mm512_extracti64x4_epi64(b, 0));
                        auto mm = _mm512_castps256_ps512(mm256);
                        mm256 = coding_benchmark::simd::mm_op<__m256i, T, std::not_equal_to>::cmp(_mm512_extracti64x4_epi64(a, 1), _mm512_extracti64x4_epi64(b, 1));
                        return _mm512_inserti64x4(mm, mm256, 1);
#endif
                    }

                    static inline mask_t cmp_mask(
                            __m512i a,
                            __m512i b) {
#ifdef __AVX512BW__
                        if constexpr (std::is_signed_v<T>) {
                            return _mm512_cmp_epi16_mask(a, b, _MM_CMPINT_NE);
                        } else {
                            return _mm512_cmp_epu16_mask(a, b, _MM_CMPINT_NE);
                        }
#else
                        auto mask1 = static_cast<mask_t>(coding_benchmark::simd::mm_op<__m256i, T, std::not_equal_to>::cmp_mask(_mm512_extracti64x4_epi64(a, 0), _mm512_extracti64x4_epi64(b, 0)));
                        auto mask2 = static_cast<mask_t>(coding_benchmark::simd::mm_op<__m256i, T, std::not_equal_to>::cmp_mask(_mm512_extracti64x4_epi64(a, 1), _mm512_extracti64x4_epi64(b, 1)));
                        return mask1 | (mask2 << (sizeof(mask_t) * 4));
#endif
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
                        return _mm512op<T, std::not_equal_to>::cmp_mask(_mm512_setzero_si512(), cmp(a, b));
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
                        return _mm512op<T, std::not_equal_to>::cmp_mask(_mm512_setzero_si512(), cmp(a, b));
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
                        return _mm512op<T, std::not_equal_to>::cmp_mask(_mm512_setzero_si512(), cmp(a, b));
                    }
                };

                template<typename T>
                struct _mm512op<T, coding_benchmark::is_not> {

                    typedef typename _mm512<T>::mask_t mask_t;

                    static inline __m512i cmp(
                            __m512i a,
                            __m512i b) {
                        return _mm512_or_si512(a, b);
                    }

                    static inline mask_t cmp_mask(
                            __m512i a,
                            __m512i b) {
                        return _mm512op<T, std::not_equal_to>::cmp_mask(_mm512_setzero_si512(), cmp(a, b));
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
#ifdef __AVX512BW__
                        return _mm512_add_epi8(a, b);
#else
                        auto mm256 = coding_benchmark::simd::mm_op<__m256i, T, coding_benchmark::add>::add(_mm512_extracti64x4_epi64(a, 0), _mm512_extracti64x4_epi64(b, 0));
                        auto mm = _mm512_castps256_ps512(mm256);
                        mm256 = coding_benchmark::simd::mm_op<__m256i, T, coding_benchmark::add>::add(_mm512_extracti64x4_epi64(a, 1), _mm512_extracti64x4_epi64(b, 1));
                        return _mm512_inserti64x4(mm, mm256, 1);
#endif
                    }
                };

                template<typename T>
                struct _mm512op<T, coding_benchmark::sub> {

                    static inline __m512i compute(
                            __m512i a,
                            __m512i b) {
                        return sub(a, b);
                    }

                    static __m512i sub(
                            __m512i a,
                            __m512i b);
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
#ifdef __AVX512BW__
                        _mm512_mullo_epi16(a, b);
#else
                        auto mmA0 = _mm512_extracti64x4_epi64(a, 0);
                        auto mmA1 = _mm512_extracti64x4_epi64(a, 1);
                        auto mmB0 = _mm512_extracti64x4_epi64(b, 0);
                        auto mmB1 = _mm512_extracti64x4_epi64(b, 1);
                        auto mm0 = mm_op<__m256i, T, coding_benchmark::mul>::mullo(mmA0, mmB0);
                        auto mm1 = mm_op<__m256i, T, coding_benchmark::mul>::mullo(mmA1, mmB1);
                        auto c = _mm512_castsi256_si512(mm0);
                        return _mm512_inserti64x4(c, mm1, 1);
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
                        auto mmA0 = _mm512_extracti64x4_epi64(a, 0);
                        auto mmA1 = _mm512_extracti64x4_epi64(a, 1);
                        auto mmB0 = _mm512_extracti64x4_epi64(b, 0);
                        auto mmB1 = _mm512_extracti64x4_epi64(b, 1);
                        auto mm0 = mm_op<__m256i, T, coding_benchmark::div>::div(mmA0, mmB0);
                        auto mm1 = mm_op<__m256i, T, coding_benchmark::div>::div(mmA1, mmB1);
                        auto c = _mm512_castsi256_si512(mm0);
                        return _mm512_inserti64x4(c, mm1, 1);
                    }
                };

            } /* Private16 */

            template<>
            struct mm512<int16_t> :
                    public Private16::_mm512<int16_t> {
                typedef Private16::_mm512<int16_t> BASE;
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
            struct mm512op<int16_t, std::greater_equal> :
                    private Private08::_mm512op<int16_t, std::greater_equal> {
                typedef Private08::_mm512op<int16_t, std::greater_equal> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm512op<int16_t, std::greater> :
                    private Private08::_mm512op<int16_t, std::greater> {
                typedef Private08::_mm512op<int16_t, std::greater> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm512op<int16_t, std::less_equal> :
                    private Private08::_mm512op<int16_t, std::less_equal> {
                typedef Private08::_mm512op<int16_t, std::less_equal> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm512op<int16_t, std::less> :
                    private Private08::_mm512op<int16_t, std::less> {
                typedef Private08::_mm512op<int16_t, std::less> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm512op<int16_t, std::equal_to> :
                    private Private08::_mm512op<int16_t, std::equal_to> {
                typedef Private08::_mm512op<int16_t, std::equal_to> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm512op<int16_t, std::not_equal_to> :
                    private Private08::_mm512op<int16_t, std::not_equal_to> {
                typedef Private08::_mm512op<int16_t, std::not_equal_to> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm512op<int16_t, coding_benchmark::and_is> :
                    private Private08::_mm512op<int16_t, coding_benchmark::and_is> {
                typedef Private08::_mm512op<int16_t, coding_benchmark::and_is> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm512op<int16_t, coding_benchmark::or_is> :
                    private Private08::_mm512op<int16_t, coding_benchmark::or_is> {
                typedef Private08::_mm512op<int16_t, coding_benchmark::or_is> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm512op<int16_t, coding_benchmark::add> :
                    private Private08::_mm512op<int16_t, coding_benchmark::add> {
                typedef Private08::_mm512op<int16_t, coding_benchmark::add> BASE;
                using BASE::add;
                using BASE::compute;
            };

            template<>
            struct mm512op<int16_t, coding_benchmark::sub> :
                    private Private08::_mm512op<int16_t, coding_benchmark::sub> {
                typedef Private08::_mm512op<int16_t, coding_benchmark::sub> BASE;
                using BASE::sub;
                using BASE::compute;
            };

            template<>
            struct mm512op<int16_t, coding_benchmark::mul> :
                    private Private08::_mm512op<int16_t, coding_benchmark::mul> {
                typedef Private08::_mm512op<int16_t, coding_benchmark::mul> BASE;
                using BASE::mullo;
                using BASE::compute;
            };

            template<>
            struct mm512op<int16_t, coding_benchmark::div> :
                    private Private08::_mm512op<int16_t, coding_benchmark::div> {
                typedef Private08::_mm512op<int16_t, coding_benchmark::div> BASE;
                using BASE::div;
                using BASE::compute;
            };

            template<>
            struct mm512<uint16_t> :
                    public Private16::_mm512<uint16_t> {
                typedef Private16::_mm512<uint16_t> BASE;
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
            struct mm512op<uint16_t, std::greater_equal> :
                    private Private08::_mm512op<uint16_t, std::greater_equal> {
                typedef Private08::_mm512op<uint16_t, std::greater_equal> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm512op<uint16_t, std::greater> :
                    private Private08::_mm512op<uint16_t, std::greater> {
                typedef Private08::_mm512op<uint16_t, std::greater> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm512op<uint16_t, std::less_equal> :
                    private Private08::_mm512op<uint16_t, std::less_equal> {
                typedef Private08::_mm512op<uint16_t, std::less_equal> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm512op<uint16_t, std::less> :
                    private Private08::_mm512op<uint16_t, std::less> {
                typedef Private08::_mm512op<uint16_t, std::less> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm512op<uint16_t, std::equal_to> :
                    private Private08::_mm512op<uint16_t, std::equal_to> {
                typedef Private08::_mm512op<uint16_t, std::equal_to> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm512op<uint16_t, std::not_equal_to> :
                    private Private08::_mm512op<uint16_t, std::not_equal_to> {
                typedef Private08::_mm512op<uint16_t, std::not_equal_to> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm512op<uint16_t, coding_benchmark::and_is> :
                    private Private08::_mm512op<uint16_t, coding_benchmark::and_is> {
                typedef Private08::_mm512op<uint16_t, coding_benchmark::and_is> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm512op<uint16_t, coding_benchmark::or_is> :
                    private Private08::_mm512op<uint16_t, coding_benchmark::or_is> {
                typedef Private08::_mm512op<uint16_t, coding_benchmark::or_is> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm512op<uint16_t, coding_benchmark::add> :
                    private Private08::_mm512op<uint16_t, coding_benchmark::add> {
                typedef Private08::_mm512op<uint16_t, coding_benchmark::add> BASE;
                using BASE::add;
                using BASE::compute;
            };

            template<>
            struct mm512op<uint16_t, coding_benchmark::sub> :
                    private Private08::_mm512op<uint16_t, coding_benchmark::sub> {
                typedef Private08::_mm512op<uint16_t, coding_benchmark::sub> BASE;
                using BASE::sub;
                using BASE::compute;
            };

            template<>
            struct mm512op<uint16_t, coding_benchmark::mul> :
                    private Private08::_mm512op<uint16_t, coding_benchmark::mul> {
                typedef Private08::_mm512op<uint16_t, coding_benchmark::mul> BASE;
                using BASE::mullo;
                using BASE::compute;
            };

            template<>
            struct mm512op<uint16_t, coding_benchmark::div> :
                    private Private08::_mm512op<uint16_t, coding_benchmark::div> {
                typedef Private08::_mm512op<uint16_t, coding_benchmark::div> BASE;
                using BASE::div;
                using BASE::compute;
            };

        }
    }
}
