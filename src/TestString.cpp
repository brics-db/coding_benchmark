// Copyright 2017 Till Kolditz
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
 * TestString.cpp
 *
 *  Created on: 24.03.2018
 *      Author: Till Kolditz - Till.Kolditz@gmail.com
 */

#include <iostream>
#include <iomanip>
#include <cstring>
#include <cstdlib>
#include <optional>
#include <random>
#include <limits>
#include <type_traits>

#include <immintrin.h>

#include <Util/Stopwatch.hpp>
#include <Util/AlignedBlock.hpp>
#include <Util/Euclidean.hpp>

const constexpr long charsPerMM128 = 16; // asm ("rdx") does not work
const constexpr long shortsPerMM128 = 8; // asm ("rdx") does not work
const constexpr long intsPerMM128 = 4; // asm ("rdx") does not work

/* https://www.strchr.com/strcmp_and_strlen_using_sse_4.2 */
int _mm_strcmp(
        const char* cs,
        const char* ct,
        const size_t __attribute__((unused)) NUM) {
    const long diff = cs - ct;
    ct -= charsPerMM128;

    // Force GCC to use a register for nextbytes? Makes the code much worse! (adds LEA)
    // __asm__ __volatile__( "mov $16, %0" : "=r"(nextbytes) );

    loop:
    // could align it ASMVOLATILE( ".align 16\n" : : : "memory" );
    __m128i ct16chars = _mm_lddqu_si128((const __m128i *) (ct += charsPerMM128));
    int offset = _mm_cmpistri(ct16chars, *(const __m128i * ) (ct + diff), _SIDD_SBYTE_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_NEGATIVE_POLARITY);
    __asm__ __volatile__ goto( "ja %l[loop] \n jc %l[not_equal]" : : : "memory" : loop, not_equal );
#ifndef NDEBUG
    if ((((ct + charsPerMM128) + diff) - NUM) != cs) {
        std::cerr << "\tpos=" << (((ct + charsPerMM128) + diff) - cs) << " != " << NUM << std::endl;
    }
#endif
    return 0;

    not_equal: return ct[diff + offset] - ct[offset];
}
int _mm_strcmp(
        const unsigned char* cs,
        const unsigned char* ct,
        const size_t __attribute__((unused)) NUM) {
    const long diff = cs - ct;
    ct -= charsPerMM128;

    // Force GCC to use a register for nextbytes? Makes the code much worse! (adds LEA)
    // __asm__ __volatile__( "mov $16, %0" : "=r"(charsPerMM128) );

    loop:
    // could align it ASMVOLATILE( ".align 16\n" : : : "memory" );
    __m128i ct16chars = _mm_lddqu_si128((const __m128i *) (ct += charsPerMM128));
    int offset = _mm_cmpistri(ct16chars, *(const __m128i * ) (ct + diff), _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_NEGATIVE_POLARITY);
    __asm__ __volatile__ goto( "ja %l[loop] \n jc %l[not_equal]" : : : "memory" : loop, not_equal );
#ifndef NDEBUG
    if ((((ct + charsPerMM128) + diff) - NUM) != cs) {
        std::cerr << "\tpos=" << (((ct + charsPerMM128) + diff) - cs) << " != " << NUM << std::endl;
    }
#endif
    return 0;

    not_equal: return ct[diff + offset] - ct[offset];
}
int _mm_strcmp(
        const short* cs,
        const short* ct,
        const size_t __attribute__((unused)) NUM) {
    const long diff = cs - ct;
    ct -= shortsPerMM128;

    // Force GCC to use a register for nextbytes? Makes the code much worse! (adds LEA)
    // __asm__ __volatile__( "mov $16, %0" : "=r"(shortsPerMM128) );

    loop:
    // could align it ASMVOLATILE( ".align 16\n" : : : "memory" );
    __m128i ct16chars = _mm_lddqu_si128((const __m128i *) (ct += shortsPerMM128));
    int offset = _mm_cmpistri(ct16chars, *(const __m128i * ) (ct + diff), _SIDD_SWORD_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_NEGATIVE_POLARITY);
    __asm__ __volatile__ goto( "ja %l[loop] \n jc %l[not_equal]" : : : "memory" : loop, not_equal );
#ifndef NDEBUG
    if ((((ct + shortsPerMM128) + diff) - NUM) != cs) {
        std::cerr << "\tpos=" << (((ct + charsPerMM128) + diff) - cs) << " != " << NUM << std::endl;
    }
#endif
    return 0;

    not_equal: return ct[diff + offset] - ct[offset];
}
int _mm_strcmp(
        const unsigned short* cs,
        const unsigned short* ct,
        const size_t __attribute__((unused)) NUM) {
    const long diff = cs - ct;
    ct -= shortsPerMM128;

    // Force GCC to use a register for nextbytes? Makes the code much worse! (adds LEA)
    // __asm__ __volatile__( "mov $16, %0" : "=r"(nextbytes) );

    loop:
    // could align it ASMVOLATILE( ".align 16\n" : : : "memory" );
    __m128i ct16chars = _mm_lddqu_si128((const __m128i *) (ct += shortsPerMM128));
    int offset = _mm_cmpistri(ct16chars, *(const __m128i * ) (ct + diff), _SIDD_UWORD_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_NEGATIVE_POLARITY);
    __asm__ __volatile__ goto( "ja %l[loop] \n jc %l[not_equal]" : : : "memory" : loop, not_equal );
#ifndef NDEBUG
    if (((ct + shortsPerMM128 + diff) - NUM) != cs) {
        std::cerr << "\tpos=" << ((ct + charsPerMM128 + diff) - cs) << " != " << NUM << std::endl;
    }
#endif
    return 0;

    not_equal: return ct[diff + offset] - ct[offset];
}

template<typename T>
int strcmp2(
        const T *s1,
        const T *s2,
        const size_t __attribute__((unused)) NUM) {
    const T * ps1 = s1;
    const T * ps2 = s2;
    while (1) {
        int res = ((*ps1 == 0) || (*ps1 != *ps2));
        if (__builtin_expect((res), 0)) {
            break;
        }
        ++ps1;
        ++ps2;
    }
#ifndef NDEBUG
    if (static_cast<size_t>(ps1 - s1) != (NUM - 1)) { // the loop does LATE increment
        std::cerr << "\tpos1=" << (ps1 - s1) << " != " << NUM << std::endl;
    }
    if (static_cast<size_t>(ps2 - s2) != (NUM - 1)) { // the loop does LATE increment
        std::cerr << "\tpos2=" << (ps2 - s2) << " != " << NUM << std::endl;
    }
#endif
    return (*s1 - *s2);
}

