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
 * File:   CRC_base.hpp
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 24-01-2018 10:11
 */

#pragma once

#if not defined(CRC_BASE) and not defined(CRC_SCALAR)
#error "Clients must not include this file directly, but e.g. file <CRC32/CRC32_scalar.hpp>!"
#endif

#ifndef __SSE4_2__
#error "This CRC implementation requires SSE4.2! Feel free to improve on this."
#endif

#include <cstdint>

namespace coding_benchmark {

    template<typename DATA, typename CS>
    struct CRC;

    template<>
    struct CRC<uint8_t, uint32_t> {
        static uint32_t
        compute(
                uint32_t crc32,
                uint8_t value);
    };

    template<>
    struct CRC<uint16_t, uint32_t> {
        static uint32_t
        compute(
                uint32_t crc32,
                uint16_t value);
    };

    template<>
    struct CRC<uint32_t, uint32_t> {
        static uint32_t
        compute(
                uint32_t crc32,
                uint32_t value);
    };

    template<>
    struct CRC<uint64_t, uint32_t> {
        static uint32_t
        compute(
                uint32_t crc32,
                uint64_t value);
    };

}
