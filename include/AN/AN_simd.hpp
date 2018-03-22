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
 * File:   AN_simd.hpp
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 15-08-2017 18:20
 * Renamed in 19-03-2018 16:08
 */

#pragma once

#if defined(__SSE4_2__) or defined(__AVX2__) or defined(__AVX512F__)

#define AN_SIMD

#include <AN/AN_simd_divmod.tcc>
#include <AN/AN_simd_inv.tcc>

#undef AN_SIMD

namespace coding_benchmark {

#ifdef __SSE4_2__

    template<size_t UNROLL>
    struct AN_sse42_8_16_s_inv :
            public AN_simd_inv<int8_t, int16_t, __m128i, UNROLL> {

        using AN_simd_inv<int8_t, int16_t, __m128i, UNROLL>::AN_simd_inv;

        virtual ~AN_sse42_8_16_s_inv() {
        }
    };

    template<size_t UNROLL>
    struct AN_sse42_8_16_u_inv :
            public AN_simd_inv<uint8_t, uint16_t, __m128i, UNROLL> {

        using AN_simd_inv<uint8_t, uint16_t, __m128i, UNROLL>::AN_simd_inv;

        virtual ~AN_sse42_8_16_u_inv() {
        }
    };

    template<size_t UNROLL>
    struct AN_sse42_16_32_s_inv :
            public AN_simd_inv<int16_t, int32_t, __m128i, UNROLL> {

        using AN_simd_inv<int16_t, int32_t, __m128i, UNROLL>::AN_simd_inv;

        virtual ~AN_sse42_16_32_s_inv() {
        }
    };

    template<size_t UNROLL>
    struct AN_sse42_16_32_u_inv :
            public AN_simd_inv<uint16_t, uint32_t, __m128i, UNROLL> {

        using AN_simd_inv<uint16_t, uint32_t, __m128i, UNROLL>::AN_simd_inv;

        virtual ~AN_sse42_16_32_u_inv() {
        }
    };

    template<size_t UNROLL>
    struct AN_sse42_32_64_s_inv :
            public AN_simd_inv<int32_t, int64_t, __m128i, UNROLL> {

        using AN_simd_inv<int32_t, int64_t, __m128i, UNROLL>::AN_simd_inv;

        virtual ~AN_sse42_32_64_s_inv() {
        }
    };

    template<size_t UNROLL>
    struct AN_sse42_32_64_u_inv :
            public AN_simd_inv<uint32_t, uint64_t, __m128i, UNROLL> {

        using AN_simd_inv<uint32_t, uint64_t, __m128i, UNROLL>::AN_simd_inv;

        virtual ~AN_sse42_32_64_u_inv() {
        }
    };

    template<size_t UNROLL>
    struct AN_sse42_8_16_s_divmod :
            public AN_simd_divmod<int8_t, int16_t, __m128i, UNROLL> {

        using AN_simd_divmod<int8_t, int16_t, __m128i, UNROLL>::AN_simd_divmod;

        virtual ~AN_sse42_8_16_s_divmod() {
        }
    };

    template<size_t UNROLL>
    struct AN_sse42_8_16_u_divmod :
            public AN_simd_divmod<uint8_t, uint16_t, __m128i, UNROLL> {

        using AN_simd_divmod<uint8_t, uint16_t, __m128i, UNROLL>::AN_simd_divmod;

        virtual ~AN_sse42_8_16_u_divmod() {
        }
    };

    template<size_t UNROLL>
    struct AN_sse42_16_32_s_divmod :
            public AN_simd_divmod<int16_t, int32_t, __m128i, UNROLL> {

        using AN_simd_divmod<int16_t, int32_t, __m128i, UNROLL>::AN_simd_divmod;

        virtual ~AN_sse42_16_32_s_divmod() {
        }
    };

    template<size_t UNROLL>
    struct AN_sse42_16_32_u_divmod :
            public AN_simd_divmod<uint16_t, uint32_t, __m128i, UNROLL> {

        using AN_simd_divmod<uint16_t, uint32_t, __m128i, UNROLL>::AN_simd_divmod;

        virtual ~AN_sse42_16_32_u_divmod() {
        }
    };