int strcmp2_AN(
        const unsigned short *s1,
        const unsigned short *s2,
        const unsigned short A,
        const size_t __attribute__((unused)) NUM) {
    const unsigned short Ainv = static_cast<unsigned short>(ext_euclidean(uint32_t(A), 16));
    const unsigned short * ps1 = s1;
    const unsigned short * ps2 = s2;
    while (1) {
        int s1dec = static_cast<unsigned short>(*ps1 * Ainv);
        int s2dec = static_cast<unsigned short>(*ps2 * Ainv);
        if (s1dec > std::numeric_limits<unsigned char>::max()) {
            std::cerr << "@" << (ps1 - s1) << ": s1dec=" << s1dec << '\n';
            throw std::runtime_error("first string is corrupt");
        }
        if (s2dec > std::numeric_limits<unsigned char>::max()) {
            std::cerr << "@" << (ps2 - s2) << ": s2dec=" << s2dec << '\n';
            throw std::runtime_error("second string is corrupt");
        }
        int res = ((s1dec == 0) || (s1dec != s2dec));
        if (__builtin_expect((res), 0)) {
            break;
        }
        ++ps1;
        ++ps2;
    }
#ifndef NDEBUG
    if (static_cast<size_t>(ps1 - s1) != (NUM - 1)) { // the loop does LATE increment
        std::cerr << "\tpos1=" << (ps1 - s1) << " != " << NUM << std::endl;
    }
    if (static_cast<size_t>(ps2 - s2) != (NUM - 1)) { // the loop does LATE increment
        std::cerr << "\tpos2=" << (ps2 - s2) << " != " << NUM << std::endl;
    }
#endif
    return (*ps1 - *ps2);
}

int strcmp2_AN_accu(
        const unsigned short *s1,
        const unsigned short *s2,
        const unsigned short A,
        const size_t __attribute__((unused)) NUM) {
    const unsigned short Ainv = static_cast<unsigned short>(ext_euclidean(uint32_t(A), 16));
    const size_t accuAinv = Private::extractor<size_t, uint128_t>::doIt(ext_euclidean(uint128_t(A), 64));
    const unsigned short * ps1 = s1;
    const unsigned short * ps2 = s2;
    size_t accu1 = 0;
    size_t accu2 = 0;
    while (1) {
#ifndef NDEBUG
        if ((accu1 + *ps1) < accu1) {
            std::cerr << "overflow: accu1=" << std::hex << std::showbase << accu1 << " *ps1=" << *ps1 << std::dec << std::noshowbase << std::endl;
        }
#endif
        accu1 += *ps1;
        unsigned short s1dec = static_cast<unsigned short>(*ps1 * Ainv);
#ifndef NDEBUG
        if ((accu2 + *ps2) < accu2) {
            std::cerr << "overflow: accu2=" << std::hex << std::showbase << accu1 << " *ps2=" << *ps1 << std::dec << std::noshowbase << std::endl;
        }
#endif
        accu2 += *ps2;
        unsigned short s2dec = static_cast<unsigned short>(*ps2 * Ainv);
        int res = ((s1dec == 0) || (s1dec != s2dec));
        if (__builtin_expect((res), 0)) {
            break;
        }
        ++ps1;
        ++ps2;
    }
    // we assume at most a 10-bit A for the string characters here, so we can at most accumulate up to 2^54.
    const constexpr size_t MAX = (1ull << 54) - 1ull;
    if ((accu1 * accuAinv) > MAX) {
#ifndef NDEBUG
        std::cerr << std::hex << std::showbase << "accuAinv=" << accuAinv << " A*inv=" << (size_t(A) * accuAinv) << " accu1=" << accu1 << " dec1=" << (accu1 * accuAinv) << " accu2=" << accu2
                << " dec2=" << (accu2 * accuAinv) << std::dec << std::noshowbase << '\n';
#endif
        throw std::runtime_error("first string is corrupt");
    }
    if ((accu2 * accuAinv) > MAX) {
#ifndef NDEBUG
        std::cerr << std::hex << std::showbase << "accuAinv=" << accuAinv << " A*inv=" << (size_t(A) * accuAinv) << " accu1=" << accu1 << " dec1=" << (accu1 * accuAinv) << " accu2=" << accu2
                << " dec2=" << (accu2 * accuAinv) << std::dec << std::noshowbase << '\n';
#endif
        throw std::runtime_error("second string is corrupt");
    }
#ifndef NDEBUG
    if (static_cast<size_t>(ps1 - s1) != (NUM - 1)) { // the loop does LATE increment
        std::cerr << "\tpos1=" << (ps1 - s1) << " != " << NUM << std::endl;
    }
    if (static_cast<size_t>(ps2 - s2) != (NUM - 1)) { // the loop does LATE increment
        std::cerr << "\tpos2=" << (ps2 - s2) << " != " << NUM << std::endl;
    }
#endif
    return (*ps1 - *ps2);
}

int strcmp2_AN(
        const unsigned int *s1,
        const unsigned int *s2,
        const unsigned short A,
        const size_t __attribute__((unused)) NUM) {
    const unsigned int Ainv = static_cast<unsigned int>(ext_euclidean(uint64_t(A), 32));
    const unsigned int * ps1 = s1;
    const unsigned int * ps2 = s2;
    while (1) {
        auto s1dec = static_cast<unsigned int>(*ps1 * Ainv);
        auto s2dec = static_cast<unsigned int>(*ps2 * Ainv);
        if (s1dec > std::numeric_limits<unsigned short>::max()) {
            std::cerr << "@" << (ps1 - s1) << ": s1dec=" << s1dec << '\n';
            throw std::runtime_error("first string is corrupt");
        }
        if (s2dec > std::numeric_limits<unsigned short>::max()) {
            std::cerr << "@" << (ps2 - s2) << ": s2dec=" << s2dec << '\n';
            throw std::runtime_error("second string is corrupt");
        }
        int res = ((s1dec == 0) || (s1dec != s2dec));
        if (__builtin_expect((res), 0)) {
            break;
        }
        ++ps1;
        ++ps2;
    }
#ifndef NDEBUG
    if (static_cast<size_t>(ps1 - s1) != (NUM - 1)) { // the loop does LATE increment
        std::cerr << "\tpos1=" << (ps1 - s1) << " != " << NUM << std::endl;
    }
    if (static_cast<size_t>(ps2 - s2) != (NUM - 1)) { // the loop does LATE increment
        std::cerr << "\tpos2=" << (ps2 - s2) << " != " << NUM << std::endl;
    }
#endif
    return (*ps1 - *ps2);
}

