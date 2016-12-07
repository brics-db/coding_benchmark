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
 * File:   Hamming_sse42_16.hpp
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 07. December 2016, 22:10
 */

#pragma once

#include "Hamming_sse42.hpp"

template<size_t UNROLL>
struct Hamming_sse42_16 : public Hamming_sse42<uint16_t, UNROLL>
{

	Hamming_sse42_16(const char* const name, AlignedBlock & in, AlignedBlock & out) :
			Hamming_sse42<uint16_t, UNROLL>(name, in, out) { }

	virtual
	~Hamming_sse42_16() { }

};