    template<size_t UNROLL>
    struct AN_sse42_32_64_s_divmod :
            public AN_simd_divmod<int32_t, int64_t, __m128i, UNROLL> {

        using AN_simd_divmod<int32_t, int64_t, __m128i, UNROLL>::AN_simd_divmod;

        virtual ~AN_sse42_32_64_s_divmod() {
        }
    };

    template<size_t UNROLL>
    struct AN_sse42_32_64_u_divmod :
            public AN_simd_divmod<uint32_t, uint64_t, __m128i, UNROLL> {

        using AN_simd_divmod<uint32_t, uint64_t, __m128i, UNROLL>::AN_simd_divmod;

        virtual ~AN_sse42_32_64_u_divmod() {
        }
    };

    extern template
    struct AN_sse42_8_16_s_inv<1> ;
    extern template
    struct AN_sse42_8_16_s_inv<2> ;
    extern template
    struct AN_sse42_8_16_s_inv<4> ;
    extern template
    struct AN_sse42_8_16_s_inv<8> ;
    extern template
    struct AN_sse42_8_16_s_inv<16> ;
    extern template
    struct AN_sse42_8_16_s_inv<32> ;
    extern template
    struct AN_sse42_8_16_s_inv<64> ;
    extern template
    struct AN_sse42_8_16_s_inv<128> ;
    extern template
    struct AN_sse42_8_16_s_inv<256> ;
    extern template
    struct AN_sse42_8_16_s_inv<512> ;
    extern template
    struct AN_sse42_8_16_s_inv<1024> ;

    extern template
    struct AN_sse42_8_16_u_inv<1> ;
    extern template
    struct AN_sse42_8_16_u_inv<2> ;
    extern template
    struct AN_sse42_8_16_u_inv<4> ;
    extern template
    struct AN_sse42_8_16_u_inv<8> ;
    extern template
    struct AN_sse42_8_16_u_inv<16> ;
    extern template
    struct AN_sse42_8_16_u_inv<32> ;
    extern template
    struct AN_sse42_8_16_u_inv<64> ;
    extern template
    struct AN_sse42_8_16_u_inv<128> ;
    extern template
    struct AN_sse42_8_16_u_inv<256> ;
    extern template
    struct AN_sse42_8_16_u_inv<512> ;
    extern template
    struct AN_sse42_8_16_u_inv<1024> ;

    extern template
    struct AN_sse42_16_32_s_inv<1> ;
    extern template
    struct AN_sse42_16_32_s_inv<2> ;
    extern template
    struct AN_sse42_16_32_s_inv<4> ;
    extern template
    struct AN_sse42_16_32_s_inv<8> ;
    extern template
    struct AN_sse42_16_32_s_inv<16> ;
    extern template
    struct AN_sse42_16_32_s_inv<32> ;
    extern template
    struct AN_sse42_16_32_s_inv<64> ;
    extern template
    struct AN_sse42_16_32_s_inv<128> ;
    extern template
    struct AN_sse42_16_32_s_inv<256> ;
    extern template
    struct AN_sse42_16_32_s_inv<512> ;
    extern template
    struct AN_sse42_16_32_s_inv<1024> ;

    extern template
    struct AN_sse42_16_32_u_inv<1> ;
    extern template
    struct AN_sse42_16_32_u_inv<2> ;
    extern template
    struct AN_sse42_16_32_u_inv<4> ;
    extern template
    struct AN_sse42_16_32_u_inv<8> ;
    extern template
    struct AN_sse42_16_32_u_inv<16> ;
    extern template
    struct AN_sse42_16_32_u_inv<32> ;
    extern template
    struct AN_sse42_16_32_u_inv<64> ;
    extern template
    struct AN_sse42_16_32_u_inv<128> ;
    extern template
    struct AN_sse42_16_32_u_inv<256> ;
    extern template
    struct AN_sse42_16_32_u_inv<512> ;
    extern template
    struct AN_sse42_16_32_u_inv<1024> ;