int strcmp2_AN_accu(
        const unsigned int *s1,
        const unsigned int *s2,
        const unsigned short A,
        const size_t __attribute__((unused)) NUM) {
    const unsigned int Ainv = static_cast<unsigned short>(ext_euclidean(uint64_t(A), 32));
    const size_t accuAinv = Private::extractor<size_t, uint128_t>::doIt(ext_euclidean(uint128_t(A), 64));
    const unsigned int * ps1 = s1;
    const unsigned int * ps2 = s2;
    size_t accu1 = 0;
    size_t accu2 = 0;
    while (1) {
        accu1 += *ps1;
        auto s1dec = static_cast<unsigned int>(*ps1 * Ainv);
#ifndef NDEBUG
        if ((accu2 + *ps2) < accu2) {
            std::cerr << "overflow: accu2=" << std::hex << std::showbase << accu1 << " *ps2=" << *ps1 << std::dec << std::noshowbase << std::endl;
        }
#endif
        accu2 += *ps2;
        auto s2dec = static_cast<unsigned int>(*ps2 * Ainv);
#ifndef NDEBUG
        if ((accu2 + *ps2) < accu2) {
            std::cerr << "overflow: accu2=" << std::hex << std::showbase << accu1 << " *ps2=" << *ps1 << std::dec << std::noshowbase << std::endl;
        }
#endif
        int res = ((s1dec == 0) || (s1dec != s2dec));
        if (__builtin_expect((res), 0)) {
            break;
        }
        ++ps1;
        ++ps2;
    }
    // we assume at most a 10-bit A for the string characters here, so we can at most accumulate up to 2^54.
    const constexpr size_t MAX = (1ull << 54) - 1ull;
    if ((accu1 * accuAinv) > MAX) {
        std::cerr << std::hex << "accuAinv=0x" << accuAinv << " accu1=0x" << accu1 << " accu2=0x" << accu2 << std::dec << '\n';
        throw std::runtime_error("first string is corrupt");
    }
    if ((accu2 * accuAinv) > MAX) {
        std::cerr << std::hex << "accuAinv=0x" << accuAinv << " accu1=0x" << accu1 << " accu2=0x" << accu2 << std::dec << '\n';
        throw std::runtime_error("second string is corrupt");
    }
#ifndef NDEBUG
    if (static_cast<size_t>(ps1 - s1) != (NUM - 1)) { // the loop does LATE increment
        std::cerr << "\tpos1=" << (ps1 - s1) << " != " << NUM << std::endl;
    }
    if (static_cast<size_t>(ps2 - s2) != (NUM - 1)) { // the loop does LATE increment
        std::cerr << "\tpos2=" << (ps2 - s2) << " != " << NUM << std::endl;
    }
#endif
    return (*ps1 - *ps2);
}

__m128i add_test_overflow(
        __m128i a,
        __m128i b) {
    auto res = _mm_add_epi64(a, b);
    if (_mm_extract_epi64(res, 0) < _mm_extract_epi64(a, 0)) {
        std::cerr << std::showbase << std::hex << "#\t0: " << _mm_extract_epi64(a, 0) << '+' << _mm_extract_epi64(b, 0) << '=' << _mm_extract_epi64(res, 0) << '\n' << std::noshowbase << std::dec;
    }
    if (_mm_extract_epi64(res, 1) < _mm_extract_epi64(a, 1)) {
        std::cerr << std::showbase << std::hex << "#\t1: " << _mm_extract_epi64(a, 1) << '+' << _mm_extract_epi64(b, 1) << '=' << _mm_extract_epi64(res, 1) << '\n' << std::noshowbase << std::dec;
    }
    return res;
}

int _mm_strcmp_AN(
        const unsigned short* s1,
        const unsigned short* s2,
        const unsigned short A,
        const size_t __attribute__((unused)) NUM) {
    const unsigned short Ainv = static_cast<unsigned short>(ext_euclidean(uint32_t(A), 16));
    __m128i mmAinv = _mm_set1_epi16(Ainv);
    __m128i mmMax = _mm_set1_epi16(std::numeric_limits<unsigned char>::max());
#ifndef NDEBUG
    const unsigned short* const s2Org = s2;
#endif
    const long diff = s1 - s2;
    s2 -= shortsPerMM128;

    // Force GCC to use a register for nextbytes? Makes the code much worse! (adds LEA)
    // __asm__ __volatile__( "mov $16, %0" : "=r"(nextbytes) );

    loopAN:
    // could align it ASMVOLATILE( ".align 16\n" : : : "memory" );
    __m128i ct16chars1 = _mm_lddqu_si128((const __m128i *) (s2 += shortsPerMM128));
    if (_mm_movemask_epi8(_mm_cmpgt_epi16(_mm_mullo_epi16(ct16chars1, mmAinv), mmMax))) {
        throw std::runtime_error("first string is corrupt");
    }
    __m128i cs16chars1 = _mm_lddqu_si128((const __m128i *) (s2 + diff));
    if (_mm_movemask_epi8(_mm_cmpgt_epi16(_mm_mullo_epi16(cs16chars1, mmAinv), mmMax))) {
        throw std::runtime_error("first string is corrupt");
    }
    int offset = _mm_cmpistri(ct16chars1, cs16chars1, _SIDD_UWORD_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_NEGATIVE_POLARITY);
    __asm__ __volatile__ goto( "ja %l[loopAN] \n jc %l[not_equalAN]" : : : "memory" : loopAN, not_equalAN );
#ifndef NDEBUG
    if (static_cast<size_t>((s2 + shortsPerMM128) - s2Org) != NUM) {
        std::cerr << "\tpos=" << ((s2 + shortsPerMM128) - s2Org) << " != " << NUM << std::endl;
    }
#endif
    return 0;

    not_equalAN: return s2[diff + offset] - s2[offset];
}

