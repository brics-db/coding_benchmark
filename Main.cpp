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

#include <string>
#include <iostream>
#include <iomanip>
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

#define WarmUp(type, name) do { std::cout << "# WarmUp " << #type << std::endl; ExpandTest< type , 1, 1024>::WarmUp(#name , iterations, input, output); } while (0)

	WarmUp(CopyTest, "Copy");

#undef WarmUp

#define TestCase(type,name) do { std::cout << "# " << std::setw(2) << (vecTestInfos.size() + 1) <<  ": Testing " << #type << " (" << name << ")" << std::endl; vecTestInfos.emplace_back(); auto & vec = *vecTestInfos.rbegin(); vec.reserve(ComputeNumRuns<1, 1024>()()); ExpandTest< type , 1, 1024>::Execute(vec, name , iterations, input, output); } while (0)

	TestCase(CopyTest, "Copy");

	// 16-bit data tests
	// TestCase(XOR_seq_16_8, "XOR Seq 8");
	TestCase(XOR_seq_16_16, "XOR Seq");
	// TestCase(XOR_sse42_8x16_16, "XOR SSE4.2 16");
	TestCase(XOR_sse42_8x16_8x16, "XOR SSE4.2");
#ifdef __AVX2__
	// TestCase(XOR_avx2_16x16_16, "XOR AVX2 16");
	TestCase(XOR_avx2_16x16_16x16, "XOR AVX2");
#endif

	TestCase(AN_seq_16_32, "AN Seq");
	TestCase(AN_sse42_8x16_8x32, "AN SSE4.2");
#ifdef __AVX2__
	TestCase(AN_avx2_16x16_16x32, "AN AVX2");
#endif
	TestCase(Hamming_seq_16, "Hamming");
	
	// 32-bit data tests
	// TestCase(XOR_seq_32_8, "XOR Seq 8");
	TestCase(XOR_seq_32_32, "XOR Seq");
	// TestCase(XOR_sse42_4x32_32, "XOR SSE4.2 16");
	TestCase(XOR_sse42_4x32_4x32, "XOR SSE4.2");
#ifdef __AVX2__
	// TestCase(XOR_avx2_8x32_32, "XOR AVX2 16");
	TestCase(XOR_avx2_8x32_8x32, "XOR AVX2");
#endif

	TestCase(Hamming_seq_32, "Hamming");

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