    extern template
    struct AN_sse42_32_64_s_inv<1> ;
    extern template
    struct AN_sse42_32_64_s_inv<2> ;
    extern template
    struct AN_sse42_32_64_s_inv<4> ;
    extern template
    struct AN_sse42_32_64_s_inv<8> ;
    extern template
    struct AN_sse42_32_64_s_inv<16> ;
    extern template
    struct AN_sse42_32_64_s_inv<32> ;
    extern template
    struct AN_sse42_32_64_s_inv<64> ;
    extern template
    struct AN_sse42_32_64_s_inv<128> ;
    extern template
    struct AN_sse42_32_64_s_inv<256> ;
    extern template
    struct AN_sse42_32_64_s_inv<512> ;
    extern template
    struct AN_sse42_32_64_s_inv<1024> ;

    extern template
    struct AN_sse42_32_64_u_inv<1> ;
    extern template
    struct AN_sse42_32_64_u_inv<2> ;
    extern template
    struct AN_sse42_32_64_u_inv<4> ;
    extern template
    struct AN_sse42_32_64_u_inv<8> ;
    extern template
    struct AN_sse42_32_64_u_inv<16> ;
    extern template
    struct AN_sse42_32_64_u_inv<32> ;
    extern template
    struct AN_sse42_32_64_u_inv<64> ;
    extern template
    struct AN_sse42_32_64_u_inv<128> ;
    extern template
    struct AN_sse42_32_64_u_inv<256> ;
    extern template
    struct AN_sse42_32_64_u_inv<512> ;
    extern template
    struct AN_sse42_32_64_u_inv<1024> ;

    extern template
    struct AN_sse42_8_16_s_divmod<1> ;
    extern template
    struct AN_sse42_8_16_s_divmod<2> ;
    extern template
    struct AN_sse42_8_16_s_divmod<4> ;
    extern template
    struct AN_sse42_8_16_s_divmod<8> ;
    extern template
    struct AN_sse42_8_16_s_divmod<16> ;
    extern template
    struct AN_sse42_8_16_s_divmod<32> ;
    extern template
    struct AN_sse42_8_16_s_divmod<64> ;
    extern template
    struct AN_sse42_8_16_s_divmod<128> ;
    extern template
    struct AN_sse42_8_16_s_divmod<256> ;
    extern template
    struct AN_sse42_8_16_s_divmod<512> ;
    extern template
    struct AN_sse42_8_16_s_divmod<1024> ;

    extern template
    struct AN_sse42_8_16_u_divmod<1> ;
    extern template
    struct AN_sse42_8_16_u_divmod<2> ;
    extern template
    struct AN_sse42_8_16_u_divmod<4> ;
    extern template
    struct AN_sse42_8_16_u_divmod<8> ;
    extern template
    struct AN_sse42_8_16_u_divmod<16> ;
    extern template
    struct AN_sse42_8_16_u_divmod<32> ;
    extern template
    struct AN_sse42_8_16_u_divmod<64> ;
    extern template
    struct AN_sse42_8_16_u_divmod<128> ;
    extern template
    struct AN_sse42_8_16_u_divmod<256> ;
    extern template
    struct AN_sse42_8_16_u_divmod<512> ;
    extern template
    struct AN_sse42_8_16_u_divmod<1024> ;

    extern template
    struct AN_sse42_16_32_s_divmod<1> ;
    extern template
    struct AN_sse42_16_32_s_divmod<2> ;
    extern template
    struct AN_sse42_16_32_s_divmod<4> ;
    extern template
    struct AN_sse42_16_32_s_divmod<8> ;
    extern template
    struct AN_sse42_16_32_s_divmod<16> ;
    extern template
    struct AN_sse42_16_32_s_divmod<32> ;
    extern template
    struct AN_sse42_16_32_s_divmod<64> ;
    extern template
    struct AN_sse42_16_32_s_divmod<128> ;
    extern template
    struct AN_sse42_16_32_s_divmod<256> ;
    extern template
    struct AN_sse42_16_32_s_divmod<512> ;
    extern template
    struct AN_sse42_16_32_s_divmod<1024> ;

