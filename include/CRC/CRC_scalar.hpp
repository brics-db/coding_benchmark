// Copyright (c) 2018 Till Kolditz
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
 * File:   CRC_scalar.hpp
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 24-01-2018 10:11
 */

#pragma once

#define CRC_SCALAR

#include <CRC/CRC_scalar.tcc>

#undef CRC_SCALAR

namespace coding_benchmark {

    template<size_t BLOCKSIZE>
    struct CRC32_scalar_32 :
            public CRC_scalar<uint32_t, uint32_t, BLOCKSIZE>,
            public ScalarTest {

        using CRC_scalar<uint32_t, uint32_t, BLOCKSIZE>::CRC_scalar;

        virtual ~CRC32_scalar_32() {
        }
    };

    template<size_t BLOCKSIZE>
    struct CRC32_scalar_16 :
            public CRC_scalar<uint16_t, uint32_t, BLOCKSIZE>,
            public ScalarTest {

        using CRC_scalar<uint16_t, uint32_t, BLOCKSIZE>::CRC_scalar;

        virtual ~CRC32_scalar_16() {
        }
    };

    extern template
    struct CRC32_scalar_32<1> ;
    extern template
    struct CRC32_scalar_32<2> ;
    extern template
    struct CRC32_scalar_32<4> ;
    extern template
    struct CRC32_scalar_32<8> ;
    extern template
    struct CRC32_scalar_32<16> ;
    extern template
    struct CRC32_scalar_32<32> ;
    extern template
    struct CRC32_scalar_32<64> ;
    extern template
    struct CRC32_scalar_32<128> ;
    extern template
    struct CRC32_scalar_32<256> ;
    extern template
    struct CRC32_scalar_32<512> ;
    extern template
    struct CRC32_scalar_32<1024> ;

    extern template
    struct CRC32_scalar_16<1> ;
    extern template
    struct CRC32_scalar_16<2> ;
    extern template
    struct CRC32_scalar_16<4> ;
    extern template
    struct CRC32_scalar_16<8> ;
    extern template
    struct CRC32_scalar_16<16> ;
    extern template
    struct CRC32_scalar_16<32> ;
    extern template
    struct CRC32_scalar_16<64> ;
    extern template
    struct CRC32_scalar_16<128> ;
    extern template
    struct CRC32_scalar_16<256> ;
    extern template
    struct CRC32_scalar_16<512> ;
    extern template
    struct CRC32_scalar_16<1024> ;

}
