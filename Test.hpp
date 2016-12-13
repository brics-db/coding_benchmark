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

#include <string>
#include <cstring>
#include <cstdint>

#include "Util/AlignedBlock.hpp"
#include "Util/TestInfo.hpp"

struct TestBase0 {

    virtual const char* getSIMDtypeName () = 0;

    virtual bool HasCapabilities () = 0;
};

struct TestBase : virtual public TestBase0 {

protected:
    const char* name;
    AlignedBlock in;
    AlignedBlock out;

public:
    TestBase (const char* const name, AlignedBlock & in, AlignedBlock & out);

    virtual ~TestBase ();

    virtual void ResetBuffers () = 0;

    virtual size_t getInputTypeSize () = 0;

    virtual size_t getOutputTypeSize () = 0;

    // Encoding
    virtual void PreEnc (const size_t numIterations);

    virtual void RunEnc (const size_t numIterations) = 0;

    // Check
    virtual bool DoCheck ();

    virtual void PreCheck (const size_t numIterations);

    virtual void RunCheck (const size_t numIterations);

    // Arithmetic
    virtual bool DoArith ();

    virtual void PreArith (const size_t numIterations);

    virtual void RunArith (const size_t numIterations, uint16_t value);

    // Decoding
    virtual bool DoDec ();

    virtual void PreDec (const size_t numIterations);

    virtual void RunDec (const size_t numIterations);

    // Execute test:
    TestInfos Execute (const size_t numIterations);
};

struct SequentialTest : virtual public TestBase0 {

    virtual const char* getSIMDtypeName () override;

    virtual bool HasCapabilities () override;
};

struct SSE42Test : virtual public TestBase0 {

    virtual const char* getSIMDtypeName () override;

    virtual bool HasCapabilities () override;
};

struct AVX2Test : virtual public TestBase0 {

    virtual const char* getSIMDtypeName () override;

    virtual bool HasCapabilities () override;
};

#ifdef _MSC_VER
#ifdef DATA
#undef DATA
#endif

#ifdef CS
#undef CS
#endif
#endif

template<typename DATA, typename CS>
struct Test : public TestBase {

    Test (const char* const name, AlignedBlock & in, AlignedBlock & out) :
            TestBase (name, in, out) {
    }

    virtual
    ~Test () {
    }

    virtual size_t
    getInputTypeSize () override {
        return sizeof (DATA);
    }

    virtual size_t
    getOutputTypeSize () override {
        return sizeof (CS);
    }

    void
    ResetBuffers () override {
        // Reset buffers:
        // uint16_t* const pInEnd = in.end<uint16_t>();
        DATA value = 12783u;
        for (DATA* pIn = this->in.template begin<DATA>(); pIn < this->in.template end<DATA>(); ++pIn) {
            *pIn = value;
            value = value * 7577u + 10467u;
        }

        memset(this->out.begin(), 0, this->out.nBytes);
    }
};
