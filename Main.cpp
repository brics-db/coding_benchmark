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

#include <string>
#include <iostream>
#include <vector>
#include <algorithm>

#ifdef _MSC_VER
// disable stupid diamond-inheritance warnings (the compiler does not see that there is only a single implementation for each of the functions)
#pragma warning(disable: 4250)
#endif

#include "Test.hpp"
#include "Util/AlignedBlock.h"
#include "Util/TestInfo.hpp"
#include "Util/ErrorInfo.hpp"

#include "CopyTest.hpp"

#include "XOR/XOR_seq_16_8.h"
#include "XOR/XOR_seq_16_16.h"
#include "XOR/XOR_seq_32_8.h"
#include "XOR/XOR_seq_32_32.h"
#include "XOR/XOR_sse42_8x16_16.h"
#include "XOR/XOR_sse42_8x16_8x16.h"
#include "XOR/XOR_sse42_4x32_32.h"
#include "XOR/XOR_sse42_4x32_4x32.h"
#ifdef __AVX2__
#include "XOR/XOR_avx2_16x16_16.h"
#include "XOR/XOR_avx2_16x16_16x16.h"
#include "XOR/XOR_avx2_8x32_32.h"
#include "XOR/XOR_avx2_8x32_8x32.h"
#endif

#include "AN/AN_seq_16_32.h"
#include "AN/AN_sse42_8x16_8x32.h"
#ifdef __AVX2__
#include "AN/AN_avx2_16x16_16x32.h"
#endif

#include "Hamming/Hamming_seq_16.hpp"
#include "Hamming/Hamming_seq_32.hpp"

template<size_t start, size_t end>
struct ComputeNumRuns
{
	constexpr size_t operator()() const
	{
		return 1 + ComputeNumRuns<start * 2, end>()();
	}
};

template<size_t start>
struct ComputeNumRuns<start, start>
{
	constexpr size_t operator()() const
	{
		return 1;
	}
};

template <template <size_t BlockSize> class TestCollection, size_t start, size_t end>
struct ExpandTest
{
	static void WarmUp(const char* const name, const size_t numIterations, AlignedBlock & in, AlignedBlock & out)
	{
		{
			TestCollection<start> test(name, in, out);
			test.Execute(numIterations);
		}
		ExpandTest<TestCollection, start * 2, end>::WarmUp(name, numIterations, in, out);
	}

	static void Execute(std::vector<TestInfos> & vecTestInfos, const char* const name, const size_t numIterations, AlignedBlock & in, AlignedBlock & out)
	{
		{
			// Execute:
			TestCollection<start> test(name, in, out);
			vecTestInfos.push_back(test.Execute(numIterations));
		}

		// Recurse:
		ExpandTest<TestCollection, start * 2, end>::Execute(vecTestInfos, name, numIterations, in, out);
	}
};

template <template <size_t BlockSize> class TestCollection, size_t start>
struct ExpandTest<TestCollection, start, start>
{
	static void WarmUp(const char* const name, const size_t numIterations, AlignedBlock & in, AlignedBlock & out)
	{
		TestCollection<start> test(name, in, out);
		test.Execute(numIterations);
	}

	static void Execute(std::vector<TestInfos> & vecTestInfos, const char* const name, const size_t numIterations, AlignedBlock & in, AlignedBlock & out)
	{
		// Execute:
		TestCollection<start> test(name, in, out);
		vecTestInfos.push_back(test.Execute(numIterations));
	}
};

void printResults(std::vector<std::vector<TestInfos>> & vector);

