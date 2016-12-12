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
#include "Util/AlignedBlock.hpp"
#include "Util/TestInfo.hpp"
#include "Util/ErrorInfo.hpp"
#include "Util/VFunc.hpp"
#include "Util/Euclidean.hpp"

#include "CopyTest.hpp"

#include "XOR/XOR_seq_16_8.hpp"
#include "XOR/XOR_seq_16_16.hpp"
#include "XOR/XOR_seq_32_8.hpp"
#include "XOR/XOR_seq_32_32.hpp"
#include "XOR/XOR_sse42_8x16_16.hpp"
#include "XOR/XOR_sse42_8x16_8x16.hpp"
#include "XOR/XOR_sse42_4x32_32.hpp"
#include "XOR/XOR_sse42_4x32_4x32.hpp"
#ifdef __AVX2__
#include "XOR/XOR_avx2_16x16_16.hpp"
#include "XOR/XOR_avx2_16x16_16x16.hpp"
#include "XOR/XOR_avx2_8x32_32.hpp"
#include "XOR/XOR_avx2_8x32_8x32.hpp"
#endif

#include "AN/AN_seq_16_32_u.hpp"
#include "AN/AN_seq_16_32_s.hpp"
#include "AN/AN_sse42_8x16_8x32.hpp"
#ifdef __AVX2__
#include "AN/AN_avx2_16x16_16x32.hpp"
#endif

#include "Hamming/Hamming_seq_16.hpp"
#include "Hamming/Hamming_seq_32.hpp"
#include "Hamming/Hamming_sse42_16.hpp"
#include "Hamming/Hamming_sse42_32.hpp"

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

	static void
	WarmUp(const char* const name, const size_t numIterations, AlignedBlock & in, AlignedBlock & out)
	{
		{
			TestCollection<start> test(name, in, out);
			test.Execute(numIterations);
		}
		ExpandTest<TestCollection, start * 2, end>::WarmUp(name, numIterations, in, out);
	}

	template<typename ... ArgTypes>
	static void
	Execute(std::vector<TestInfos> & vecTestInfos, const char* const name, const size_t numIterations, AlignedBlock & in, AlignedBlock & out, ArgTypes && ... args)
	{
		{
			// Execute:
			TestCollection<start> test(name, in, out, std::forward<ArgTypes>(args)...);
			vecTestInfos.push_back(test.Execute(numIterations));
		}

		// Recurse:
		ExpandTest<TestCollection, start * 2, end>::Execute(vecTestInfos, name, numIterations, in, out);
	}
};

template <template <size_t BlockSize> class TestCollection, size_t start>
struct ExpandTest<TestCollection, start, start>
{

	static void
	WarmUp(const char* const name, const size_t numIterations, AlignedBlock & in, AlignedBlock & out)
	{
		TestCollection<start> test(name, in, out);
		test.Execute(numIterations);
	}

	static void
	Execute(std::vector<TestInfos> & vecTestInfos, const char* const name, const size_t numIterations, AlignedBlock & in, AlignedBlock & out)
	{
		// Execute:
		TestCollection<start> test(name, in, out);
		vecTestInfos.push_back(test.Execute(numIterations));
	}
};

template<bool doRelative>
void printResults(std::vector<std::vector<TestInfos>> &vector);

void
printUsage(char* argv[])
{
	std::cerr << "Usage: " << argv[0] << " <A>\n" << std::setw(6) << "A" << ":   AN coding parameter (positive, odd, non-zero integer, 0 < A < 2^16)" << std::endl;
}

int
checkArgs(int argc, char* argv[], size_t & AUser1)
{
	if (argc == 1)
	{
		return 0;
	}
	if (argc != 2)
	{
		printUsage(argv);
		return 1;
	}
	char* endPtr = nullptr;
	AUser1 = strtoull(argv[1], &endPtr, 0);
	if (endPtr == argv[1] || AUser1 == 0 || (AUser1 & 1) == 0)
	{
		std::cerr << "Error: A is not a valid positive, non-zero, odd integer!\n";
		printUsage(argv);
		return 2;
	}
	if (AUser1 > (1ull << 16))
	{
		std::cerr << "Error: Given A is too large!\n";
		printUsage(argv);
		return 3;
	}
	return 0;
}

