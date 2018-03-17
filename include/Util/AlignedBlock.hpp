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

#include <cstring>
#include <cstdint>
#include <memory>

struct AlignedBlock {

    const size_t nBytes;
    const size_t alignment;

private:
    std::shared_ptr<char> baseptr;
    void* const data;

public:

    AlignedBlock()
            : nBytes(0),
              alignment(0),
              baseptr(nullptr),
              data(nullptr) {
    }

    AlignedBlock(
            size_t nBytes,
            size_t alignment)
            : nBytes(nBytes),
              alignment(alignment),
              baseptr(new char[nBytes + alignment]),
              data(baseptr.get() + (alignment - (reinterpret_cast<size_t>(baseptr.get()) & (alignment - 1)))) {
    }

    AlignedBlock(
            AlignedBlock & other)
            : nBytes(other.nBytes),
              alignment(other.alignment),
              baseptr(other.baseptr),
              data(other.data) {
    }

    AlignedBlock & operator=(
            AlignedBlock & other) {
        this->~AlignedBlock();
        new (this) AlignedBlock(other);
        return *this;
    }

    template<typename T = void>
    constexpr T*
    begin() const {
        return static_cast<T*>(data);
    }

    template<typename T = void>
    constexpr T*
    end() const {
        return reinterpret_cast<T*>(static_cast<char*>(data) + nBytes);
    }

    void clear() const { // OK this is quite shitty but we need it when AlignedBlock is a member of a function's const argument
        memset(data, 0, nBytes);
    }

    virtual ~AlignedBlock() {
        // force to set everything to zero to avoid bad use-after-free
        // we call the constructor, because I want data to be a constant pointer
        new (this) AlignedBlock();
    }
};