    extern template
    struct AN_sse42_16_32_u_divmod<1> ;
    extern template
    struct AN_sse42_16_32_u_divmod<2> ;
    extern template
    struct AN_sse42_16_32_u_divmod<4> ;
    extern template
    struct AN_sse42_16_32_u_divmod<8> ;
    extern template
    struct AN_sse42_16_32_u_divmod<16> ;
    extern template
    struct AN_sse42_16_32_u_divmod<32> ;
    extern template
    struct AN_sse42_16_32_u_divmod<64> ;
    extern template
    struct AN_sse42_16_32_u_divmod<128> ;
    extern template
    struct AN_sse42_16_32_u_divmod<256> ;
    extern template
    struct AN_sse42_16_32_u_divmod<512> ;
    extern template
    struct AN_sse42_16_32_u_divmod<1024> ;

    extern template
    struct AN_sse42_32_64_s_divmod<1> ;
    extern template
    struct AN_sse42_32_64_s_divmod<2> ;
    extern template
    struct AN_sse42_32_64_s_divmod<4> ;
    extern template
    struct AN_sse42_32_64_s_divmod<8> ;
    extern template
    struct AN_sse42_32_64_s_divmod<16> ;
    extern template
    struct AN_sse42_32_64_s_divmod<32> ;
    extern template
    struct AN_sse42_32_64_s_divmod<64> ;
    extern template
    struct AN_sse42_32_64_s_divmod<128> ;
    extern template
    struct AN_sse42_32_64_s_divmod<256> ;
    extern template
    struct AN_sse42_32_64_s_divmod<512> ;
    extern template
    struct AN_sse42_32_64_s_divmod<1024> ;

    extern template
    struct AN_sse42_32_64_u_divmod<1> ;
    extern template
    struct AN_sse42_32_64_u_divmod<2> ;
    extern template
    struct AN_sse42_32_64_u_divmod<4> ;
    extern template
    struct AN_sse42_32_64_u_divmod<8> ;
    extern template
    struct AN_sse42_32_64_u_divmod<16> ;
    extern template
    struct AN_sse42_32_64_u_divmod<32> ;
    extern template
    struct AN_sse42_32_64_u_divmod<64> ;
    extern template
    struct AN_sse42_32_64_u_divmod<128> ;
    extern template
    struct AN_sse42_32_64_u_divmod<256> ;
    extern template
    struct AN_sse42_32_64_u_divmod<512> ;
    extern template
    struct AN_sse42_32_64_u_divmod<1024> ;

#endif /* __SSE4_2__ */

#ifdef __AVX2__

    template<size_t UNROLL>
    struct AN_avx2_8_16_s_inv :
            public AN_simd_inv<int8_t, int16_t, __m256i, UNROLL> {

        using AN_simd_inv<int8_t, int16_t, __m256i, UNROLL>::AN_simd_inv;

        virtual ~AN_avx2_8_16_s_inv() {
        }
    };

    template<size_t UNROLL>
    struct AN_avx2_8_16_u_inv :
            public AN_simd_inv<uint8_t, uint16_t, __m256i, UNROLL> {

        using AN_simd_inv<uint8_t, uint16_t, __m256i, UNROLL>::AN_simd_inv;

        virtual ~AN_avx2_8_16_u_inv() {
        }
    };

    template<size_t UNROLL>
    struct AN_avx2_16_32_s_inv :
            public AN_simd_inv<int16_t, int32_t, __m256i, UNROLL> {

        using AN_simd_inv<int16_t, int32_t, __m256i, UNROLL>::AN_simd_inv;

        virtual ~AN_avx2_16_32_s_inv() {
        }
    };

    template<size_t UNROLL>
    struct AN_avx2_16_32_u_inv :
            public AN_simd_inv<uint16_t, uint32_t, __m256i, UNROLL> {

        using AN_simd_inv<uint16_t, uint32_t, __m256i, UNROLL>::AN_simd_inv;

        virtual ~AN_avx2_16_32_u_inv() {
        }
    };

    template<size_t UNROLL>
    struct AN_avx2_32_64_s_inv :
            public AN_simd_inv<int32_t, int64_t, __m256i, UNROLL> {

        using AN_simd_inv<int32_t, int64_t, __m256i, UNROLL>::AN_simd_inv;

        virtual ~AN_avx2_32_64_s_inv() {
        }
    };

