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

#include <immintrin.h>

#include <Util/Stopwatch.hpp>
#include <Util/AlignedBlock.hpp>
#include <Util/Euclidean.hpp>

/* https://www.strchr.com/strcmp_and_strlen_using_sse_4.2 */
int _mm_strcmp(
        const char* cs,
        const char* ct) {
    const long diff = cs - ct;
    const constexpr long nextbytes = 16; // asm ("rdx") does not work
    ct -= nextbytes;

    // Force GCC to use a register for nextbytes? Makes the code much worse! (adds LEA)
    // __asm__ __volatile__( "mov $16, %0" : "=r"(nextbytes) );

    loop:
    // could align it ASMVOLATILE( ".align 16\n" : : : "memory" );
    __m128i ct16chars = _mm_loadu_si128((const __m128i *) (ct += nextbytes));
    int offset = _mm_cmpistri(ct16chars, *(const __m128i * ) (ct + diff), _SIDD_SBYTE_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_NEGATIVE_POLARITY);
    __asm__ __volatile__ goto( "ja %l[loop] \n jc %l[not_equal]" : : : "memory" : loop, not_equal );
    return 0;

    not_equal: return ct[diff + offset] - ct[offset];
}
int _mm_strcmp(
        const unsigned char* cs,
        const unsigned char* ct) {
    const long diff = cs - ct;
    const constexpr long nextbytes = 16; // asm ("rdx") does not work
    ct -= nextbytes;

    // Force GCC to use a register for nextbytes? Makes the code much worse! (adds LEA)
    // __asm__ __volatile__( "mov $16, %0" : "=r"(nextbytes) );

    loop:
    // could align it ASMVOLATILE( ".align 16\n" : : : "memory" );
    __m128i ct16chars = _mm_loadu_si128((const __m128i *) (ct += nextbytes));
    int offset = _mm_cmpistri(ct16chars, *(const __m128i * ) (ct + diff), _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_NEGATIVE_POLARITY);
    __asm__ __volatile__ goto( "ja %l[loop] \n jc %l[not_equal]" : : : "memory" : loop, not_equal );
    return 0;

    not_equal: return ct[diff + offset] - ct[offset];
}
int _mm_strcmp(
        const short* cs,
        const short* ct) {
    const long diff = cs - ct;
    const constexpr long nextbytes = 8; // asm ("rdx") does not work
    ct -= nextbytes;

    // Force GCC to use a register for nextbytes? Makes the code much worse! (adds LEA)
    // __asm__ __volatile__( "mov $16, %0" : "=r"(nextbytes) );

    loop:
    // could align it ASMVOLATILE( ".align 16\n" : : : "memory" );
    __m128i ct16chars = _mm_loadu_si128((const __m128i *) (ct += nextbytes));
    int offset = _mm_cmpistri(ct16chars, *(const __m128i * ) (ct + diff), _SIDD_SWORD_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_NEGATIVE_POLARITY);
    __asm__ __volatile__ goto( "ja %l[loop] \n jc %l[not_equal]" : : : "memory" : loop, not_equal );
    return 0;

    not_equal: return ct[diff + offset] - ct[offset];
}
int _mm_strcmp(
        const unsigned short* cs,
        const unsigned short* ct) {
    const long diff = cs - ct;
    const constexpr long nextbytes = 8; // asm ("rdx") does not work
    ct -= nextbytes;

    // Force GCC to use a register for nextbytes? Makes the code much worse! (adds LEA)
    // __asm__ __volatile__( "mov $16, %0" : "=r"(nextbytes) );

    loop:
    // could align it ASMVOLATILE( ".align 16\n" : : : "memory" );
    __m128i ct16chars = _mm_loadu_si128((const __m128i *) (ct += nextbytes));
    int offset = _mm_cmpistri(ct16chars, *(const __m128i * ) (ct + diff), _SIDD_UWORD_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_NEGATIVE_POLARITY);
    __asm__ __volatile__ goto( "ja %l[loop] \n jc %l[not_equal]" : : : "memory" : loop, not_equal );
    return 0;

    not_equal: return ct[diff + offset] - ct[offset];
}

int strcmp2(
        const char *s1,
        const char *s2) {
    while (1) {
        int res = ((*s1 == 0) || (*s1 != *s2));
        if (__builtin_expect((res), 0)) {
            break;
        }
        ++s1;
        ++s2;
    }
    return (*s1 - *s2);
}

