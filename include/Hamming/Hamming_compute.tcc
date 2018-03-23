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
 * File:   Hamming_compute.tcc
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 22-08-2017 10:47
 */

#pragma once

#include <Util/Test.hpp>
#include <Hamming/Hamming_scalar.hpp>
#include <Hamming/Hamming_simd.hpp>

namespace coding_benchmark {

    template<typename DATAIN, size_t UNROLL, size_t StoreVersion = 1>
    struct Hamming_compute_scalar :
            public Test<DATAIN, hamming_t<DATAIN, DATAIN>>,
            public ScalarTest {
        Hamming_scalar<DATAIN, UNROLL, StoreVersion> impl;

        Hamming_compute_scalar(
                const std::string & name,
                AlignedBlock & bufRaw,
                AlignedBlock & bufEncoded,
                AlignedBlock & bufResult)
                : Test<DATAIN, hamming_t<DATAIN, DATAIN>>(name, bufRaw, bufEncoded, bufResult),
                  ScalarTest(),
                  impl(name, bufRaw, bufEncoded, bufResult) {
        }

        virtual ~Hamming_compute_scalar() {
        }

        void RunEncode(
                const EncodeConfiguration & config) override {
            impl.RunEncode(config);
        }

        void RunDecodeChecked(
                const DecodeConfiguration & config) override {
            impl.RunDecodeChecked(config);
        }
    };

#ifdef __SSE4_2__

    template<typename DATAIN, size_t UNROLL, size_t StoreVersion>
    struct Hamming_compute_sse42 :
            public Test<DATAIN, hamming_t<DATAIN, __m128i >>,
            public SSE42Test {
        Hamming_simd<DATAIN, __m128i, UNROLL, StoreVersion> impl;

        Hamming_compute_sse42(
                const std::string & name,
                AlignedBlock & bufRaw,
                AlignedBlock & bufEncoded,
                AlignedBlock & bufResult)
                : Test<DATAIN, hamming_t<DATAIN, __m128i >>(name, bufRaw, bufEncoded, bufResult),
                  SSE42Test(),
                  impl(name, bufRaw, bufEncoded, bufResult) {
        }

        virtual ~Hamming_compute_sse42() {
        }

        void RunEncode(
                const EncodeConfiguration & config) override {
            impl.RunEncode(config);
        }

        void RunDecodeChecked(
                const DecodeConfiguration & config) override {
            impl.RunDecodeChecked(config);
        }
    };

#endif

#ifdef __AVX2__

    template<typename DATAIN, size_t UNROLL, size_t StoreVersion>
    struct Hamming_compute_avx2 :
            public Test<DATAIN, hamming_t<DATAIN, __m256i >>,
            public AVX2Test {
        Hamming_simd<DATAIN, __m256i, UNROLL, StoreVersion> impl;

        Hamming_compute_avx2(
                const std::string & name,
                AlignedBlock & bufRaw,
                AlignedBlock & bufEncoded,
                AlignedBlock & bufResult)
                : Test<DATAIN, hamming_t<DATAIN, __m256i >>(name, bufRaw, bufEncoded, bufResult),
                  AVX2Test(),
                  impl(name, bufRaw, bufEncoded, bufResult) {
        }

        virtual ~Hamming_compute_avx2() {
        }

        void RunEncode(
                const EncodeConfiguration & config) override {
            impl.RunEncode(config);
        }

        void RunDecodeChecked(
                const DecodeConfiguration & config) override {
            impl.RunDecodeChecked(config);
        }
    };

#endif

}
