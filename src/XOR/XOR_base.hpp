// Copyright 2016 Till Kolditz, Stefan de Bruijn
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

#pragma once

#include <cstdint>

template<typename DATA, typename CS>
struct XOR;

template<>
struct XOR<uint16_t, uint8_t> {
    static uint8_t
    computeFinalChecksum(
            uint16_t & checksum);
};

template<>
struct XOR<uint16_t, uint16_t> {
    static uint16_t
    computeFinalChecksum(
            uint16_t & checksum);
};

template<>
struct XOR<uint32_t, uint8_t> {
    static uint8_t
    computeFinalChecksum(
            uint32_t & checksum);
};

template<>
struct XOR<uint32_t, uint32_t> {
    static uint32_t
    computeFinalChecksum(
            uint32_t & checksum);
};

template<typename CS>
struct XORdiff;

template<>
struct XORdiff<uint8_t> {
    static bool
    checksumsDiffer(
            uint8_t cs1,
            uint8_t cs2);
};

template<>
struct XORdiff<uint16_t> {
    static bool
    checksumsDiffer(
            uint16_t cs1,
            uint16_t cs2);
};

template<>
struct XORdiff<uint32_t> {
    static bool
    checksumsDiffer(
            uint32_t cs1,
            uint32_t cs2);
};

template<>
struct XORdiff<uint64_t> {
    static bool
    checksumsDiffer(
            uint64_t cs1,
            uint64_t cs2);
};
