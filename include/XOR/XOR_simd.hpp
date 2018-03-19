// Copyright (c) 2017-2018 Till Kolditz
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
 * File:   XOR_sse42.hpp
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 07-07-2017 17:18
 * Renamed on 19-03-2018 14:40
 */

#pragma once

#if defined(__SSE4_2__) or defined(__AVX2__) or defined(__AVX512_F__)

#define XOR_SIMD

#include <XOR/XOR_simd.tcc>

#undef XOR_SIMD

namespace coding_benchmark {

#ifdef __SSE4_2__

    template<size_t BLOCKSIZE>
    struct XOR_sse42_4x32_32 :
            public XOR_simd<uint32_t, uint32_t, __m128i, BLOCKSIZE> {

        using XOR_simd<uint32_t, uint32_t, __m128i, BLOCKSIZE>::XOR_simd;

        virtual ~XOR_sse42_4x32_32() {
        }
    };

    template<size_t BLOCKSIZE>
    struct XOR_sse42_4x32_4x32 :
            public XOR_simd<uint32_t, uint32_t, __m128i, BLOCKSIZE> {

        using XOR_simd<uint32_t, uint32_t, __m128i, BLOCKSIZE>::XOR_simd;

        virtual ~XOR_sse42_4x32_4x32() {
        }
    };

    template<size_t BLOCKSIZE>
    struct XOR_sse42_8x16_16 :
            public XOR_simd<uint16_t, uint16_t, __m128i, BLOCKSIZE> {

        using XOR_simd<uint16_t, uint16_t, __m128i, BLOCKSIZE>::XOR_simd;

        virtual ~XOR_sse42_8x16_16() {
        }
    };

    template<size_t BLOCKSIZE>
    struct XOR_sse42_8x16_8x16 :
            public XOR_simd<uint16_t, __m128i, __m128i, BLOCKSIZE> {

        using XOR_simd<uint16_t, __m128i, __m128i, BLOCKSIZE>::XOR_simd;

        virtual ~XOR_sse42_8x16_8x16() {
        }
    };

    extern template
    struct XOR_sse42_4x32_32<1> ;
    extern template
    struct XOR_sse42_4x32_32<2> ;
    extern template
    struct XOR_sse42_4x32_32<4> ;
    extern template
    struct XOR_sse42_4x32_32<8> ;
    extern template
    struct XOR_sse42_4x32_32<16> ;
    extern template
    struct XOR_sse42_4x32_32<32> ;
    extern template
    struct XOR_sse42_4x32_32<64> ;
    extern template
    struct XOR_sse42_4x32_32<128> ;
    extern template
    struct XOR_sse42_4x32_32<256> ;
    extern template
    struct XOR_sse42_4x32_32<512> ;
    extern template
    struct XOR_sse42_4x32_32<1024> ;

    extern template
    struct XOR_sse42_4x32_4x32<1> ;
    extern template
    struct XOR_sse42_4x32_4x32<2> ;
    extern template
    struct XOR_sse42_4x32_4x32<4> ;
    extern template
    struct XOR_sse42_4x32_4x32<8> ;
    extern template
    struct XOR_sse42_4x32_4x32<16> ;
    extern template
    struct XOR_sse42_4x32_4x32<32> ;
    extern template
    struct XOR_sse42_4x32_4x32<64> ;
    extern template
    struct XOR_sse42_4x32_4x32<128> ;
    extern template
    struct XOR_sse42_4x32_4x32<256> ;
    extern template
    struct XOR_sse42_4x32_4x32<512> ;
    extern template
    struct XOR_sse42_4x32_4x32<1024> ;

    extern template
    struct XOR_sse42_8x16_16<1> ;
    extern template
    struct XOR_sse42_8x16_16<2> ;
    extern template
    struct XOR_sse42_8x16_16<4> ;
    extern template
    struct XOR_sse42_8x16_16<8> ;
    extern template
    struct XOR_sse42_8x16_16<16> ;
    extern template
    struct XOR_sse42_8x16_16<32> ;
    extern template
    struct XOR_sse42_8x16_16<64> ;
    extern template
    struct XOR_sse42_8x16_16<128> ;
    extern template
    struct XOR_sse42_8x16_16<256> ;
    extern template
    struct XOR_sse42_8x16_16<512> ;
    extern template
    struct XOR_sse42_8x16_16<1024> ;

    extern template
    struct XOR_sse42_8x16_8x16<1> ;
    extern template
    struct XOR_sse42_8x16_8x16<2> ;
    extern template
    struct XOR_sse42_8x16_8x16<4> ;
    extern template
    struct XOR_sse42_8x16_8x16<8> ;
    extern template
    struct XOR_sse42_8x16_8x16<16> ;
    extern template
    struct XOR_sse42_8x16_8x16<32> ;
    extern template
    struct XOR_sse42_8x16_8x16<64> ;
    extern template
    struct XOR_sse42_8x16_8x16<128> ;
    extern template
    struct XOR_sse42_8x16_8x16<256> ;
    extern template
    struct XOR_sse42_8x16_8x16<512> ;
    extern template
    struct XOR_sse42_8x16_8x16<1024> ;

#endif /* __SSE4_2__ */

#ifdef __AVX2__

