// Copyright 2017 Till Kolditz
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
 * StringBase.hpp
 *
 *  Created on: 25.03.2018
 *      Author: Till Kolditz - Till.Kolditz@gmail.com
 */


#pragma once

#include <cstdlib>

#include <Util/Intrinsics.hpp>

const constexpr long charsPerMM128 = 16; // asm ("rdx") does not work
const constexpr long shortsPerMM128 = 8; // asm ("rdx") does not work
const constexpr long intsPerMM128 = 4; // asm ("rdx") does not work