int _mm_strcmp_AN_accu(
        const unsigned short* s1,
        const unsigned short* s2,
        const unsigned short A,
        const size_t __attribute__((unused)) NUM) {
    const size_t accuAinv = Private::extractor<size_t, uint128_t>::doIt(ext_euclidean(uint128_t(A), 64));
    const constexpr size_t maxAccu = (1ull << 54) - 1ull; // we assume at most an 8-bit A for the string characters here, so we can at most accumulate up to 2^56.
    __m128i mmAccu11 = _mm_set1_epi64x(0);
    __m128i mmAccu12 = _mm_set1_epi64x(0);
    __m128i mmAccu13 = _mm_set1_epi64x(0);
    __m128i mmAccu14 = _mm_set1_epi64x(0);
    __m128i mmAccu21 = _mm_set1_epi64x(0);
    __m128i mmAccu22 = _mm_set1_epi64x(0);
    __m128i mmAccu23 = _mm_set1_epi64x(0);
    __m128i mmAccu24 = _mm_set1_epi64x(0);
#ifndef NDEBUG
    const unsigned short* const s2Org = s2;
#endif
    const long diff = s1 - s2;
    s2 -= shortsPerMM128;

    // Force GCC to use a register for nextbytes? Makes the code much worse! (adds LEA)
    // __asm__ __volatile__( "mov $16, %0" : "=r"(nextbytes) );

    loopANaccu:
    // could align it ASMVOLATILE( ".align 16\n" : : : "memory" );
    __m128i ct16chars = _mm_lddqu_si128((const __m128i *) (s2 += shortsPerMM128));
    mmAccu11 = _mm_add_epi64(mmAccu11, _mm_cvtepu16_epi64(ct16chars));
    mmAccu12 = _mm_add_epi64(mmAccu12, _mm_cvtepu16_epi64(_mm_srli_si128(ct16chars, 4)));
    mmAccu13 = _mm_add_epi64(mmAccu13, _mm_cvtepu16_epi64(_mm_srli_si128(ct16chars, 8)));
    mmAccu14 = _mm_add_epi64(mmAccu14, _mm_cvtepu16_epi64(_mm_srli_si128(ct16chars, 12)));
    __m128i cs16chars = _mm_lddqu_si128((const __m128i *) (s2 + diff));
    mmAccu21 = _mm_add_epi64(mmAccu21, _mm_cvtepu16_epi64(cs16chars));
    mmAccu22 = _mm_add_epi64(mmAccu22, _mm_cvtepu16_epi64(_mm_srli_si128(cs16chars, 4)));
    mmAccu23 = _mm_add_epi64(mmAccu23, _mm_cvtepu16_epi64(_mm_srli_si128(cs16chars, 8)));
    mmAccu24 = _mm_add_epi64(mmAccu24, _mm_cvtepu16_epi64(_mm_srli_si128(cs16chars, 12)));
    int offset = _mm_cmpistri(ct16chars, cs16chars, _SIDD_UWORD_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_NEGATIVE_POLARITY);
    __asm__ __volatile__ goto( "ja %l[loopANaccu] \n jc %l[not_equalANaccu]" : : : "memory" : loopANaccu, not_equalANaccu );

    {
        auto mmAccu = _mm_add_epi64(mmAccu11, _mm_add_epi64(mmAccu12, _mm_add_epi64(mmAccu13, mmAccu14)));
        if (((_mm_extract_epi64(mmAccu, 0) * accuAinv) > maxAccu) || ((_mm_extract_epi64(mmAccu, 1) * accuAinv) > maxAccu)) {
            throw std::runtime_error("first string is corrupt");
        }
        mmAccu = _mm_add_epi64(mmAccu21, _mm_add_epi64(mmAccu22, _mm_add_epi64(mmAccu23, mmAccu24)));
        if (((_mm_extract_epi64(mmAccu, 0) * accuAinv) > maxAccu) || ((_mm_extract_epi64(mmAccu, 1) * accuAinv) > maxAccu)) {
            throw std::runtime_error("second string is corrupt");
        }
#ifndef NDEBUG
        if (static_cast<size_t>((s2 + shortsPerMM128) - s2Org) != NUM) {
            std::cerr << "\tpos=" << ((s2 + shortsPerMM128) - s2Org) << " != " << NUM << std::endl;
        }
#endif
        return 0;
    }

    not_equalANaccu: {
        auto mmAccu = _mm_add_epi64(mmAccu11, _mm_add_epi64(mmAccu12, _mm_add_epi64(mmAccu13, mmAccu14)));
        if (((_mm_extract_epi64(mmAccu, 0) * accuAinv) > maxAccu) || ((_mm_extract_epi64(mmAccu, 1) * accuAinv) > maxAccu)) {
            throw std::runtime_error("first string is corrupt");
        }
        mmAccu = _mm_add_epi64(mmAccu21, _mm_add_epi64(mmAccu22, _mm_add_epi64(mmAccu23, mmAccu24)));
        if (((_mm_extract_epi64(mmAccu, 0) * accuAinv) > maxAccu) || ((_mm_extract_epi64(mmAccu, 1) * accuAinv) > maxAccu)) {
            throw std::runtime_error("second string is corrupt");
        }
        const unsigned short Ainv = static_cast<unsigned short>(ext_euclidean(uint32_t(A), 16));
        return (Ainv * s2[diff + offset]) - (Ainv * s2[offset]);
    }
}

int _mm_strcmp_AN(
        const unsigned int* i1,
        const unsigned int* i2,
        const unsigned short A,
        const size_t __attribute__((unused)) NUM) {
    const unsigned int Ainv = static_cast<unsigned int>(ext_euclidean(uint64_t(A), 32));
    __m128i mmAinv = _mm_set1_epi32(Ainv);
    __m128i mmMax = _mm_set1_epi32(std::numeric_limits<unsigned short>::max());
#ifndef NDEBUG
    const unsigned int* const i2Org = i2;
#endif
    const long diff = i1 - i2;
    i2 -= intsPerMM128;

    // Force GCC to use a register for nextbytes? Makes the code much worse! (adds LEA)
    // __asm__ __volatile__( "mov $16, %0" : "=r"(nextbytes) );

    loopAN:
    // could align it ASMVOLATILE( ".align 16\n" : : : "memory" );
    __m128i ct4ints = _mm_lddqu_si128((const __m128i *) (i2 += intsPerMM128));
    if (_mm_movemask_epi8(_mm_cmpgt_epi32(_mm_mullo_epi32(ct4ints, mmAinv), mmMax))) {
        throw std::runtime_error("first string is corrupt");
    }
    __m128i cs4ints = _mm_lddqu_si128((const __m128i *) (i2 + diff));
    if (_mm_movemask_epi8(_mm_cmpgt_epi32(_mm_mullo_epi32(cs4ints, mmAinv), mmMax))) {
        throw std::runtime_error("second string is corrupt");
    }
    int offset = _mm_cmpistri(ct4ints, cs4ints, _SIDD_UWORD_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_NEGATIVE_POLARITY);
    __asm__ __volatile__ goto( "ja %l[loopAN] \n jc %l[not_equalAN]" : : : "memory" : loopAN, not_equalAN );
#ifndef NDEBUG
    if (static_cast<size_t>((i2 + intsPerMM128) - i2Org) != NUM) {
        std::cerr << "\tpos=" << ((i2 + intsPerMM128) - i2Org) << " != " << NUM << std::endl;
    }
#endif
    return 0;

    not_equalAN: return i2[diff + offset] - i2[offset];
}

