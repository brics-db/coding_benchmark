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
#ifndef XOR_XOR_AVX2_HPP_
#define XOR_XOR_AVX2_HPP_

#include <XOR/XOR_avx2.tcc>

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

#endif /* XOR_XOR_AVX2_HPP_ */
