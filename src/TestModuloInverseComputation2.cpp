// Copyright (c) 2016 Till Kolditz
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
 * File:   TestModuloInverseComputation.cpp
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 20. Februar 2017, 18:21
 */

#include <cstdlib>
#include <iostream>
#include <iomanip>

#include <boost/multiprecision/cpp_int.hpp>

#include "Util/Euclidean.hpp"
#include "Util/Stopwatch.hpp"

using boost::multiprecision::uint128_t;

const size_t wCmin = 16;
const size_t wCmax = 127;
const size_t wAmin = 3;
//const size_t wAmax = 16;

uint64_t convert(uint128_t & source) {
    uint64_t target = 0;
    const unsigned limb_num = source.backend().size(); // number of limbs
    const unsigned limb_bits = sizeof(boost::multiprecision::limb_type) * CHAR_BIT; // size of limb in bits
    for (unsigned i = 0; i < limb_num && ((i * limb_bits) < (sizeof(target) * 8)); ++i) {
        target |= (source.backend().limbs()[i]) << (i * limb_bits);
    }
    return target;
}

int main(int argc, char ** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <totalnum [#iterations]>" << std::endl;
        return 1;
    }

    size_t TOTALNUM = strtoll(argv[1], nullptr, 0);
    uint128_t result;
    // volatile uint64_t result64;

    std::cout << TOTALNUM << " iterations per combination of |A| and |C|." << std::endl;
    std::cout << "|C|";
    for (size_t wA = wAmin; wA <= wCmax; ++wA) {
        std::cout << '\t' << wA;
    }
    std::cout << std::endl;

    for (size_t wC = wCmin; wC <= wCmax; ++wC) {
        std::cout << wC;
        for (size_t wA = wAmin; wA <= (wC - (wCmin - (wAmin - 1))); ++wA) {
            std::cout << "\t-";
        }
        for (size_t wA = (wC - (wCmin - (wAmin - 1))); wA < wC; ++wA) {
            uint128_t A(1);
            A <<= (wA - 1);
            A += 1;
            Stopwatch sw;
            for (size_t i = 0; i < TOTALNUM; ++i) {
                result = ext_euclidean(A, wC);
                // result64 = convert(result);
            }
            auto nanoseconds = sw.Current();
            std::cout << '\t' << std::hex << std::showbase << A << ':' << std::dec << std::noshowbase << nanoseconds;
        }
        std::cout << std::endl;
    }
}