int
main(int argc, char* argv[])
{
	const size_t rawDataSize = 1'024 * 1'024; // size in BYTES
	const size_t iterations = 4'000;

	size_t AUser1 = 64'311;
	int result = checkArgs(argc, argv, AUser1);
	if (result != 0)
	{
		return result;
	}
	size_t AUser1Inv = ext_euclidean(AUser1, 32);

	AlignedBlock input(rawDataSize, 64);
	AlignedBlock output(2 * rawDataSize, 64); // AN coding generates twice as much output data as input data
	std::vector<std::vector < TestInfos>> vecTestInfos;

#define WarmUp(type, name) do { std::cout << "# WarmUp " << #type << std::endl; ExpandTest< type , 1, 1024>::WarmUp(#name , iterations, input, output); } while (0)

#define TestCase(...) VFUNC(TestCase, __VA_ARGS__)

#define TestCase2(type,name) do { std::cout << "# " << std::setw(2) << (vecTestInfos.size() + 1) <<  ": Testing " << #type << " (" << name << ")" << std::endl; vecTestInfos.emplace_back(); auto & vec = *vecTestInfos.rbegin(); vec.reserve(ComputeNumRuns<1, 1024>()()); ExpandTest< type , 1, 1024>::Execute(vec, name , iterations, input, output); } while (0)

#define TestCase4(type,name,A,Ainv) do { std::cout << "# " << std::setw(2) << (vecTestInfos.size() + 1) <<  ": Testing " << #type << " (" << name << " " << A << ")" << std::endl; vecTestInfos.emplace_back(); auto & vec = *vecTestInfos.rbegin(); vec.reserve(ComputeNumRuns<1, 1024>()()); ExpandTest< type , 1, 1024>::Execute(vec, name, iterations, input, output, A, Ainv); } while (0)

	WarmUp(CopyTest, "Copy");

	TestCase(CopyTest, "Copy");

	// 16-bit data sequential tests
	TestCase(XOR_seq_16_16, "XOR Seq");
	TestCase(AN_seq_16_32_u, "AN Seq U", 28'691, 1'441'585'691);
	TestCase(AN_seq_16_32_s, "AN Seq S", 28'691, 1'441'585'691);
	TestCase(AN_seq_16_32_u, "AN Seq U", AUser1, AUser1Inv);
	TestCase(AN_seq_16_32_s, "AN Seq S", AUser1, AUser1Inv);
	TestCase(Hamming_seq_16, "Hamming Seq");

	// 16-bit data vectorized tests
	TestCase(XOR_sse42_8x16_8x16, "XOR SSE4.2");
	TestCase(AN_sse42_8x16_8x32, "AN SSE4.2", 28'691, 1'441'585'691);
	TestCase(AN_sse42_8x16_8x32, "AN SSE4.2", AUser1, AUser1Inv);
	TestCase(Hamming_sse42_16, "Hamming SSE4.2");
#ifdef __AVX2__
	TestCase(XOR_avx2_16x16_16x16, "XOR AVX2");
	TestCase(AN_avx2_16x16_16x32, "AN AVX2", 28'691, 1'441'585'691);
	TestCase(AN_avx2_16x16_16x32, "AN AVX2", AUser1, AUser1Inv);
#endif

	// 32-bit data sequential tests
	TestCase(XOR_seq_32_32, "XOR Seq");
	TestCase(Hamming_seq_32, "Hamming");

	// 32-bit data vectorized tests
	TestCase(XOR_sse42_4x32_4x32, "XOR SSE4.2");
	TestCase(Hamming_sse42_32, "Hamming SSE4.2");
#ifdef __AVX2__
	TestCase(XOR_avx2_8x32_8x32, "XOR AVX2");
#endif

#undef WarmUp
#undef TestCase
#undef TestCase2
#undef TestCase4

	printResults<true>(vecTestInfos);
}

