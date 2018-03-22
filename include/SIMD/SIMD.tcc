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
 * File:   SIMD.tcc
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 09-08-2017 00:18
 * Renamed on 19-03-2018 10:41
 */

#pragma once

#if not defined(LIB_COLUMN_OPERATORS_SIMD_SSE_HPP_) and not defined(LIB_COLUMN_OPERATORS_SIMD_AVX2_HPP_) and not defined(LIB_COLUMN_OPERATORS_SIMD_AVX512_HPP_)
#error "This file must not be included by client code!"
#endif

#include <cstdint>
#include <cstdlib>
#include <functional>
#include <type_traits>

#include <Util/Intrinsics.hpp>
#include <Util/Functors.hpp>

namespace coding_benchmark {
    namespace simd {

        template<typename ... Args>
        struct mm;

        template<typename V, typename T, template<typename > class Op>
        struct mm_op;

        template<>
        struct mm<uint8_t> {
            static inline uint8_t loadu(
                    uint8_t * src) {
                return *src;
            }

            static inline void storeu(
                    uint8_t * dst,
                    uint8_t src) {
                *dst = src;
            }
        };

        template<>
        struct mm<int8_t> {
            static inline int8_t loadu(
                    int8_t * src) {
                return *src;
            }

            static inline void storeu(
                    int8_t * dst,
                    int8_t src) {
                *dst = src;
            }
        };

        template<>
        struct mm<uint16_t> {
            static inline uint16_t loadu(
                    uint16_t * src) {
                return *src;
            }

            static inline void storeu(
                    uint16_t * dst,
                    uint16_t src) {
                *dst = src;
            }
        };

        template<>
        struct mm<int16_t> {
            static inline int16_t loadu(
                    int16_t * src) {
                return *src;
            }

            static inline void storeu(
                    int16_t * dst,
                    int16_t src) {
                *dst = src;
            }
        };

        template<>
        struct mm<uint32_t> {
            static inline uint32_t loadu(
                    uint32_t * src) {
                return *src;
            }

            static inline void storeu(
                    uint32_t * dst,
                    uint32_t src) {
                *dst = src;
            }
        };

        template<>
        struct mm<int32_t> {
            static inline int32_t loadu(
                    int32_t * src) {
                return *src;
            }

            static inline void storeu(
                    int32_t * dst,
                    int32_t src) {
                *dst = src;
            }
        };

        template<>
        struct mm<uint64_t> {
            static inline uint64_t loadu(
                    uint64_t * src) {
                return *src;
            }

            static inline void storeu(
                    uint64_t * dst,
                    uint64_t src) {
                *dst = src;
            }
        };

        template<>
        struct mm<int64_t> {
            static inline int64_t loadu(
                    int64_t * src) {
                return *src;
            }

            static inline void storeu(
                    int64_t * dst,
                    int64_t src) {
                *dst = src;
            }
        };

    }
}