int _mm_strcmp_AN_accu(
        const unsigned int* i1,
        const unsigned int* i2,
        const unsigned short A,
        const size_t __attribute__((unused)) NUM) {
    const size_t accuAinv = Private::extractor<size_t, uint128_t>::doIt(ext_euclidean(uint128_t(A), 64));
    const constexpr size_t maxAccu = (1ull << 48) - 1ull; // we assume at most an 8-bit A for the string characters here, so we can at most accumulate up to 2^56.
    __m128i mmAccu11 = _mm_set1_epi64x(0);
    __m128i mmAccu12 = _mm_set1_epi64x(0);
    __m128i mmAccu21 = _mm_set1_epi64x(0);
    __m128i mmAccu22 = _mm_set1_epi64x(0);
#ifndef NDEBUG
    const unsigned int* const i2Org = i2;
#endif
    const long diff = i1 - i2;
    i2 -= intsPerMM128;

    // Force GCC to use a register for nextbytes? Makes the code much worse! (adds LEA)
    // __asm__ __volatile__( "mov $16, %0" : "=r"(nextbytes) );

    loopANaccu:
    // could align it ASMVOLATILE( ".align 16\n" : : : "memory" );
    __m128i ct4ints = _mm_lddqu_si128((const __m128i *) (i2 += intsPerMM128));
    mmAccu11 = _mm_add_epi64(mmAccu11, _mm_cvtepu32_epi64(ct4ints));
    mmAccu12 = _mm_add_epi64(mmAccu12, _mm_cvtepu32_epi64(_mm_srli_si128(ct4ints, 8)));
    __m128i cs4ints = _mm_lddqu_si128((const __m128i *) (i2 + diff));
    mmAccu21 = _mm_add_epi64(mmAccu21, _mm_cvtepu32_epi64(cs4ints));
    mmAccu22 = _mm_add_epi64(mmAccu22, _mm_cvtepu32_epi64(_mm_srli_si128(cs4ints, 8)));
    int offset = _mm_cmpistri(ct4ints, cs4ints, _SIDD_UWORD_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_NEGATIVE_POLARITY);
    __asm__ __volatile__ goto( "ja %l[loopANaccu] \n jc %l[not_equalANaccu]" : : : "memory" : loopANaccu, not_equalANaccu );

    {
        auto mmAccu = _mm_add_epi64(mmAccu11, mmAccu12);
        if (((_mm_extract_epi64(mmAccu, 0) * accuAinv) > maxAccu) || ((_mm_extract_epi64(mmAccu, 1) * accuAinv) > maxAccu)) {
            throw std::runtime_error("first string is corrupt");
        }
        mmAccu = _mm_add_epi64(mmAccu21, mmAccu22);
        if (((_mm_extract_epi64(mmAccu, 0) * accuAinv) > maxAccu) || ((_mm_extract_epi64(mmAccu, 1) * accuAinv) > maxAccu)) {
            throw std::runtime_error("second string is corrupt");
        }
#ifndef NDEBUG
        if (static_cast<size_t>((i2 + intsPerMM128) - i2Org) != NUM) {
            std::cerr << "\tpos=" << ((i2 + intsPerMM128) - i2Org) << " != " << NUM << std::endl;
        }
#endif
        return 0;
    }

    not_equalANaccu: {
        auto mmAccu = _mm_add_epi64(mmAccu11, mmAccu12);
        if (((_mm_extract_epi64(mmAccu, 0) * accuAinv) > maxAccu) || ((_mm_extract_epi64(mmAccu, 1) * accuAinv) > maxAccu)) {
            throw std::runtime_error("first string is corrupt");
        }
        const unsigned short Ainv = static_cast<unsigned short>(ext_euclidean(uint32_t(A), 16));
        return (Ainv * i2[diff + offset]) - (Ainv * i2[offset]);
    }
}

template<typename T>
T XOR(
        const T* t) {
    T res = 0;
    while (*t != 0) {
        res ^= *t++;
    }
    return res;
}

template<typename T>
__m128i _mm_XOR(
        const __m128i * mm,
        const size_t NUM) {
    __m128i mmXOR = _mm_setzero_si128();
    const __m128i * const mmEnd = mm + (NUM / (sizeof(__m128i) / sizeof(T)));
    while (mm <= (mmEnd - 1)) {
        mmXOR = _mm_xor_si128(mmXOR, _mm_lddqu_si128(mm++));
    }
    return mmXOR;
}

template<typename T>
int strcmp2_XOR(
        const T *s1,
        const T *s2,
        const T old_xor1,
        const T old_xor2,
        const size_t NUM) {
    T new_xor1 = T(0);
    T new_xor2 = T(0);
    const T * ps1 = s1;
    const T * ps2 = s2;
    while (1) {
        new_xor1 ^= *ps1;
        new_xor2 ^= *ps2;
        int res = ((*ps1 == 0) || (*ps1 != *ps2));
        if (__builtin_expect((res), 0)) {
            break;
        }
        ++ps1;
        ++ps2;
    }
    const T * const s1end = s1 + NUM;
    while (ps1 < s1end) {
        new_xor1 ^= *ps1++;
    }
    const T * const s2end = s2 + NUM;
    while (ps2 < s2end) {
        new_xor2 ^= *ps2++;
    }
    if (new_xor1 != old_xor1) {
        throw new std::runtime_error("old_xor1 != new_xor1");
    }
    if (new_xor2 != old_xor2) {
        throw new std::runtime_error("old_xor2 != new_xor2");
    }
#ifndef NDEBUG
    if (static_cast<size_t>(ps1 - s1) != NUM) {
        std::cerr << "\tpos1=" << (ps1 - s1) << " != " << NUM << std::endl;
    }
    if (static_cast<size_t>(ps2 - s2) != NUM) {
        std::cerr << "\tpos2=" << (ps2 - s2) << " != " << NUM << std::endl;
    }
#endif
    return (*ps1 - *ps2);
}

