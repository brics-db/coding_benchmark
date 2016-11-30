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

#include "ANTest.h"

template<size_t UNROLL>
struct AN_seq_16_32 : public ANTest<uint16_t, uint32_t, UNROLL>, public SequentialTest
{
	AN_seq_16_32(const char* const name, AlignedBlock & in, AlignedBlock & out) :
		ANTest<uint16_t, uint32_t, UNROLL>(name, in, out, 63877ul, 3510769485ul)
	{}
	
	virtual ~AN_seq_16_32()
	{}

	void RunEnc(const size_t numIterations) override
	{
		for (size_t iteration = 0; iteration < numIterations; ++iteration)
		{
			auto dataIn = this->in.template begin<uint16_t>();
			auto dataInEnd = this->in.template end<uint16_t>();
			auto dataOut = this->out.template begin<uint32_t>();
			// compiler-unrolled loop
			while (dataIn <= (dataInEnd - UNROLL))
			{
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

	virtual bool DoCheck() override
	{
		return true;
	}

	virtual void RunCheck(const size_t numIterations)
	{
		for (size_t iteration = 0; iteration < numIterations; ++iteration)
		{
			const size_t numValues = this->in.template end<uint16_t>() - this->in.template begin<uint16_t>();
			size_t i = 0;
			auto data = this->out.template begin<uint32_t>();
			uint32_t maxUnenc = std::numeric_limits<uint16_t>::max();
			// compiler-unrolled loop
			while (i <= (numValues - UNROLL))
			{
				for (size_t k = 0; k < UNROLL; ++k)
				{
					if ((*data * this->A_INV) > maxUnenc)
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
					if ((*data * this->A_INV) > maxUnenc)
					{
						throw ErrorInfo(data - this->out.template begin<uint32_t>(), numIterations);
					}
					++data;
					++i;
				} while (i <= numValues);
			}
		}
	}
};
