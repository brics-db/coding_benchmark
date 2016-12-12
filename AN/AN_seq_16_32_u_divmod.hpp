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

#include "ANTest.hpp"

template<size_t UNROLL>
struct AN_seq_16_32_u_divmod : public ANTest<uint16_t, uint32_t, UNROLL>, public SequentialTest
{

	AN_seq_16_32_u_divmod(const char* const name, AlignedBlock & in, AlignedBlock & out, uint32_t A = 63'877ul, uint32_t Ainv = 3'510'769'485ul) :
			ANTest<uint16_t, uint32_t, UNROLL>(name, in, out, A, Ainv) { }

	virtual
	~AN_seq_16_32_u_divmod() { }

	void
	RunEnc(const size_t numIterations) override
	{
		for (size_t iteration = 0; iteration < numIterations; ++iteration)
		{
			auto dataIn = this->in.template begin<uint16_t>();
			auto dataInEnd = this->in.template end<uint16_t>();
			auto dataOut = this->out.template begin<uint32_t>();
			while (dataIn <= (dataInEnd - UNROLL))
			{
				// let the compiler unroll the loop
				for (size_t unroll = 0; unroll < UNROLL; ++unroll)
				{
					*dataOut++ = *dataIn++ * this->A;
				}
			}
			// remaining numbers
			while (dataIn < dataInEnd)
			{
				*dataOut++ = *dataIn++ * this->A;
			}
		}
	}

	virtual bool
	DoCheck() override
	{
		return true;
	}

	virtual void
	RunCheck(const size_t numIterations)
	{
		for (size_t iteration = 0; iteration < numIterations; ++iteration)
		{
			const size_t numValues = this->in.template end<uint16_t>() - this->in.template begin<uint16_t>();
			size_t i = 0;
			auto data = this->out.template begin<uint32_t>();
			while (i <= (numValues - UNROLL))
			{
				// let the compiler unroll the loop
				for (size_t k = 0; k < UNROLL; ++k)
				{
					if ((*data % this->A) != 0)
					{
						throw ErrorInfo(data - this->out.template begin<uint32_t>(), iteration);
					}
					++data;
				}
				i += UNROLL;
			}
			// remaining numbers
			if (i < numValues)
			{
				do
				{
					if ((*data % this->A) != 0)
					{
						throw ErrorInfo(data - this->out.template begin<uint32_t>(), numIterations);
					}
					++data;
					++i;
				}
				while (i <= numValues);
			}
		}
	}
};
