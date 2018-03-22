// Copyright (c) 2016-2017 Till Kolditz, Stefan de Bruijn
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
 * File:   AN_simd_divmod.tcc
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on
 * Renamed in 19-03-2018 16:06
 */

#pragma once

#ifndef AN_SIMD
#error "Clients must not include this file directly, but file <AN/AN_sse42.hpp>!"
#endif

#include <AN/AN_simd.tcc>

namespace coding_benchmark {

    template<typename T, typename VEC, size_t I, size_t MAX>
    struct Detector {
        static bool isValid(
                VEC mmIn,
                T A) {
            return (mm<VEC, T>::template extract<I>(mmIn) % A == 0) && Detector<T, VEC, I + 1, MAX>::isValid(mmIn, A);
        }
    };

    template<typename T, typename VEC, size_t I>
    struct Detector<T, VEC, I, I> {
        static bool isValid(
                VEC mmIn,
                T A) {
            return (mm<VEC, T>::template extract<I>(mmIn) % A == 0);
        }
    };

    template<typename T, typename VEC>
    bool isValid(
            VEC mmIn,
            T A) {
        return Detector<T, VEC, 0, (sizeof(VEC) / sizeof(T) - 1)>::isValid(mmIn, A);
    }

    template<typename DATARAW, typename DATAENC, typename VEC, size_t UNROLL>
    struct AN_simd_divmod :
            public AN_simd<DATARAW, DATAENC, VEC, UNROLL> {

        typedef AN_simd<DATARAW, DATAENC, VEC, UNROLL> BASE;

        using BASE::AN_simd;

        virtual ~AN_simd_divmod() {
        }

        virtual bool DoCheck() override {
            return true;
        }

        virtual void RunCheck(
                const CheckConfiguration & config) override {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                auto inV = config.target.template begin<VEC>();
                const auto inVend = this->template ComputeEnd<DATAENC>(inV, config);
                while (inV <= (inVend - UNROLL)) {
                    // let the compiler unroll the loop
                    for (size_t k = 0; k < UNROLL; ++k) {
                        auto mmIn = *inV;
                        if (isValid<DATAENC, VEC>(mmIn, this->A)) {
                            ++inV;
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATAENC*>(inV) - config.target.template begin<DATAENC>(), iteration);
                        }
                    }
                }
                // here follows the non-unrolled remainder
                while (inV <= (inVend - 1)) {
                    auto mmIn = *inV;
                    if (isValid<DATAENC, VEC>(mmIn, this->A)) {
                        ++inV;
                    } else {
                        throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATAENC*>(inV) - config.target.template begin<DATAENC>(), iteration);
                    }
                }
                if (inV < inVend) {
                    auto inS = reinterpret_cast<DATAENC*>(inV);
                    auto inSend = reinterpret_cast<DATAENC* const >(inVend);
                    while (inS < inSend) {
                        if ((*inS % this->A) == 0) {
                            ++inS;
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, inS - config.target.template begin<DATAENC>(), iteration);
                        }
                    }
                }
            }
        }

        bool DoArithmeticChecked(
                const ArithmeticConfiguration & config) override {
            return std::visit(ArithmeticSelector(), config.mode);
        }

        struct ArithmetorChecked {
            AN_simd_divmod & test;
            const ArithmeticConfiguration & config;
            const size_t iteration;
            ArithmetorChecked(
                    AN_simd_divmod & test,
                    const ArithmeticConfiguration & config,
                    const size_t iteration)
                    : test(test),
                      config(config),
                      iteration(iteration) {
            }
            template<template<typename = void> class Functor>
            void impl() {
                auto inV = config.source.template begin<VEC>();
                const auto inVend = test.template ComputeEnd<DATAENC>(inV, config);
                auto outV = config.target.template begin<VEC>();
                DATAENC operand = config.operand;
                if constexpr (std::is_same_v<Functor<void>, add<void>> || std::is_same_v<Functor<void>, sub<void>> || std::is_same_v<Functor<void>, div<void>>) {
                    operand = config.operand * test.A;
                } else if constexpr (std::is_same_v<Functor<void>, mul<void>>) {
                    // do not encode operand here, otherwise we will have non-code values after the operation!
                } else {
                    throw std::runtime_error("Functor not known!");
                }
                VEC mmOperand = mm<VEC, DATAENC>::set1(operand);
                VEC mmA = mm<VEC, DATAENC>::set1(test.A);
                while (inV <= (inVend - UNROLL)) {
                    // let the compiler unroll the loop
                    for (size_t unroll = 0; unroll < UNROLL; ++unroll) {
                        auto mmIn = *inV++;
                        if (isValid(mmIn, test.A)) {
                            auto x = mm_op<VEC, DATAENC, Functor>::compute(mmIn, mmOperand);
                            if (std::is_same_v<Functor<void>, div<void>>) {
                                x = mm_op<VEC, DATAENC, mul>::compute(x, mmA); // make sure we get a code word again
                            }
                            *outV++ = x;
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATAENC*>(inV - 1) - config.source.template begin<DATAENC>(), iteration);
                        }
                    }
                }
                // remaining numbers
                while (inV <= (inVend - 1)) {
                    auto mmIn = *inV++;
                    if (isValid(mmIn, test.A)) {
                        auto x = mm_op<VEC, DATAENC, Functor>::compute(mmIn, mmOperand);
                        if (std::is_same_v<Functor<void>, div<void>>) {
                            x = mm_op<VEC, DATAENC, mul>::compute(x, mmA); // make sure we get a code word again
                        }
                        *outV++ = x;
                    } else {
                        throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATAENC*>(inV - 1) - config.source.template begin<DATAENC>(), iteration);
                    }
                }
                if (inV < inVend) {
                    Functor<> functor;
                    auto inS = reinterpret_cast<DATAENC*>(inV);
                    const auto inSend = reinterpret_cast<DATAENC* const >(inVend);
                    auto outS = reinterpret_cast<DATAENC*>(outV);
                    while (inS < inSend) {
                        if (*inS % test.A == 0) {
                            auto x = functor(*inS++, operand);
                            if (std::is_same_v<Functor<void>, div<void>>) {
                                x *= test.A; // make sure we get a code word again
                            }
                            *outS++ = x;
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, inS - config.source.template begin<DATAENC>(), iteration);
                        }
                    }
                }
            }
            void operator()(
                    ArithmeticConfiguration::Add) {
                impl<add>();
            }
            void operator()(
                    ArithmeticConfiguration::Sub) {
                impl<sub>();
            }
            void operator()(
                    ArithmeticConfiguration::Mul) {
                impl<mul>();
            }
            void operator()(
                    ArithmeticConfiguration::Div) {
                impl<div>();
            }
        };

        void RunArithmeticChecked(
                const ArithmeticConfiguration & config) override {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                _ReadWriteBarrier();
                std::visit(ArithmetorChecked(*this, config, iteration), config.mode);
            }
        }

        bool DoAggregateChecked(
                const AggregateConfiguration & config) override {
            return std::visit(AggregateSelector(), config.mode);
        }

        struct AggregatorChecked {
            typedef typename Larger<DATAENC>::larger_t larger_t;
            AN_simd_divmod & test;
            const AggregateConfiguration & config;
            size_t iteration;
            AggregatorChecked(
                    AN_simd_divmod & test,
                    const AggregateConfiguration & config,
                    size_t iteration)
                    : test(test),
                      config(config),
                      iteration(iteration) {
            }
            template<typename Aggregate, typename InitializeVector, typename KernelVector, typename KernelScalar, typename VectorToScalar, typename Finalize>
            void impl(
                    InitializeVector && funcInitVector,
                    KernelVector && funcKernelVector,
                    VectorToScalar && funcVectorToScalar,
                    KernelScalar && funcKernelScalar,
                    Finalize && funcFinal) {
                auto inV = config.source.template begin<VEC>();
                const auto inVend = test.template ComputeEnd<DATAENC>(inV, config);
                auto mmValue = funcInitVector();
                while (inV <= (inVend - UNROLL)) {
                    for (size_t k = 0; k < UNROLL; ++k) {
                        auto mmIn = *inV++;
                        if (isValid(mmIn, test.A)) {
                            mmValue = funcKernelVector(mmValue, mmIn);
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATAENC*>(inV - 1) - config.source.template begin<DATAENC>(), iteration);
                        }
                    }
                }
                while (inV <= (inVend - 1)) {
                    auto mmIn = *inV++;
                    if (isValid(mmIn, test.A)) {
                        mmValue = funcKernelVector(mmValue, mmIn);
                    } else {
                        throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATAENC*>(inV - 1) - config.source.template begin<DATAENC>(), iteration);
                    }
                }
                Aggregate value = funcVectorToScalar(mmValue);
                if (inV < inVend) {
                    auto inS = reinterpret_cast<DATAENC*>(inV);
                    const auto inSend = reinterpret_cast<DATAENC* const >(inVend);
                    while (inS < inSend) {
                        if ((*inS % test.A) == 0) {
                            value = funcKernelScalar(value, *inS++);
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, inS - config.source.template begin<DATAENC>(), iteration);
                        }
                    }
                }
                auto final = funcFinal(value, config.numValues);
                auto dataOut = test.bufScratchPad.template begin<Aggregate>();
                *dataOut = final / test.A; // decode here, because we encode again in the next step. This is currently required!
                EncodeConfiguration encConf(1, 2, test.bufScratchPad, config.target);
                test.RunEncode(encConf);
            }
            void operator()(
                    AggregateConfiguration::Sum) {
                impl<larger_t>([] {return simd::mm<VEC, larger_t>::set1(0);}, [](VEC mmSum, VEC mmTmp) {
                    auto mmLo = simd::mm<VEC, DATAENC>::cvt_larger_lo(mmTmp);
                    mmLo = simd::mm_op<VEC, larger_t, add>::compute(mmSum, mmLo);
                    auto mmHi = simd::mm<VEC, DATAENC>::cvt_larger_hi(mmTmp);
                    return simd::mm_op<VEC, larger_t, add>::compute(mmLo, mmHi);
                }, [](VEC mmSum) {return simd::mm<VEC, larger_t>::sum(mmSum);}, [](larger_t sum, DATAENC tmp) {return sum + tmp;}, [](larger_t sum, size_t numValues) {return sum;});
            }
            void operator()(
                    AggregateConfiguration::Min) {
                impl<DATAENC>([] {return simd::mm<VEC, DATAENC>::set1(std::numeric_limits<DATAENC>::max());}, [](VEC mmMin, VEC mmTmp) {return simd::mm<VEC, DATAENC>::min(mmMin, mmTmp);},
                        [](VEC mmMin) {return simd::mm<VEC, DATAENC>::min(mmMin);}, [](DATAENC min, DATAENC tmp) {return min < tmp ? min : tmp;}, [](DATAENC min, size_t numValues) {return min;});
            }
            void operator()(
                    AggregateConfiguration::Max) {
                impl<DATAENC>([] {return simd::mm<VEC, DATAENC>::set1(std::numeric_limits<DATAENC>::min());}, [](VEC mmMax, VEC mmTmp) {return simd::mm<VEC, DATAENC>::max(mmMax, mmTmp);},
                        [](VEC mmMax) {return simd::mm<VEC, DATAENC>::max(mmMax);}, [](DATAENC max, DATAENC tmp) {return max > tmp ? max : tmp;}, [](DATAENC max, size_t numValues) {return max;});
            }
            void operator()(
                    AggregateConfiguration::Avg) {
                impl<larger_t>([] {return simd::mm<VEC, larger_t>::set1(0);}, [](VEC mmSum, VEC mmTmp) {
                    auto mmLo = simd::mm<VEC, DATAENC>::cvt_larger_lo(mmTmp);
                    mmLo = simd::mm_op<VEC, larger_t, add>::compute(mmSum, mmLo);
                    auto mmHi = simd::mm<VEC, DATAENC>::cvt_larger_hi(mmTmp);
                    return simd::mm_op<VEC, larger_t, add>::compute(mmLo, mmHi);
                }, [](VEC mmSum) {return simd::mm<VEC, larger_t>::sum(mmSum);}, [](larger_t sum, DATAENC tmp) {return sum + tmp;},
                        [this](larger_t sum, size_t numValues) {return (sum / (numValues * test.A)) * test.A;});
            }
        };

        void RunAggregateChecked(
                const AggregateConfiguration & config) override {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                _ReadWriteBarrier();
                std::visit(AggregatorChecked(*this, config, iteration), config.mode);
            }
        }

        template<bool check>
        void RunDecodeInternal(
                const DecodeConfiguration & config) {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                auto inV = config.source.template begin<VEC>();
                auto inVend = this->template ComputeEnd<DATAENC>(inV, config);
                auto outS = config.target.template begin<DATARAW>();
                auto mmA = mm<VEC, DATAENC>::set1(static_cast<DATAENC>(this->A));
                while (inV <= (inVend - UNROLL)) { // let the compiler unroll the loop
                    for (size_t unroll = 0; unroll < UNROLL; ++unroll) {
                        if ((!check) || isValid(*inV, this->A)) {
                            VEC dec = mm_op<VEC, DATAENC, div>::compute(*inV, mmA);
                            writeout<DATARAW, DATAENC, VEC>(dec, outS);
                            outS += (sizeof(VEC) / sizeof(DATAENC));
                        } else if (check) {
                            throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATAENC*>(inV) - config.source.template begin<DATAENC>(), iteration);
                        }
                        ++inV;
                    }
                }
                // remaining numbers
                while (inV <= (inVend - 1)) {
                    if ((!check) || isValid(*inV, this->A)) {
                        VEC dec = mm_op<VEC, DATAENC, div>::compute(*inV, mmA);
                        writeout<DATARAW, DATAENC, VEC>(dec, outS);
                        outS += (sizeof(VEC) / sizeof(DATAENC));
                    } else if (check) {
                        throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATAENC*>(inV) - config.source.template begin<DATAENC>(), iteration);
                    }
                    ++inV;
                }
                if (inV < inVend) {
                    auto inS = reinterpret_cast<DATAENC*>(inV);
                    auto inSend = reinterpret_cast<DATAENC * const >(inVend);
                    while (inS < inSend) {
                        if ((!check) || (*inS % this->A) == 0) {
                            *outS++ = static_cast<DATARAW>(*inS++ / this->A);
                        } else if (check) {
                            throw ErrorInfo(__FILE__, __LINE__, inS - config.source.template begin<DATAENC>(), iteration);
                        }
                    }
                }
            }
        }

        bool DoDecode() override {
            return true;
        }

        void RunDecode(
                const DecodeConfiguration & config) override {
            RunDecodeInternal<false>(config);
        }

        bool DoDecodeChecked() override {
            return true;
        }

        void RunDecodeChecked(
                const DecodeConfiguration & config) override {
            RunDecodeInternal<true>(config);
        }
    };

}
