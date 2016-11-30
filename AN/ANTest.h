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

#include "../Test.hpp"
#include "../Util/Intrinsics.h"

template<typename DATA, typename CS, size_t UNROLL>
class ANTest : public Test<DATA, CS>
{
protected:
	const uint32_t A;
	const uint32_t A_INV;

public:
	ANTest(const char* const name, AlignedBlock & in, AlignedBlock & out, const uint32_t A, const uint32_t A_INV) :
		Test<DATA, CS>(name, in, out),
		A(A),
		A_INV(A_INV)
	{}

	virtual ~ANTest()
	{}
};