int _mm_strcmp_XOR(
        const char* cs,
        const char* ct,
        __m128i old_xor1,
        __m128i old_xor2,
        const size_t NUM) {
    __m128i mmXOR1 = _mm_setzero_si128();
    __m128i mmXOR2 = _mm_setzero_si128();
    const char* const ctEnd = ct + NUM;
#ifndef NDEBUG
    const char* const csOrg = cs;
    const char* const ctOrg = ct;
#endif
    const long diff = cs - ct;
    ct -= charsPerMM128;

    // Force GCC to use a register for nextbytes? Makes the code much worse! (adds LEA)
    // __asm__ __volatile__( "mov $16, %0" : "=r"(nextbytes) );

    loopXORchar:
    // could align it ASMVOLATILE( ".align 16\n" : : : "memory" );
    __m128i ct16chars = _mm_lddqu_si128((const __m128i *) (ct += charsPerMM128));
    mmXOR1 = _mm_xor_si128(mmXOR1, ct16chars);
    __m128i cs16chars = _mm_lddqu_si128((const __m128i *) (ct + diff));
    mmXOR2 = _mm_xor_si128(mmXOR2, cs16chars);
    int offset = _mm_cmpistri(ct16chars, cs16chars, _SIDD_SBYTE_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_NEGATIVE_POLARITY);
    __asm__ __volatile__ goto( "ja %l[loopXORchar] \n jc %l[not_equalXORchar]" : : : "memory" : loopXORchar, not_equalXORchar );
    if (_mm_movemask_epi8(_mm_cmpeq_epi8(mmXOR1, old_xor1)) == 0xFFFF) {
        if (_mm_movemask_epi8(_mm_cmpeq_epi8(mmXOR2, old_xor2)) == 0xFFFF) {
#ifndef NDEBUG
            if (static_cast<size_t>((ct + diff + charsPerMM128) - csOrg) != NUM) {
                std::cerr << "\tpos1=" << ((ct + diff + charsPerMM128) - csOrg) << " != " << NUM << std::endl;
            }
            if (static_cast<size_t>((ct + charsPerMM128) - ctOrg) != NUM) {
                std::cerr << "\tpos2=" << ((ct + charsPerMM128) - ctOrg) << " != " << NUM << std::endl;
            }
#endif
            return 0;
        } else {
            throw std::runtime_error("checksum 2 doesn't match!");
        }
    } else {
        throw std::runtime_error("checksum 1 doesn't match!");
    }

    not_equalXORchar:
    //
    do {
        __m128i ct16chars = _mm_lddqu_si128((const __m128i *) (ct += charsPerMM128));
        mmXOR1 = _mm_xor_si128(mmXOR1, ct16chars);
        __m128i cs16chars = _mm_lddqu_si128((const __m128i *) (ct + diff));
        mmXOR2 = _mm_xor_si128(mmXOR2, cs16chars);
    } while (ct < ctEnd);
#ifndef NDEBUG
    if (static_cast<size_t>((ct + diff + charsPerMM128) - csOrg) != NUM) {
        std::cerr << "\tpos1=" << ((ct + diff + charsPerMM128) - csOrg) << " != " << NUM << std::endl;
    }
    if (static_cast<size_t>((ct + charsPerMM128) - ctOrg) != NUM) {
        std::cerr << "\tpos2=" << ((ct + charsPerMM128) - ctOrg) << " != " << NUM << std::endl;
    }
#endif
    if (_mm_movemask_epi8(_mm_cmpeq_epi8(mmXOR1, old_xor1))) {
        if (_mm_movemask_epi8(_mm_cmpeq_epi8(mmXOR2, old_xor2))) {
            return ct[diff + offset] - ct[offset];
        } else {
            throw std::runtime_error("checksum 2 doesn't match!");
        }
    } else {
        throw std::runtime_error("checksum 1 doesn't match!");
    }
}

int _mm_strcmp_XOR(
        const unsigned short* cs,
        const unsigned short* ct,
        __m128i old_xor1,
        __m128i old_xor2,
        const size_t NUM) {
    __m128i mmXOR1 = _mm_setzero_si128();
    __m128i mmXOR2 = _mm_setzero_si128();
    const unsigned short* const ctEnd = ct + NUM;
#ifndef NDEBUG
    const unsigned short* const csOrg = cs;
    const unsigned short* const ctOrg = ct;
#endif

    const long diff = cs - ct;
    ct -= shortsPerMM128;

    // Force GCC to use a register for nextbytes? Makes the code much worse! (adds LEA)
    // __asm__ __volatile__( "mov $16, %0" : "=r"(nextbytes) );

    loopXORchar:
    // could align it ASMVOLATILE( ".align 16\n" : : : "memory" );
    __m128i ct8shorts = _mm_lddqu_si128((const __m128i *) (ct += shortsPerMM128));
    mmXOR1 = _mm_xor_si128(mmXOR1, ct8shorts);
    __m128i cs8shorts = _mm_lddqu_si128((const __m128i *) (ct + diff));
    mmXOR2 = _mm_xor_si128(mmXOR2, cs8shorts);
    int offset = _mm_cmpistri(ct8shorts, cs8shorts, _SIDD_UWORD_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_NEGATIVE_POLARITY);
    __asm__ __volatile__ goto( "ja %l[loopXORchar] \n jc %l[not_equalXORchar]" : : : "memory" : loopXORchar, not_equalXORchar );
    if (_mm_movemask_epi8(_mm_cmpeq_epi8(mmXOR1, old_xor1)) == 0xFFFF) {
        if (_mm_movemask_epi8(_mm_cmpeq_epi8(mmXOR2, old_xor2)) == 0xFFFF) {
#ifndef NDEBUG
            if (static_cast<size_t>((ct + diff + shortsPerMM128) - csOrg) != NUM) {
                std::cerr << "\tpos1=" << ((ct + diff + shortsPerMM128) - csOrg) << " != " << NUM << std::endl;
            }
            if (static_cast<size_t>((ct + shortsPerMM128) - ctOrg) != NUM) {
                std::cerr << "\tpos2=" << ((ct + shortsPerMM128) - ctOrg) << " != " << NUM << std::endl;
            }
#endif
            return 0;
        } else {
            throw std::runtime_error("checksum 2 doesn't match!");
        }
    } else {
        throw std::runtime_error("checksum 1 doesn't match!");
    }

    not_equalXORchar:
    //
    do {
        __m128i ct8shorts = _mm_lddqu_si128((const __m128i *) (ct += charsPerMM128));
        mmXOR1 = _mm_xor_si128(mmXOR1, ct8shorts);
        __m128i cs8shorts = _mm_lddqu_si128((const __m128i *) (ct + diff));
        mmXOR2 = _mm_xor_si128(mmXOR2, cs8shorts);
    } while (ct < ctEnd);
#ifndef NDEBUG
    if (static_cast<size_t>((ct + diff + charsPerMM128) - csOrg) != NUM) {
        std::cerr << "\tpos1=" << ((ct + diff + charsPerMM128) - csOrg) << " != " << NUM << std::endl;
    }
    if (static_cast<size_t>((ct + charsPerMM128) - ctOrg) != NUM) {
        std::cerr << "\tpos2=" << ((ct + charsPerMM128) - ctOrg) << " != " << NUM << std::endl;
    }
#endif
    if (_mm_movemask_epi8(_mm_cmpeq_epi8(mmXOR1, old_xor1))) {
        if (_mm_movemask_epi8(_mm_cmpeq_epi8(mmXOR2, old_xor2))) {
            return ct[diff + offset] - ct[offset];
        } else {
            throw std::runtime_error("checksum 2 doesn't match!");
        }
    } else {
        throw std::runtime_error("checksum 1 doesn't match!");
    }
}

