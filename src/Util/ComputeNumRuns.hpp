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
 * File:   ComputeNumRuns.hpp
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 17. Februar 2017, 12:45
 */

#ifndef COMPUTENUMRUNS_HPP
#define COMPUTENUMRUNS_HPP

template<size_t start, size_t end>
struct ComputeNumRuns {

    constexpr size_t operator()() const {
        return 1 + ComputeNumRuns<start * 2, end>()();
    }
};

template<size_t start>
struct ComputeNumRuns<start, start> {

    constexpr size_t operator()() const {
        return 1;
    }
};

#endif /* COMPUTENUMRUNS_HPP */
