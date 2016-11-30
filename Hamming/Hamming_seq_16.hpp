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
/* 
 * File:   Hamming_seq_16.hpp
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 30. November 2016, 13:06
 */

#pragma once

#include "HammingTest.hpp"

template<size_t UNROLL>
struct Hamming_seq_16 : public HammingTest<uint16_t, UNROLL>, public SequentialTest
{

    Hamming_seq_16(const char* const name, AlignedBlock & in, AlignedBlock & out) :
    HammingTest<uint16_t, UNROLL>(name, in, out) { }

    virtual
    ~Hamming_seq_16() { }

};