class DataGenerationConfiguration {

public:
    const std::optional<size_t> numEffectiveBitsData;
    const std::optional<size_t> numIneffectiveLSBsData;
    const std::optional<size_t> multiplicator;

    DataGenerationConfiguration();

    DataGenerationConfiguration(
            const size_t numEffectiveLSBs);

    DataGenerationConfiguration(
            const size_t numEffectiveLSBs,
            const size_t numIneffectiveLSBsData);

    DataGenerationConfiguration(
            const size_t numEffectiveLSBs,
            const size_t numIneffectiveLSBsData,
            const size_t multiplicator);
};

template<typename DATARAW>
void ResetBlock(
        const DataGenerationConfiguration & dataGenConfig,
        AlignedBlock buf) {
    DATARAW mask;
    if (std::is_signed_v<DATARAW>) {
        mask = std::numeric_limits<DATARAW>::min();
    } else {
        mask = std::numeric_limits<DATARAW>::max();
    }
    if (dataGenConfig.numEffectiveBitsData && dataGenConfig.numEffectiveBitsData.value() < (sizeof(DATARAW) * 8)) {
        mask = static_cast<DATARAW>((1ull << dataGenConfig.numEffectiveBitsData.value()) - 1ull);
    }
    if (dataGenConfig.numIneffectiveLSBsData) {
        if (dataGenConfig.numEffectiveBitsData && dataGenConfig.numEffectiveBitsData.value() <= dataGenConfig.numIneffectiveLSBsData) {
            throw std::runtime_error("dataGenConfig.numEffectiveBitsData <= dataGenConfig.numIneffectiveLSBsData makes no sense!");
        }
        mask &= ~static_cast<DATARAW>((1ull << dataGenConfig.numIneffectiveLSBsData.value()) - 1ull);
    }
    auto pInEnd = buf.template end<DATARAW>();
    DATARAW value = static_cast<DATARAW>(12783);
    DATARAW* pIn = buf.template begin<DATARAW>();
    while (pIn < pInEnd) {
        DATARAW x;
        do {
            x = mask & value;
            if (dataGenConfig.multiplicator) {
                x *= dataGenConfig.multiplicator.value();
            }
            value = value * static_cast<DATARAW>(7577) + static_cast<DATARAW>(10467);
        } while (x == 0);
        *pIn++ = x;
    }
}

template<typename T>
void test_buffer(
        const T * beg,
        const char * const name,
        const size_t NUM) {
    auto x = beg;
    while (*x) {
        ++x;
    }
    if (static_cast<size_t>(x - beg) != (NUM - 1)) {
        std::cerr << "#zero value found in " << name << " at position " << (x - beg) << " instead of expected " << (NUM - 1) << std::endl;
    }
}

