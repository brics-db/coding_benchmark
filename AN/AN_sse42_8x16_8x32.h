// Copyright (c) 2016 Till Kolditz, Stefan de Bruijn
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

#pragma once

#include <cstdint>
#include <limits>

#include "ANTest.h"

template<size_t UNROLL>
struct AN_sse42_8x16_8x32 : public ANTest<uint16_t, uint32_t, UNROLL>, public SSE42Test
{
	AN_sse42_8x16_8x32(const char* const name, AlignedBlock & in, AlignedBlock & out) :
		ANTest<uint16_t, uint32_t, UNROLL>(name, in, out, 63877ul, 3510769485ul)
	{}

	virtual ~AN_sse42_8x16_8x32()
	{}

	void RunEnc(const size_t numIterations) override
	{
		for (size_t iteration = 0; iteration < numIterations; ++iteration)
		{
			__m128i *dataIn = this->in.template begin<__m128i>();
			__m128i * const dataInEnd = this->in.template end<__m128i>();
			__m128i *dataOut = this->out.template begin<__m128i>();
			__m128i mm_A = _mm_set1_epi32(this->A);
			__m128i mmShuffle1 = _mm_set_epi32(0xFFFF0706, 0xFFFF0504, 0xFFFF0302, 0xFFFF0100);
			__m128i mmShuffle2 = _mm_set_epi32(0xFFFF0F0E, 0xFFFF0D0C, 0xFFFF0B0A, 0xFFFF0908);
			// compiler-unrolled loop
			while (dataIn <= (dataInEnd - UNROLL))
			{
				for (size_t unroll = 0; unroll < UNROLL; ++unroll)
				{
					auto mmIn = _mm_lddqu_si128(dataIn++);
					_mm_storeu_si128(dataOut++, _mm_mullo_epi32(_mm_shuffle_epi8(mmIn, mmShuffle1), mm_A));
					_mm_storeu_si128(dataOut++, _mm_mullo_epi32(_mm_shuffle_epi8(mmIn, mmShuffle2), mm_A));
				}
			}
			// remaining numbers
			while (dataIn <= (dataInEnd - 1))
			{
				auto mmIn = _mm_lddqu_si128(dataIn++);
				_mm_storeu_si128(dataOut++, _mm_mullo_epi32(_mm_shuffle_epi8(mmIn, mmShuffle1), mm_A));
				_mm_storeu_si128(dataOut++, _mm_mullo_epi32(_mm_shuffle_epi8(mmIn, mmShuffle2), mm_A));
			}
			if (dataIn < dataInEnd)
			{
				auto data16End = reinterpret_cast<uint16_t*> (dataInEnd);
				auto out32 = reinterpret_cast<uint32_t*> (dataOut);
				for (auto data16 = reinterpret_cast<uint16_t*> (dataIn); data16 < data16End; ++data16, ++out32)
					*out32 = *data16 * this->A;
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
			auto data = this->out.template begin<__m128i>();
			auto dataEnd = this->out.template end<__m128i>();
			uint32_t unencMax = std::numeric_limits<uint16_t>::max();
			__m128i mm_unencmax = _mm_set1_epi32(unencMax); // we assume 16-bit input data
			__m128i mm_ainv = _mm_set1_epi32(this->A_INV);
			while (data <= (dataEnd - UNROLL))
			{
				// compiler unrolling
				for (size_t k = 0; k < UNROLL; ++k)
				{
					auto mmIn = _mm_mullo_epi32(_mm_lddqu_si128(data), mm_ainv);
					if (0xFFFF != _mm_movemask_epi8(_mm_cmpeq_epi32(_mm_min_epu32(mmIn, mm_unencmax), mmIn)))
					{ // we need to do this "hack" because comparison is only on signed integers!
						throw ErrorInfo(reinterpret_cast<uint32_t*> (data) - this->out.template begin<uint32_t>(), iteration);
					}
					++data;
				}
			}
			// here follows the non-unrolled remainder
			while (data <= (dataEnd - 1))
			{
				auto mmIn = _mm_mullo_epi32(_mm_lddqu_si128(data), mm_ainv);
				if (0xFFFF != _mm_movemask_epi8(_mm_cmpeq_epi32(_mm_min_epu32(mmIn, mm_unencmax), mmIn)))
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
						throw ErrorInfo(data2 - this->out.template begin<uint32_t>(), iteration);
					}
				}
			}
		}
	}

	/*
	bool DoArith() override
	{
		return false;
	}

	void checkError(__m128i& mm_old, __m128i*& data, __m128i*& dataEnd, __m128i& mm_max, uint32_t*& out2, size_t& iter)
	{
		// _mm_cmpgt_epu32(a, b) ==> _mm_cmpeq_epi32(_mm_min_epu32(a, b), b)
		if (!_mm_movemask_epi8(_mm_cmpeq_epi32(_mm_min_epu32(mm_old, mm_max), mm_old)))
		{ // we need to do this "hack" because comparison is only on signed integers!
			std::cerr << std::hex << std::setfill('.') << "0x" << std::setw(8) << _mm_extract_epi32(*(data - 1), 0) << " 0x" << std::setw(8) << _mm_extract_epi32(*(data - 1), 1)
				<< " 0x" << std::setw(8) << _mm_extract_epi32(*(data - 1), 2) << " 0x" << std::setw(8) << _mm_extract_epi32(*(data - 1), 3) << " 0x"
				<< std::setw(8) << _mm_extract_epi32(mm_max, 3) << std::dec << std::setfill(' ') << std::endl;
			throw ErrorInfo((dataEnd - (data - 1)) * 4, iter);
		}
		for (size_t i = 0; i < (sizeof(__m128i) / sizeof(uint32_t)); ++i, ++out2)
		{
			if ((*out2 % this->A) != 0)
			{
				std::cerr << std::hex << std::setfill('.') << "0x" << std::setw(8) << *(out2 - 1) << " % " << "0x" << std::setw(8) << this->A << " = 0x" << std::setw(8)
					<< (*(out2 - 1) % this->A) << std::dec << std::setfill(' ') << std::endl;
				throw ErrorInfo((dataEnd - (data - 1)) * 4 + i, iter);
			}
		}
	}

	void add(__m128i*& data, __m128i*& dataEnd, __m128i*& mm_out, __m128i& mm_Val, __m128i& mm_valEnc, __m128i& mm_A, __m128i& mm_AINV, __m128i& mm_max, uint32_t*& out2, size_t& iter)
	{
		static __m128i mm_And = _mm_set1_epi32(0xFFFF);
		// we assume that there is already an out-value available!
		__m128i mm_old = _mm_lddqu_si128(data++);
		checkError(mm_old, data, dataEnd, mm_max, out2, iter);
		__m128i mm_new = _mm_add_epi32(mm_old, mm_valEnc);
		// int mask1 = _mm_movemask_epi8(_mm_cmplt_epi32(mm_new, mm_old));
		int mask1 = _mm_movemask_epi8(_mm_cmpeq_epi32(_mm_min_epu32(mm_new, mm_old), mm_new));
		// int mask2 = _mm_movemask_epi8(_mm_cmpgt_epi32(mm_new, mm_max));
		int mask2 = _mm_movemask_epi8(_mm_cmpeq_epi32(_mm_max_epu32(mm_new, mm_max), mm_new));
		// mask != 0 --> overflow --> since this is an arithmetic overflow in the original data space (16 bit here), we have to decode, force 16-bit addition, encode again
		__m128i x = (mask1 | mask2) ? _mm_mullo_epi32(_mm_add_epi16(_mm_and_si128(_mm_mullo_epi32(mm_old, mm_AINV), mm_And), mm_Val), mm_A) : mm_new;
		_mm_storeu_si128(mm_out++, x);
	}

	void RunArith(const size_t numIterations, uint16_t value) override
	{
		for (size_t iteration = 0; iteration < numIterations; ++iteration)
		{
			__m128i *data = this->out.begin<__m128i>();
			__m128i *dataEnd = this->out.end<__m128i>();
			__m128i mm_Val = _mm_set1_epi32(static_cast<uint32_t> (value));
			__m128i mm_valEnc = _mm_mullo_epi32(mm_Val, _mm_set1_epi32(this->A));
			__m128i mm_AINV = _mm_set1_epi32(this->A_INV);
			__m128i mm_A = _mm_set1_epi32(this->A);
			__m128i mm_max = _mm_set1_epi32(std::numeric_limits<uint16_t>::max() * this->A);
			auto out128 = data;
			while (data < dataEnd)
			{
				add(data, dataEnd, out128, mm_Val, mm_valEnc, mm_A, mm_AINV, mm_max, out2, iteration);
			}
			// TODO remaining values like in RunEnc
		}
	}
	*/
};
