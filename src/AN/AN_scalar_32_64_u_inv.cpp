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
 * AN_scalar_32_64_u_inv.cpp
 *
 *  Created on: 07.01.2018
 *      Author: Till Kolditz - Till.Kolditz@gmail.com
 */

#include <AN/AN_scalar.hpp>

namespace coding_benchmark {

    template
    struct AN_scalar_32_64_u_inv<1> ;
    template
    struct AN_scalar_32_64_u_inv<2> ;
    template
    struct AN_scalar_32_64_u_inv<4> ;
    template
    struct AN_scalar_32_64_u_inv<8> ;
    template
    struct AN_scalar_32_64_u_inv<16> ;
    template
    struct AN_scalar_32_64_u_inv<32> ;
    template
    struct AN_scalar_32_64_u_inv<64> ;
    template
    struct AN_scalar_32_64_u_inv<128> ;
    template
    struct AN_scalar_32_64_u_inv<256> ;
    template
    struct AN_scalar_32_64_u_inv<512> ;
    template
    struct AN_scalar_32_64_u_inv<1024> ;

}
