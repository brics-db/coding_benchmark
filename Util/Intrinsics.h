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

#if defined(_MSC_VER) // Visual C++

#include <intrin.h>

#define __builtin_popcount __popcnt
#define __builtin_popcountl __popcnt
#define __builtin_popcountll __popcnt64

#elif defined(__arm__) // ARM

#error "Not supported"

#else // Linux

#include <immintrin.h>

inline void _ReadWriteBarrier() // just to make it the same name as in Visual Studio, which we want to support
{
	asm volatile ("" : : : "memory");
}

#endif

