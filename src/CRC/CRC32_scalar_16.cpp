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
 * File:   CRC32_scalar_16.cpp
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 24-01-2018 10:14
 */

#include <CRC/CRC_scalar.hpp>

namespace coding_benchmark {

    template
    struct CRC32_scalar_16<1> ;
    template
    struct CRC32_scalar_16<2> ;
    template
    struct CRC32_scalar_16<4> ;
    template
    struct CRC32_scalar_16<8> ;
    template
    struct CRC32_scalar_16<16> ;
    template
    struct CRC32_scalar_16<32> ;
    template
    struct CRC32_scalar_16<64> ;
    template
    struct CRC32_scalar_16<128> ;
    template
    struct CRC32_scalar_16<256> ;
    template
    struct CRC32_scalar_16<512> ;
    template
    struct CRC32_scalar_16<1024> ;

}
