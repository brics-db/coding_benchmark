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
 * AN_avx2_16x16_16x32_u_divmod.cpp
 *
 *  Created on: 07.01.2018
 *      Author: Till Kolditz - Till.Kolditz@gmail.com
 */

#ifdef __AVX2__

#include <AN/AN_avx2.hpp>

namespace coding_benchmark {

    template
    struct AN_avx2_16x16_16x32_u_divmod<1>;
    template
    struct AN_avx2_16x16_16x32_u_divmod<2>;
    template
    struct AN_avx2_16x16_16x32_u_divmod<4>;
    template
    struct AN_avx2_16x16_16x32_u_divmod<8>;
    template
    struct AN_avx2_16x16_16x32_u_divmod<16>;
    template
    struct AN_avx2_16x16_16x32_u_divmod<32>;
    template
    struct AN_avx2_16x16_16x32_u_divmod<64>;
    template
    struct AN_avx2_16x16_16x32_u_divmod<128>;
    template
    struct AN_avx2_16x16_16x32_u_divmod<256>;
    template
    struct AN_avx2_16x16_16x32_u_divmod<512>;
    template
    struct AN_avx2_16x16_16x32_u_divmod<1024>;

}

#endif