int main()
{
	const size_t rawDataSize = 1'024 * 1'024; // size in BYTES
	const size_t iterations = 1'024;

	AlignedBlock input(rawDataSize, 64);
	AlignedBlock output(2 * rawDataSize, 64); // AN coding generates twice as much output data as input data
	std::vector<std::vector<TestInfos>> vecTestInfos;

#define WarmUp(a) do { std::cout << "# WarmUp " << #a << std::endl; ExpandTest< a , 1, 1024>::WarmUp(#a , iterations, input, output); } while (0)

	WarmUp(CopyTest);

#undef WarmUp

#define TestCase(a) do { std::cout << "# Testing " << #a << std::endl; vecTestInfos.emplace_back(); auto & vec = *vecTestInfos.rbegin(); vec.reserve(ComputeNumRuns<1, 1024>()()); ExpandTest< a , 1, 1024>::Execute(vec, #a , iterations, input, output); } while (0)

	TestCase(CopyTest);

	TestCase(XOR_seq_16_8);
	TestCase(XOR_seq_16_16);
	TestCase(XOR_sse42_8x16_16);
	TestCase(XOR_sse42_8x16_8x16);
#ifdef __AVX2__
	TestCase(XOR_avx2_16x16_16);
	TestCase(XOR_avx2_16x16_16x16);
#endif

	TestCase(AN_seq_16_32);
	TestCase(AN_sse42_8x16_8x32);
#ifdef __AVX2__
	TestCase(AN_avx2_16x16_16x32);
#endif
	
	TestCase(XOR_seq_32_8);
	TestCase(XOR_seq_32_32);
	TestCase(XOR_sse42_4x32_32);
	TestCase(XOR_sse42_4x32_4x32);
#ifdef __AVX2__
	TestCase(XOR_avx2_8x32_32);
	TestCase(XOR_avx2_8x32_8x32);
#endif

	TestCase(Hamming_seq_16);
	TestCase(Hamming_seq_32);

#undef TestCase

	printResults(vecTestInfos);
}

void printResults(std::vector<std::vector<TestInfos>> & results)
{
	size_t maxPos = 0;
	for (auto & v : results) {
		maxPos = std::max(maxPos, v.size());
	}

	// The following does pretty-print everything so that it can be easily used as input for gnuplot & co.
	// print headline
	std::cout << "unroll/block";
	// first all encode columns, then all check columns etc.
	for (auto & v : results) {
		TestInfos &ti = v[0];
		if (ti.encode.isExecuted) {
			std::cout << ',' << ti.name << " enc";
		}
	}
	for (auto & v : results) {
		TestInfos &ti = v[0];
		if (ti.check.isExecuted) {
			std::cout << ',' << ti.name  << " check";
		}
	}
	for (auto & v : results) {
		TestInfos &ti = v[0];
		if (ti.arithmetic.isExecuted) {
			std::cout << ',' << ti.name  << " arith";
		}
	}
	for (auto & v : results) {
		TestInfos &ti = v[0];
		if (ti.decode.isExecuted) {
			std::cout << ',' << ti.name << " dec";
		}
	}
	std::cout << '\n';

	// print values, again first all encode columns, ...
	for (size_t pos = 0, blocksize = 1; pos < maxPos; ++pos, blocksize *= 2) {
		std::cout << blocksize;
		for (auto & v : results) {
			TestInfos &ti = v[0];
			if (ti.encode.isExecuted) {
				std::cout << ',';
				if (pos < v.size() && v[pos].encode.error == nullptr) {
					std::cout << v[pos].encode.nanos;
				}
			}
		}
		for (auto & v : results) {
			TestInfos &ti = v[0];
			if (ti.check.isExecuted) {
				std::cout << ',';
				if (pos < v.size() && v[pos].check.error == nullptr) {
					std::cout << v[pos].check.nanos;
				}
			}
		}
		for (auto & v : results) {
			TestInfos &ti = v[0];
			if (ti.arithmetic.isExecuted) {
				std::cout << ',';
				if (pos < v.size() && v[pos].arithmetic.error == nullptr) {
					std::cout << v[pos].arithmetic.nanos;
				}
			}
		}
		for (auto & v : results) {
			TestInfos &ti = v[0];
			if (ti.decode.isExecuted) {
				std::cout << ',';
				if (pos < v.size() && v[pos].decode.error == nullptr) {
					std::cout << v[pos].decode.nanos;
				}
			}
		}
		std::cout << std::endl;
	}
}
