// Copyright 2017 Till Kolditz, Stefan de Bruijn
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

#include <AN/AN_seq_s_inv.hpp>

template<size_t UNROLL>
struct AN_seq_8_16_s_inv :
        public AN_seq_s_inv<int8_t, int16_t, UNROLL> {

    AN_seq_8_16_s_inv(
            const char* const name,
            AlignedBlock & in,
            AlignedBlock & out,
            int16_t A,
            int16_t AInv)
            : AN_seq_s_inv<int8_t, int16_t, UNROLL>(name, in, out, A, AInv) {
    }

    virtual ~AN_seq_8_16_s_inv() {
    }
};
