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
#include <sstream>

#include "Util/AlignedBlock.hpp"
#include "Util/Euclidean.hpp"
#include "Util/Stopwatch.hpp"

int
main (int argc, char ** argv) {
    if (argc != 6) {
        std::cerr << "Usage: " << argv[0] << " <size [Bytes]> <totalnum [#values]> <A16 <= 2^15> <A32 <= 2^31> <A64 <= 2^63>" << std::endl;
        return 1;
    }

    const uint16_t MASK16 = 0x7FFF;
    const uint32_t MASK32 = 0x7FFFFFFFul;
    const uint64_t MASK64 = 0x7FFFFFFFFFFFFFFFull;

    size_t SIZE = strtoll(argv[1], nullptr, 0);
    size_t TOTALNUM = strtoll(argv[2], nullptr, 0);
    uint16_t A16 = MASK16 & static_cast<uint16_t>(strtol(argv[3], nullptr, 0)); // test for code widths up to 15 bits
    uint32_t A32 = MASK32 & static_cast<uint32_t>(strtol(argv[4], nullptr, 0)); // test for code widths up to 31 bits
    uint64_t A64 = MASK64 & static_cast<uint64_t>(strtoll(argv[5], nullptr, 0)); // test for code widths up to 63 bits

    const size_t NUM16 = SIZE / sizeof (uint16_t);
    const size_t NUM32 = SIZE / sizeof (uint32_t);
    const size_t NUM64 = SIZE / sizeof (uint64_t);
    TOTALNUM += (NUM16 - (TOTALNUM & (NUM16 - 1)));
    AlignedBlock data(SIZE, SIZE);

#ifdef DEBUG
    std::cout << TOTALNUM << " numbers." << std::endl;
#endif

#ifdef DEBUG
    std::cout << "16-bit: " << (TOTALNUM / NUM16) << " iterations, each on " << NUM16 << " uint16_t." << std::endl;
#endif
    auto data16 = data.template begin<uint16_t>();
    Stopwatch sw;
    for (size_t k = 0; k < (TOTALNUM / NUM16); k++) {
        for (size_t i = 0; i < NUM16; ++i) {
            data16[0] = ext_euclidean(A16, 15);
        }
        std::stringstream ss;
        for (size_t i = 0; i < NUM16; ++i) {
            ss << data16[0];
        }
        std::cerr << ss.str();
    }
    auto nanoseconds = sw.Current();
#ifdef DEBUG
    std::cout << "Computing " << TOTALNUM << " inverses for 1..15-bit codewords took " << nanoseconds << " ns." << std::endl;
    std::cout << "\t" << (static_cast<double>(nanoseconds) / static_cast<double>(TOTALNUM)) << " ns / inverse." << std::endl;
    std::cout << "\t" << A16 << " * " << data16[0] << " = " << ((A16 * data16[0]) & MASK16) << std::endl;
#else
    std::cout << A16 << '\t' << nanoseconds << '\t' << (static_cast<double>(nanoseconds) / static_cast<double>(TOTALNUM));
#endif

#ifdef DEBUG
    std::cout << "32-bit: " << (TOTALNUM / NUM32) << " iterations, each on " << NUM32 << " uint32_t." << std::endl;
#endif
    auto data32 = data.template begin<uint32_t>();
    sw.Reset();
    for (size_t k = 0; k < (TOTALNUM / NUM32); k++) {
        for (size_t i = 0; i < NUM32; ++i) {
            data32[i] = ext_euclidean(A32, 31);
        }
    }
    nanoseconds = sw.Current();
#ifdef DEBUG
    std::cout << "Computing " << TOTALNUM << " inverses for 16..31-bit codewords took " << nanoseconds << " ns." << std::endl;
    std::cout << "\t " << (static_cast<double>(nanoseconds) / static_cast<double>(TOTALNUM)) << " ns / inverse." << std::endl;
    std::cout << "\t" << A32 << " * " << data32[0] << " = " << ((A32 * data32[0]) & MASK32) << std::endl;
#else
    std::cout << '\t' << A32 << '\t' << nanoseconds << '\t' << (static_cast<double>(nanoseconds) / static_cast<double>(TOTALNUM));
#endif

#ifdef DEBUG
    std::cout << "64-bit: " << (TOTALNUM / NUM64) << " iterations, each on " << NUM64 << " uint64_t." << std::endl;
#endif
    auto data64 = data.template begin<uint64_t>();
    sw.Reset();
    for (size_t k = 0; k < (TOTALNUM / NUM64); k++) {
        for (size_t i = 0; i < NUM64; ++i) {
            data64[i] = ext_euclidean(A64, 63);
        }
    }
    nanoseconds = sw.Current();
#ifdef DEBUG
    std::cout << "Computing " << TOTALNUM << " inverses for 32..63-bit codewords took " << nanoseconds << " ns." << std::endl;
    std::cout << "\t " << (static_cast<double>(nanoseconds) / static_cast<double>(TOTALNUM)) << " ns / inverse." << std::endl;
    std::cout << "\t" << A64 << " * " << data64[0] << " = " << ((A64 * data64[0]) & MASK64) << std::endl;
#else
    std::cout << '\t' << A64 << '\t' << nanoseconds << '\t' << (static_cast<double>(nanoseconds) / static_cast<double>(TOTALNUM)) << std::endl;
#endif
}
