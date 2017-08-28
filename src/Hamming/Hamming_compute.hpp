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
 * File:   Hamming_compute.hpp
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 22-08-2017 10:51
 */

#pragma once

#include <Hamming/Hamming_compute.tcc>

template<size_t UNROLL>
struct Hamming_compute_scalar_16 :
        public Hamming_compute_scalar<uint16_t, UNROLL> {

    using Hamming_compute_scalar<uint16_t, UNROLL>::Hamming_compute_scalar;

    virtual ~Hamming_compute_scalar_16() {
    }

};

template<size_t UNROLL>
struct Hamming_compute_scalar_32 :
        public Hamming_compute_scalar<uint32_t, UNROLL> {

    using Hamming_compute_scalar<uint32_t, UNROLL>::Hamming_compute_scalar;

    virtual ~Hamming_compute_scalar_32() {
    }

};

#ifdef __SSE4_2__

template<size_t UNROLL>
struct Hamming_compute_sse42_1_16 :
        public Hamming_compute_sse42_1<uint16_t, UNROLL> {

    using Hamming_compute_sse42_1<uint16_t, UNROLL>::Hamming_compute_sse42_1;

    virtual ~Hamming_compute_sse42_1_16() {
    }

};

template<size_t UNROLL>
struct Hamming_compute_sse42_1_32 :
        public Hamming_compute_sse42_1<uint32_t, UNROLL> {

    using Hamming_compute_sse42_1<uint32_t, UNROLL>::Hamming_compute_sse42_1;

    virtual ~Hamming_compute_sse42_1_32() {
    }

};

template<size_t UNROLL>
struct Hamming_compute_sse42_2_16 :
        public Hamming_compute_sse42_2<uint16_t, UNROLL> {

    using Hamming_compute_sse42_2<uint16_t, UNROLL>::Hamming_compute_sse42_2;

    virtual ~Hamming_compute_sse42_2_16() {
    }

};

template<size_t UNROLL>
struct Hamming_compute_sse42_2_32 :
        public Hamming_compute_sse42_2<uint32_t, UNROLL> {

    using Hamming_compute_sse42_2<uint32_t, UNROLL>::Hamming_compute_sse42_2;

    virtual ~Hamming_compute_sse42_2_32() {
    }

};

template<size_t UNROLL>
struct Hamming_compute_sse42_3_16 :
        public Hamming_compute_sse42_3<uint16_t, UNROLL> {

    using Hamming_compute_sse42_3<uint16_t, UNROLL>::Hamming_compute_sse42_3;

    virtual ~Hamming_compute_sse42_3_16() {
    }

};

template<size_t UNROLL>
struct Hamming_compute_sse42_3_32 :
        public Hamming_compute_sse42_3<uint32_t, UNROLL> {

    using Hamming_compute_sse42_3<uint32_t, UNROLL>::Hamming_compute_sse42_3;

    virtual ~Hamming_compute_sse42_3_32() {
    }

};

#endif

#ifdef __AVX2__

template<size_t UNROLL>
struct Hamming_compute_avx2_1_16 :
        public Hamming_compute_avx2_1<uint16_t, UNROLL> {

    using Hamming_compute_avx2_1<uint16_t, UNROLL>::Hamming_compute_avx2_1;

    virtual ~Hamming_compute_avx2_1_16() {
    }

};

template<size_t UNROLL>
struct Hamming_compute_avx2_1_32 :
        public Hamming_compute_avx2_1<uint32_t, UNROLL> {

    using Hamming_compute_avx2_1<uint32_t, UNROLL>::Hamming_compute_avx2_1;

    virtual ~Hamming_compute_avx2_1_32() {
    }

};

template<size_t UNROLL>
struct Hamming_compute_avx2_2_16 :
        public Hamming_compute_avx2_2<uint16_t, UNROLL> {

    using Hamming_compute_avx2_2<uint16_t, UNROLL>::Hamming_compute_avx2_2;

    virtual ~Hamming_compute_avx2_2_16() {
    }

};

