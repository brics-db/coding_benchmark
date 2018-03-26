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
 * Strings.cpp
 *
 *  Created on: 25.03.2018
 *      Author: Till Kolditz - Till.Kolditz@gmail.com
 */

#include <iostream>

#include <Strings/Strings.hpp>

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

template int strcmp2(
        const unsigned char * s1,
        const unsigned char * s2,
        const size_t NUM);

template int strcmp2(
        const signed char * s1,
        const signed char * s2,
        const size_t NUM);

template int strcmp2(
        const unsigned short * s1,
        const unsigned short * s2,
        const size_t NUM);

template int strcmp2(
        const signed short * s1,
        const signed short * s2,
        const size_t NUM);
