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
 * File:   Macros.hpp
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 06-07-2017 16:25
 */
#ifndef UTIL_MACROS_HPP_
#define UTIL_MACROS_HPP_

/*
 * Concatenate preprocessor tokens A and B without expanding macro definitions
 * (however, if invoked from a macro, macro arguments are expanded).
 */
#define CONCAT0(A, B) A B
#define CONCATX(A, B) CONCAT0(A, B)

/*
 * Concatenate preprocessor tokens A and B after macro-expanding them.
 */
#define CONCAT(...) VFUNC(CONCAT, __VA_ARGS__)
#define CONCAT2(A, B) CONCATX(A, B)
#define CONCAT3(A, B, C) CONCAT2(CONCAT2(A, B), C)
#define CONCAT4(A, B, C, D) CONCAT2(CONCAT2(A, B), CONCAT2(C, D))
#define CONCAT5(A, B, C, D, E) CONCAT2(CONCAT4(A, B, C, D), E)

#define TOSTRING0(str) #str
#define TOSTRING(...) VFUNC(TOSTRING, __VA_ARGS__)
#define TOSTRING1(str) TOSTRING0(str)
#define TOSTRING2(str1, str2) TOSTRING0(str1) TOSTRING0(str2)
#define TOSTRING3(str1, str2, str3) TOSTRING0(CONCAT(str1, str2, str3))
#define TOSTRING4(str1, str2, str3, str4) TOSTRING0(CONCAT(str1, str2, str3, str4))

#endif /* UTIL_MACROS_HPP_ */