template<size_t UNROLL>
struct Hamming_compute_avx2_2_32 :
        public Hamming_compute_avx2_2<uint32_t, UNROLL> {

    using Hamming_compute_avx2_2<uint32_t, UNROLL>::Hamming_compute_avx2_2;

    virtual ~Hamming_compute_avx2_2_32() {
    }

};

template<size_t UNROLL>
struct Hamming_compute_avx2_3_16 :
        public Hamming_compute_avx2_3<uint16_t, UNROLL> {

    using Hamming_compute_avx2_3<uint16_t, UNROLL>::Hamming_compute_avx2_3;

    virtual ~Hamming_compute_avx2_3_16() {
    }

};

template<size_t UNROLL>
struct Hamming_compute_avx2_3_32 :
        public Hamming_compute_avx2_3<uint32_t, UNROLL> {

    using Hamming_compute_avx2_3<uint32_t, UNROLL>::Hamming_compute_avx2_3;

    virtual ~Hamming_compute_avx2_3_32() {
    }

};

#endif

extern template
struct Hamming_compute_scalar_16<1> ;
extern template
struct Hamming_compute_scalar_16<2> ;
extern template
struct Hamming_compute_scalar_16<4> ;
extern template
struct Hamming_compute_scalar_16<8> ;
extern template
struct Hamming_compute_scalar_16<16> ;
extern template
struct Hamming_compute_scalar_16<32> ;
extern template
struct Hamming_compute_scalar_16<64> ;
extern template
struct Hamming_compute_scalar_16<128> ;
extern template
struct Hamming_compute_scalar_16<256> ;
extern template
struct Hamming_compute_scalar_16<512> ;
extern template
struct Hamming_compute_scalar_16<1024> ;

extern template
struct Hamming_compute_scalar_32<1> ;
extern template
struct Hamming_compute_scalar_32<2> ;
extern template
struct Hamming_compute_scalar_32<4> ;
extern template
struct Hamming_compute_scalar_32<8> ;
extern template
struct Hamming_compute_scalar_32<16> ;
extern template
struct Hamming_compute_scalar_32<32> ;
extern template
struct Hamming_compute_scalar_32<64> ;
extern template
struct Hamming_compute_scalar_32<128> ;
extern template
struct Hamming_compute_scalar_32<256> ;
extern template
struct Hamming_compute_scalar_32<512> ;
extern template
struct Hamming_compute_scalar_32<1024> ;

#ifdef __SSE4_2__

extern template
struct Hamming_compute_sse42_1_16<1> ;
extern template
struct Hamming_compute_sse42_1_16<2> ;
extern template
struct Hamming_compute_sse42_1_16<4> ;
extern template
struct Hamming_compute_sse42_1_16<8> ;
extern template
struct Hamming_compute_sse42_1_16<16> ;
extern template
struct Hamming_compute_sse42_1_16<32> ;
extern template
struct Hamming_compute_sse42_1_16<64> ;
extern template
struct Hamming_compute_sse42_1_16<128> ;
extern template
struct Hamming_compute_sse42_1_16<256> ;
extern template
struct Hamming_compute_sse42_1_16<512> ;
extern template
struct Hamming_compute_sse42_1_16<1024> ;

extern template
struct Hamming_compute_sse42_1_32<1> ;
extern template
struct Hamming_compute_sse42_1_32<2> ;
extern template
struct Hamming_compute_sse42_1_32<4> ;
extern template
struct Hamming_compute_sse42_1_32<8> ;
extern template
struct Hamming_compute_sse42_1_32<16> ;
extern template
struct Hamming_compute_sse42_1_32<32> ;
extern template
struct Hamming_compute_sse42_1_32<64> ;
extern template
struct Hamming_compute_sse42_1_32<128> ;
extern template
struct Hamming_compute_sse42_1_32<256> ;
extern template
struct Hamming_compute_sse42_1_32<512> ;
extern template
struct Hamming_compute_sse42_1_32<1024> ;

