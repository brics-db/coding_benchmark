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
 * File:   AN_simd_inv.tcc
 * Author: Till Kolditz <till.kolditz@gmail.com>
 *
 * Created on 08-01-2018
 * Renamed in 19-03-2018 16:06
 */

#pragma once

#ifndef AN_SIMD
#error "Clients must not include this file directly, but file <AN/AN_sse42.hpp>!"
#endif

#include <AN/AN_simd.tcc>

using namespace coding_benchmark::simd;

namespace coding_benchmark {

    template<typename DATARAW, typename DATAENC, typename VEC, size_t UNROLL>
    struct AN_simd_inv :
            public AN_simd<DATARAW, DATAENC, VEC, UNROLL> {

        typedef AN_simd<DATARAW, DATAENC, VEC, UNROLL> BASE;
        typedef mm<VEC, DATAENC> mmEnc;
        typedef mm_op<VEC, DATAENC, std::less_equal> mmEncLE;
        typedef mm_op<VEC, DATAENC, std::greater_equal> mmEncGE;

        using BASE::AN_simd;

        virtual ~AN_simd_inv() {
        }

        virtual bool DoCheck() override {
            return true;
        }

        virtual void RunCheck(
                const CheckConfiguration & config) override {
            for (size_t iteration = 0; iteration < config.numIterations; ++iteration) {
                _ReadWriteBarrier();
                auto inV = config.target.template begin<VEC>();
                const auto inVend = this->template ComputeEnd<DATAENC>(inV, config);
                const constexpr DATAENC __attribute__((unused)) dMin = std::numeric_limits<DATARAW>::min();
                const constexpr DATAENC dMax = std::numeric_limits<DATARAW>::max();
                VEC mmDMin = mm<VEC, DATAENC>::set1(dMin);
                VEC mmDMax = mm<VEC, DATAENC>::set1(dMax);
                VEC mmAInv = mm<VEC, DATAENC>::set1(this->A_INV);
                while (inV <= (inVend - UNROLL)) {
                    // let the compiler unroll the loop
                    for (size_t k = 0; k < UNROLL; ++k) {
                        auto mmInDec = mm_op<VEC, DATAENC, mul>::compute(*inV, mmAInv);
                        if ((mmEncLE::cmp_mask(mmInDec, mmDMax) == mmEnc::FULL_MASK) && (std::is_unsigned_v<DATARAW> || (mmEncGE::cmp_mask(mmInDec, mmDMin) == mmEnc::FULL_MASK))) {
                            ++inV;
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATAENC*>(inV) - config.target.template begin<DATAENC>(), iteration);
                        }
                    }
                }
                // here follows the non-unrolled remainder
                while (inV <= (inVend - 1)) {
                    auto mmInDec = mm_op<VEC, DATAENC, mul>::compute(*inV, mmAInv);
                    if ((mmEncLE::cmp_mask(mmInDec, mmDMax) == mmEnc::FULL_MASK) && (std::is_unsigned_v<DATARAW> || (mmEncGE::cmp_mask(mmInDec, mmDMin) == mmEnc::FULL_MASK))) {
                        ++inV;
                    } else {
                        throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATAENC*>(inV) - config.target.template begin<DATAENC>(), iteration);
                    }
                }
                if (inV < inVend) {
                    auto inS = reinterpret_cast<DATAENC*>(inV);
                    const auto inSend = reinterpret_cast<DATAENC* const >(inVend);
                    while (inS < inSend) {
                        auto data = *inS * this->A_INV;
                        if ((data <= dMax) && (std::is_unsigned_v<DATARAW> || (data >= dMin))) {
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
            AN_simd_inv & test;
            const ArithmeticConfiguration & config;
            const size_t iteration;
            ArithmetorChecked(
                    AN_simd_inv & test,
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
                const constexpr DATAENC __attribute__((unused)) dMin = std::numeric_limits<int16_t>::min();
                const constexpr DATAENC dMax = std::numeric_limits<int16_t>::max();
                VEC mmDMin __attribute__((unused)) = mm<VEC, DATAENC>::set1(dMin); // we assume 16-bit input data
                VEC mmDMax = mm<VEC, DATAENC>::set1(dMax); // we assume 16-bit input data
                VEC mmAInv = mm<VEC, DATAENC>::set1(test.A_INV);
                auto outV = config.target.template begin<VEC>();
                DATAENC operand = static_cast<DATAENC>(config.operand);
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
                        auto mmInDec = mm_op<VEC, DATAENC, mul>::compute(mmIn, mmAInv);
                        if ((mmEncLE::cmp_mask(mmInDec, mmDMax) == mmEnc::FULL_MASK) && (std::is_unsigned_v<DATARAW> || (mmEncGE::cmp_mask(mmInDec, mmDMin) == mmEnc::FULL_MASK))) {
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
                    auto mmInDec = mm_op<VEC, DATAENC, mul>::compute(mmIn, mmAInv);
                    if ((mmEncLE::cmp_mask(mmInDec, mmDMax) == mmEnc::FULL_MASK) && (std::is_unsigned_v<DATARAW> || (mmEncGE::cmp_mask(mmInDec, mmDMin) == mmEnc::FULL_MASK))) {
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
                        auto dec = *inS * test.A_INV;
                        if ((dec <= dMax) && (std::is_unsigned_v<DATARAW> || (dec >= dMin))) {
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
            AN_simd_inv & test;
            const AggregateConfiguration & config;
            size_t iteration;
            AggregatorChecked(
                    AN_simd_inv & test,
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
                const constexpr DATAENC __attribute__((unused)) dMin = std::numeric_limits<DATARAW>::min();
                const constexpr DATAENC dMax = std::numeric_limits<DATARAW>::max();
                VEC __attribute__((unused)) mmDMin = mm<VEC, DATAENC>::set1(dMin);
                VEC mmDMax = mm<VEC, DATAENC>::set1(dMax);
                VEC mmAInv = mm<VEC, DATAENC>::set1(test.A_INV);
                auto mmValue = funcInitVector();
                while (inV <= (inVend - UNROLL)) {
                    for (size_t k = 0; k < UNROLL; ++k) {
                        auto mmIn = *inV++;
                        auto mmInDec = mm_op<VEC, DATAENC, mul>::compute(mmIn, mmAInv);
                        if ((mmEncLE::cmp_mask(mmInDec, mmDMax) == mmEnc::FULL_MASK) && (std::is_unsigned_v<DATARAW> || (mmEncGE::cmp_mask(mmInDec, mmDMin) == mmEnc::FULL_MASK))) {
                            mmValue = funcKernelVector(mmValue, mmIn);
                        } else {
                            throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATAENC*>(inV - 1) - config.source.template begin<DATAENC>(), iteration);
                        }
                    }
                }
                while (inV <= (inVend - 1)) {
                    auto mmIn = *inV++;
                    auto mmInDec = mm_op<VEC, DATAENC, mul>::compute(mmIn, mmAInv);
                    if ((mmEncLE::cmp_mask(mmInDec, mmDMax) == mmEnc::FULL_MASK) && (std::is_unsigned_v<DATARAW> || (mmEncGE::cmp_mask(mmInDec, mmDMin) == mmEnc::FULL_MASK))) {
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
                        auto dec = *inS * test.A_INV;
                        if ((dec <= dMax) && (std::is_unsigned_v<DATARAW> || (dec >= dMin))) {
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
                impl<larger_t>([] {return mm<VEC, larger_t>::set1(0);}, [](VEC mmSum, VEC mmTmp) {
                    auto mmLo = mm<VEC, DATAENC>::cvt_larger_lo(mmTmp);
                    mmLo = mm_op<VEC, larger_t, add>::compute(mmSum, mmLo);
                    auto mmHi = mm<VEC, DATAENC>::cvt_larger_hi(mmTmp);
                    return mm_op<VEC, larger_t, add>::compute(mmLo, mmHi);
                }, [](VEC mmSum) {return mm<VEC, larger_t>::sum(mmSum);}, [](larger_t sum, DATAENC tmp) {return sum + tmp;}, [](larger_t sum, size_t numValues) {return sum;});
            }
            void operator()(
                    AggregateConfiguration::Min) {
                impl<DATAENC>([] {return mm<VEC, DATAENC>::set1(std::numeric_limits<DATAENC>::max());}, [](VEC mmMin, VEC mmTmp) {return mm<VEC, DATAENC>::min(mmMin, mmTmp);},
                        [](VEC mmMin) {return mm<VEC, DATAENC>::min(mmMin);}, [](DATAENC min, DATAENC tmp) {return min < tmp ? min : tmp;}, [](DATAENC min, size_t numValues) {return min;});
            }
            void operator()(
                    AggregateConfiguration::Max) {
                impl<DATAENC>([] {return mm<VEC, DATAENC>::set1(std::numeric_limits<DATAENC>::min());}, [](VEC mmMax, VEC mmTmp) {return mm<VEC, DATAENC>::max(mmMax, mmTmp);},
                        [](VEC mmMax) {return mm<VEC, DATAENC>::max(mmMax);}, [](DATAENC max, DATAENC tmp) {return max > tmp ? max : tmp;}, [](DATAENC max, size_t numValues) {return max;});
            }
            void operator()(
                    AggregateConfiguration::Avg) {
                impl<larger_t>([] {return mm<VEC, larger_t>::set1(0);}, [](VEC mmSum, VEC mmTmp) {
                    auto mmLo = mm<VEC, DATAENC>::cvt_larger_lo(mmTmp);
                    mmLo = mm_op<VEC, larger_t, add>::compute(mmSum, mmLo);
                    auto mmHi = mm<VEC, DATAENC>::cvt_larger_hi(mmTmp);
                    return mm_op<VEC, larger_t, add>::compute(mmLo, mmHi);
                }, [](VEC mmSum) {return mm<VEC, larger_t>::sum(mmSum);}, [](larger_t sum, DATAENC tmp) {return sum + tmp;},
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
                const auto inVend = this->template ComputeEnd<DATAENC>(inV, config);
                auto outS = config.target.template begin<DATARAW>();
                const constexpr DATAENC __attribute__((unused)) dMin = std::numeric_limits<DATARAW>::min();
                const constexpr DATAENC __attribute__((unused)) dMax = std::numeric_limits<DATARAW>::max();
                VEC __attribute__((unused)) mmDMin = mm<VEC, DATAENC>::set1(dMin);
                VEC __attribute__((unused)) mmDMax = mm<VEC, DATAENC>::set1(dMax);
                VEC mmAInv = mm<VEC, DATAENC>::set1(this->A_INV);
                while (inV <= (inVend - UNROLL)) {
                    // let the compiler unroll the loop
                    for (size_t unroll = 0; unroll < UNROLL; ++unroll) {
                        auto mmInDec = mm_op<VEC, DATAENC, mul>::compute(*inV++, mmAInv);
                        if ((!check) || ((mmEncLE::cmp_mask(mmInDec, mmDMax) == mmEnc::FULL_MASK) && (std::is_unsigned_v<DATARAW> || (mmEncGE::cmp_mask(mmInDec, mmDMin) == mmEnc::FULL_MASK)))) {
                            writeout<DATARAW, DATAENC, VEC>(mmInDec, outS);
                            outS += (sizeof(VEC) / sizeof(DATAENC));
                        } else if (check) {
                            throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATAENC*>(inV - 1) - config.source.template begin<DATAENC>(), iteration);
                        }
                    }
                }
                // remaining numbers
                while (inV <= (inVend - 1)) {
                    auto mmInDec = mm_op<VEC, DATAENC, mul>::compute(*inV++, mmAInv);
                    if ((!check) || ((mmEncLE::cmp_mask(mmInDec, mmDMax) == mmEnc::FULL_MASK) && (std::is_unsigned_v<DATARAW> || (mmEncGE::cmp_mask(mmInDec, mmDMin) == mmEnc::FULL_MASK)))) {
                        writeout<DATARAW, DATAENC, VEC>(mmInDec, outS);
                        outS += (sizeof(VEC) / sizeof(DATAENC));
                    } else if (check) {
                        throw ErrorInfo(__FILE__, __LINE__, reinterpret_cast<DATAENC*>(inV - 1) - config.source.template begin<DATAENC>(), iteration);
                    }
                }
                if (inV < inVend) {
                    auto inS = reinterpret_cast<DATAENC*>(inV);
                    const auto in32end = reinterpret_cast<DATAENC* const >(inVend);
                    while (inS < in32end) {
                        auto dec = *inS++ * this->A_INV;
                        if ((!check) || ((dec <= dMax) && (std::is_unsigned_v<DATARAW> || (dec >= dMin)))) {
                            *outS++ = dec;
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
