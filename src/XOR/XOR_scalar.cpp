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
 * File:   XOR_scalar.cpp
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 15-08-2017 17:04
 */

#include <XOR/XOR_scalar.hpp>

namespace coding_benchmark {

    template
    struct XOR_scalar_32_8<1> ;
    template
    struct XOR_scalar_32_8<2> ;
    template
    struct XOR_scalar_32_8<4> ;
    template
    struct XOR_scalar_32_8<8> ;
    template
    struct XOR_scalar_32_8<16> ;
    template
    struct XOR_scalar_32_8<32> ;
    template
    struct XOR_scalar_32_8<64> ;
    template
    struct XOR_scalar_32_8<128> ;
    template
    struct XOR_scalar_32_8<256> ;
    template
    struct XOR_scalar_32_8<512> ;
    template
    struct XOR_scalar_32_8<1024> ;

    template
    struct XOR_scalar_32_32<1> ;
    template
    struct XOR_scalar_32_32<2> ;
    template
    struct XOR_scalar_32_32<4> ;
    template
    struct XOR_scalar_32_32<8> ;
    template
    struct XOR_scalar_32_32<16> ;
    template
    struct XOR_scalar_32_32<32> ;
    template
    struct XOR_scalar_32_32<64> ;
    template
    struct XOR_scalar_32_32<128> ;
    template
    struct XOR_scalar_32_32<256> ;
    template
    struct XOR_scalar_32_32<512> ;
    template
    struct XOR_scalar_32_32<1024> ;

    template
    struct XOR_scalar_16_8<1> ;
    template
    struct XOR_scalar_16_8<2> ;
    template
    struct XOR_scalar_16_8<4> ;
    template
    struct XOR_scalar_16_8<8> ;
    template
    struct XOR_scalar_16_8<16> ;
    template
    struct XOR_scalar_16_8<32> ;
    template
    struct XOR_scalar_16_8<64> ;
    template
    struct XOR_scalar_16_8<128> ;
    template
    struct XOR_scalar_16_8<256> ;
    template
    struct XOR_scalar_16_8<512> ;
    template
    struct XOR_scalar_16_8<1024> ;

    template
    struct XOR_scalar_16_16<1> ;
    template
    struct XOR_scalar_16_16<2> ;
    template
    struct XOR_scalar_16_16<4> ;
    template
    struct XOR_scalar_16_16<8> ;
    template
    struct XOR_scalar_16_16<16> ;
    template
    struct XOR_scalar_16_16<32> ;
    template
    struct XOR_scalar_16_16<64> ;
    template
    struct XOR_scalar_16_16<128> ;
    template
    struct XOR_scalar_16_16<256> ;
    template
    struct XOR_scalar_16_16<512> ;
    template
    struct XOR_scalar_16_16<1024> ;

}