extern template
struct Hamming_compute_sse42_2_16<1> ;
extern template
struct Hamming_compute_sse42_2_16<2> ;
extern template
struct Hamming_compute_sse42_2_16<4> ;
extern template
struct Hamming_compute_sse42_2_16<8> ;
extern template
struct Hamming_compute_sse42_2_16<16> ;
extern template
struct Hamming_compute_sse42_2_16<32> ;
extern template
struct Hamming_compute_sse42_2_16<64> ;
extern template
struct Hamming_compute_sse42_2_16<128> ;
extern template
struct Hamming_compute_sse42_2_16<256> ;
extern template
struct Hamming_compute_sse42_2_16<512> ;
extern template
struct Hamming_compute_sse42_2_16<1024> ;

extern template
struct Hamming_compute_sse42_2_32<1> ;
extern template
struct Hamming_compute_sse42_2_32<2> ;
extern template
struct Hamming_compute_sse42_2_32<4> ;
extern template
struct Hamming_compute_sse42_2_32<8> ;
extern template
struct Hamming_compute_sse42_2_32<16> ;
extern template
struct Hamming_compute_sse42_2_32<32> ;
extern template
struct Hamming_compute_sse42_2_32<64> ;
extern template
struct Hamming_compute_sse42_2_32<128> ;
extern template
struct Hamming_compute_sse42_2_32<256> ;
extern template
struct Hamming_compute_sse42_2_32<512> ;
extern template
struct Hamming_compute_sse42_2_32<1024> ;

extern template
struct Hamming_compute_sse42_3_16<1> ;
extern template
struct Hamming_compute_sse42_3_16<2> ;
extern template
struct Hamming_compute_sse42_3_16<4> ;
extern template
struct Hamming_compute_sse42_3_16<8> ;
extern template
struct Hamming_compute_sse42_3_16<16> ;
extern template
struct Hamming_compute_sse42_3_16<32> ;
extern template
struct Hamming_compute_sse42_3_16<64> ;
extern template
struct Hamming_compute_sse42_3_16<128> ;
extern template
struct Hamming_compute_sse42_3_16<256> ;
extern template
struct Hamming_compute_sse42_3_16<512> ;
extern template
struct Hamming_compute_sse42_3_16<1024> ;

extern template
struct Hamming_compute_sse42_3_32<1> ;
extern template
struct Hamming_compute_sse42_3_32<2> ;
extern template
struct Hamming_compute_sse42_3_32<4> ;
extern template
struct Hamming_compute_sse42_3_32<8> ;
extern template
struct Hamming_compute_sse42_3_32<16> ;
extern template
struct Hamming_compute_sse42_3_32<32> ;
extern template
struct Hamming_compute_sse42_3_32<64> ;
extern template
struct Hamming_compute_sse42_3_32<128> ;
extern template
struct Hamming_compute_sse42_3_32<256> ;
extern template
struct Hamming_compute_sse42_3_32<512> ;
extern template
struct Hamming_compute_sse42_3_32<1024> ;

#endif

#ifdef __AVX2__

extern template
struct Hamming_compute_avx2_1_16<1> ;
extern template
struct Hamming_compute_avx2_1_16<2> ;
extern template
struct Hamming_compute_avx2_1_16<4> ;
extern template
struct Hamming_compute_avx2_1_16<8> ;
extern template
struct Hamming_compute_avx2_1_16<16> ;
extern template
struct Hamming_compute_avx2_1_16<32> ;
extern template
struct Hamming_compute_avx2_1_16<64> ;
extern template
struct Hamming_compute_avx2_1_16<128> ;
extern template
struct Hamming_compute_avx2_1_16<256> ;
extern template
struct Hamming_compute_avx2_1_16<512> ;
extern template
struct Hamming_compute_avx2_1_16<1024> ;

