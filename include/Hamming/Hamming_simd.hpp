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
 * Hamming_simd.hpp
 *
 *  Created on: 13.12.2017
 *      Author: Till Kolditz - Till.Kolditz@gmail.com
 */

#pragma once

#define HAMMING_SIMD

#include <Hamming/Hamming_simd.tcc>

#undef HAMMING_SIMD

namespace coding_benchmark {

#ifdef __SSE4_2__
    template<size_t UNROLL>
    struct Hamming_sse42_16 :
            public Hamming_simd<uint16_t, __m128i, UNROLL> {

        using Hamming_simd<uint16_t, __m128i, UNROLL>::Hamming_simd;

        virtual ~Hamming_sse42_16() {
        }

    };

    template<size_t UNROLL>
    struct Hamming_sse42_32 :
            public Hamming_simd<uint32_t, __m128i, UNROLL> {

        using Hamming_simd<uint32_t, __m128i, UNROLL>::Hamming_simd;

        virtual ~Hamming_sse42_32() {
        }

    };

    extern template
    struct Hamming_sse42_16<1> ;
    extern template
    struct Hamming_sse42_16<2> ;
    extern template
    struct Hamming_sse42_16<4> ;
    extern template
    struct Hamming_sse42_16<8> ;
    extern template
    struct Hamming_sse42_16<16> ;
    extern template
    struct Hamming_sse42_16<32> ;
    extern template
    struct Hamming_sse42_16<64> ;
    extern template
    struct Hamming_sse42_16<128> ;
    extern template
    struct Hamming_sse42_16<256> ;
    extern template
    struct Hamming_sse42_16<512> ;
    extern template
    struct Hamming_sse42_16<1024> ;

    extern template
    struct Hamming_sse42_32<1> ;
    extern template
    struct Hamming_sse42_32<2> ;
    extern template
    struct Hamming_sse42_32<4> ;
    extern template
    struct Hamming_sse42_32<8> ;
    extern template
    struct Hamming_sse42_32<16> ;
    extern template
    struct Hamming_sse42_32<32> ;
    extern template
    struct Hamming_sse42_32<64> ;
    extern template
    struct Hamming_sse42_32<128> ;
    extern template
    struct Hamming_sse42_32<256> ;
    extern template
    struct Hamming_sse42_32<512> ;
    extern template
    struct Hamming_sse42_32<1024> ;
#endif

#ifdef __AVX2__
    template<size_t UNROLL>
    struct Hamming_avx2_16 :
            public Hamming_simd<uint16_t, __m256i, UNROLL> {
        using Hamming_simd<uint16_t, __m256i, UNROLL>::Hamming_simd;
        virtual ~Hamming_avx2_16() {
        }
    };

    template<size_t UNROLL>
    struct Hamming_avx2_32 :
            public Hamming_simd<uint32_t, __m256i, UNROLL> {
        using Hamming_simd<uint32_t, __m256i, UNROLL>::Hamming_simd;
        virtual ~Hamming_avx2_32() {
        }
    };

    extern template
    struct Hamming_avx2_16<1>;
    extern template
    struct Hamming_avx2_16<2>;
    extern template
    struct Hamming_avx2_16<4>;
    extern template
    struct Hamming_avx2_16<8>;
    extern template
    struct Hamming_avx2_16<16>;
    extern template
    struct Hamming_avx2_16<32>;
    extern template
    struct Hamming_avx2_16<64>;
    extern template
    struct Hamming_avx2_16<128>;
    extern template
    struct Hamming_avx2_16<256>;
    extern template
    struct Hamming_avx2_16<512>;
    extern template
    struct Hamming_avx2_16<1024>;

    extern template
    struct Hamming_avx2_32<1>;
    extern template
    struct Hamming_avx2_32<2>;
    extern template
    struct Hamming_avx2_32<4>;
    extern template
    struct Hamming_avx2_32<8>;
    extern template
    struct Hamming_avx2_32<16>;
    extern template
    struct Hamming_avx2_32<32>;
    extern template
    struct Hamming_avx2_32<64>;
    extern template
    struct Hamming_avx2_32<128>;
    extern template
    struct Hamming_avx2_32<256>;
    extern template
    struct Hamming_avx2_32<512>;
    extern template
    struct Hamming_avx2_32<1024>;
    #endif

#ifdef __AVX512F__
    template<size_t UNROLL>
    struct Hamming_avx512_16 :
            public Hamming_simd<uint16_t, __m512i, UNROLL> {
        using Hamming_simd<uint16_t, __m512i, UNROLL>::Hamming_simd;
        virtual ~Hamming_avx512_16() {
        }
    };

    template<size_t UNROLL>
    struct Hamming_avx512_32 :
            public Hamming_simd<uint32_t, __m512i, UNROLL> {
        using Hamming_simd<uint32_t, __m512i, UNROLL>::Hamming_simd;
        virtual ~Hamming_avx512_32() {
        }
    };

    extern template
    struct Hamming_avx512_16<1>;
    extern template
    struct Hamming_avx512_16<2>;
    extern template
    struct Hamming_avx512_16<4>;
    extern template
    struct Hamming_avx512_16<8>;
    extern template
    struct Hamming_avx512_16<16>;
    extern template
    struct Hamming_avx512_16<32>;
    extern template
    struct Hamming_avx512_16<64>;
    extern template
    struct Hamming_avx512_16<128>;
    extern template
    struct Hamming_avx512_16<256>;
    extern template
    struct Hamming_avx512_16<512>;
    extern template
    struct Hamming_avx512_16<1024>;

    extern template
    struct Hamming_avx512_32<1>;
    extern template
    struct Hamming_avx512_32<2>;
    extern template
    struct Hamming_avx512_32<4>;
    extern template
    struct Hamming_avx512_32<8>;
    extern template
    struct Hamming_avx512_32<16>;
    extern template
    struct Hamming_avx512_32<32>;
    extern template
    struct Hamming_avx512_32<64>;
    extern template
    struct Hamming_avx512_32<128>;
    extern template
    struct Hamming_avx512_32<256>;
    extern template
    struct Hamming_avx512_32<512>;
    extern template
    struct Hamming_avx512_32<1024>;
#endif

}
