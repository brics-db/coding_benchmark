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
 * File:   XOR_avx2.hpp
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 07-07-2017 17:14
 */

#pragma once

#define XOR_AVX2

#include <XOR/XOR_avx2.tcc>

#undef XOR_AVX2

namespace coding_benchmark {

    template<size_t BLOCKSIZE>
    struct XOR_avx2_16x16_16 :
            public XOR_avx2<uint16_t, uint16_t, BLOCKSIZE>,
            public AVX2Test {

        using XOR_avx2<uint16_t, uint16_t, BLOCKSIZE>::XOR_avx2;

        virtual ~XOR_avx2_16x16_16() {
        }
    };

    template<size_t BLOCKSIZE>
    struct XOR_avx2_16x16_16x16 :
            public XOR_avx2<uint16_t, __m256i, BLOCKSIZE>,
            public AVX2Test {

        using XOR_avx2<uint16_t, __m256i, BLOCKSIZE>::XOR_avx2;

        virtual ~XOR_avx2_16x16_16x16() {
        }
    };

    template<size_t BLOCKSIZE>
    struct XOR_avx2_8x32_32 :
            public XOR_avx2<uint32_t, uint32_t, BLOCKSIZE>,
            public AVX2Test {

        using XOR_avx2<uint32_t, uint32_t, BLOCKSIZE>::XOR_avx2;

        virtual ~XOR_avx2_8x32_32() {
        }
    };

    template<size_t BLOCKSIZE>
    struct XOR_avx2_8x32_8x32 :
            public XOR_avx2<uint32_t, __m256i, BLOCKSIZE>,
            public AVX2Test {

        using XOR_avx2<uint32_t, __m256i, BLOCKSIZE>::XOR_avx2;

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

}
