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
 * File:   SSE.cpp
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 07. August 2017, 06:11
 */

#ifdef __SSE4_2__

#include <SIMD/SSE.hpp>

namespace coding_benchmark {
    namespace simd {
        namespace sse {

            const uint64_t * const mm128<uint8_t>::SHUFFLE_TABLE_L = nullptr;
            const uint64_t * const mm128<uint8_t>::SHUFFLE_TABLE_H = nullptr;

            const __m128i * const mm128<uint16_t>::SHUFFLE_TABLE = nullptr;

            const __m128i * const mm128<uint32_t>::SHUFFLE_TABLE = nullptr;

            const __m128i * const mm128<uint64_t>::SHUFFLE_TABLE = nullptr;

        }
    }
}

#endif
