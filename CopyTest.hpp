// Copyright (c) 2016 Till Kolditz
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

/* 
 * File:   CopyTest.hpp
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 30. November 2016, 13:42
 */

#pragma once

#include <cstring>

#include "Test.hpp"
#include "Util/Intrinsics.h"

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

};
