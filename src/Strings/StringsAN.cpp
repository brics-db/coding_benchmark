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
 * StringsAN.cpp
 *
 *  Created on: 25.03.2018
 *      Author: Till Kolditz - Till.Kolditz@gmail.com
 */

#include <iostream>

#include <Strings/StringsAN.hpp>
#include <Util/Euclidean.hpp>

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
            std::cerr << "[" __FILE__ "@" << __LINE__ << "] @" << (ps1 - s1) << ": s1dec=" << s1dec << '\n';
            throw std::runtime_error("[" __FILE__ "@40] first string is corrupt");
        }
        if (s2dec > std::numeric_limits<unsigned char>::max()) {
            std::cerr << "[" __FILE__ "@" << __LINE__ << "] @" << (ps2 - s2) << ": s2dec=" << s2dec << '\n';
            throw std::runtime_error("[" __FILE__ "@44] second string is corrupt");
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
            std::cerr << "[" __FILE__ "@" << __LINE__ << "] overflow: accu1=" << std::hex << std::showbase << accu1 << " *ps1=" << *ps1 << std::dec << std::noshowbase << std::endl;
        }
#endif
        accu1 += *ps1;
        unsigned short s1dec = static_cast<unsigned short>(*ps1 * Ainv);
#ifndef NDEBUG
        if ((accu2 + *ps2) < accu2) {
            std::cerr << "[" __FILE__ "@" << __LINE__ << "] overflow: accu2=" << std::hex << std::showbase << accu1 << " *ps2=" << *ps1 << std::dec << std::noshowbase << std::endl;
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
        std::cerr << "[" __FILE__ "@" << __LINE__ << "] " << std::hex << std::showbase << "accuAinv=" << accuAinv << " A*inv=" << (size_t(A) * accuAinv) << " accu1=" << accu1 << " dec1="
                << (accu1 * accuAinv) << " accu2=" << accu2 << " dec2=" << (accu2 * accuAinv) << std::dec << std::noshowbase << '\n';
#endif
        throw std::runtime_error("[" __FILE__ "@104] first string is corrupt");
    }
    if ((accu2 * accuAinv) > MAX) {
#ifndef NDEBUG
        std::cerr << "[" __FILE__ "@" << __LINE__ << "] " << std::hex << std::showbase << "accuAinv=" << accuAinv << " A*inv=" << (size_t(A) * accuAinv) << " accu1=" << accu1 << " dec1="
                << (accu1 * accuAinv) << " accu2=" << accu2 << " dec2=" << (accu2 * accuAinv) << std::dec << std::noshowbase << '\n';
#endif
        throw std::runtime_error("[" __FILE__ "@111] second string is corrupt");
    }
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
            std::cerr << "[" __FILE__ "@" << __LINE__ << "] @" << (ps1 - s1) << ": s1dec=" << s1dec << '\n';
            throw std::runtime_error("[" __FILE__ "@137] first string is corrupt");
        }
        if (s2dec > std::numeric_limits<unsigned short>::max()) {
            std::cerr << "[" __FILE__ "@" << __LINE__ << "] @" << (ps2 - s2) << ": s2dec=" << s2dec << '\n';
            throw std::runtime_error("[" __FILE__ "@141] second string is corrupt");
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
            std::cerr << "[" __FILE__ "@" << __LINE__ << "] overflow: accu2=" << std::hex << std::showbase << accu1 << " *ps2=" << *ps1 << std::dec << std::noshowbase << std::endl;
        }
#endif
        accu2 += *ps2;
        auto s2dec = static_cast<unsigned int>(*ps2 * Ainv);
#ifndef NDEBUG
        if ((accu2 + *ps2) < accu2) {
            std::cerr << "[" __FILE__ "@" << __LINE__ << "] overflow: accu2=" << std::hex << std::showbase << accu1 << " *ps2=" << *ps1 << std::dec << std::noshowbase << std::endl;
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
        std::cerr << "[" __FILE__ "@" << __LINE__ << "] " << std::hex << "accuAinv=0x" << accuAinv << " accu1=0x" << accu1 << " accu2=0x" << accu2 << std::dec << '\n';
        throw std::runtime_error("[" __FILE__ "@198] first string is corrupt");
    }
    if ((accu2 * accuAinv) > MAX) {
        std::cerr << "[" __FILE__ "@" << __LINE__ << "] " << std::hex << "accuAinv=0x" << accuAinv << " accu1=0x" << accu1 << " accu2=0x" << accu2 << std::dec << '\n';
        throw std::runtime_error("[" __FILE__ "@202] second string is corrupt");
    }
    return (*ps1 - *ps2);
}

