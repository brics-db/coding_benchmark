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
 * File:   Hamming_compute_sse42_3_32.cpp
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 23-03-2018 13:06
 */

#include <Hamming/Hamming_compute.hpp>

namespace coding_benchmark {

#ifdef __SSE4_2__

    template
    struct Hamming_compute_sse42_3_32<1> ;
    template
    struct Hamming_compute_sse42_3_32<2> ;
    template
    struct Hamming_compute_sse42_3_32<4> ;
    template
    struct Hamming_compute_sse42_3_32<8> ;
    template
    struct Hamming_compute_sse42_3_32<16> ;
    template
    struct Hamming_compute_sse42_3_32<32> ;
    template
    struct Hamming_compute_sse42_3_32<64> ;
    template
    struct Hamming_compute_sse42_3_32<128> ;
    template
    struct Hamming_compute_sse42_3_32<256> ;
    template
    struct Hamming_compute_sse42_3_32<512> ;
    template
    struct Hamming_compute_sse42_3_32<1024> ;

#endif

}
