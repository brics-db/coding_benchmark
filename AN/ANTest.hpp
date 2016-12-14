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

#include <cstdint>
#include <sstream>

#include "../Test.hpp"
#include "../Util/Intrinsics.hpp"

template<typename DATAIN, typename DATAOUT, size_t UNROLL>
class ANTest : public Test<DATAIN, DATAOUT> {

protected:
    const DATAOUT A;
    const DATAOUT A_INV;

public:

    ANTest (const char* const name, AlignedBlock & in, AlignedBlock & out, const DATAOUT A, const DATAOUT A_INV) :
            Test<DATAIN, DATAOUT>(nullptr, in, out),
            A (A),
            A_INV (A_INV) {
        std::stringstream ss;
        ss << name << " " << A;
        std::string str = ss.str();
        size_t len = str.size() + 1;
        this->name = new char[len];
        strncpy_s(const_cast<char*>(this->name), len, str.c_str(), len);
    }

    virtual
    ~ANTest () {
        delete[] this->name;
    }
};
