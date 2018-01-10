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
 * AVX512_uint08.tcc
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

            namespace Private08 {

                template<size_t current = 0>
                inline void pack_right2_int8(
                        int8_t * & result,
                        __m512i & a,
                        uint64_t mask) {
                    *result = reinterpret_cast<int8_t*>(&a)[current];
                    result += (mask >> current) & 0x1;
                    pack_right2_int8<current + 1>(result, a, mask);
                }

                template<>
                inline void pack_right2_int8<63>(
                        int8_t * & result,
                        __m512i & a,
                        uint64_t mask) {
                    *result = reinterpret_cast<int8_t*>(&a)[63];
                    result += (mask >> 31) & 0x1;
                }

                template<size_t current = 0>
                inline void pack_right2_int8(
                        uint8_t * & result,
                        __m512i & a,
                        uint64_t mask) {
                    *result = reinterpret_cast<uint8_t*>(&a)[current];
                    result += (mask >> current) & 0x1;
                    pack_right2_int8<current + 1>(result, a, mask);
                }

                template<>
                inline void pack_right2_int8<63>(
                        uint8_t * & result,
                        __m512i & a,
                        uint64_t mask) {
                    *result = reinterpret_cast<uint8_t*>(&a)[63];
                    result += (mask >> 31) & 0x1;
                }

                template<typename T>
                struct _mm512 {

                    typedef __mmask64 mask_t;
                    typedef __m512i popcnt_t;

                    static const constexpr mask_t FULL_MASK = 0xFFFFFFFFFFFFFFFFull;

                    static inline __m512i set1(
                            T value) {
                        return _mm512_set1_epi8(value);
                    }

                    static inline __m512i set(
                            T v63,
                            T v62,
                            T v61,
                            T v60,
                            T v59,
                            T v58,
                            T v57,
                            T v56,
                            T v55,
                            T v54,
                            T v53,
                            T v52,
                            T v51,
                            T v50,
                            T v49,
                            T v48,
                            T v47,
                            T v46,
                            T v45,
                            T v44,
                            T v43,
                            T v42,
                            T v41,
                            T v40,
                            T v39,
                            T v38,
                            T v37,
                            T v36,
                            T v35,
                            T v34,
                            T v33,
                            T v32,
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
                        return _mm512_set_epi8(v63, v62, v61, v60, v59, v58, v57, v56, v55, v54, v53, v52, v51, v50, v49, v48, v47, v46, v45, v44, v43, v42, v41, v40, v39, v38, v37, v36, v35, v34,
                                v33, v32, v31, v30, v29, v28, v27, v26, v25, v24, v23, v22, v21, v20, v19, v18, v17, v16, v15, v14, v13, v12, v11, v10, v9, v8, v7, v6, v5, v4, v3, v2, v1, v0);
                    }

                    static inline __m512i set_inc(
                            T v0) {
#ifdef __AVX512BW__
                        auto mm = _mm512_set_epi8(63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27,
                                62, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0);
                        return _mm512<T>::add(mm, _mm512_set1_epi8(v0));
#else
                        return _mm512_set_epi8(v0 + 63, v0 + 62, v0 + 61, v0 + 60, v0 + 59, v0 + 58, v0 + 57, v0 + 56, v0 + 55, v0 + 54, v0 + 53, v0 + 52, v0 + 51, v0 + 50, v0 + 49, v0 + 48, v0 + 47,
                                v0 + 46, v0 + 45, v0 + 44, v0 + 43, v0 + 42, v0 + 41, v0 + 40, v0 + 39, v0 + 38, v0 + 37, v0 + 36, v0 + 35, v0 + 34, v0 + 33, v0 + 32, v0 + 31, v0 + 30, v0 + 29,
                                v0 + 28, v0 + 27, v0 + 26, v0 + 25, v0 + 24, v0 + 23, v0 + 22, v0 + 21, v0 + 20, v0 + 19, v0 + 18, v0 + 17, v0 + 16, v0 + 15, v0 + 14, v0 + 13, v0 + 12, v0 + 11,
                                v0 + 10, v0 + 9, v0 + 8, v0 + 7, v0 + 6, v0 + 5, v0 + 4, v0 + 3, v0 + 2, v0 + 1, v0);
#endif
                    }

                    static inline __m512i set_inc(
                            T v0,
                            T inc) {
                        return _mm512_set_epi8(v0 + 63 * inc, v0 + 62 * inc, v0 + 61 * inc, v0 + 60 * inc, v0 + 59 * inc, v0 + 58 * inc, v0 + 57 * inc, v0 + 56 * inc, v0 + 55 * inc, v0 + 54 * inc,
                                v0 + 53 * inc, v0 + 52 * inc, v0 + 51 * inc, v0 + 50 * inc, v0 + 49 * inc, v0 + 48 * inc, v0 + 47 * inc, v0 + 46 * inc, v0 + 45 * inc, v0 + 44 * inc, v0 + 43 * inc,
                                v0 + 42 * inc, v0 + 41 * inc, v0 + 40 * inc, v0 + 39 * inc, v0 + 38 * inc, v0 + 37 * inc, v0 + 36 * inc, v0 + 35 * inc, v0 + 34 * inc, v0 + 33 * inc, v0 + 32 * inc,
                                v0 + 31 * inc, v0 + 30 * inc, v0 + 29 * inc, v0 + 28 * inc, v0 + 27 * inc, v0 + 26 * inc, v0 + 25 * inc, v0 + 24 * inc, v0 + 23 * inc, v0 + 22 * inc, v0 + 21 * inc,
                                v0 + 20 * inc, v0 + 19 * inc, v0 + 18 * inc, v0 + 17 * inc, v0 + 16 * inc, v0 + 15 * inc, v0 + 14 * inc, v0 + 13 * inc, v0 + 12 * inc, v0 + 11 * inc, v0 + 10 * inc,
                                v0 + 9 * inc, v0 + 8 * inc, v0 + 7 * inc, v0 + 6 * inc, v0 + 5 * inc, v0 + 4 * inc, v0 + 3 * inc, v0 + 2 * inc, v0 + inc, v0);
                    }

                    static inline __m512i min(
                            __m512i a,
                            __m512i b) {
#ifdef __AVX512BW__
                        if constexpr (std::is_signed_v<T>) {
                            return _mm512_min_epi8(a, b);
                        } else {
                            return _mm512_min_epu8(a, b);
                        }
#else
                        auto a0 = _mm512_cvtepi8_epi32(_mm512_extracti32x4_epi32(a, 0));
                        auto a1 = _mm512_cvtepi8_epi32(_mm512_extracti32x4_epi32(a, 1));
                        auto a2 = _mm512_cvtepi8_epi32(_mm512_extracti32x4_epi32(a, 2));
                        auto a3 = _mm512_cvtepi8_epi32(_mm512_extracti32x4_epi32(a, 3));
                        auto b0 = _mm512_cvtepi8_epi32(_mm512_extracti32x4_epi32(b, 0));
                        auto b1 = _mm512_cvtepi8_epi32(_mm512_extracti32x4_epi32(b, 1));
                        auto b2 = _mm512_cvtepi8_epi32(_mm512_extracti32x4_epi32(b, 2));
                        auto b3 = _mm512_cvtepi8_epi32(_mm512_extracti32x4_epi32(b, 3));
                        if constexpr (std::is_signed_v<T>) {
                            auto r0 = _mm512_min_epi32(a0, b0);
                            auto r1 = _mm512_min_epi32(a1, b1);
                            auto r2 = _mm512_min_epi32(a2, b2);
                            auto r3 = _mm512_min_epi32(a3, b3);
                            auto x0 = _mm256_inserti128_si256(_mm256_castsi128_si256(_mm512_cvtepi32_epi8(r0)), _mm512_cvtepi32_epi8(r1), 1);
                            auto x1 = _mm256_inserti128_si256(_mm256_castsi128_si256(_mm512_cvtepi32_epi8(r2)), _mm512_cvtepi32_epi8(r3), 1);
                            return _mm512_inserti32x8(_mm512_castsi256_si512(x0), x1, 1);
                        } else {
                            auto r0 = _mm512_min_epu32(a0, b0);
                            auto r1 = _mm512_min_epu32(a1, b1);
                            auto r2 = _mm512_min_epu32(a2, b2);
                            auto r3 = _mm512_min_epu32(a3, b3);
                            auto x0 = _mm256_inserti128_si256(_mm256_castsi128_si256(_mm512_cvtepi32_epi8(r0)), _mm512_cvtepi32_epi8(r1), 1);
                            auto x1 = _mm256_inserti128_si256(_mm256_castsi128_si256(_mm512_cvtepi32_epi8(r2)), _mm512_cvtepi32_epi8(r3), 1);
                            return _mm512_inserti32x8(_mm512_castsi256_si512(x0), x1, 1);
                        }
#endif
                    }

                    static inline __m512i max(
                            __m512i a,
                            __m512i b) {
#ifdef __AVX512BW__
                        if constexpr (std::is_signed_v<T>) {
                            return _mm512_max_epi8(a, b);
                        } else {
                            return _mm512_max_epu8(a, b);
                        }
#else
                        auto a0 = _mm512_cvtepi8_epi32(_mm512_extracti32x4_epi32(a, 0));
                        auto a1 = _mm512_cvtepi8_epi32(_mm512_extracti32x4_epi32(a, 1));
                        auto a2 = _mm512_cvtepi8_epi32(_mm512_extracti32x4_epi32(a, 2));
                        auto a3 = _mm512_cvtepi8_epi32(_mm512_extracti32x4_epi32(a, 3));
                        auto b0 = _mm512_cvtepi8_epi32(_mm512_extracti32x4_epi32(b, 0));
                        auto b1 = _mm512_cvtepi8_epi32(_mm512_extracti32x4_epi32(b, 1));
                        auto b2 = _mm512_cvtepi8_epi32(_mm512_extracti32x4_epi32(b, 2));
                        auto b3 = _mm512_cvtepi8_epi32(_mm512_extracti32x4_epi32(b, 3));
                        if constexpr (std::is_signed_v<T>) {
                            auto r0 = _mm512_max_epi32(a0, b0);
                            auto r1 = _mm512_max_epi32(a1, b1);
                            auto r2 = _mm512_max_epi32(a2, b2);
                            auto r3 = _mm512_max_epi32(a3, b3);
                            auto x0 = _mm256_inserti128_si256(_mm256_castsi128_si256(_mm512_cvtepi32_epi8(r0)), _mm512_cvtepi32_epi8(r1), 1);
                            auto x1 = _mm256_inserti128_si256(_mm256_castsi128_si256(_mm512_cvtepi32_epi8(r2)), _mm512_cvtepi32_epi8(r3), 1);
                            return _mm512_inserti32x8(_mm512_castsi256_si512(x0), x1, 1);
                        } else {
                            auto r0 = _mm512_max_epu32(a0, b0);
                            auto r1 = _mm512_max_epu32(a1, b1);
                            auto r2 = _mm512_max_epu32(a2, b2);
                            auto r3 = _mm512_max_epu32(a3, b3);
                            auto x0 = _mm256_inserti128_si256(_mm256_castsi128_si256(_mm512_cvtepi32_epi8(r0)), _mm512_cvtepi32_epi8(r1), 1);
                            auto x1 = _mm256_inserti128_si256(_mm256_castsi128_si256(_mm512_cvtepi32_epi8(r2)), _mm512_cvtepi32_epi8(r3), 1);
                            return _mm512_inserti32x8(_mm512_castsi256_si512(x0), x1, 1);
                        }
#endif
                    }

                    static inline __m512i geq(
                            __m512i a,
                            __m512i b) {
#ifdef __AVX512BW__
                        auto mm = max(a, b);
                        return _mm512_cmpeq_epi8(a, mm);
#else
                        auto mm256 = coding_benchmark::simd::mm_op<__m256i, T, std::greater_equal>::cmp(_mm512_extracti64x4_epi64(a, 0), _mm512_extracti64x4_epi64(b, 0));
                        auto mm = _mm512_castps256_ps512(mm256);
                        mm256 = coding_benchmark::simd::mm_op<__m256i, T, std::greater_equal>::cmp(_mm512_extracti64x4_epi64(a, 1), _mm512_extracti64x4_epi64(b, 1));
                        return _mm512_inserti64x4(mm, mm256, 1);
#endif
                    }

                    static inline uint64_t geq_mask(
                            __m512i a,
                            __m512i b) {
#ifdef __AVX512BW__
                        return _mm512_cmpge_epi8_mask(a, b);
#else
                        auto mm256 = coding_benchmark::simd::mm<__m256i, T>::geq_mask(_mm512_extracti64x4_epi64(a, 0), _mm512_extracti64x4_epi64(b, 0));
                        auto mm = _mm512_castps256_ps512(mm256);
                        mm256 = coding_benchmark::simd::mm<__m256i, T>::geq_mask(_mm512_extracti64x4_epi64(a, 1), _mm512_extracti64x4_epi64(b, 1));
                        return _mm512_inserti64x4(mm, mm256, 1);
#endif
                    }

                    static inline uint8_t sum(
                            __m512i a) {
                        throw std::runtime_error("__m512i sum uint8_t not supported yet"); // TODO
                    }

                    static inline __m512i pack_right(
                            __m512i a,
                            mask_t mask) {
                        throw std::runtime_error("__m512i pack_right uint8_t not supported yet"); // TODO
                    }

                    static inline void pack_right2(
                            uint8_t * & result,
                            __m512i a,
                            mask_t mask) {
                        pack_right2_int8(result, a, mask);
                    }

                    static inline void pack_right3(
                            uint8_t * & result,
                            __m512i a,
                            mask_t mask) {
                        typedef mm<__m128i, uint8_t>::mask_t sse_mask_t;
                        auto subMask = static_cast<sse_mask_t>(mask);
                        _mm_storeu_si128(reinterpret_cast<__m128i *>(result), mm<__m128i, uint8_t>::pack_right(_mm512_extracti32x4_epi32(a, 0), subMask));
                        result += __builtin_popcount(subMask);
                        subMask = static_cast<sse_mask_t>(mask >> 16);
                        _mm_storeu_si128(reinterpret_cast<__m128i *>(result), mm<__m128i, uint8_t>::pack_right(_mm512_extracti32x4_epi32(a, 1), subMask));
                        result += __builtin_popcount(subMask);
                        subMask = static_cast<sse_mask_t>(mask >> 32);
                        _mm_storeu_si128(reinterpret_cast<__m128i *>(result), mm<__m128i, uint8_t>::pack_right(_mm512_extracti32x4_epi32(a, 2), subMask));
                        result += __builtin_popcount(subMask);
                        subMask = static_cast<sse_mask_t>(mask >> 48);
                        _mm_storeu_si128(reinterpret_cast<__m128i *>(result), mm<__m128i, uint8_t>::pack_right(_mm512_extracti32x4_epi32(a, 3), subMask));
                        result += __builtin_popcount(subMask);
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
                            return _mm512_mask_set1_epi8(_mm512_setzero_epi32(), _mm512_cmp_epi8_mask(a, b, _MM_CMPINT_NLE), static_cast<char>(0xFF));
                        } else {
                            return _mm512_mask_set1_epi8(_mm512_setzero_epi32(), _mm512_cmp_epu8_mask(a, b, _MM_CMPINT_NLE), static_cast<char>(0xFF));
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
                            return _mm512_cmp_epi8_mask(a, b, _MM_CMPINT_NLE);
                        } else {
                            return _mm512_cmp_epu8_mask(a, b, _MM_CMPINT_NLE);
                        }
#else
                        auto mask1 = static_cast<mask_t>(coding_benchmark::simd::mm_op<__m256i, T, std::greater>::cmp_mask(_mm512_extracti64x4_epi64(a, 0), _mm512_extracti64x4_epi64(b, 0)));
                        return mask1
                                | (static_cast<mask_t>(coding_benchmark::simd::mm_op<__m256i, T, std::greater>::cmp_mask(_mm512_extracti64x4_epi64(a, 1), _mm512_extracti64x4_epi64(b, 1)))
                                        << (sizeof(mask_t) * 4));
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
                            return _mm512_mask_set1_epi8(_mm512_setzero_epi32(), _mm512_cmp_epi8_mask(a, b, _MM_CMPINT_NLT), static_cast<char>(0xFF));
                        } else {
                            return _mm512_mask_set1_epi8(_mm512_setzero_epi32(), _mm512_cmp_epu8_mask(a, b, _MM_CMPINT_NLT), static_cast<char>(0xFF));
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
                            return _mm512_cmp_epi8_mask(a, b, _MM_CMPINT_NLT);
                        } else {
                            return _mm512_cmp_epu8_mask(a, b, _MM_CMPINT_NLT);
                        }
#else
                        auto mask1 = static_cast<mask_t>(coding_benchmark::simd::mm_op<__m256i, T, std::greater_equal>::cmp_mask(_mm512_extracti64x4_epi64(a, 0), _mm512_extracti64x4_epi64(b, 0)));
                        return mask1
                                | (static_cast<mask_t>(coding_benchmark::simd::mm_op<__m256i, T, std::greater_equal>::cmp_mask(_mm512_extracti64x4_epi64(a, 1), _mm512_extracti64x4_epi64(b, 1)))
                                        << (sizeof(mask_t) * 4));
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
                            return _mm512_mask_set1_epi8(_mm512_setzero_epi32(), _mm512_cmp_epi8_mask(a, b, _MM_CMPINT_LT), static_cast<char>(0xFF));
                        } else {
                            return _mm512_mask_set1_epi8(_mm512_setzero_epi32(), _mm512_cmp_epu8_mask(a, b, _MM_CMPINT_LT), static_cast<char>(0xFF));
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
                            return _mm512_cmp_epi8_mask(a, b, _MM_CMPINT_LT);
                        } else {
                            return _mm512_cmp_epu8_mask(a, b, _MM_CMPINT_LT);
                        }
