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

#include "XOR_sse42.hpp"

template<size_t BLOCKSIZE>
struct XOR_sse42_8x16_16 : public XOR_sse42<uint16_t, uint16_t, BLOCKSIZE>, public SSE42Test
{
	XOR_sse42_8x16_16(const char* const name, AlignedBlock & in, AlignedBlock & out) :
		XOR_sse42<uint16_t, uint16_t, BLOCKSIZE>(name, in, out)
	{}

	virtual ~XOR_sse42_8x16_16()
	{}
};
