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
 * File:   CRC32_base.cpp
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 24-01-2018 10:14
 */

#define CRC_BASE

#include <CRC/CRC_base.hpp>

#undef CRC_BASE

#include <immintrin.h>

namespace coding_benchmark {

    uint32_t CRC<uint8_t, uint32_t>::compute(
            uint32_t crc32,
            uint8_t value) {
        return _mm_crc32_u8(crc32, value);
    }

    uint32_t CRC<uint16_t, uint32_t>::compute(
            uint32_t crc32,
            uint16_t value) {
        return _mm_crc32_u16(crc32, value);
    }

    uint32_t CRC<uint32_t, uint32_t>::compute(
            uint32_t crc32,
            uint32_t value) {
        return _mm_crc32_u32(crc32, value);
    }

    uint32_t CRC<uint64_t, uint32_t>::compute(
            uint32_t crc32,
            uint64_t value) {
        return _mm_crc32_u64(crc32, value);
    }

}
