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
 * StringsAN.hpp
 *
 *  Created on: 25.03.2018
 *      Author: Till Kolditz - Till.Kolditz@gmail.com
 */


#pragma once

#include <Strings/StringBase.hpp>

int strcmp2_AN(
        const unsigned short *s1,
        const unsigned short *s2,
        const unsigned short A,
        const size_t NUM);

int strcmp2_AN_accu(
        const unsigned short *s1,
        const unsigned short *s2,
        const unsigned short A,
        const size_t NUM);

int strcmp2_AN(
        const unsigned int *s1,
        const unsigned int *s2,
        const unsigned short A,
        const size_t NUM);

int strcmp2_AN_accu(
        const unsigned int *s1,
        const unsigned int *s2,
        const unsigned short A,
        const size_t NUM);

int _mm_strcmp_AN(
        const unsigned short* s1,
        const unsigned short* s2,
        const unsigned short A,
        const size_t NUM);

int _mm_strcmp_AN_accu(
        const unsigned short* s1,
        const unsigned short* s2,
        const unsigned short A,
        const size_t NUM);

int _mm_strcmp_AN(
        const unsigned int* i1,
        const unsigned int* i2,
        const unsigned short A,
        const size_t NUM);

int _mm_strcmp_AN_accu(
        const unsigned int* i1,
        const unsigned int* i2,
        const unsigned short A,
        const size_t NUM);
