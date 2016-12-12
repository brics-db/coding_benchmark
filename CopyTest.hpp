// Copyright (c) 2016 Till Kolditz
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
 * File:   CopyTest.hpp
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 30. November 2016, 13:42
 */

#pragma once

#include <cstring>

#include "Test.hpp"
#include "Util/Intrinsics.hpp"

template<size_t UNROLL>
struct CopyTest : public Test<uint16_t, uint16_t>, public SequentialTest
{

	CopyTest(const char* const name, AlignedBlock & in, AlignedBlock & out) :
			Test<uint16_t, uint16_t>(name, in, out) { }

	virtual
	~CopyTest() { }

	void
	RunEnc(const size_t numIterations) override
	{
		for (size_t iteration = 0; iteration < numIterations; ++iteration)
		{
			_ReadWriteBarrier();
			memcpy(this->out.begin(), this->in.begin(), this->in.template end<uint8_t>() - this->in.template begin<uint8_t>());
		}
	}

	bool
	DoCheck() override
	{
		return true;
	}

	void
	RunCheck(const size_t numIterations) override
	{
		for (size_t iteration = 0; iteration < numIterations; ++iteration)
		{
			_ReadWriteBarrier();
			int ret = memcmp(this->out.begin(), this->in.begin(), this->in.template end<uint8_t>() - this->in.template begin<uint8_t>());
			if (ret != 0)
			{
				throw ErrorInfo(ret, numIterations);
			}
		}
	}

	bool
	DoDec() override
	{
		return true;
	}

	void
	RunDec(const size_t numIterations) override
	{
		for (size_t iteration = 0; iteration < numIterations; ++iteration)
		{
			_ReadWriteBarrier();
			memcpy(this->in.begin(), this->out.begin(), this->in.template end<uint8_t>() - this->in.template begin<uint8_t>());
		}
	}
};
