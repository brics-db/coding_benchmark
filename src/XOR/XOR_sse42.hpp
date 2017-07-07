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
 * File:   XOR_sse42.hpp
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 07-07-2017 17:18
 */
#ifndef XOR_XOR_SSE42_HPP_
#define XOR_XOR_SSE42_HPP_

#include <XOR/XOR_sse42.tcc>

template<size_t BLOCKSIZE>
struct XOR_sse42_4x32_32 :
        public XOR_sse42<uint32_t, uint32_t, BLOCKSIZE>,
        public SSE42Test {

    using XOR_sse42<uint32_t, uint32_t, BLOCKSIZE>::XOR_sse42;

    virtual ~XOR_sse42_4x32_32() {
    }
};

template<size_t BLOCKSIZE>
struct XOR_sse42_4x32_4x32 :
        public XOR_sse42<uint32_t, uint32_t, BLOCKSIZE>,
        public SSE42Test {

    using XOR_sse42<uint32_t, uint32_t, BLOCKSIZE>::XOR_sse42;

    virtual ~XOR_sse42_4x32_4x32() {
    }
};

template<size_t BLOCKSIZE>
struct XOR_sse42_8x16_16 :
        public XOR_sse42<uint16_t, uint16_t, BLOCKSIZE>,
        public SSE42Test {

    using XOR_sse42<uint16_t, uint16_t, BLOCKSIZE>::XOR_sse42;

    virtual ~XOR_sse42_8x16_16() {
    }
};

template<size_t BLOCKSIZE>
struct XOR_sse42_8x16_8x16 :
        public XOR_sse42<uint16_t, __m128i, BLOCKSIZE>,
        public SSE42Test {

    using XOR_sse42<uint16_t, __m128i, BLOCKSIZE>::XOR_sse42;

    virtual ~XOR_sse42_8x16_8x16() {
    }
};

#endif /* XOR_XOR_SSE42_HPP_ */
