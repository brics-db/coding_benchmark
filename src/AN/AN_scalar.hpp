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
 * File:   AN_scalar.hpp
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 07-07-2017 10:32
 */
#ifndef AN_AN_SCALAR_HPP_
#define AN_AN_SCALAR_HPP_

#include <AN/AN_scalar_s_inv.tcc>
#include <AN/AN_scalar_u_inv.tcc>
#include <AN/AN_scalar_divmod.tcc>

template<size_t UNROLL>
struct AN_seq_8_16_s_inv :
        public AN_seq_s_inv<int8_t, int16_t, UNROLL> {

    using AN_seq_s_inv<int8_t, int16_t, UNROLL>::AN_seq_s_inv;

    virtual ~AN_seq_8_16_s_inv() {
    }
};

template<size_t UNROLL>
struct AN_seq_8_16_u_inv :
        public AN_seq_u_inv<uint8_t, uint16_t, UNROLL> {

    using AN_seq_u_inv<uint8_t, uint16_t, UNROLL>::AN_seq_u_inv;

    virtual ~AN_seq_8_16_u_inv() {
    }
};

template<size_t UNROLL>
struct AN_seq_32_64_s_inv :
        public AN_seq_s_inv<int32_t, int64_t, UNROLL> {

    using AN_seq_s_inv<int32_t, int64_t, UNROLL>::AN_seq_s_inv;

    virtual ~AN_seq_32_64_s_inv() {
    }
};

template<size_t UNROLL>
struct AN_seq_32_64_u_inv :
        public AN_seq_u_inv<uint32_t, uint64_t, UNROLL> {

    using AN_seq_u_inv<uint32_t, uint64_t, UNROLL>::AN_seq_u_inv;

    virtual ~AN_seq_32_64_u_inv() {
    }
};

template<size_t UNROLL>
struct AN_seq_16_32_s_divmod :
        public AN_seq_divmod<int16_t, int32_t, UNROLL> {

    using AN_seq_divmod<int16_t, int32_t, UNROLL>::AN_seq_divmod;

    virtual ~AN_seq_16_32_s_divmod() {
    }
};

template<size_t UNROLL>
struct AN_seq_16_32_u_divmod :
        public AN_seq_divmod<uint16_t, uint32_t, UNROLL> {

    using AN_seq_divmod<uint16_t, uint32_t, UNROLL>::AN_seq_divmod;

    virtual ~AN_seq_16_32_u_divmod() {
    }
};

template<size_t UNROLL>
struct AN_seq_16_32_s_inv :
        public AN_seq_u_inv<int16_t, int32_t, UNROLL> {

    using AN_seq_u_inv<int16_t, int32_t, UNROLL>::AN_seq_u_inv;

    virtual ~AN_seq_16_32_s_inv() {
    }
};

template<size_t UNROLL>
struct AN_seq_16_32_u_inv :
        public AN_seq_u_inv<uint16_t, uint32_t, UNROLL> {

    using AN_seq_u_inv<uint16_t, uint32_t, UNROLL>::AN_seq_u_inv;

    virtual ~AN_seq_16_32_u_inv() {
    }
};

#endif /* AN_AN_SCALAR_HPP_ */
