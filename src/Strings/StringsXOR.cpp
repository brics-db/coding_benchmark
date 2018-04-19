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
 * StringsXOR.cpp
 *
 *  Created on: 25.03.2018
 *      Author: Till Kolditz - Till.Kolditz@gmail.com
 */

#include <iostream>

#include <Strings/StringBase.hpp>
#include <Strings/StringsXOR.hpp>
#include <Util/Intrinsics.hpp>

template
uint8_t strxor(
        const uint8_t* t);

template
int8_t strxor(
        const int8_t* t);

template
uint16_t strxor(
        const uint16_t* t);

template
int16_t strxor(
        const int16_t* t);

template
uint32_t strxor(
        const uint32_t* t);

template
int32_t strxor(
        const int32_t* t);

template __m128i _mm_strxor<uint8_t>(
        const __m128i * mm,
        const size_t NUM);

template __m128i _mm_strxor<int8_t>(
        const __m128i * mm,
        const size_t NUM);

template __m128i _mm_strxor<uint16_t>(
        const __m128i * mm,
        const size_t NUM);

template __m128i _mm_strxor<int16_t>(
        const __m128i * mm,
        const size_t NUM);

template __m128i _mm_strxor<uint32_t>(
        const __m128i * mm,
        const size_t NUM);

template __m128i _mm_strxor<int32_t>(
        const __m128i * mm,
        const size_t NUM);

template
int strcmp2_xor(
        const uint8_t *s1,
        const uint8_t *s2,
        const uint8_t old_xor1,
        const uint8_t old_xor2,
        const size_t NUM);

template
int strcmp2_xor(
        const int8_t *s1,
        const int8_t *s2,
        const int8_t old_xor1,
        const int8_t old_xor2,
        const size_t NUM);

template
int strcmp2_xor(
        const uint16_t *s1,
        const uint16_t *s2,
        const uint16_t old_xor1,
        const uint16_t old_xor2,
        const size_t NUM);

template
int strcmp2_xor(
        const int16_t *s1,
        const int16_t *s2,
        const int16_t old_xor1,
        const int16_t old_xor2,
        const size_t NUM);

template
int strcmp2_xor(
        const uint32_t *s1,
        const uint32_t *s2,
        const uint32_t old_xor1,
        const uint32_t old_xor2,
        const size_t NUM);

template
int strcmp2_xor(
        const int32_t *s1,
        const int32_t *s2,
        const int32_t old_xor1,
        const int32_t old_xor2,
        const size_t NUM);

inline char compactXORchar(
        __m128i & mmXOR) {
    int64_t tmp = _mm_extract_epi64(mmXOR, 0) ^ _mm_extract_epi64(mmXOR, 1);
    tmp = static_cast<int32_t>(tmp) ^ static_cast<int32_t>(tmp >> 32);
    tmp = static_cast<int16_t>(tmp) ^ static_cast<int16_t>(tmp >> 16);
    return static_cast<char>(tmp) ^ static_cast<char>(tmp >> 8);
}

int _mm_strcmp_xor(
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
    while ((ct += charsPerMM128) < ctEnd) {
        __m128i ct16chars = _mm_lddqu_si128((const __m128i *) ct);
        mmXOR1 = _mm_xor_si128(mmXOR1, ct16chars);
        __m128i cs16chars = _mm_lddqu_si128((const __m128i *) (ct + diff));
        mmXOR2 = _mm_xor_si128(mmXOR2, cs16chars);
    }
    if (_mm_movemask_epi8(_mm_cmpeq_epi8(mmXOR1, old_xor1)) == 0xFFFF) {
        if (_mm_movemask_epi8(_mm_cmpeq_epi8(mmXOR2, old_xor2)) == 0xFFFF) {
#ifndef NDEBUG
            if (static_cast<size_t>((ct + diff) - csOrg) != NUM) {
                std::cerr << '[' << __FILE__ << '@' << __LINE__ << "] pos1=" << ((ct + diff) - csOrg) << " != " << NUM << std::endl;
            }
            if (static_cast<size_t>(ct - ctOrg) != NUM) {
                std::cerr << '[' << __FILE__ << '@' << __LINE__ << "] pos2=" << (ct - ctOrg) << " != " << NUM << std::endl;
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
    while ((ct += charsPerMM128) <= (ctEnd - charsPerMM128)) {
        __m128i ct16chars = _mm_lddqu_si128((const __m128i *) ct);
        mmXOR1 = _mm_xor_si128(mmXOR1, ct16chars);
        __m128i cs16chars = _mm_lddqu_si128((const __m128i *) (ct + diff));
        mmXOR2 = _mm_xor_si128(mmXOR2, cs16chars);
    }
#ifndef NDEBUG
    if (static_cast<size_t>((ct + diff) - csOrg) != NUM) {
        std::cerr << '[' << __FILE__ << '@' << __LINE__ << "] pos1=" << ((ct + diff) - csOrg) << " != " << NUM << std::endl;
    }
    if (static_cast<size_t>(ct - ctOrg) != NUM) {
        std::cerr << '[' << __FILE__ << '@' << __LINE__ << "] pos2=" << (ct - ctOrg) << " != " << NUM << std::endl;
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

int _mm_strcmp_xor(
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
    while ((ct += shortsPerMM128) < ctEnd) {
        __m128i ct16chars = _mm_lddqu_si128((const __m128i *) ct);
        mmXOR1 = _mm_xor_si128(mmXOR1, ct16chars);
        __m128i cs16chars = _mm_lddqu_si128((const __m128i *) (ct + diff));
        mmXOR2 = _mm_xor_si128(mmXOR2, cs16chars);
    }
    if (_mm_movemask_epi8(_mm_cmpeq_epi8(mmXOR1, old_xor1)) == 0xFFFF) {
        if (_mm_movemask_epi8(_mm_cmpeq_epi8(mmXOR2, old_xor2)) == 0xFFFF) {
#ifndef NDEBUG
            if (static_cast<size_t>((ct + diff) - csOrg) != NUM) {
                std::cerr << '[' << __FILE__ << '@' << __LINE__ << "] pos1=" << ((ct + diff) - csOrg) << " != " << NUM << std::endl;
            }
            if (static_cast<size_t>(ct - ctOrg) != NUM) {
                std::cerr << '[' << __FILE__ << '@' << __LINE__ << "] pos2=" << (ct - ctOrg) << " != " << NUM << std::endl;
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
    while ((ct += shortsPerMM128) < ctEnd) {
        __m128i ct16chars = _mm_lddqu_si128((const __m128i *) ct);
        mmXOR1 = _mm_xor_si128(mmXOR1, ct16chars);
        __m128i cs16chars = _mm_lddqu_si128((const __m128i *) (ct + diff));
        mmXOR2 = _mm_xor_si128(mmXOR2, cs16chars);
    }
#ifndef NDEBUG
    if (static_cast<size_t>((ct + diff) - csOrg) != NUM) {
        std::cerr << '[' << __FILE__ << '@' << __LINE__ << "] pos1=" << ((ct + diff) - csOrg) << " != " << NUM << std::endl;
    }
    if (static_cast<size_t>(ct - ctOrg) != NUM) {
        std::cerr << '[' << __FILE__ << '@' << __LINE__ << "] pos2=" << (ct - ctOrg) << " != " << NUM << std::endl;
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