    template<size_t UNROLL>
    struct AN_avx2_32_64_u_inv :
            public AN_simd_inv<uint32_t, uint64_t, __m256i, UNROLL> {

        using AN_simd_inv<uint32_t, uint64_t, __m256i, UNROLL>::AN_simd_inv;

        virtual ~AN_avx2_32_64_u_inv() {
        }
    };

    template<size_t UNROLL>
    struct AN_avx2_8_16_s_divmod :
            public AN_simd_divmod<int8_t, int16_t, __m256i, UNROLL> {

        using AN_simd_divmod<int8_t, int16_t, __m256i, UNROLL>::AN_simd_divmod;

        virtual ~AN_avx2_8_16_s_divmod() {
        }
    };

    template<size_t UNROLL>
    struct AN_avx2_8_16_u_divmod :
            public AN_simd_divmod<uint8_t, uint16_t, __m256i, UNROLL> {

        using AN_simd_divmod<uint8_t, uint16_t, __m256i, UNROLL>::AN_simd_divmod;

        virtual ~AN_avx2_8_16_u_divmod() {
        }
    };

    template<size_t UNROLL>
    struct AN_avx2_16_32_s_divmod :
            public AN_simd_divmod<int16_t, int32_t, __m256i, UNROLL> {

        using AN_simd_divmod<int16_t, int32_t, __m256i, UNROLL>::AN_simd_divmod;

        virtual ~AN_avx2_16_32_s_divmod() {
        }
    };

    template<size_t UNROLL>
    struct AN_avx2_16_32_u_divmod :
            public AN_simd_divmod<uint16_t, uint32_t, __m256i, UNROLL> {

        using AN_simd_divmod<uint16_t, uint32_t, __m256i, UNROLL>::AN_simd_divmod;

        virtual ~AN_avx2_16_32_u_divmod() {
        }
    };

    template<size_t UNROLL>
    struct AN_avx2_32_64_s_divmod :
            public AN_simd_divmod<int32_t, int64_t, __m256i, UNROLL> {

        using AN_simd_divmod<int32_t, int64_t, __m256i, UNROLL>::AN_simd_divmod;

        virtual ~AN_avx2_32_64_s_divmod() {
        }
    };

    template<size_t UNROLL>
    struct AN_avx2_32_64_u_divmod :
            public AN_simd_divmod<uint32_t, uint64_t, __m256i, UNROLL> {

        using AN_simd_divmod<uint32_t, uint64_t, __m256i, UNROLL>::AN_simd_divmod;

        virtual ~AN_avx2_32_64_u_divmod() {
        }
    };

    extern template
    struct AN_avx2_8_16_s_inv<1> ;
    extern template
    struct AN_avx2_8_16_s_inv<2> ;
    extern template
    struct AN_avx2_8_16_s_inv<4> ;
    extern template
    struct AN_avx2_8_16_s_inv<8> ;
    extern template
    struct AN_avx2_8_16_s_inv<16> ;
    extern template
    struct AN_avx2_8_16_s_inv<32> ;
    extern template
    struct AN_avx2_8_16_s_inv<64> ;
    extern template
    struct AN_avx2_8_16_s_inv<128> ;
    extern template
    struct AN_avx2_8_16_s_inv<256> ;
    extern template
    struct AN_avx2_8_16_s_inv<512> ;
    extern template
    struct AN_avx2_8_16_s_inv<1024> ;

    extern template
    struct AN_avx2_8_16_u_inv<1> ;
    extern template
    struct AN_avx2_8_16_u_inv<2> ;
    extern template
    struct AN_avx2_8_16_u_inv<4> ;
    extern template
    struct AN_avx2_8_16_u_inv<8> ;
    extern template
    struct AN_avx2_8_16_u_inv<16> ;
    extern template
    struct AN_avx2_8_16_u_inv<32> ;
    extern template
    struct AN_avx2_8_16_u_inv<64> ;
    extern template
    struct AN_avx2_8_16_u_inv<128> ;
    extern template
    struct AN_avx2_8_16_u_inv<256> ;
    extern template
    struct AN_avx2_8_16_u_inv<512> ;
    extern template
    struct AN_avx2_8_16_u_inv<1024> ;