extern template
struct Hamming_compute_avx2_1_32<1> ;
extern template
struct Hamming_compute_avx2_1_32<2> ;
extern template
struct Hamming_compute_avx2_1_32<4> ;
extern template
struct Hamming_compute_avx2_1_32<8> ;
extern template
struct Hamming_compute_avx2_1_32<16> ;
extern template
struct Hamming_compute_avx2_1_32<32> ;
extern template
struct Hamming_compute_avx2_1_32<64> ;
extern template
struct Hamming_compute_avx2_1_32<128> ;
extern template
struct Hamming_compute_avx2_1_32<256> ;
extern template
struct Hamming_compute_avx2_1_32<512> ;
extern template
struct Hamming_compute_avx2_1_32<1024> ;

extern template
struct Hamming_compute_avx2_2_16<1> ;
extern template
struct Hamming_compute_avx2_2_16<2> ;
extern template
struct Hamming_compute_avx2_2_16<4> ;
extern template
struct Hamming_compute_avx2_2_16<8> ;
extern template
struct Hamming_compute_avx2_2_16<16> ;
extern template
struct Hamming_compute_avx2_2_16<32> ;
extern template
struct Hamming_compute_avx2_2_16<64> ;
extern template
struct Hamming_compute_avx2_2_16<128> ;
extern template
struct Hamming_compute_avx2_2_16<256> ;
extern template
struct Hamming_compute_avx2_2_16<512> ;
extern template
struct Hamming_compute_avx2_2_16<1024> ;

extern template
struct Hamming_compute_avx2_2_32<1> ;
extern template
struct Hamming_compute_avx2_2_32<2> ;
extern template
struct Hamming_compute_avx2_2_32<4> ;
extern template
struct Hamming_compute_avx2_2_32<8> ;
extern template
struct Hamming_compute_avx2_2_32<16> ;
extern template
struct Hamming_compute_avx2_2_32<32> ;
extern template
struct Hamming_compute_avx2_2_32<64> ;
extern template
struct Hamming_compute_avx2_2_32<128> ;
extern template
struct Hamming_compute_avx2_2_32<256> ;
extern template
struct Hamming_compute_avx2_2_32<512> ;
extern template
struct Hamming_compute_avx2_2_32<1024> ;

extern template
struct Hamming_compute_avx2_3_16<1> ;
extern template
struct Hamming_compute_avx2_3_16<2> ;
extern template
struct Hamming_compute_avx2_3_16<4> ;
extern template
struct Hamming_compute_avx2_3_16<8> ;
extern template
struct Hamming_compute_avx2_3_16<16> ;
extern template
struct Hamming_compute_avx2_3_16<32> ;
extern template
struct Hamming_compute_avx2_3_16<64> ;
extern template
struct Hamming_compute_avx2_3_16<128> ;
extern template
struct Hamming_compute_avx2_3_16<256> ;
extern template
struct Hamming_compute_avx2_3_16<512> ;
extern template
struct Hamming_compute_avx2_3_16<1024> ;

extern template
struct Hamming_compute_avx2_3_32<1> ;
extern template
struct Hamming_compute_avx2_3_32<2> ;
extern template
struct Hamming_compute_avx2_3_32<4> ;
extern template
struct Hamming_compute_avx2_3_32<8> ;
extern template
struct Hamming_compute_avx2_3_32<16> ;
extern template
struct Hamming_compute_avx2_3_32<32> ;
extern template
struct Hamming_compute_avx2_3_32<64> ;
extern template
struct Hamming_compute_avx2_3_32<128> ;
extern template
struct Hamming_compute_avx2_3_32<256> ;
extern template
struct Hamming_compute_avx2_3_32<512> ;
extern template
struct Hamming_compute_avx2_3_32<1024> ;

#endif
