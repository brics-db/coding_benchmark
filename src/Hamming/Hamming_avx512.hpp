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
 * File:   Hamming_avx2.hpp
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 14. October 2017, 21:57
 */

#pragma once

#include <Hamming/Hamming_avx2.tcc>

namespace coding_benchmark {

    template<size_t UNROLL>
    struct Hamming_avx2_16 :
            public Hamming_avx2<uint16_t, UNROLL> {

        using Hamming_avx2<uint16_t, UNROLL>::Hamming_avx2;

        virtual ~Hamming_avx2_16() {
        }

    };

    template<size_t UNROLL>
    struct Hamming_avx2_32 :
            public Hamming_avx2<uint32_t, UNROLL> {

        using Hamming_avx2<uint32_t, UNROLL>::Hamming_avx2;

        virtual ~Hamming_avx2_32() {
        }

    };

    extern template
    struct Hamming_avx2_16<1> ;
    extern template
    struct Hamming_avx2_16<2> ;
    extern template
    struct Hamming_avx2_16<4> ;
    extern template
    struct Hamming_avx2_16<8> ;
    extern template
    struct Hamming_avx2_16<16> ;
    extern template
    struct Hamming_avx2_16<32> ;
    extern template
    struct Hamming_avx2_16<64> ;
    extern template
    struct Hamming_avx2_16<128> ;
    extern template
    struct Hamming_avx2_16<256> ;
    extern template
    struct Hamming_avx2_16<512> ;
    extern template
    struct Hamming_avx2_16<1024> ;

    extern template
    struct Hamming_avx2_32<1> ;
    extern template
    struct Hamming_avx2_32<2> ;
    extern template
    struct Hamming_avx2_32<4> ;
    extern template
    struct Hamming_avx2_32<8> ;
    extern template
    struct Hamming_avx2_32<16> ;
    extern template
    struct Hamming_avx2_32<32> ;
    extern template
    struct Hamming_avx2_32<64> ;
    extern template
    struct Hamming_avx2_32<128> ;
    extern template
    struct Hamming_avx2_32<256> ;
    extern template
    struct Hamming_avx2_32<512> ;
    extern template
    struct Hamming_avx2_32<1024> ;

}