#else
                        auto mask1 = static_cast<mask_t>(coding_benchmark::simd::mm_op<__m256i, T, std::less>::cmp_mask(_mm512_extracti64x4_epi64(a, 0), _mm512_extracti64x4_epi64(b, 0)));
                        return mask1
                                | (static_cast<mask_t>(coding_benchmark::simd::mm_op<__m256i, T, std::less>::cmp_mask(_mm512_extracti64x4_epi64(a, 1), _mm512_extracti64x4_epi64(b, 1)))
                                        << (sizeof(mask_t) * 4));
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
                            return _mm512_mask_set1_epi8(_mm512_setzero_epi32(), _mm512_cmp_epi8_mask(a, b, _MM_CMPINT_LE), static_cast<char>(0xFF));
                        } else {
                            return _mm512_mask_set1_epi8(_mm512_setzero_epi32(), _mm512_cmp_epu8_mask(a, b, _MM_CMPINT_LE), static_cast<char>(0xFF));
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
                            return _mm512_cmp_epi8_mask(a, b, _MM_CMPINT_LE);
                        } else {
                            return _mm512_cmp_epu8_mask(a, b, _MM_CMPINT_LE);
                        }
#else
                        auto mask1 = static_cast<mask_t>(coding_benchmark::simd::mm_op<__m256i, T, std::less_equal>::cmp_mask(_mm512_extracti64x4_epi64(a, 0), _mm512_extracti64x4_epi64(b, 0)));
                        return mask1
                                | (static_cast<mask_t>(coding_benchmark::simd::mm_op<__m256i, T, std::less_equal>::cmp_mask(_mm512_extracti64x4_epi64(a, 1), _mm512_extracti64x4_epi64(b, 1)))
                                        << (sizeof(mask_t) * 4));
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
                            return _mm512_mask_set1_epi8(_mm512_setzero_epi32(), _mm512_cmp_epi8_mask(a, b, _MM_CMPINT_EQ), static_cast<char>(0xFF));
                        } else {
                            return _mm512_mask_set1_epi8(_mm512_setzero_epi32(), _mm512_cmp_epu8_mask(a, b, _MM_CMPINT_EQ), static_cast<char>(0xFF));
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
                            return _mm512_cmp_epi8_mask(a, b, _MM_CMPINT_EQ);
                        } else {
                            return _mm512_cmp_epu8_mask(a, b, _MM_CMPINT_EQ);
                        }