int strcmp2_AN(
        const unsigned short *s1,
        const unsigned short *s2,
        const unsigned short A) {
    const unsigned short Ainv = static_cast<unsigned short>(ext_euclidean(uint32_t(A), 16));
    const unsigned short * ps1 = s1;
    const unsigned short * ps2 = s2;
    while (1) {
        int s1dec = static_cast<unsigned short>(*ps1 * Ainv);
        int s2dec = static_cast<unsigned short>(*ps2 * Ainv);
        if (s1dec > 256) {
            std::cerr << "@" << (ps1 - s1) << ": s1dec=" << s1dec << '\n';
            throw std::runtime_error("first string is corrupt");
        }
        if (s2dec > 256) {
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
    return (*ps1 - *ps2);
}

int strcmp2_AN_accu(
        const unsigned short *s1,
        const unsigned short *s2,
        const unsigned short A) {
    const unsigned short Ainv = static_cast<unsigned short>(ext_euclidean(uint32_t(A), 16));
    const size_t accuAinv = Private::extractor<size_t, uint128_t>::doIt(ext_euclidean(uint128_t(A), 64));
    const unsigned short * ps1 = s1;
    const unsigned short * ps2 = s2;
    size_t accu1 = 0;
    size_t accu2 = 0;
    while (1) {
        accu1 += *ps1;
        unsigned short s1dec = static_cast<unsigned short>(*ps1 * Ainv);
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
    if ((accu1 * accuAinv) >= (1ull << 54)) {
        std::cerr << std::hex << "accuAinv=0x" << accuAinv << " accu1=0x" << accu1 << " accu2=0x" << accu2 << std::dec << '\n';
        throw std::runtime_error("first string is corrupt");
    }
    if ((accu2 * accuAinv) >= (1ull << 54)) {
        std::cerr << std::hex << "accuAinv=0x" << accuAinv << " accu1=0x" << accu1 << " accu2=0x" << accu2 << std::dec << '\n';
        throw std::runtime_error("second string is corrupt");
    }
    return (*ps1 - *ps2);
}

int _mm_strcmp_AN(
        const unsigned short* cs,
        const unsigned short* ct,
        const unsigned short A) {
    const unsigned short Ainv = static_cast<unsigned short>(ext_euclidean(uint32_t(A), 16));
    __m128i mmAinv = _mm_set1_epi16(Ainv);
    __m128i mmMax = _mm_set1_epi16(std::numeric_limits<unsigned char>::max());
    const long diff = cs - ct;
    const constexpr long nextbytes = 8; // asm ("rdx") does not work
    ct -= nextbytes;

    // Force GCC to use a register for nextbytes? Makes the code much worse! (adds LEA)
    // __asm__ __volatile__( "mov $16, %0" : "=r"(nextbytes) );

    loopAN:
    // could align it ASMVOLATILE( ".align 16\n" : : : "memory" );
    __m128i ct16chars1 = _mm_loadu_si128((const __m128i *) (ct + nextbytes));
    __m128i ct16chars2 = _mm_loadu_si128((const __m128i *) (ct + 2 * nextbytes));
    ct16chars1 = _mm_mullo_epi16(ct16chars1, mmAinv);
    ct16chars2 = _mm_mullo_epi16(ct16chars2, mmAinv);
    if (_mm_movemask_epi8(_mm_cmpgt_epi16(ct16chars1, mmMax)) | _mm_movemask_epi8(_mm_cmpgt_epi16(ct16chars2, mmMax))) {
        throw std::runtime_error("first string is corrupt");
    }
    __m128i cs16chars1 = _mm_loadu_si128((const __m128i *) (ct + diff + nextbytes));
    __m128i cs16chars2 = _mm_loadu_si128((const __m128i *) (ct + diff + 2 * nextbytes));
    cs16chars1 = _mm_mullo_epi16(cs16chars1, mmAinv);
    cs16chars2 = _mm_mullo_epi16(cs16chars2, mmAinv);
    if (_mm_movemask_epi8(_mm_cmpgt_epi16(cs16chars1, mmMax)) | _mm_movemask_epi8(_mm_cmpgt_epi16(cs16chars2, mmMax))) {
        throw std::runtime_error("first string is corrupt");
    }
    ct += 2 * nextbytes;
    int offset = _mm_cmpistri(ct16chars1, cs16chars1, _SIDD_UWORD_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_NEGATIVE_POLARITY);
    __asm__ __volatile__ goto( "ja %l[secondAN] \n jc %l[not_equalAN]" : : : "memory" : secondAN, not_equalAN );

    secondAN: offset = _mm_cmpistri(ct16chars2, cs16chars2, _SIDD_UWORD_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_NEGATIVE_POLARITY);
    __asm__ __volatile__ goto( "ja %l[loopAN] \n jc %l[not_equalAN]" : : : "memory" : loopAN, not_equalAN );
    return 0;

    not_equalAN: return ct[diff + offset] - ct[offset];
}

__m128i add_test_overflow(
        __m128i a,
        __m128i b) {
    auto res = _mm_add_epi64(a, b);
    if (_mm_extract_epi64(res, 0) < _mm_extract_epi64(a, 0)) {
        std::cerr << std::showbase << std::hex << "\t\t0: " << _mm_extract_epi64(a, 0) << '+' << _mm_extract_epi64(b, 0) << '=' << _mm_extract_epi64(res, 0) << '\n' << std::noshowbase << std::dec;
    }
    if (_mm_extract_epi64(res, 1) < _mm_extract_epi64(a, 1)) {
        std::cerr << std::showbase << std::hex << "\t\t1: " << _mm_extract_epi64(a, 1) << '+' << _mm_extract_epi64(b, 1) << '=' << _mm_extract_epi64(res, 1) << '\n' << std::noshowbase << std::dec;
    }
    return res;
}

int _mm_strcmp_AN_accu(
        const unsigned short* cs,
        const unsigned short* ct,
        const unsigned short A) {
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
    const long diff = cs - ct;
    const constexpr long nextbytes = 8; // asm ("rdx") does not work
    ct -= nextbytes;

    // Force GCC to use a register for nextbytes? Makes the code much worse! (adds LEA)
    // __asm__ __volatile__( "mov $16, %0" : "=r"(nextbytes) );

    loopANaccu:
    // could align it ASMVOLATILE( ".align 16\n" : : : "memory" );
    __m128i ct16chars = _mm_loadu_si128((const __m128i *) (ct += nextbytes));
    mmAccu11 = _mm_add_epi64(mmAccu11, _mm_cvtepu16_epi64(ct16chars));
    mmAccu12 = _mm_add_epi64(mmAccu12, _mm_cvtepu16_epi64(_mm_srli_si128(ct16chars, 4)));
    mmAccu13 = _mm_add_epi64(mmAccu13, _mm_cvtepu16_epi64(_mm_srli_si128(ct16chars, 8)));
    mmAccu14 = _mm_add_epi64(mmAccu14, _mm_cvtepu16_epi64(_mm_srli_si128(ct16chars, 12)));
    __m128i cs16chars = _mm_loadu_si128((const __m128i *) (ct + diff));
    mmAccu21 = _mm_add_epi64(mmAccu21, _mm_cvtepu16_epi64(cs16chars));
    mmAccu22 = _mm_add_epi64(mmAccu22, _mm_cvtepu16_epi64(_mm_srli_si128(cs16chars, 4)));
    mmAccu23 = _mm_add_epi64(mmAccu23, _mm_cvtepu16_epi64(_mm_srli_si128(cs16chars, 8)));
    mmAccu24 = _mm_add_epi64(mmAccu24, _mm_cvtepu16_epi64(_mm_srli_si128(cs16chars, 12)));
    int offset = _mm_cmpistri(ct16chars, cs16chars, _SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_NEGATIVE_POLARITY);
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
        return (Ainv * ct[diff + offset]) - (Ainv * ct[offset]);
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

template<>
__m128i XOR(
        const __m128i * mm) {
    __m128i mmXOR = _mm_setzero_si128();
    __m128i mmZero = _mm_setzero_si128();
    while (true) {
        __m128i mmValue = _mm_loadu_si128(mm++);
        mmXOR = _mm_xor_si128(mmXOR, mmValue);
        if (_mm_movemask_epi8(_mm_cmpeq_epi8(mmValue, mmZero))) {
            break;
        }
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
    return (*ps1 - *ps2);
}

int _mm_strcmp_xor(
        const char* cs,
        const char* ct,
        __m128i old_xor1,
        __m128i old_xor2,
        const size_t NUM) {
    __m128i mmXOR1 = _mm_setzero_si128();
    __m128i mmXOR2 = _mm_setzero_si128();
    const char* const csEnd = cs + NUM;
    const char* const ctEnd = ct + NUM;

    const long diff = cs - ct;
    const constexpr long nextbytes = 16; // asm ("rdx") does not work
    ct -= nextbytes;

    // Force GCC to use a register for nextbytes? Makes the code much worse! (adds LEA)
    // __asm__ __volatile__( "mov $16, %0" : "=r"(nextbytes) );

    loopXORchar:
    // could align it ASMVOLATILE( ".align 16\n" : : : "memory" );
    __m128i ct16chars = _mm_loadu_si128((const __m128i *) (ct += nextbytes));
    mmXOR1 = _mm_xor_si128(mmXOR1, ct16chars);
    __m128i cs16chars = _mm_loadu_si128((const __m128i *) (ct + diff));
    mmXOR2 = _mm_xor_si128(mmXOR2, cs16chars);
    int offset = _mm_cmpistri(ct16chars, cs16chars, _SIDD_SBYTE_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_NEGATIVE_POLARITY);
    __asm__ __volatile__ goto( "ja %l[loopXORchar] \n jc %l[not_equalXORchar]" : : : "memory" : loopXORchar, not_equalXORchar );
    {
        const char* const ctOld = ct;
        while (ct < ctEnd) {
            __m128i ct16chars = _mm_loadu_si128((const __m128i *) (ct += nextbytes));
            mmXOR1 = _mm_xor_si128(mmXOR1, ct16chars);
        }
        cs = ctOld + diff;
        while (cs < csEnd) {
            __m128i cs16chars = _mm_loadu_si128((const __m128i *) (cs += nextbytes));
            mmXOR2 = _mm_xor_si128(mmXOR2, cs16chars);
        }
        if (_mm_movemask_epi8(_mm_cmpeq_epi8(mmXOR1, old_xor1))) {
            if (_mm_movemask_epi8(_mm_cmpeq_epi8(mmXOR2, old_xor2))) {
                return 0;
            } else {
                throw std::runtime_error("checksum 2 doesn't match!");
            }
        } else {
            throw std::runtime_error("checksum 1 doesn't match!");
        }
    }

    not_equalXORchar: {
        const char* const ctOld = ct;
        while (ct < ctEnd) {
            __m128i ct16chars = _mm_loadu_si128((const __m128i *) (ct += nextbytes));
            mmXOR1 = _mm_xor_si128(mmXOR1, ct16chars);
        }
        cs = ctOld + diff;
        while (cs < ctEnd) {
            __m128i cs16chars = _mm_loadu_si128((const __m128i *) (cs += nextbytes));
            mmXOR2 = _mm_xor_si128(mmXOR2, cs16chars);
        }
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
}

int _mm_strcmp_xor(
        const unsigned short* cs,
        const unsigned short* ct,
        __m128i old_xor1,
        __m128i old_xor2,
        const size_t NUM) {
    __m128i mmXOR1 = _mm_setzero_si128();
    __m128i mmXOR2 = _mm_setzero_si128();
    const unsigned short* const csEnd = cs + NUM;
    const unsigned short* const ctEnd = ct + NUM;

    const long diff = cs - ct;
    const constexpr long nextbytes = 8; // asm ("rdx") does not work
    ct -= nextbytes;

    // Force GCC to use a register for nextbytes? Makes the code much worse! (adds LEA)
    // __asm__ __volatile__( "mov $16, %0" : "=r"(nextbytes) );

    loopXORchar:
    // could align it ASMVOLATILE( ".align 16\n" : : : "memory" );
    __m128i ct16chars = _mm_loadu_si128((const __m128i *) (ct += nextbytes));
    mmXOR1 = _mm_xor_si128(mmXOR1, ct16chars);
    __m128i cs16chars = _mm_loadu_si128((const __m128i *) (ct + diff));
    mmXOR2 = _mm_xor_si128(mmXOR2, cs16chars);
    int offset = _mm_cmpistri(ct16chars, cs16chars, _SIDD_UWORD_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_NEGATIVE_POLARITY);
    __asm__ __volatile__ goto( "ja %l[loopXORchar] \n jc %l[not_equalXORchar]" : : : "memory" : loopXORchar, not_equalXORchar );
    {
        const unsigned short* const ctOld = ct;
        while (ct < ctEnd) {
            __m128i ct16chars = _mm_loadu_si128((const __m128i *) (ct += nextbytes));
            mmXOR1 = _mm_xor_si128(mmXOR1, ct16chars);
        }
        cs = ctOld + diff;
        while (cs < csEnd) {
            __m128i cs16chars = _mm_loadu_si128((const __m128i *) (cs += nextbytes));
            mmXOR2 = _mm_xor_si128(mmXOR2, cs16chars);
        }
        if (_mm_movemask_epi8(_mm_cmpeq_epi8(mmXOR1, old_xor1))) {
            if (_mm_movemask_epi8(_mm_cmpeq_epi8(mmXOR2, old_xor2))) {
                return 0;
            } else {
                throw std::runtime_error("checksum 2 doesn't match!");
            }
        } else {
            throw std::runtime_error("checksum 1 doesn't match!");
        }
    }

    not_equalXORchar: {
        const unsigned short* const ctOld = ct;
        while (ct < ctEnd) {
            __m128i ct16chars = _mm_loadu_si128((const __m128i *) (ct += nextbytes));
            mmXOR1 = _mm_xor_si128(mmXOR1, ct16chars);
        }
        cs = ctOld + diff;
        while (cs < ctEnd) {
            __m128i cs16chars = _mm_loadu_si128((const __m128i *) (cs += nextbytes));
            mmXOR2 = _mm_xor_si128(mmXOR2, cs16chars);
        }
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

int main(
        int argc,
        char** argv) {
    const constexpr size_t NUM = 64 * 1024 * 1024;
    const constexpr size_t NUM_BYTES_CHAR = NUM * sizeof(unsigned char);
    const constexpr size_t NUM_BYTES_SHORT = NUM * sizeof(unsigned short);

    Stopwatch sw;
    int res;
    int64_t time;

    std::cout << "Short buffer with " << NUM_BYTES_CHAR << " bytes:\n";
    AlignedBlock buf1(NUM_BYTES_CHAR, 16);
    AlignedBlock buf2(NUM_BYTES_CHAR, 16);
    DataGenerationConfiguration config(8);
    ResetBlock<unsigned char>(config, buf1);
    *(buf1.template end<char>() - 1) = 0;
    ResetBlock<unsigned char>(config, buf2);
    *(buf2.template end<char>() - 1) = 0;
    const char * uc1 = buf1.template begin<const char>();
    const char * uc2 = buf2.template begin<const char>();
    auto x = uc1;
    while (*x) {
        ++x;
    }
    if ((x - uc1) != (NUM - 1)) {
        std::cerr << "zero value found in buf1 at position " << (x - uc1) << " instead of expected " << (NUM - 1) << std::endl;
    }
    x = uc2;
    while (*x) {
        ++x;
    }
    if ((x - uc2) != (NUM - 1)) {
        std::cerr << "zero value found in buf2 at position " << (x - uc2) << " instead of expected " << (NUM - 1) << std::endl;
    }

    sw.Reset();
    res = std::strcmp(uc1, uc2);
    time = sw.Current();
    std::cout << "\tstd::strcmp(buf1, buf2)\t" << res << "\t" << time << '\n';

    sw.Reset();
    res = _mm_strcmp(uc1, uc2);
    time = sw.Current();
    std::cout << "\t_mm_strcmp(buf1, buf2)\t" << res << "\t" << time << '\n';

    sw.Reset();
    res = strcmp2(uc1, uc2);
    time = sw.Current();
    std::cout << "\tstd::strcmp2(buf1, buf2)\t" << res << "\t" << time << '\n';

    char old_xor_char1 = XOR(uc1);
    char old_xor_char2 = XOR(uc2);
    sw.Reset();
    res = strcmp2_XOR(uc1, uc2, old_xor_char1, old_xor_char2, NUM);
    time = sw.Current();
    std::cout << "\tstrcmp2_XOR<char>(buf1, buf2, old_xor_char1, old_xor_char2, NUM)\t" << res << "\t" << time << '\n';

    __m128i old_xor_mm1 = XOR(reinterpret_cast<const __m128i *>(uc1));
    __m128i old_xor_mm2 = XOR(reinterpret_cast<const __m128i *>(uc2));
    sw.Reset();
    res = _mm_strcmp_xor(uc1, uc2, old_xor_mm1, old_xor_mm2, NUM);
    time = sw.Current();
    std::cout << "\t_mm_strcmp_xor(buf1, buf2, old_xor_mm1, old_xor_mm2, NUM)\t" << res << "\t" << time << '\n';

    char* ptr_end;
    unsigned short A = 857;
    // force the compiler to not assume A a constant
    if (argc > 1) {
        A = strtol(argv[1], &ptr_end, 10);
    }

    std::cout << "Long buffer with " << NUM_BYTES_SHORT << " bytes:\n";
    AlignedBlock buf3(NUM_BYTES_SHORT, 16);
    AlignedBlock buf4(NUM_BYTES_SHORT, 16);
    DataGenerationConfiguration config2(6, 0, A);
    ResetBlock<unsigned short>(config2, buf3);
    *(buf3.template end<unsigned short>() - 1) = 0;
    ResetBlock<unsigned short>(config2, buf4);
    *(buf4.template end<unsigned short>() - 1) = 0;
    const unsigned short * us1 = buf3.template begin<const unsigned short>();
    const unsigned short * us2 = buf4.template begin<const unsigned short>();
    uc1 = reinterpret_cast<const char*>(us1);
    uc2 = reinterpret_cast<const char*>(us2);
    auto y = us1;
    while (*y) {
        ++y;
    }
    if ((y - us1) != (NUM - 1)) {
        std::cerr << "zero value found in buf3 at position " << (y - us1) << " instead of expected " << (NUM - 1) << std::endl;
    }
    y = us2;
    while (*y) {
        ++y;
    }
    if ((y - us2) != (NUM - 1)) {
        std::cerr << "zero value found in buf4 at position " << (y - us2) << " instead of expected " << (NUM - 1) << std::endl;
    }

    sw.Reset();
    res = std::strcmp(uc1, uc2);
    time = sw.Current();
    std::cout << "\tstd::strcmp(buf3, buf4)\t" << res << "\t" << time << '\n';

    sw.Reset();
    res = _mm_strcmp(uc1, uc2);
    time = sw.Current();
    std::cout << "\t_mm_strcmp<char>(buf3, buf4)\t" << res << "\t" << time << '\n';

    sw.Reset();
    res = _mm_strcmp(us1, us2);
    time = sw.Current();
    std::cout << "\t_mm_strcmp<short>(buf3, buf4)\t" << res << "\t" << time << '\n';

    sw.Reset();
    res = strcmp2(uc1, uc2);
    time = sw.Current();
    std::cout << "\tstrcmp2(buf3, buf4)\t" << res << "\t" << time << '\n';

    sw.Reset();
    res = strcmp2_AN(us1, us2, A);
    time = sw.Current();
    std::cout << "\tstrcmp2_AN\t" << res << "\t" << time << '\n';

    sw.Reset();
    res = strcmp2_AN_accu(us1, us2, A);
    time = sw.Current();
    std::cout << "\tstrcmp2_AN_accu\t" << res << "\t" << time << '\n';

    sw.Reset();
    res = _mm_strcmp_AN(us1, us2, A);
    time = sw.Current();
    std::cout << "\t_mm_strcmp_AN\t" << res << "\t" << time << '\n';

    sw.Reset();
    res = _mm_strcmp_AN_accu(us1, us2, A);
    time = sw.Current();
    std::cout << "\t_mm_strcmp_AN_accu\t" << res << "\t" << time << '\n';

    unsigned short old_xor_short1 = XOR(us1);
    unsigned short old_xor_short2 = XOR(us2);
    sw.Reset();
    res = strcmp2_XOR(us1, us2, old_xor_short1, old_xor_short2, NUM);
    time = sw.Current();
    std::cout << "\tstrcmp2_XOR<unsigned short>(buf3, buf4, old_xor1, old_xor2, NUM)\t" << res << "\t" << time << '\n';

    old_xor_mm1 = XOR(reinterpret_cast<const __m128i *>(us1));
    old_xor_mm2 = XOR(reinterpret_cast<const __m128i *>(us2));
    sw.Reset();
    res = _mm_strcmp_xor(us1, us2, old_xor_mm1, old_xor_mm2, NUM);
    time = sw.Current();
    std::cout << "\t_mm_strcmp_xor(buf3, buf4, old_xor_mm1, old_xor_mm2, NUM)\t" << res << "\t" << time << '\n';

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