    template<size_t BLOCKSIZE>
    struct XOR_avx2_16x16_16 :
            public XOR_simd<uint16_t, uint16_t, __m256i, BLOCKSIZE> {

        using XOR_simd<uint16_t, uint16_t, __m256i, BLOCKSIZE>::XOR_simd;

        virtual ~XOR_avx2_16x16_16() {
        }
    };

    template<size_t BLOCKSIZE>
    struct XOR_avx2_16x16_16x16 :
            public XOR_simd<uint16_t, __m256i, __m256i, BLOCKSIZE> {

        using XOR_simd<uint16_t, __m256i, __m256i, BLOCKSIZE>::XOR_simd;

        virtual ~XOR_avx2_16x16_16x16() {
        }
    };

    template<size_t BLOCKSIZE>
    struct XOR_avx2_8x32_32 :
            public XOR_simd<uint32_t, uint32_t, __m256i, BLOCKSIZE> {

        using XOR_simd<uint32_t, uint32_t, __m256i, BLOCKSIZE>::XOR_simd;

        virtual ~XOR_avx2_8x32_32() {
        }
    };

    template<size_t BLOCKSIZE>
    struct XOR_avx2_8x32_8x32 :
            public XOR_simd<uint32_t, __m256i, __m256i, BLOCKSIZE> {

        using XOR_simd<uint32_t, __m256i, __m256i, BLOCKSIZE>::XOR_simd;

        virtual ~XOR_avx2_8x32_8x32() {
        }
    };

    extern template
    struct XOR_avx2_16x16_16<1> ;
    extern template
    struct XOR_avx2_16x16_16<2> ;
    extern template
    struct XOR_avx2_16x16_16<4> ;
    extern template
    struct XOR_avx2_16x16_16<8> ;
    extern template
    struct XOR_avx2_16x16_16<16> ;
    extern template
    struct XOR_avx2_16x16_16<32> ;
    extern template
    struct XOR_avx2_16x16_16<64> ;
    extern template
    struct XOR_avx2_16x16_16<128> ;
    extern template
    struct XOR_avx2_16x16_16<256> ;
    extern template
    struct XOR_avx2_16x16_16<512> ;
    extern template
    struct XOR_avx2_16x16_16<1024> ;

    extern template
    struct XOR_avx2_16x16_16x16<1> ;
    extern template
    struct XOR_avx2_16x16_16x16<2> ;
    extern template
    struct XOR_avx2_16x16_16x16<4> ;
    extern template
    struct XOR_avx2_16x16_16x16<8> ;
    extern template
    struct XOR_avx2_16x16_16x16<16> ;
    extern template
    struct XOR_avx2_16x16_16x16<32> ;
    extern template
    struct XOR_avx2_16x16_16x16<64> ;
    extern template
    struct XOR_avx2_16x16_16x16<128> ;
    extern template
    struct XOR_avx2_16x16_16x16<256> ;
    extern template
    struct XOR_avx2_16x16_16x16<512> ;
    extern template
    struct XOR_avx2_16x16_16x16<1024> ;

    extern template
    struct XOR_avx2_8x32_32<1> ;
    extern template
    struct XOR_avx2_8x32_32<2> ;
    extern template
    struct XOR_avx2_8x32_32<4> ;
    extern template
    struct XOR_avx2_8x32_32<8> ;
    extern template
    struct XOR_avx2_8x32_32<16> ;
    extern template
    struct XOR_avx2_8x32_32<32> ;
    extern template
    struct XOR_avx2_8x32_32<64> ;
    extern template
    struct XOR_avx2_8x32_32<128> ;
    extern template
    struct XOR_avx2_8x32_32<256> ;
    extern template
    struct XOR_avx2_8x32_32<512> ;
    extern template
    struct XOR_avx2_8x32_32<1024> ;

    extern template
    struct XOR_avx2_8x32_8x32<1> ;
    extern template
    struct XOR_avx2_8x32_8x32<2> ;
    extern template
    struct XOR_avx2_8x32_8x32<4> ;
    extern template
    struct XOR_avx2_8x32_8x32<8> ;
    extern template
    struct XOR_avx2_8x32_8x32<16> ;
    extern template
    struct XOR_avx2_8x32_8x32<32> ;
    extern template
    struct XOR_avx2_8x32_8x32<64> ;
    extern template
    struct XOR_avx2_8x32_8x32<128> ;
    extern template
    struct XOR_avx2_8x32_8x32<256> ;
    extern template
    struct XOR_avx2_8x32_8x32<512> ;
    extern template
    struct XOR_avx2_8x32_8x32<1024> ;

#endif /* __AVX2__ */

}

#endif /* defined(__SSE4_2__) or defined(__AVX2__) or defined(__AVX512_F__) */