#else
                        auto mask1 = static_cast<mask_t>(coding_benchmark::simd::mm_op<__m256i, T, std::equal_to>::cmp_mask(_mm512_extracti64x4_epi64(a, 0), _mm512_extracti64x4_epi64(b, 0)));
                        return mask1
                                | (static_cast<mask_t>(coding_benchmark::simd::mm_op<__m256i, T, std::equal_to>::cmp_mask(_mm512_extracti64x4_epi64(a, 1), _mm512_extracti64x4_epi64(b, 1)))
                                        << (sizeof(mask_t) * 4));
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
                            return _mm512_mask_set1_epi8(_mm512_setzero_epi32(), _mm512_cmp_epi8_mask(a, b, _MM_CMPINT_NE), static_cast<char>(0xFF));
                        } else {
                            return _mm512_mask_set1_epi8(_mm512_setzero_epi32(), _mm512_cmp_epu8_mask(a, b, _MM_CMPINT_NE), static_cast<char>(0xFF));
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
                            return _mm512_cmp_epi8_mask(a, b, _MM_CMPINT_NE);
                        } else {
                            return _mm512_cmp_epu8_mask(a, b, _MM_CMPINT_NE);
                        }
#else
                        auto mask1 = static_cast<mask_t>(coding_benchmark::simd::mm_op<__m256i, T, std::not_equal_to>::cmp_mask(_mm512_extracti64x4_epi64(a, 0), _mm512_extracti64x4_epi64(b, 0)));
                        return mask1
                                | (static_cast<mask_t>(coding_benchmark::simd::mm_op<__m256i, T, std::not_equal_to>::cmp_mask(_mm512_extracti64x4_epi64(a, 1), _mm512_extracti64x4_epi64(b, 1)))
                                        << (sizeof(mask_t) * 4));
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

                    static inline __m512i sub(
                            __m512i a,
                            __m512i b) {
#ifdef __AVX512BW__
                        return _mm512_sub_epi8(a, b);
#else
                        auto mm256 = coding_benchmark::simd::mm_op<__m256i, T, coding_benchmark::sub>::sub(_mm512_extracti64x4_epi64(a, 0), _mm512_extracti64x4_epi64(b, 0));
                        auto mm = _mm512_castps256_ps512(mm256);
                        mm256 = coding_benchmark::simd::mm_op<__m256i, T, coding_benchmark::sub>::sub(_mm512_extracti64x4_epi64(a, 1), _mm512_extracti64x4_epi64(b, 1));
                        return _mm512_inserti64x4(mm, mm256, 1);
#endif
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
                        auto mmA0 = _mm512_extracti64x4_epi64(a, 0);
                        auto mmA1 = _mm512_extracti64x4_epi64(a, 1);
                        auto mmB0 = _mm512_extracti64x4_epi64(b, 0);
                        auto mmB1 = _mm512_extracti64x4_epi64(b, 1);
                        auto mm0 = mm_op<__m256i, T, coding_benchmark::mul>::mullo(mmA0, mmB0);
                        auto mm1 = mm_op<__m256i, T, coding_benchmark::mul>::mullo(mmA1, mmB1);
                        auto c = _mm512_castsi256_si512(mm0);
                        return _mm512_insertf64x4(c, mm1, 1);
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
                        return _mm512_insertf64x4(c, mm1, 1);
                    }
                };

            } /* Private08 */

            template<>
            struct mm512<int8_t> :
                    public Private08::_mm512<int8_t> {
                typedef Private08::_mm512<int8_t> BASE;
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
                // TODO using BASE::popcount;
                // TODO using BASE::popcount2;
                // TODO using BASE::popcount3;
                // TODO using BASE::cvt_larger_lo;
                // TODO using BASE::cvt_larger_hi;
            };

            template<>
            struct mm512op<int8_t, std::greater_equal> :
                    private Private08::_mm512op<int8_t, std::greater_equal> {
                typedef Private08::_mm512op<int8_t, std::greater_equal> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm512op<int8_t, std::greater> :
                    private Private08::_mm512op<int8_t, std::greater> {
                typedef Private08::_mm512op<int8_t, std::greater> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm512op<int8_t, std::less_equal> :
                    private Private08::_mm512op<int8_t, std::less_equal> {
                typedef Private08::_mm512op<int8_t, std::less_equal> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm512op<int8_t, std::less> :
                    private Private08::_mm512op<int8_t, std::less> {
                typedef Private08::_mm512op<int8_t, std::less> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm512op<int8_t, std::equal_to> :
                    private Private08::_mm512op<int8_t, std::equal_to> {
                typedef Private08::_mm512op<int8_t, std::equal_to> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm512op<int8_t, std::not_equal_to> :
                    private Private08::_mm512op<int8_t, std::not_equal_to> {
                typedef Private08::_mm512op<int8_t, std::not_equal_to> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm512op<int8_t, coding_benchmark::and_is> :
                    private Private08::_mm512op<int8_t, coding_benchmark::and_is> {
                typedef Private08::_mm512op<int8_t, coding_benchmark::and_is> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm512op<int8_t, coding_benchmark::or_is> :
                    private Private08::_mm512op<int8_t, coding_benchmark::or_is> {
                typedef Private08::_mm512op<int8_t, coding_benchmark::or_is> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm512op<int8_t, coding_benchmark::add> :
                    private Private08::_mm512op<int8_t, coding_benchmark::add> {
                typedef Private08::_mm512op<int8_t, coding_benchmark::add> BASE;
                using BASE::add;
                using BASE::compute;
            };

            template<>
            struct mm512op<int8_t, coding_benchmark::sub> :
                    private Private08::_mm512op<int8_t, coding_benchmark::sub> {
                typedef Private08::_mm512op<int8_t, coding_benchmark::sub> BASE;
                using BASE::sub;
                using BASE::compute;
            };

            template<>
            struct mm512op<int8_t, coding_benchmark::mul> :
                    private Private08::_mm512op<int8_t, coding_benchmark::mul> {
                typedef Private08::_mm512op<int8_t, coding_benchmark::mul> BASE;
                using BASE::mullo;
                using BASE::compute;
            };

            template<>
            struct mm512op<int8_t, coding_benchmark::div> :
                    private Private08::_mm512op<int8_t, coding_benchmark::div> {
                typedef Private08::_mm512op<int8_t, coding_benchmark::div> BASE;
                using BASE::div;
                using BASE::compute;
            };

            template<>
            struct mm512<uint8_t> :
                    public Private08::_mm512<uint8_t> {
                typedef Private08::_mm512<uint8_t> BASE;
                using BASE::mask_t;
                // TODO using BASE::popcnt_t;
                using BASE::set1;
                using BASE::set;
                using BASE::set_inc;
                using BASE::min;
                using BASE::max;
                using BASE::sum;
                using BASE::pack_right;
                using BASE::pack_right2;
                // TODO using BASE::popcount;
                // TODO using BASE::popcount2;
                // TODO using BASE::popcount3;
                // TODO using BASE::cvt_larger_lo;
                // TODO using BASE::cvt_larger_hi;
            };

            template<>
            struct mm512op<uint8_t, std::greater_equal> :
                    private Private08::_mm512op<uint8_t, std::greater_equal> {
                typedef Private08::_mm512op<uint8_t, std::greater_equal> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm512op<uint8_t, std::greater> :
                    private Private08::_mm512op<uint8_t, std::greater> {
                typedef Private08::_mm512op<uint8_t, std::greater> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm512op<uint8_t, std::less_equal> :
                    private Private08::_mm512op<uint8_t, std::less_equal> {
                typedef Private08::_mm512op<uint8_t, std::less_equal> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm512op<uint8_t, std::less> :
                    private Private08::_mm512op<uint8_t, std::less> {
                typedef Private08::_mm512op<uint8_t, std::less> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm512op<uint8_t, std::equal_to> :
                    private Private08::_mm512op<uint8_t, std::equal_to> {
                typedef Private08::_mm512op<uint8_t, std::equal_to> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm512op<uint8_t, std::not_equal_to> :
                    private Private08::_mm512op<uint8_t, std::not_equal_to> {
                typedef Private08::_mm512op<uint8_t, std::not_equal_to> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm512op<uint8_t, coding_benchmark::and_is> :
                    private Private08::_mm512op<uint8_t, coding_benchmark::and_is> {
                typedef Private08::_mm512op<uint8_t, coding_benchmark::and_is> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm512op<uint8_t, coding_benchmark::or_is> :
                    private Private08::_mm512op<uint8_t, coding_benchmark::or_is> {
                typedef Private08::_mm512op<uint8_t, coding_benchmark::or_is> BASE;
                using BASE::mask_t;
                using BASE::cmp;
                using BASE::cmp_mask;
            };

            template<>
            struct mm512op<uint8_t, coding_benchmark::add> :
                    private Private08::_mm512op<uint8_t, coding_benchmark::add> {
                typedef Private08::_mm512op<uint8_t, coding_benchmark::add> BASE;
                using BASE::add;
                using BASE::compute;
            };

            template<>
            struct mm512op<uint8_t, coding_benchmark::sub> :
                    private Private08::_mm512op<uint8_t, coding_benchmark::sub> {
                typedef Private08::_mm512op<uint8_t, coding_benchmark::sub> BASE;
                using BASE::sub;
                using BASE::compute;
            };

            template<>
            struct mm512op<uint8_t, coding_benchmark::mul> :
                    private Private08::_mm512op<uint8_t, coding_benchmark::mul> {
                typedef Private08::_mm512op<uint8_t, coding_benchmark::mul> BASE;
                using BASE::mullo;
                using BASE::compute;
            };

            template<>
            struct mm512op<uint8_t, coding_benchmark::div> :
                    private Private08::_mm512op<uint8_t, coding_benchmark::div> {
                typedef Private08::_mm512op<uint8_t, coding_benchmark::div> BASE;
                using BASE::div;
                using BASE::compute;
            };

        }
    }
}