template<bool doRelative>
void
printResults(std::vector<std::vector<TestInfos>> &results)
{
	size_t maxPos = 0;
	for (auto & v : results)
	{
		maxPos = std::max(maxPos, v.size());
	}
	std::vector<double> baseEncode(maxPos);
	std::vector<double> baseCheck(maxPos);
	std::vector<double> baseArith(maxPos);
	std::vector<double> baseDecode(maxPos);

	for (size_t i = 0; i < maxPos; ++i)
	{
		auto & r = results[0][i]; // copy results
		baseEncode[i] = static_cast<double> (r.encode.nanos);
		baseCheck[i] = static_cast<double> (r.check.nanos);
		baseArith[i] = static_cast<double> (r.arithmetic.nanos);
		baseDecode[i] = static_cast<double> (r.decode.nanos);
	}

	// The following does pretty-print everything so that it can be easily used as input for gnuplot & co.
	// print headline
	std::cout << "unroll/block";
	// first all encode columns, then all check columns etc.
	for (auto & v : results)
	{
		TestInfos &ti = v[0];
		if (ti.encode.isExecuted)
		{
			std::cout << ',' << ti.name << " enc";
		}
	}
	for (auto & v : results)
	{
		TestInfos &ti = v[0];
		if (ti.check.isExecuted)
		{
			std::cout << ',' << ti.name << " check";
		}
	}
	for (auto & v : results)
	{
		TestInfos &ti = v[0];
		if (ti.arithmetic.isExecuted)
		{
			std::cout << ',' << ti.name << " arith";
		}
	}
	for (auto & v : results)
	{
		TestInfos &ti = v[0];
		if (ti.decode.isExecuted)
		{
			std::cout << ',' << ti.name << " dec";
		}
	}
	std::cout << '\n';

	// print values, again first all encode columns, ...
	for (size_t pos = 0, blocksize = 1; pos < maxPos; ++pos, blocksize *= 2)
	{
		std::cout << blocksize;
		for (auto & v : results)
		{
			TestInfos &ti = v[0];
			if (ti.encode.isExecuted)
			{
				std::cout << ',';
				if (pos < v.size() && v[pos].encode.error == nullptr)
				{
					if (doRelative)
						std::cout << (static_cast<double> (v[pos].encode.nanos) / baseEncode[pos]);
					else
						std::cout << v[pos].encode.nanos;
				}
			}
		}
		for (auto & v : results)
		{
			TestInfos &ti = v[0];
			if (ti.check.isExecuted)
			{
				std::cout << ',';
				if (pos < v.size() && v[pos].check.error == nullptr)
				{
					if (doRelative)
						std::cout << (static_cast<double> (v[pos].check.nanos) / baseCheck[pos]);
					else
						std::cout << v[pos].check.nanos;
				}
			}
		}
		for (auto & v : results)
		{
			TestInfos &ti = v[0];
			if (ti.arithmetic.isExecuted)
			{
				std::cout << ',';
				if (pos < v.size() && v[pos].arithmetic.error == nullptr)
				{
					if (doRelative)
						std::cout << (static_cast<double> (v[pos].arithmetic.nanos) / baseArith[pos]);
					else
						std::cout << v[pos].arithmetic.nanos;
				}
			}
		}
		for (auto & v : results)
		{
			TestInfos &ti = v[0];
			if (ti.decode.isExecuted)
			{
				std::cout << ',';
				if (pos < v.size() && v[pos].decode.error == nullptr)
				{
					if (doRelative)
						std::cout << (static_cast<double> (v[pos].decode.nanos) / baseDecode[pos]);
					else
						std::cout << v[pos].decode.nanos;
				}
			}
		}
		std::cout << std::endl;
	}
}
