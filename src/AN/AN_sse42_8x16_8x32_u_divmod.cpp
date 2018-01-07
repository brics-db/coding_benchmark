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
 * AN_sse42_8x16_8x32_u_divmod.cpp
 *
 *  Created on: 07.01.2018
 *      Author: Till Kolditz - Till.Kolditz@gmail.com
 */

#include <AN/AN_sse42.hpp>

namespace coding_benchmark {

    extern template
    struct AN_sse42_8x16_8x32_u_divmod<1> ;
    extern template
    struct AN_sse42_8x16_8x32_u_divmod<2> ;
    extern template
    struct AN_sse42_8x16_8x32_u_divmod<4> ;
    extern template
    struct AN_sse42_8x16_8x32_u_divmod<8> ;
    extern template
    struct AN_sse42_8x16_8x32_u_divmod<16> ;
    extern template
    struct AN_sse42_8x16_8x32_u_divmod<32> ;
    extern template
    struct AN_sse42_8x16_8x32_u_divmod<64> ;
    extern template
    struct AN_sse42_8x16_8x32_u_divmod<128> ;
    extern template
    struct AN_sse42_8x16_8x32_u_divmod<256> ;
    extern template
    struct AN_sse42_8x16_8x32_u_divmod<512> ;
    extern template
    struct AN_sse42_8x16_8x32_u_divmod<1024> ;

}
