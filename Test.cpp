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

#include <cstring>
#include <cstdint>
#include <iostream>

#include "Test.hpp"
#include "Util/CPU.h"
#include "Util/Stopwatch.hpp"

TestBase::TestBase(const char* const name, AlignedBlock & in, AlignedBlock & out) :
	name(name),
	in(in),
	out(out)
{}

TestBase::~TestBase()
{}

void TestBase::PreEnc(const size_t numIterations)
{}

bool TestBase::DoCheck()
{
	return false;
}

void TestBase::PreCheck(const size_t numIterations)
{}

void TestBase::RunCheck(const size_t numIterations)
{}

bool TestBase::DoArith()
{
	return false;
}

void TestBase::PreArith(const size_t numIterations)
{}

void TestBase::RunArith(const size_t numIterations, uint16_t value)
{}

bool TestBase::DoDec()
{
	return false;
}

void TestBase::PreDec(const size_t numIterations)
{}

void TestBase::RunDec(const size_t numIterations)
{}

TestInfos TestBase::Execute(const size_t numIterations)
{
	if (!this->HasCapabilities()) {
		return TestInfos(this->name, getSIMDtypeName());
	}

	ResetBuffers();

	int64_t nanos;
	Stopwatch sw;
	
	// Start test:
	this->PreEnc(numIterations);

	sw.Reset();
	this->RunEnc(numIterations);
	nanos = sw.Current();
	TestInfo tiEnc(nanos), tiCheck, tiArith, tiDec;

	if (this->DoCheck()) {
		this->PreCheck(numIterations);

		sw.Reset();
		this->RunCheck(numIterations);
		nanos = sw.Current();
		tiCheck.set(nanos);
	}

	if (this->DoArith())
	{
		this->PreArith(numIterations);
		sw.Reset();
		this->RunArith(numIterations, 1351);
		nanos = sw.Current();
		tiArith.set(nanos);
	}

	if (this->DoDec())
	{
		this->PreDec(numIterations);
		sw.Reset();
		this->RunDec(numIterations);
		nanos = sw.Current();
		tiDec.set(nanos);
	}

	return TestInfos(this->name, getSIMDtypeName(), tiEnc, tiCheck, tiArith, tiDec);
}

const char* SequentialTest::getSIMDtypeName()
{
	static const char* SIMDtypeName = "Seq";
	return SIMDtypeName;
}

bool SequentialTest::HasCapabilities()
{
	return true;
}

const char* SSE42Test::getSIMDtypeName() 
{
	static const char* SIMDtypeName = "SSE4.2";
	return SIMDtypeName;
}

bool SSE42Test::HasCapabilities()
{
	auto& cpu = CPU::Instance();
	return cpu.SSE42 && cpu.OS_X64;
}

const char* AVX2Test::getSIMDtypeName() 
{
	static const char* SIMDtypeName = "AVX2";
	return SIMDtypeName;
}

bool AVX2Test::HasCapabilities()
{
	auto& cpu = CPU::Instance();
	return cpu.AVX2 && cpu.OS_X64;
}