    extern template
    struct AN_avx2_16_32_s_inv<1> ;
    extern template
    struct AN_avx2_16_32_s_inv<2> ;
    extern template
    struct AN_avx2_16_32_s_inv<4> ;
    extern template
    struct AN_avx2_16_32_s_inv<8> ;
    extern template
    struct AN_avx2_16_32_s_inv<16> ;
    extern template
    struct AN_avx2_16_32_s_inv<32> ;
    extern template
    struct AN_avx2_16_32_s_inv<64> ;
    extern template
    struct AN_avx2_16_32_s_inv<128> ;
    extern template
    struct AN_avx2_16_32_s_inv<256> ;
    extern template
    struct AN_avx2_16_32_s_inv<512> ;
    extern template
    struct AN_avx2_16_32_s_inv<1024> ;

    extern template
    struct AN_avx2_16_32_u_inv<1> ;
    extern template
    struct AN_avx2_16_32_u_inv<2> ;
    extern template
    struct AN_avx2_16_32_u_inv<4> ;
    extern template
    struct AN_avx2_16_32_u_inv<8> ;
    extern template
    struct AN_avx2_16_32_u_inv<16> ;
    extern template
    struct AN_avx2_16_32_u_inv<32> ;
    extern template
    struct AN_avx2_16_32_u_inv<64> ;
    extern template
    struct AN_avx2_16_32_u_inv<128> ;
    extern template
    struct AN_avx2_16_32_u_inv<256> ;
    extern template
    struct AN_avx2_16_32_u_inv<512> ;
    extern template
    struct AN_avx2_16_32_u_inv<1024> ;

    extern template
    struct AN_avx2_32_64_s_inv<1> ;
    extern template
    struct AN_avx2_32_64_s_inv<2> ;
    extern template
    struct AN_avx2_32_64_s_inv<4> ;
    extern template
    struct AN_avx2_32_64_s_inv<8> ;
    extern template
    struct AN_avx2_32_64_s_inv<16> ;
    extern template
    struct AN_avx2_32_64_s_inv<32> ;
    extern template
    struct AN_avx2_32_64_s_inv<64> ;
    extern template
    struct AN_avx2_32_64_s_inv<128> ;
    extern template
    struct AN_avx2_32_64_s_inv<256> ;
    extern template
    struct AN_avx2_32_64_s_inv<512> ;
    extern template
    struct AN_avx2_32_64_s_inv<1024> ;

    extern template
    struct AN_avx2_32_64_u_inv<1> ;
    extern template
    struct AN_avx2_32_64_u_inv<2> ;
    extern template
    struct AN_avx2_32_64_u_inv<4> ;
    extern template
    struct AN_avx2_32_64_u_inv<8> ;
    extern template
    struct AN_avx2_32_64_u_inv<16> ;
    extern template
    struct AN_avx2_32_64_u_inv<32> ;
    extern template
    struct AN_avx2_32_64_u_inv<64> ;
    extern template
    struct AN_avx2_32_64_u_inv<128> ;
    extern template
    struct AN_avx2_32_64_u_inv<256> ;
    extern template
    struct AN_avx2_32_64_u_inv<512> ;
    extern template
    struct AN_avx2_32_64_u_inv<1024> ;

    extern template
    struct AN_avx2_8_16_s_divmod<1> ;
    extern template
    struct AN_avx2_8_16_s_divmod<2> ;
    extern template
    struct AN_avx2_8_16_s_divmod<4> ;
    extern template
    struct AN_avx2_8_16_s_divmod<8> ;
    extern template
    struct AN_avx2_8_16_s_divmod<16> ;
    extern template
    struct AN_avx2_8_16_s_divmod<32> ;
    extern template
    struct AN_avx2_8_16_s_divmod<64> ;
    extern template
    struct AN_avx2_8_16_s_divmod<128> ;
    extern template
    struct AN_avx2_8_16_s_divmod<256> ;
    extern template
    struct AN_avx2_8_16_s_divmod<512> ;
    extern template
    struct AN_avx2_8_16_s_divmod<1024> ;

