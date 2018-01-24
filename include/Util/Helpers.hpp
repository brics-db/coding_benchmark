// Copyright 2017 Till Kolditz
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
 * Helpers.hpp
 *
 *  Created on: 06.12.2017
 *      Author: Till Kolditz - Till.Kolditz@gmail.com
 */

#pragma once

#include <cstdint>

template<typename T>
struct Larger;

template<>
struct Larger<uint8_t> {
    typedef uint16_t larger_t;
};

template<>
struct Larger<int8_t> {
    typedef int16_t larger_t;
};

template<>
struct Larger<uint16_t> {
    typedef uint32_t larger_t;
};

template<>
struct Larger<int16_t> {
    typedef int32_t larger_t;
};

template<>
struct Larger<uint32_t> {
    typedef uint64_t larger_t;
};

template<>
struct Larger<int32_t> {
    typedef int64_t larger_t;
};

template<>
struct Larger<uint64_t> {
    typedef uint64_t larger_t; // TODO we have to assume for now that we do not get above 64 bits... :-(
};

template<>
struct Larger<int64_t> {
    typedef int64_t larger_t; // TODO we have to assume for now that we do not get above 64 bits... :-(
};

template<typename T>
struct Smaller;

template<>
struct Smaller<uint8_t> {
    typedef uint8_t smaller_t;
};

template<>
struct Smaller<int8_t> {
    typedef int16_t smaller_t;
};

template<>
struct Smaller<uint16_t> {
    typedef uint8_t smaller_t;
};

template<>
struct Smaller<int16_t> {
    typedef int8_t smaller_t;
};

template<>
struct Smaller<uint32_t> {
    typedef uint16_t smaller_t;
};

template<>
struct Smaller<int32_t> {
    typedef int16_t smaller_t;
};
