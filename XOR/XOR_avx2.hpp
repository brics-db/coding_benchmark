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

#include "XOR_base.hpp"
#include "../Util/Intrinsics.hpp"

template<>
__m256i computeFinalChecksum<__m256i, __m256i>(__m256i & checksum)
{
	return checksum;
}

template<>
uint32_t computeFinalChecksum<__m256i, uint32_t>(__m256i & checksum)
{
	auto pChk = reinterpret_cast<uint32_t*> (&checksum);
	return pChk[0] ^ pChk[1] ^ pChk[2] ^ pChk[3] ^ pChk[4] ^ pChk[5] ^ pChk[6] ^ pChk[7];
}

template<>
uint16_t computeFinalChecksum<__m256i, uint16_t>(__m256i & checksum)
{
	auto pChk = reinterpret_cast<uint16_t*> (&checksum);
	return pChk[0] ^ pChk[1] ^ pChk[2] ^ pChk[3] ^ pChk[4] ^ pChk[5] ^ pChk[6] ^ pChk[7] ^ pChk[8] ^ pChk[9] ^ pChk[10] ^ pChk[11] ^ pChk[12] ^ pChk[13] ^ pChk[14] ^ pChk[15];
}

template<>
uint8_t computeFinalChecksum<__m256i, uint8_t>(__m256i & checksum)
{
	auto pChk = reinterpret_cast<uint16_t*> (&checksum);
	return pChk[0] ^ pChk[1] ^ pChk[2] ^ pChk[3] ^ pChk[4] ^ pChk[5] ^ pChk[6] ^ pChk[7] ^ pChk[8] ^ pChk[9] ^ pChk[10] ^ pChk[11] ^ pChk[12] ^ pChk[13] ^ pChk[14] ^ pChk[15] ^ pChk[16] ^ pChk[17] ^ pChk[18] ^ pChk[19] ^ pChk[20] ^ pChk[21] ^ pChk[22] ^ pChk[23] ^ pChk[24] ^ pChk[25] ^ pChk[26] ^ pChk[27] ^ pChk[28] ^ pChk[29] ^ pChk[30] ^ pChk[31];
}

template<>
bool checksumsDiffer<__m256i>(__m256i checksum1, __m256i checksum2)
{
	// check if any of the 16 bytes differ
	return 0xFFFFFFFF != _mm256_movemask_epi8(_mm256_cmpeq_epi8(checksum1, checksum2));
}

template<typename DATA, typename CS, size_t BLOCKSIZE>
struct XOR_avx2 : public Test<DATA, CS>
{
	XOR_avx2(const char* const name, AlignedBlock & in, AlignedBlock & out) :
		Test<DATA, CS>(name, in, out)
	{}

	virtual ~XOR_avx2()
	{}

	void RunEnc(const size_t numIterations) override
	{
		for (size_t iteration = 0; iteration < numIterations; ++iteration)
		{
			auto dataIn = this->in.template begin<__m256i>();
			auto dataInEnd = this->in.template end<__m256i>();
			auto dataOut = this->out.template begin<CS>();
			while (dataIn <= (dataInEnd - BLOCKSIZE))
			{
				__m256i checksum = _mm256_setzero_si256();
				auto dataOut2 = reinterpret_cast<__m256i*>(dataOut);
				for (size_t k = 0; k < BLOCKSIZE; ++k)
				{
					auto tmp = _mm256_lddqu_si256(dataIn++);
					_mm256_storeu_si256(dataOut2++, tmp);
					checksum = _mm256_xor_si256(checksum, tmp);
				}
				dataOut = reinterpret_cast<CS*>(dataOut2);
				*dataOut++ = computeFinalChecksum<__m256i, CS>(checksum);
			}
			// checksum remaining values which do not fit in the block size
			if (dataIn <= (dataInEnd - 1))
			{
				__m256i checksum = _mm256_setzero_si256();
				auto dataOut2 = reinterpret_cast<__m256i*>(dataOut);
				do
				{
					auto tmp = _mm256_lddqu_si256(dataIn++);
					_mm256_storeu_si256(dataOut2++, tmp);
					checksum = _mm256_xor_si256(checksum, tmp);
				} while (dataIn <= (dataInEnd - 1));
				dataOut = reinterpret_cast<CS*>(dataOut2);
				*dataOut++ = computeFinalChecksum<__m256i, CS>(checksum);
			}
			// checksum remaining integers which do not fit in the SIMD register
			if (dataIn < dataInEnd)
			{
				DATA checksum = 0;
				auto dataEnd = reinterpret_cast<DATA*> (dataInEnd);
				auto dataOut2 = reinterpret_cast<DATA*>(dataOut);
				for (auto data = reinterpret_cast<DATA*> (dataIn); data < dataEnd; ++data)
				{
					auto & tmp = *data;
					*dataOut2++ = tmp;
					checksum ^= tmp;
				}
				*dataOut2 = computeFinalChecksum<DATA, DATA>(checksum);
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
			auto data256 = this->out.template begin<__m256i>();
			auto data256End = this->out.template end<__m256i>();
			while (data256 <= (data256End - BLOCKSIZE))
			{
				__m256i checksum = _mm256_setzero_si256();
				for (size_t k = 0; k < BLOCKSIZE; ++k)
				{
					checksum = _mm256_xor_si256(checksum, _mm256_lddqu_si256(data256++));
				}
				auto dataOut = reinterpret_cast<CS*>(data256);
				if (checksumsDiffer<CS>(*dataOut, computeFinalChecksum<__m256i, CS>(checksum))) // third, test checksum
				{
					throw ErrorInfo(dataOut - this->out.template begin<CS>(), iteration); // this is not completely accurate, but not SO necessary for our �-Benchmark
				}
				++dataOut; // fourth, advance after the checksum to the next block of values
				data256 = reinterpret_cast<__m256i*>(dataOut);
			}
			// checksum remaining values which do not fit in the block size
			if (data256 <= (data256End - 1))
			{
				__m256i checksum = _mm256_setzero_si256();
				do
				{
					checksum = _mm256_xor_si256(checksum, _mm256_lddqu_si256(data256++));
				} while (data256 <= (data256End - 1));
				auto dataOut = reinterpret_cast<CS*>(data256);
				if (checksumsDiffer<CS>(*dataOut, computeFinalChecksum<__m256i, CS>(checksum))) // third, test checksum
				{
					throw ErrorInfo(dataOut - this->out.template begin<CS>(), iteration); // this is not completely accurate, but not SO necessary for our �-Benchmark
				}
				++dataOut; // fourth, advance after the checksum to the next block of values
				data256 = reinterpret_cast<__m256i*>(dataOut);
			}
			// checksum remaining integers which do not fit in the SIMD register, so we do it on the actual data width denoted by template parameter IN
			if (data256 < data256End)
			{
				DATA checksum = 0;
				auto dataEnd = reinterpret_cast<DATA*> (data256End);
				auto data = reinterpret_cast<DATA*>(data256);
				while (data < dataEnd)
				{
					checksum ^= *data++;
				}
				auto dataOut = reinterpret_cast<DATA*>(data);
				if (checksumsDiffer<DATA>(*dataOut, computeFinalChecksum<DATA, DATA>(checksum))) // third, test checksum
				{
					throw ErrorInfo(dataOut - this->out.template begin<DATA>(), iteration); // this is not completely accurate, but not SO necessary for our �-Benchmark
				}
			}
		}
	}
};