    extern template
    struct AN_avx2_8_16_u_divmod<1> ;
    extern template
    struct AN_avx2_8_16_u_divmod<2> ;
    extern template
    struct AN_avx2_8_16_u_divmod<4> ;
    extern template
    struct AN_avx2_8_16_u_divmod<8> ;
    extern template
    struct AN_avx2_8_16_u_divmod<16> ;
    extern template
    struct AN_avx2_8_16_u_divmod<32> ;
    extern template
    struct AN_avx2_8_16_u_divmod<64> ;
    extern template
    struct AN_avx2_8_16_u_divmod<128> ;
    extern template
    struct AN_avx2_8_16_u_divmod<256> ;
    extern template
    struct AN_avx2_8_16_u_divmod<512> ;
    extern template
    struct AN_avx2_8_16_u_divmod<1024> ;

    extern template
    struct AN_avx2_16_32_s_divmod<1> ;
    extern template
    struct AN_avx2_16_32_s_divmod<2> ;
    extern template
    struct AN_avx2_16_32_s_divmod<4> ;
    extern template
    struct AN_avx2_16_32_s_divmod<8> ;
    extern template
    struct AN_avx2_16_32_s_divmod<16> ;
    extern template
    struct AN_avx2_16_32_s_divmod<32> ;
    extern template
    struct AN_avx2_16_32_s_divmod<64> ;
    extern template
    struct AN_avx2_16_32_s_divmod<128> ;
    extern template
    struct AN_avx2_16_32_s_divmod<256> ;
    extern template
    struct AN_avx2_16_32_s_divmod<512> ;
    extern template
    struct AN_avx2_16_32_s_divmod<1024> ;

    extern template
    struct AN_avx2_16_32_u_divmod<1> ;
    extern template
    struct AN_avx2_16_32_u_divmod<2> ;
    extern template
    struct AN_avx2_16_32_u_divmod<4> ;
    extern template
    struct AN_avx2_16_32_u_divmod<8> ;
    extern template
    struct AN_avx2_16_32_u_divmod<16> ;
    extern template
    struct AN_avx2_16_32_u_divmod<32> ;
    extern template
    struct AN_avx2_16_32_u_divmod<64> ;
    extern template
    struct AN_avx2_16_32_u_divmod<128> ;
    extern template
    struct AN_avx2_16_32_u_divmod<256> ;
    extern template
    struct AN_avx2_16_32_u_divmod<512> ;
    extern template
    struct AN_avx2_16_32_u_divmod<1024> ;

    extern template
    struct AN_avx2_32_64_s_divmod<1> ;
    extern template
    struct AN_avx2_32_64_s_divmod<2> ;
    extern template
    struct AN_avx2_32_64_s_divmod<4> ;
    extern template
    struct AN_avx2_32_64_s_divmod<8> ;
    extern template
    struct AN_avx2_32_64_s_divmod<16> ;
    extern template
    struct AN_avx2_32_64_s_divmod<32> ;
    extern template
    struct AN_avx2_32_64_s_divmod<64> ;
    extern template
    struct AN_avx2_32_64_s_divmod<128> ;
    extern template
    struct AN_avx2_32_64_s_divmod<256> ;
    extern template
    struct AN_avx2_32_64_s_divmod<512> ;
    extern template
    struct AN_avx2_32_64_s_divmod<1024> ;

    extern template
    struct AN_avx2_32_64_u_divmod<1> ;
    extern template
    struct AN_avx2_32_64_u_divmod<2> ;
    extern template
    struct AN_avx2_32_64_u_divmod<4> ;
    extern template
    struct AN_avx2_32_64_u_divmod<8> ;
    extern template
    struct AN_avx2_32_64_u_divmod<16> ;
    extern template
    struct AN_avx2_32_64_u_divmod<32> ;
    extern template
    struct AN_avx2_32_64_u_divmod<64> ;
    extern template
    struct AN_avx2_32_64_u_divmod<128> ;
    extern template
    struct AN_avx2_32_64_u_divmod<256> ;
    extern template
    struct AN_avx2_32_64_u_divmod<512> ;
    extern template
    struct AN_avx2_32_64_u_divmod<1024> ;

#endif /* __AVX2__ */

}

#endif /* defined(__SSE4_2__) or defined(__AVX2__) or defined(__AVX512F__) */
