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
 * File:   HammingTest.hpp
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 30. November 2016, 13:12
 */

#pragma once

#include <cstdint>

#include "../Test.hpp"
#include "../Util/Intrinsics.h"

struct hamming16_t
{
	uint16_t data;
	uint8_t code;
};

struct hamming32_t
{
	uint32_t data;
	uint8_t code;
};

template<typename OrigType>
struct TypeMap;

template<>
struct TypeMap<uint16_t>
{
	typedef hamming16_t hamming_t;

	static uint8_t
	computeHamming(uint16_t & data)
	{
		uint8_t hamming = 0;
		hamming |= (__builtin_popcount(data & 0xAD5B) & 0x1) << 1;
		hamming |= (__builtin_popcount(data & 0x366D) & 0x1) << 2;
		hamming |= (__builtin_popcount(data & 0xC78E) & 0x1) << 3;
		hamming |= (__builtin_popcount(data & 0x07F0) & 0x1) << 4;
		hamming |= (__builtin_popcount(data & 0xF800) & 0x1) << 5;
		hamming |= (__builtin_popcount(data & 0xFFFF) + __builtin_popcount(hamming)) & 0x1;
		return hamming;
	}

	static uint8_t
	computeHamming(uint16_t && data)
	{
		uint8_t hamming = 0;
		hamming |= (__builtin_popcount(data & 0xAD5B) & 0x1) << 1;
		hamming |= (__builtin_popcount(data & 0x366D) & 0x1) << 2;
		hamming |= (__builtin_popcount(data & 0xC78E) & 0x1) << 3;
		hamming |= (__builtin_popcount(data & 0x07F0) & 0x1) << 4;
		hamming |= (__builtin_popcount(data & 0xF800) & 0x1) << 5;
		hamming |= (__builtin_popcount(data & 0xFFFF) + __builtin_popcount(hamming)) & 0x1;
		return hamming;
	}
};

template<>
struct TypeMap<uint32_t>
{
	typedef hamming32_t hamming_t;

	static uint8_t
	computeHamming(uint32_t && data)
	{
		uint8_t hamming = 0;
		hamming |= (__builtin_popcount(data & 0x56AAAD5B) & 0x1) << 1;
		hamming |= (__builtin_popcount(data & 0x9B33366D) & 0x1) << 2;
		hamming |= (__builtin_popcount(data & 0xE3C3C78E) & 0x1) << 3;
		hamming |= (__builtin_popcount(data & 0x03FC07F0) & 0x1) << 4;
		hamming |= (__builtin_popcount(data & 0x03FFF800) & 0x1) << 5;
		hamming |= (__builtin_popcount(data & 0xFC000000) & 0x1) << 6;
		hamming |= (__builtin_popcount(data & 0xFFFFFFFF) + __builtin_popcount(hamming)) & 0x1;
		return hamming;
	}

	static uint8_t
	computeHamming(uint32_t & data)
	{
		uint8_t hamming = 0;
		hamming |= (__builtin_popcount(data & 0x56AAAD5B) & 0x1) << 1;
		hamming |= (__builtin_popcount(data & 0x9B33366D) & 0x1) << 2;
		hamming |= (__builtin_popcount(data & 0xE3C3C78E) & 0x1) << 3;
		hamming |= (__builtin_popcount(data & 0x03FC07F0) & 0x1) << 4;
		hamming |= (__builtin_popcount(data & 0x03FFF800) & 0x1) << 5;
		hamming |= (__builtin_popcount(data & 0xFC000000) & 0x1) << 6;
		hamming |= (__builtin_popcount(data & 0xFFFFFFFF) + __builtin_popcount(hamming)) & 0x1;
		return hamming;
	}
};

template<typename DATAIN, size_t UNROLL>
struct HammingTest : public Test<DATAIN, typename TypeMap<DATAIN>::hamming_t>
{
	typedef typename TypeMap<DATAIN>::hamming_t hamming_t;

	HammingTest(const char* const name, AlignedBlock & in, AlignedBlock & out) :
	Test<DATAIN, hamming_t>(name, in, out) { }

	virtual
	~HammingTest() { }

	void
	RunEnc(const size_t numIterations) override
	{
		for (size_t iteration = 0; iteration < numIterations; ++iteration)
		{
			auto data = this->in.template begin<DATAIN>();
			auto dataEnd = this->in.template begin<DATAIN>();
			auto dataOut = this->out.template begin<hamming_t>();
			while (data <= (dataEnd - UNROLL))
			{
				for (size_t k = 0; k < UNROLL; ++k, ++data, ++dataOut)
				{
					dataOut->data = *data;
					dataOut->code = TypeMap<DATAIN>::computeHamming(*data);
				}
			}
			for (; data < dataEnd; ++data, ++dataOut)
			{
				dataOut->data = *data;
				dataOut->code = TypeMap<DATAIN>::computeHamming(*data);
			}
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
			size_t numValues = this->in.template end<DATAIN>() - this->in.template begin<DATAIN>();
			size_t i = 0;
			auto data = this->out.template begin<hamming_t>();
			while (i <= (numValues - UNROLL))
			{
				for (size_t k = 0; k < UNROLL; ++k, ++i, ++data)
				{
					if (data->code != TypeMap<DATAIN>::computeHamming(data->data))
					{
						throw ErrorInfo(data - this->out.template begin<hamming_t>(), numIterations);
					}
				}
			}
		}
	}

};