int main(
        int argc,
        char** argv) {
    const constexpr size_t NUM = 64 * 1024 * 1024;
    const constexpr size_t NUM_BYTES_CHAR = NUM * sizeof(unsigned char);
    const constexpr size_t NUM_BYTES_SHORT = NUM * sizeof(unsigned short);
    const constexpr size_t NUM_BYTES_INT = NUM * sizeof(unsigned int);

    Stopwatch sw;
    int res;
    int64_t time_base;
    int64_t time;

    char* ptr_end;
    unsigned short A = 233;
    // force the compiler to not assume A a constant
    if (argc > 1) {
        A = strtol(argv[1], &ptr_end, 10);
    }

    {
        std::cout << "#Buffer with " << NUM << " 8-bit limbs --> " << NUM_BYTES_CHAR << " bytes for unprotected and XOR, " << NUM_BYTES_SHORT << " bytes for AN\n";
        AlignedBlock buf_char_1(NUM_BYTES_CHAR, 16);
        AlignedBlock buf_char_2(NUM_BYTES_CHAR, 16);
        DataGenerationConfiguration config(8);
        ResetBlock<unsigned char>(config, buf_char_1);
        *(buf_char_1.template end<char>() - 1) = 0;
        ResetBlock<unsigned char>(config, buf_char_2);
        *(buf_char_2.template end<char>() - 1) = 0;
        const char * uc1 = buf_char_1.template begin<const char>();
        const char * uc2 = buf_char_2.template begin<const char>();
        test_buffer(uc1, "buf_char_1", NUM);
        test_buffer(uc2, "buf_char_2", NUM);

        sw.Reset();
        res = std::strcmp(uc1, uc2);
        time = time_base = sw.Current();
        std::cout << "std::strcmp," << res << ',' << time << ',' << time_base << '\n';

        sw.Reset();
        res = strcmp2(uc1, uc2, NUM);
        time = sw.Current();
        std::cout << "naive," << res << ',' << time << ',' << time_base << '\n';

        char old_xor_char1 = XOR(uc1);
        char old_xor_char2 = XOR(uc2);
        sw.Reset();
        res = strcmp2_XOR(uc1, uc2, old_xor_char1, old_xor_char2, NUM);
        time = sw.Current();
        std::cout << "naive XOR," << res << ',' << time << ',' << time_base << '\n';

        sw.Reset();
        res = _mm_strcmp(uc1, uc2, NUM);
        time = sw.Current();
        std::cout << "Kankovski," << res << "," << time << ',' << time_base << '\n';

        __m128i old_xor_mm1 = _mm_XOR<unsigned char>(reinterpret_cast<const __m128i *>(uc1), NUM);
        __m128i old_xor_mm2 = _mm_XOR<unsigned char>(reinterpret_cast<const __m128i *>(uc2), NUM);
        sw.Reset();
        res = _mm_strcmp_XOR(uc1, uc2, old_xor_mm1, old_xor_mm2, NUM);
        time = sw.Current();
        std::cout << "K. XOR," << res << ',' << time << ',' << time_base << '\n';

        AlignedBlock buf_short_1(NUM_BYTES_SHORT, 16);
        AlignedBlock buf_short_2(NUM_BYTES_SHORT, 16);
        DataGenerationConfiguration config2(8, 0, A);
        ResetBlock<unsigned short>(config2, buf_short_1);
        *(buf_short_1.template end<unsigned short>() - 1) = 0;
        ResetBlock<unsigned short>(config2, buf_short_2);
        *(buf_short_2.template end<unsigned short>() - 1) = 0;
        const unsigned short * us1 = buf_short_1.template begin<const unsigned short>();
        const unsigned short * us2 = buf_short_2.template begin<const unsigned short>();
        test_buffer(us1, "buf_short_1", NUM);
        test_buffer(us2, "buf_short_2", NUM);

        sw.Reset();
        res = strcmp2_AN(us1, us2, A, NUM);
        time = sw.Current();
        std::cout << "naive AN," << res << ',' << time << ',' << time_base << '\n';

        sw.Reset();
        res = strcmp2_AN_accu(us1, us2, A, NUM);
        time = sw.Current();
        std::cout << "naive AN accu," << res << ',' << time << ',' << time_base << '\n';

        sw.Reset();
        res = _mm_strcmp_AN(us1, us2, A, NUM);
        time = sw.Current();
        std::cout << "K. AN," << res << ',' << time << ',' << time_base << '\n';

        sw.Reset();
        res = _mm_strcmp_AN_accu(us1, us2, A, NUM);
        time = sw.Current();
        std::cout << "K. AN accu," << res << ',' << time << ',' << time_base << '\n';
    }

    {
        std::cout << "\n\n#Buffer with " << NUM << " 16-bit limbs --> " << NUM_BYTES_SHORT << " bytes for unprotected and XOR, " << NUM_BYTES_INT << " bytes for AN\n";
        AlignedBlock buf_short_3(NUM_BYTES_SHORT, 16);
        AlignedBlock buf_short_4(NUM_BYTES_SHORT, 16);
        DataGenerationConfiguration config3(8); // make sure we have no zero-bytes
        ResetBlock<unsigned char>(config3, buf_short_3);
        ResetBlock<unsigned char>(config3, buf_short_4);
        const unsigned short * us1 = buf_short_3.template begin<const unsigned short>();
        const unsigned short * us2 = buf_short_4.template begin<const unsigned short>();
        *(buf_short_3.template end<unsigned char>() - 1) = 0;
        *(buf_short_4.template end<unsigned char>() - 1) = 0;
        test_buffer(reinterpret_cast<const char*>(us1), "buf_char_3", NUM_BYTES_SHORT);
        test_buffer(reinterpret_cast<const char*>(us2), "buf_char_4", NUM_BYTES_SHORT);
        *(buf_short_3.template end<unsigned short>() - 1) = 0;
        *(buf_short_4.template end<unsigned short>() - 1) = 0;
        test_buffer(us1, "buf_short_3", NUM_BYTES_CHAR);
        test_buffer(us2, "buf_short_4", NUM_BYTES_CHAR);

        sw.Reset();
        res = std::strcmp(reinterpret_cast<const char*>(us1), reinterpret_cast<const char*>(us2));
        time = time_base = sw.Current();
        std::cout << "std::strcmp," << res << ',' << time << ',' << time_base << '\n';

        sw.Reset();
        res = strcmp2(us1, us2, NUM);
        time = sw.Current();
        std::cout << "naive," << res << ',' << time << ',' << time_base << '\n';

        unsigned short old_xor_short1 = XOR(us1);
        unsigned short old_xor_short2 = XOR(us2);
        sw.Reset();
        res = strcmp2_XOR(us1, us2, old_xor_short1, old_xor_short2, NUM);
        time = sw.Current();
        std::cout << "naive XOR," << res << ',' << time << ',' << time_base << '\n';

        sw.Reset();
        res = _mm_strcmp(us1, us2, NUM);
        time = sw.Current();
        std::cout << "Kankovski," << res << ',' << time << ',' << time_base << '\n';

        auto old_xor_mm1 = _mm_XOR<unsigned short>(reinterpret_cast<const __m128i *>(us1), NUM);
        auto old_xor_mm2 = _mm_XOR<unsigned short>(reinterpret_cast<const __m128i *>(us2), NUM);
        sw.Reset();
        res = _mm_strcmp_XOR(us1, us2, old_xor_mm1, old_xor_mm2, NUM);
        time = sw.Current();
        std::cout << "K. XOR," << res << ',' << time << ',' << time_base << '\n';

        if (argc > 2) {
            A = strtol(argv[2], &ptr_end, 10);
        } else {
            A = 63877;
        }

        AlignedBlock buf_int_1(NUM_BYTES_INT, 16);
        AlignedBlock buf_int_2(NUM_BYTES_INT, 16);
        DataGenerationConfiguration config4(16, 1, A); // make at least 2-bit limbs so that the upper short is never null!
        ResetBlock<unsigned int>(config4, buf_int_1);
        *(buf_int_1.template end<unsigned int>() - 1) = 0;
        ResetBlock<unsigned int>(config4, buf_int_2);
        *(buf_int_2.template end<unsigned int>() - 1) = 0;
        const unsigned int * ui1 = buf_int_1.template begin<const unsigned int>();
        const unsigned int * ui2 = buf_int_2.template begin<const unsigned int>();
        test_buffer(ui1, "buf_int_1", NUM);
        test_buffer(ui2, "buf_int_2", NUM);

        sw.Reset();
        res = strcmp2_AN(ui1, ui2, A, NUM);
        time = sw.Current();
        std::cout << "naive AN," << res << ',' << time << ',' << time_base << '\n';

        sw.Reset();
        res = strcmp2_AN_accu(ui1, ui2, A, NUM);
        time = sw.Current();
        std::cout << "naive AN accu," << res << ',' << time << ',' << time_base << '\n';

        sw.Reset();
        res = _mm_strcmp_AN(ui1, ui2, A, NUM);
        time = sw.Current();
        std::cout << "K. AN," << res << ',' << time << ',' << time_base << '\n';

        sw.Reset();
        res = _mm_strcmp_AN_accu(ui1, ui2, A, NUM);
        time = sw.Current();
        std::cout << "K. AN accu," << res << ',' << time << ',' << time_base << '\n';
    }

    return 0;
}

DataGenerationConfiguration::DataGenerationConfiguration()
        : numEffectiveBitsData(std::nullopt),
          numIneffectiveLSBsData(std::nullopt),
          multiplicator(std::nullopt) {
}

DataGenerationConfiguration::DataGenerationConfiguration(
        const size_t numEffectiveLSBs)
        : numEffectiveBitsData(numEffectiveLSBs),
          numIneffectiveLSBsData(std::nullopt),
          multiplicator(std::nullopt) {
}

DataGenerationConfiguration::DataGenerationConfiguration(
        const size_t numEffectiveLSBs,
        const size_t numIneffectiveLSBsData)
        : numEffectiveBitsData(numEffectiveLSBs),
          numIneffectiveLSBsData(numIneffectiveLSBsData),
          multiplicator(std::nullopt) {
}

DataGenerationConfiguration::DataGenerationConfiguration(
        const size_t numEffectiveLSBs,
        const size_t numIneffectiveLSBsData,
        const size_t multiplicator)
        : numEffectiveBitsData(numEffectiveLSBs),
          numIneffectiveLSBsData(numIneffectiveLSBsData),
          multiplicator(multiplicator) {
}
