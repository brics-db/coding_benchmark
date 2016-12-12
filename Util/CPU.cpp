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

#include "CPU.hpp"

#include <iostream>
#include <cstring>

// ----------------
// CPUID / OS code:
// ----------------

#if defined(_WIN32)

// Windows CPUID code:

#include <Windows.h>
#include <intrin.h>
#include <cstdint>

typedef BOOL(WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
BOOL IsWow64()
{
	BOOL bIsWow64 = FALSE;

	LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(
		GetModuleHandle(TEXT("kernel32")), "IsWow64Process");

	if (NULL != fnIsWow64Process)
	{
		if (!fnIsWow64Process(GetCurrentProcess(), &bIsWow64))
		{
			std::cout << "Error Detecting Operating System. Defaulting to 32-bit OS." << std::endl;
			bIsWow64 = FALSE;
		}
	}
	return bIsWow64;
}
bool CPU::x64Supported()
{
#ifdef _M_X64
	return true;
#else
	BOOL bIsWow64 = FALSE;

	LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle(TEXT("kernel32")), "IsWow64Process");

	if (NULL != fnIsWow64Process)
	{
		if (!fnIsWow64Process(GetCurrentProcess(), &bIsWow64))
		{
			return false;
		}
	}
	return bIsWow64 != FALSE;
#endif
}

void CPU::CPUID(int32_t out[4], int32_t x)
{
	__cpuidex(out, x, 0);
}

uint64_t CPU::xgetbv(unsigned int x)
{
	return _xgetbv(x);
}

#else 

// Linux CPUID code:

#include <cstdint>
#include <cpuid.h>

bool CPU::x64Supported()
{
	return true;
}

void CPU::CPUID(int32_t out[4], int32_t x)
{
	__cpuid_count(x, 0, out[0], out[1], out[2], out[3]);
}

uint64_t CPU::xgetbv(unsigned int index)
{
	uint32_t eax, edx;
	__asm__ __volatile__("xgetbv" : "=a"(eax), "=d"(edx) : "c"(index));
	return ((uint64_t)edx << 32) | eax;
}

#endif

#ifndef _XCR_XFEATURE_ENABLED_MASK
#define _XCR_XFEATURE_ENABLED_MASK 0
#endif

CPU::CPU()
{
	// Reset all the flags;
	std::memset(this, 0, sizeof(*this));

	// Start with the OS support:

	bool avxSupported = false;

	int cpuInfo[4];
	CPUID(cpuInfo, 1);

	bool osUsesXSAVE_XRSTORE = (cpuInfo[2] & (1 << 27)) != 0;
	bool cpuAVXSuport = (cpuInfo[2] & (1 << 28)) != 0;

	if (osUsesXSAVE_XRSTORE && cpuAVXSuport)
	{
		uint64_t xcrFeatureMask = xgetbv(_XCR_XFEATURE_ENABLED_MASK);
		avxSupported = (xcrFeatureMask & 0x6) == 0x6;
	}

	if (avxSupported)
	{
		uint64_t xcrFeatureMask = xgetbv(_XCR_XFEATURE_ENABLED_MASK);
		if ((xcrFeatureMask & 0xe6) == 0xe6)
		{
			this->OS_AVX512 = true;
		}
		else
		{
			this->OS_AVX512 = false;
		}

		this->OS_AVX = true;
	}
	else
	{
		this->OS_AVX = false;
	}

	this->OS_X64 = x64Supported();

	// Vendor:

	int32_t CPUInfo[4];
	char name[13];

	CPUID(CPUInfo, 0);
	memcpy(name + 0, &CPUInfo[1], 4);
	memcpy(name + 4, &CPUInfo[3], 4);
	memcpy(name + 8, &CPUInfo[2], 4);
	name[12] = '\0';

	std::string nm(name);
	if (nm == "GenuineIntel")
	{
		CPUVendor = Vendor::Intel;
	}
	else if (nm == "AuthenticAMD")
	{
		CPUVendor = Vendor::AMD;
	}
	else
	{
		CPUVendor = Vendor::Other;
	}

	// Features:

	int info[4];
	CPUID(info, 0);
	int nIds = info[0];

	CPUID(info, 0x80000000);
	uint32_t nExIds = info[0];

	//  Detect Features from CPUID flags:

	if (nIds >= 0x00000001)
	{
		CPUID(info, 0x00000001);
		MMX = (info[3] & ((int)1 << 23)) != 0;
		SSE = (info[3] & ((int)1 << 25)) != 0;
		SSE2 = (info[3] & ((int)1 << 26)) != 0;
		SSE3 = (info[2] & ((int)1 << 0)) != 0;

		SSSE3 = (info[2] & ((int)1 << 9)) != 0;
		SSE41 = (info[2] & ((int)1 << 19)) != 0;
		SSE42 = (info[2] & ((int)1 << 20)) != 0;
		AES = (info[2] & ((int)1 << 25)) != 0;

		AVX = (info[2] & ((int)1 << 28)) != 0;
		FMA3 = (info[2] & ((int)1 << 12)) != 0;

		RDRAND = (info[2] & ((int)1 << 30)) != 0;
	}

	if (nIds >= 0x00000007)
	{
		CPUID(info, 0x00000007);

		AVX2 = (info[1] & ((int)1 << 5)) != 0;

		BMI1 = (info[1] & ((int)1 << 3)) != 0;
		BMI2 = (info[1] & ((int)1 << 8)) != 0;
		ADX = (info[1] & ((int)1 << 19)) != 0;
		MPX = (info[1] & ((int)1 << 14)) != 0;
		SHA = (info[1] & ((int)1 << 29)) != 0;
		PREFETCHWT1 = (info[2] & ((int)1 << 0)) != 0;

		AVX512_F = (info[1] & ((int)1 << 16)) != 0;
		AVX512_CD = (info[1] & ((int)1 << 28)) != 0;
		AVX512_PF = (info[1] & ((int)1 << 26)) != 0;
		AVX512_ER = (info[1] & ((int)1 << 27)) != 0;
		AVX512_VL = (info[1] & ((int)1 << 31)) != 0;
		AVX512_BW = (info[1] & ((int)1 << 30)) != 0;
		AVX512_DQ = (info[1] & ((int)1 << 17)) != 0;
		AVX512_IFMA = (info[1] & ((int)1 << 21)) != 0;
		AVX512_VBMI = (info[2] & ((int)1 << 1)) != 0;
	}

	if (nExIds >= 0x80000001)
	{
		CPUID(info, 0x80000001);

		x64 = (info[3] & ((int)1 << 29)) != 0;
		ABM = (info[2] & ((int)1 << 5)) != 0;
		SSE4a = (info[2] & ((int)1 << 6)) != 0;
		FMA4 = (info[2] & ((int)1 << 16)) != 0;
		XOP = (info[2] & ((int)1 << 11)) != 0;
	}
}
