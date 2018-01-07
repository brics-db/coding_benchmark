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
 * Hamming_avx2_16.cpp
 *
 *  Created on: 07.01.2018
 *      Author: Till Kolditz - Till.Kolditz@gmail.com
 */

#ifdef __AVX2__

#include <Hamming/Hamming_simd.hpp>

namespace coding_benchmark {

    template
    struct Hamming_avx2_16<1>;
    template
    struct Hamming_avx2_16<2>;
    template
    struct Hamming_avx2_16<4>;
    template
    struct Hamming_avx2_16<8>;
    template
    struct Hamming_avx2_16<16>;
    template
    struct Hamming_avx2_16<32>;
    template
    struct Hamming_avx2_16<64>;
    template
    struct Hamming_avx2_16<128>;
    template
    struct Hamming_avx2_16<256>;
    template
    struct Hamming_avx2_16<512>;
    template
    struct Hamming_avx2_16<1024>;

}

#endif
