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

#pragma once

#define XOR_SCALAR

#include <XOR/XOR_scalar.tcc>

#undef XOR_SCALAR

namespace coding_benchmark {

    template<size_t BLOCKSIZE>
    struct XOR_scalar_32_8 :
            public XOR_scalar<uint32_t, uint8_t, BLOCKSIZE>,
            public ScalarTest {

        using XOR_scalar<uint32_t, uint8_t, BLOCKSIZE>::XOR_scalar;

        virtual ~XOR_scalar_32_8() {
        }
    };

    template<size_t BLOCKSIZE>
    struct XOR_scalar_32_32 :
            public XOR_scalar<uint32_t, uint32_t, BLOCKSIZE>,
            public ScalarTest {

        using XOR_scalar<uint32_t, uint32_t, BLOCKSIZE>::XOR_scalar;

        virtual ~XOR_scalar_32_32() {
        }
    };

    template<size_t BLOCKSIZE>
    struct XOR_scalar_16_8 :
            public XOR_scalar<uint16_t, uint8_t, BLOCKSIZE>,
            public ScalarTest {

        using XOR_scalar<uint16_t, uint8_t, BLOCKSIZE>::XOR_scalar;

        virtual ~XOR_scalar_16_8() {
        }
    };

    template<size_t BLOCKSIZE>
    struct XOR_scalar_16_16 :
            public XOR_scalar<uint16_t, uint16_t, BLOCKSIZE>,
            public ScalarTest {

        using XOR_scalar<uint16_t, uint16_t, BLOCKSIZE>::XOR_scalar;

        virtual ~XOR_scalar_16_16() {
        }
    };

    extern template
    struct XOR_scalar_32_8<1> ;
    extern template
    struct XOR_scalar_32_8<2> ;
    extern template
    struct XOR_scalar_32_8<4> ;
    extern template
    struct XOR_scalar_32_8<8> ;
    extern template
    struct XOR_scalar_32_8<16> ;
    extern template
    struct XOR_scalar_32_8<32> ;
    extern template
    struct XOR_scalar_32_8<64> ;
    extern template
    struct XOR_scalar_32_8<128> ;
    extern template
    struct XOR_scalar_32_8<256> ;
    extern template
    struct XOR_scalar_32_8<512> ;
    extern template
    struct XOR_scalar_32_8<1024> ;

    extern template
    struct XOR_scalar_32_32<1> ;
    extern template
    struct XOR_scalar_32_32<2> ;
    extern template
    struct XOR_scalar_32_32<4> ;
    extern template
    struct XOR_scalar_32_32<8> ;
    extern template
    struct XOR_scalar_32_32<16> ;
    extern template
    struct XOR_scalar_32_32<32> ;
    extern template
    struct XOR_scalar_32_32<64> ;
    extern template
    struct XOR_scalar_32_32<128> ;
    extern template
    struct XOR_scalar_32_32<256> ;
    extern template
    struct XOR_scalar_32_32<512> ;
    extern template
    struct XOR_scalar_32_32<1024> ;

    extern template
    struct XOR_scalar_16_8<1> ;
    extern template
    struct XOR_scalar_16_8<2> ;
    extern template
    struct XOR_scalar_16_8<4> ;
    extern template
    struct XOR_scalar_16_8<8> ;
    extern template
    struct XOR_scalar_16_8<16> ;
    extern template
    struct XOR_scalar_16_8<32> ;
    extern template
    struct XOR_scalar_16_8<64> ;
    extern template
    struct XOR_scalar_16_8<128> ;
    extern template
    struct XOR_scalar_16_8<256> ;
    extern template
    struct XOR_scalar_16_8<512> ;
    extern template
    struct XOR_scalar_16_8<1024> ;

    extern template
    struct XOR_scalar_16_16<1> ;
    extern template
    struct XOR_scalar_16_16<2> ;
    extern template
    struct XOR_scalar_16_16<4> ;
    extern template
    struct XOR_scalar_16_16<8> ;
    extern template
    struct XOR_scalar_16_16<16> ;
    extern template
    struct XOR_scalar_16_16<32> ;
    extern template
    struct XOR_scalar_16_16<64> ;
    extern template
    struct XOR_scalar_16_16<128> ;
    extern template
    struct XOR_scalar_16_16<256> ;
    extern template
    struct XOR_scalar_16_16<512> ;
    extern template
    struct XOR_scalar_16_16<1024> ;

}
