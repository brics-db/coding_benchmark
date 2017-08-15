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
 * File:   AN_sse42.cpp
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 15-08-2017 18:19
 */

#include <AN/AN_sse42.hpp>

extern template
struct AN_sse42_8x16_8x32_s_divmod<1>;
extern template
struct AN_sse42_8x16_8x32_s_divmod<2>;
extern template
struct AN_sse42_8x16_8x32_s_divmod<4>;
extern template
struct AN_sse42_8x16_8x32_s_divmod<8>;
extern template
struct AN_sse42_8x16_8x32_s_divmod<16>;
extern template
struct AN_sse42_8x16_8x32_s_divmod<32>;
extern template
struct AN_sse42_8x16_8x32_s_divmod<64>;
extern template
struct AN_sse42_8x16_8x32_s_divmod<128>;
extern template
struct AN_sse42_8x16_8x32_s_divmod<256>;
extern template
struct AN_sse42_8x16_8x32_s_divmod<512>;
extern template
struct AN_sse42_8x16_8x32_s_divmod<1024>;

extern template
struct AN_sse42_8x16_8x32_u_divmod<1>;
extern template
struct AN_sse42_8x16_8x32_u_divmod<2>;
extern template
struct AN_sse42_8x16_8x32_u_divmod<4>;
extern template
struct AN_sse42_8x16_8x32_u_divmod<8>;
extern template
struct AN_sse42_8x16_8x32_u_divmod<16>;
extern template
struct AN_sse42_8x16_8x32_u_divmod<32>;
extern template
struct AN_sse42_8x16_8x32_u_divmod<64>;
extern template
struct AN_sse42_8x16_8x32_u_divmod<128>;
extern template
struct AN_sse42_8x16_8x32_u_divmod<256>;
extern template
struct AN_sse42_8x16_8x32_u_divmod<512>;
extern template
struct AN_sse42_8x16_8x32_u_divmod<1024>;

extern template
struct AN_sse42_8x16_8x32_s_inv<1>;
extern template
struct AN_sse42_8x16_8x32_s_inv<2>;
extern template
struct AN_sse42_8x16_8x32_s_inv<4>;
extern template
struct AN_sse42_8x16_8x32_s_inv<8>;
extern template
struct AN_sse42_8x16_8x32_s_inv<16>;
extern template
struct AN_sse42_8x16_8x32_s_inv<32>;
extern template
struct AN_sse42_8x16_8x32_s_inv<64>;
extern template
struct AN_sse42_8x16_8x32_s_inv<128>;
extern template
struct AN_sse42_8x16_8x32_s_inv<256>;
extern template
struct AN_sse42_8x16_8x32_s_inv<512>;
extern template
struct AN_sse42_8x16_8x32_s_inv<1024>;

extern template
struct AN_sse42_8x16_8x32_u_inv<1>;
extern template
struct AN_sse42_8x16_8x32_u_inv<2>;
extern template
struct AN_sse42_8x16_8x32_u_inv<4>;
extern template
struct AN_sse42_8x16_8x32_u_inv<8>;
extern template
struct AN_sse42_8x16_8x32_u_inv<16>;
extern template
struct AN_sse42_8x16_8x32_u_inv<32>;
extern template
struct AN_sse42_8x16_8x32_u_inv<64>;
extern template
struct AN_sse42_8x16_8x32_u_inv<128>;
extern template
struct AN_sse42_8x16_8x32_u_inv<256>;
extern template
struct AN_sse42_8x16_8x32_u_inv<512>;
extern template
struct AN_sse42_8x16_8x32_u_inv<1024>;
