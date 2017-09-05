// Copyright (c) 2016 Till Kolditz
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

/*
 * File:   Main.hpp
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 17. Februar 2017, 12:44
 */

#ifndef MAIN_HPP
#define MAIN_HPP

#include <string>
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>

#ifdef _MSC_VER
// disable stupid diamond-inheritance warnings (the compiler does not see that there is only a single implementation for each of the functions)
#pragma warning(disable: 4250)
#endif

#include <Test.hpp>
#include <Util/AlignedBlock.hpp>
#include <Util/TestInfo.hpp>
#include <Util/ErrorInfo.hpp>
#include <Util/VFunc.hpp>
#include <Util/Euclidean.hpp>
#include <Util/Intrinsics.hpp>
#include <Util/ComputeNumRuns.hpp>
#include <Util/ExpandTest.hpp>
#include <Util/TestCase.hpp>

#include <CopyTest.hpp>

#include <XOR/XOR_scalar.hpp>
#include <XOR/XOR_sse42.hpp>
#ifdef __AVX2__
#include <XOR/XOR_avx2.hpp>
#endif

#include <AN/AN_scalar.hpp>
#include <AN/AN_sse42_8x16_8x32_u_inv.hpp>
#include <AN/AN_sse42_8x16_8x32_u_divmod.hpp>
#include <AN/AN_sse42_8x16_8x32_s_inv.hpp>
#include <AN/AN_sse42_8x16_8x32_s_divmod.hpp>
#ifdef __AVX2__
#include <AN/AN_avx2_16x16_16x32_u_inv.hpp>
#include <AN/AN_avx2_16x16_16x32_u_divmod.hpp>
#include <AN/AN_avx2_16x16_16x32_s_inv.hpp>
#include <AN/AN_avx2_16x16_16x32_s_divmod.hpp>
#endif

#include <Hamming/Hamming_scalar.hpp>
#include <Hamming/Hamming_sse42.hpp>
#ifdef __AVX2__
#include <Hamming/Hamming_avx2.hpp>
#endif

#include <Output.hpp>

#endif /* MAIN_HPP */
