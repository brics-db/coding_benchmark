// Copyright (c) 2017 Till Kolditz
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
 * File:   AVX2.cpp
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 07. August 2017, 15:47
 */

#ifdef __AVX2__

#include <SIMD/AVX2.hpp>

namespace coding_benchmark {
    namespace simd {
        namespace avx2 {

            namespace Private08 {
                template<> const int64_t * const _mm256<int8_t>::SHUFFLE_TABLE_LL = nullptr;
                template<> const int64_t * const _mm256<int8_t>::SHUFFLE_TABLE_LH = nullptr;
                template<> const int64_t * const _mm256<int8_t>::SHUFFLE_TABLE_HL = nullptr;
                template<> const int64_t * const _mm256<int8_t>::SHUFFLE_TABLE_HH = nullptr;

                template<> const int64_t * const _mm256<uint8_t>::SHUFFLE_TABLE_LL = nullptr;
                template<> const int64_t * const _mm256<uint8_t>::SHUFFLE_TABLE_LH = nullptr;
                template<> const int64_t * const _mm256<uint8_t>::SHUFFLE_TABLE_HL = nullptr;
                template<> const int64_t * const _mm256<uint8_t>::SHUFFLE_TABLE_HH = nullptr;
            }

            namespace Private16 {
                template<> const __m128i * const _mm256<int16_t>::SHUFFLE_TABLE_L = nullptr;
                template<> const __m128i * const _mm256<int16_t>::SHUFFLE_TABLE_H = nullptr;

                template<> const __m128i * const _mm256<uint16_t>::SHUFFLE_TABLE_L = nullptr;
                template<> const __m128i * const _mm256<uint16_t>::SHUFFLE_TABLE_H = nullptr;
            }

            namespace Private32 {
                template<> const __m256i * const _mm256<int32_t>::SHUFFLE_TABLE = nullptr;
                template<> const __m256i * const _mm256<uint32_t>::SHUFFLE_TABLE = nullptr;
            }

            namespace Private64 {
                template<> const __m256i * const _mm256<int64_t>::SHUFFLE_TABLE = nullptr;
                template<> const __m256i * const _mm256<uint64_t>::SHUFFLE_TABLE = nullptr;
            }

        }
    }
}

#endif
