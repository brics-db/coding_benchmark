// Copyright (c) 2016 Till Kolditz, Stefan de Bruijn
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
struct AN_avx2_16x16_16x32 : public ANTest<uint16_t, uint32_t, UNROLL>, public AVX2Test
{
	AN_avx2_16x16_16x32(const char* const name, AlignedBlock & in, AlignedBlock & out, uint32_t A = 63'877ul, uint32_t Ainv = 3'510'769'485ul) :
		ANTest<uint16_t, uint32_t, UNROLL>(name, in, out, A, Ainv)
	{}

	virtual ~AN_avx2_16x16_16x32()
	{}

	void RunEnc(const size_t numIterations) override
	{
		for (size_t iter = 0; iter < numIterations; ++iter)
		{
			__m256i *dataIn = this->in.template begin<__m256i>();
			__m256i *dataInEnd = this->in.template end<__m256i>();
			__m128i *dataOut = this->out.template begin<__m128i>(); // since AVX2 does not shuffle as desired, we need to store 128-bit vectors only
			__m256i mm_A = _mm256_set1_epi32(this->A);

			// _mm256_shuffle_epi8 works only on 128-bit lanes, so we have to work on the first 4 16-bit values and the third ones, then on the second and fourth ones
			__m256i mmShuffle1 = _mm256_set_epi32(0xFFFF0706, 0xFFFF0504, 0xFFFF0302, 0xFFFF0100, 0xFFFF0706, 0xFFFF0504, 0xFFFF0302, 0xFFFF0100);
			__m256i mmShuffle2 = _mm256_set_epi32(0xFFFF0F0E, 0xFFFF0D0C, 0xFFFF0B0A, 0xFFFF0908, 0xFFFF0F0E, 0xFFFF0D0C, 0xFFFF0B0A, 0xFFFF0908);
			while (dataIn <= (dataInEnd - UNROLL))
			{
				// let the compiler unroll the loop
				for (size_t unroll = 0; unroll < UNROLL; ++unroll)
				{
					__m256i m256 = _mm256_lddqu_si256(dataIn++);
					__m256i tmp1 = _mm256_mullo_epi32(_mm256_shuffle_epi8(m256, mmShuffle1), mm_A);
					__m256i tmp2 = _mm256_mullo_epi32(_mm256_shuffle_epi8(m256, mmShuffle2), mm_A);
					_mm_storeu_si128(dataOut++, _mm256_extractf128_si256(tmp1, 0));
					_mm_storeu_si128(dataOut++, _mm256_extractf128_si256(tmp2, 0));
					_mm_storeu_si128(dataOut++, _mm256_extractf128_si256(tmp1, 1));
					_mm_storeu_si128(dataOut++, _mm256_extractf128_si256(tmp2, 1));
				}
			}
			
			while (dataIn <= (dataEnd - 1))
			{
					__m256i m256 = _mm256_lddqu_si256(dataIn++);
					__m256i tmp1 = _mm256_mullo_epi32(_mm256_shuffle_epi8(m256, mmShuffle1), mm_A);
					__m256i tmp2 = _mm256_mullo_epi32(_mm256_shuffle_epi8(m256, mmShuffle2), mm_A);
					_mm_storeu_si128(dataOut++, _mm256_extractf128_si256(tmp1, 0));
					_mm_storeu_si128(dataOut++, _mm256_extractf128_si256(tmp2, 0));
					_mm_storeu_si128(dataOut++, _mm256_extractf128_si256(tmp1, 1));
					_mm_storeu_si128(dataOut++, _mm256_extractf128_si256(tmp2, 1));
			}

			// multiply remaining numbers sequentially
			if (dataIn < dataInEnd)
			{
				auto data16 = reinterpret_cast<uint16_t*> (dataIn);
				auto data16End = reinterpret_cast<uint16_t*> (dataInEnd);
				auto out32 = reinterpret_cast<uint32_t*> (dataOut);
				do
				{
					*out32++ = static_cast<uint32_t>(*data16++) * this->A;
				} while (data16 < data16End);
			}
		}
	}

	virtual bool DoCheck() override
	{
		return true;
	}

	virtual void RunCheck(const size_t numIterations) override
	{
		for (size_t iteration = 0; iteration < numIterations; ++iteration)
		{
			auto data = this->out.template begin<__m256i>();
			auto dataEnd = this->out.template end<__m256i>();
			uint32_t unencMax = std::numeric_limits<uint16_t>::max();
			__m256i mm_unencmax = _mm256_set1_epi32(unencMax); // we assume 16-bit input data
			__m256i mm_ainv = _mm256_set1_epi32(this->A_INV);
			while (data <= (dataEnd - UNROLL))
			{
				// let the compiler unroll the loop
				for (size_t k = 0; k < UNROLL; ++k)
				{
					auto mmIn = _mm256_mullo_epi32(_mm256_lddqu_si256(data), mm_ainv);
					if (_mm256_movemask_epi8(_mm256_cmpeq_epi32(_mm256_min_epu32(mmIn, mm_unencmax), mmIn)))
					{ // we need to do this "hack" because comparison is only on signed integers!
						throw ErrorInfo(reinterpret_cast<uint32_t*> (data) - this->out.template begin<uint32_t>(), iteration);
					}
					++data;
				}
			}
			// here follows the non-unrolled remainder
			while (data <= (dataEnd - 1))
			{
				auto mmIn = _mm256_mullo_epi32(_mm256_lddqu_si256(data), mm_ainv);
				if (!_mm256_movemask_epi8(_mm256_cmpeq_epi32(_mm256_min_epu32(mmIn, mm_unencmax), mmIn)))
				{ // we need to do this "hack" because comparison is only on signed integers!
					throw ErrorInfo(reinterpret_cast<uint32_t*> (data) - this->out.template begin<uint32_t>(), iteration);
				}
				++data;
			}
			if (data < dataEnd)
			{
				auto dataEnd2 = reinterpret_cast<uint32_t*>(dataEnd);
				for (auto data2 = reinterpret_cast<uint32_t*> (data); data2 < dataEnd2; ++data2)
				{
					if ((*data2 * this->A_INV) > unencMax)
					{
						throw ErrorInfo(reinterpret_cast<uint32_t*> (data2) - this->out.template begin<uint32_t>(), iteration);
					}
				}
			}
		}
	}
};
