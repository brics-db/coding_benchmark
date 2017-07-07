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
 * File:   XOR_seq.hpp
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 07-07-2017 17:16
 */
#ifndef XOR_XOR_SCALAR_HPP_
#define XOR_XOR_SCALAR_HPP_

#include <XOR/XOR_scalar.tcc>

template<size_t BLOCKSIZE>
struct XOR_seq_32_8 :
        public XOR_seq<uint32_t, uint8_t, BLOCKSIZE>,
        public SequentialTest {

    using XOR_seq<uint32_t, uint8_t, BLOCKSIZE>::XOR_seq;

    virtual ~XOR_seq_32_8() {
    }
};

template<size_t BLOCKSIZE>
struct XOR_seq_32_32 :
        public XOR_seq<uint32_t, uint32_t, BLOCKSIZE>,
        public SequentialTest {

    using XOR_seq<uint32_t, uint32_t, BLOCKSIZE>::XOR_seq;

    virtual ~XOR_seq_32_32() {
    }
};

template<size_t BLOCKSIZE>
struct XOR_seq_16_8 :
        public XOR_seq<uint16_t, uint8_t, BLOCKSIZE>,
        public SequentialTest {

    using XOR_seq<uint16_t, uint8_t, BLOCKSIZE>::XOR_seq;

    virtual ~XOR_seq_16_8() {
    }
};

template<size_t BLOCKSIZE>
struct XOR_seq_16_16 :
        public XOR_seq<uint16_t, uint16_t, BLOCKSIZE>,
        public SequentialTest {

    using XOR_seq<uint16_t, uint16_t, BLOCKSIZE>::XOR_seq;

    virtual ~XOR_seq_16_16() {
    }
};

#endif /* XOR_XOR_SCALAR_HPP_ */
