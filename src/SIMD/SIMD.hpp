// Copyright (c) 2017 Till Kolditz
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
 * File:   SIMD.hpp
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 09-08-2017 00:18
 */

#pragma once

#if not defined(LIB_COLUMN_OPERATORS_SIMD_SSE_HPP_) and not defined(LIB_COLUMN_OPERATORS_SIMD_AVX2_HPP_) and not defined(LIB_COLUMN_OPERATORS_SIMD_AVX512_HPP_)
#error "This file must not be included by client code!"
#endif

namespace coding_benchmark {
    namespace simd {

        template<typename V, typename T>
        struct mm;

        template<typename V, typename T, template<typename > class Op>
        struct mm_op;

    }
}