__m128i add_test_overflow(
        __m128i a,
        __m128i b) {
    auto res = _mm_add_epi64(a, b);
    if (_mm_extract_epi64(res, 0) < _mm_extract_epi64(a, 0)) {
        std::cerr << "[" __FILE__ "@" << __LINE__ << "] " << std::showbase << std::hex << "#\t0: " << _mm_extract_epi64(a, 0) << '+' << _mm_extract_epi64(b, 0) << '=' << _mm_extract_epi64(res, 0)
                << '\n' << std::noshowbase << std::dec;
    }
    if (_mm_extract_epi64(res, 1) < _mm_extract_epi64(a, 1)) {
        std::cerr << "[" __FILE__ "@" << __LINE__ << "] " << std::showbase << std::hex << "#\t1: " << _mm_extract_epi64(a, 1) << '+' << _mm_extract_epi64(b, 1) << '=' << _mm_extract_epi64(res, 1)
                << '\n' << std::noshowbase << std::dec;
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
    const long diff = s1 - s2;
    s2 -= shortsPerMM128;

    // Force GCC to use a register for nextbytes? Makes the code much worse! (adds LEA)
    // __asm__ __volatile__( "mov $16, %0" : "=r"(nextbytes) );

    loopAN:
    // could align it ASMVOLATILE( ".align 16\n" : : : "memory" );
    __m128i ct16chars1 = _mm_lddqu_si128((const __m128i *) (s2 += shortsPerMM128));
    if (_mm_movemask_epi8(_mm_cmpgt_epi16(_mm_mullo_epi16(ct16chars1, mmAinv), mmMax))) {
        throw std::runtime_error("[" __FILE__ "@251] first string is corrupt");
    }
    __m128i cs16chars1 = _mm_lddqu_si128((const __m128i *) (s2 + diff));
    if (_mm_movemask_epi8(_mm_cmpgt_epi16(_mm_mullo_epi16(cs16chars1, mmAinv), mmMax))) {
        throw std::runtime_error("[" __FILE__ "@255] first string is corrupt");
    }
    int offset = _mm_cmpistri(ct16chars1, cs16chars1, _SIDD_UWORD_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_NEGATIVE_POLARITY);
    __asm__ __volatile__ goto( "ja %l[loopAN] \n jc %l[not_equalAN]" : : : "memory" : loopAN, not_equalAN );
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
            throw std::runtime_error("[" __FILE__ "@311] first string is corrupt");
        }
        mmAccu = _mm_add_epi64(mmAccu21, _mm_add_epi64(mmAccu22, _mm_add_epi64(mmAccu23, mmAccu24)));
        if (((_mm_extract_epi64(mmAccu, 0) * accuAinv) > maxAccu) || ((_mm_extract_epi64(mmAccu, 1) * accuAinv) > maxAccu)) {
            throw std::runtime_error("[" __FILE__ "@315] second string is corrupt");
        }
        return 0;
    }

    not_equalANaccu: {
        auto mmAccu = _mm_add_epi64(mmAccu11, _mm_add_epi64(mmAccu12, _mm_add_epi64(mmAccu13, mmAccu14)));
        if (((_mm_extract_epi64(mmAccu, 0) * accuAinv) > maxAccu) || ((_mm_extract_epi64(mmAccu, 1) * accuAinv) > maxAccu)) {
            throw std::runtime_error("[" __FILE__ "@328] first string is corrupt");
        }
        mmAccu = _mm_add_epi64(mmAccu21, _mm_add_epi64(mmAccu22, _mm_add_epi64(mmAccu23, mmAccu24)));
        if (((_mm_extract_epi64(mmAccu, 0) * accuAinv) > maxAccu) || ((_mm_extract_epi64(mmAccu, 1) * accuAinv) > maxAccu)) {
            throw std::runtime_error("[" __FILE__ "@332] second string is corrupt");
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
    const long diff = i1 - i2;
    i2 -= intsPerMM128;

    // Force GCC to use a register for nextbytes? Makes the code much worse! (adds LEA)
    // __asm__ __volatile__( "mov $16, %0" : "=r"(nextbytes) );

    loopAN:
    // could align it ASMVOLATILE( ".align 16\n" : : : "memory" );
    __m128i ct4ints = _mm_lddqu_si128((const __m128i *) (i2 += intsPerMM128));
    if (_mm_movemask_epi8(_mm_cmpgt_epi32(_mm_mullo_epi32(ct4ints, mmAinv), mmMax))) {
        throw std::runtime_error("[" __FILE__ "@360] first string is corrupt");
    }
    __m128i cs4ints = _mm_lddqu_si128((const __m128i *) (i2 + diff));
    if (_mm_movemask_epi8(_mm_cmpgt_epi32(_mm_mullo_epi32(cs4ints, mmAinv), mmMax))) {
        throw std::runtime_error("[" __FILE__ "@364] second string is corrupt");
    }
    int offset = _mm_cmpistri(ct4ints, cs4ints, _SIDD_UWORD_OPS | _SIDD_CMP_EQUAL_EACH | _SIDD_NEGATIVE_POLARITY);
    __asm__ __volatile__ goto( "ja %l[loopAN] \n jc %l[not_equalAN]" : : : "memory" : loopAN, not_equalAN );
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
            throw std::runtime_error("[" __FILE__ "@412] first string is corrupt");
        }
        mmAccu = _mm_add_epi64(mmAccu21, mmAccu22);
        if (((_mm_extract_epi64(mmAccu, 0) * accuAinv) > maxAccu) || ((_mm_extract_epi64(mmAccu, 1) * accuAinv) > maxAccu)) {
            throw std::runtime_error("[" __FILE__ "@416] second string is corrupt");
        }
        return 0;
    }

    not_equalANaccu: {
        auto mmAccu = _mm_add_epi64(mmAccu11, mmAccu12);
        if (((_mm_extract_epi64(mmAccu, 0) * accuAinv) > maxAccu) || ((_mm_extract_epi64(mmAccu, 1) * accuAinv) > maxAccu)) {
            throw std::runtime_error("[" __FILE__ "] first string is corrupt");
        }
        const unsigned short Ainv = static_cast<unsigned short>(ext_euclidean(uint32_t(A), 16));
        return (Ainv * i2[diff + offset]) - (Ainv * i2[offset]);
    }
}
