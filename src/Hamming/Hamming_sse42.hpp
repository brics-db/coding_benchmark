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
 * File:   Hamming_sse42.hpp
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 07-07-2017 17:24
 */
#ifndef HAMMING_HAMMING_SSE42_HPP_
#define HAMMING_HAMMING_SSE42_HPP_

#include <Hamming/Hamming_sse42.tcc>

template<size_t UNROLL>
struct Hamming_sse42_16 :
        public Hamming_sse42<uint16_t, UNROLL> {

    using Hamming_sse42<uint16_t, UNROLL>::Hamming_sse42;

    virtual ~Hamming_sse42_16() {
    }

};

template<size_t UNROLL>
struct Hamming_sse42_32 :
        public Hamming_sse42<uint32_t, UNROLL> {

    using Hamming_sse42<uint32_t, UNROLL>::Hamming_sse42;

    virtual ~Hamming_sse42_32() {
    }

};

#endif /* HAMMING_HAMMING_SSE42_HPP_ */
