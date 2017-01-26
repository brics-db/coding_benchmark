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

#include <chrono>
#include <string>
#include <iostream>

#if defined(_WIN32)

#include <Windows.h>
#undef min
#undef max

const long long g_Frequency = []() -> long long {
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    return frequency.QuadPart;
}();

struct HighResClock {

    typedef long long rep;
    typedef std::nano period;
    typedef std::chrono::duration<rep, period> duration;
    typedef std::chrono::time_point<HighResClock> time_point;
    static const bool is_steady = true;

    static time_point
    now () {
        LARGE_INTEGER count;
        QueryPerformanceCounter(&count);
        return time_point(duration(count.QuadPart * static_cast<rep>(period::den) / g_Frequency));
    }
};

#else

// The way it should have been implemented...
typedef std::chrono::high_resolution_clock HighResClock;

#endif

class Stopwatch {

    typedef HighResClock Clock;
    Clock::time_point start;

public:
    Stopwatch ();

    void Reset ();

    int64_t Current ();
};
