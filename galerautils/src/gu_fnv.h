// Copyright (C) 2012 Codership Oy <info@codership.com>

/*!
 * @file
 *
 * This header file defines FNV hash functions for 3 hash sizes:
 * 4, 8 and 16 bytes.
 *
 * Be wary of bitshift multiplication "optimization" (FNV_BITSHIFT_OPTIMIZATION):
 * FNV authors used to claim marginal speedup when using it, however on core2
 * CPU it has shown no speedup for fnv32a and more than 2x slowdown for fnv64a
 * and fnv128a. Disabled by default.
 *
 * FNV vs. FNVa: FNVa has a better distribution: multiplication happens after
 *               XOR and hence propagates XOR effect to all bytes of the hash.
 *               Hence by default functions perform FNVa. GU_FNV_NORMAL macro
 *               is needed for unit tests.
 * @todo: endian stuff.
 */

#ifndef _gu_fnv_h_
#define _gu_fnv_h_

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <assert.h>

#define GU_FNV32_PRIME 16777619UL
#define GU_FNV32_SEED  2166136261UL

#if !defined(GU_FNVBITSHIFT_OPTIMIZATION)
#define GU_FNV32_MULT(_x) _x *= GU_FNV32_PRIME
#else /* GU_FNVBITSHIFT_OPTIMIZATION */
#define GU_FNV32_MULT(_x) \
_x += (_x << 1) + (_x << 4) + (_x << 7) + (_x << 8) + (_x << 24)
#endif /* GU_FNVBITSHIFT_OPTIMIZATION */

#if !defined(GU_FNV_NORMAL)
#define GU_FNV32_ITERATION(_s,_b) _s ^= _b; GU_FNV32_MULT(_s);
#else
#define GU_FNV32_ITERATION(_s,_b) GU_FNV32_MULT(_s); _s ^= _b;
#endif

inline static void
gu_fnv32a (const void* buf, ssize_t const len, uint32_t* seed)
{
    const uint8_t* bp = (const uint8_t*)buf;
    const uint8_t* const be = bp + len;

    while (bp <= be - 2)
    {
        GU_FNV32_ITERATION(*seed,*bp++);
        GU_FNV32_ITERATION(*seed,*bp++);
    }

    if (bp < be)
    {
        GU_FNV32_ITERATION(*seed,*bp++);
    }

    assert(be == bp);
}

#define GU_FNV64_PRIME 1099511628211ULL
#define GU_FNV64_SEED  14695981039346656037ULL

#if !defined(GU_FNVBITSHIFT_OPTIMIZATION)
#define GU_FNV64_MULT(_x) _x *= GU_FNV64_PRIME
#else /* GU_FNVBITSHIFT_OPTIMIZATION */
#define GU_FNV64_MULT(_x) \
_x += (_x << 1) + (_x << 4) + (_x << 5) + (_x << 7) + (_x << 8) + (_x << 40);
#endif /* GU_FNVBITSHIFT_OPTIMIZATION */

#if !defined(GU_FNV_NORMAL)
#define GU_FNV64_ITERATION(_s,_b) _s ^= _b; GU_FNV64_MULT(_s);
#else
#define GU_FNV64_ITERATION(_s,_b) GU_FNV64_MULT(_s); _s ^= _b;
#endif

inline static void
gu_fnv64a (const void* buf, ssize_t const len, uint64_t* seed)
{
    const uint8_t* bp = (const uint8_t*)buf;
    const uint8_t* const be = bp + len;

    while (bp <= be - 2)
    {
        GU_FNV64_ITERATION(*seed,*bp++);
        GU_FNV64_ITERATION(*seed,*bp++);
    }

    if (bp < be)
    {
        GU_FNV64_ITERATION(*seed,*bp++);
    }

    assert(be == bp);
}

typedef          int __attribute__((__mode__(__TI__)))  int128_t;
typedef unsigned int __attribute__((__mode__(__TI__))) uint128_t;

static uint128_t const GU_FNV128_PRIME =
(((uint128_t)0x0000000001000000ULL << 64) + 0x000000000000013BULL);

static uint128_t const GU_FNV128_SEED =
(((uint128_t)0x6C62272E07BB0142ULL << 64) + 0x62B821756295C58DULL);

#if !defined(GU_FNVBITSHIFT_OPTIMIZATION)
#define GU_FNV128_MULT(_x) _x *= GU_FNV128_PRIME
#else /* GU_FNVBITSHIFT_OPTIMIZATION */
#define GU_FNV128_MULT(_x) \
_x += (_x << 1) + (_x << 3) + (_x << 4) + (_x << 5) + (_x << 8) + (_x << 88);
#endif /* GU_FNVBITSHIFT_OPTIMIZATION */

#if !defined(GU_FNV_NORMAL)
#define GU_FNV128_ITERATION(_s,_b) _s ^= _b; GU_FNV128_MULT(_s);
#else
#define GU_FNV128_ITERATION(_s,_b) GU_FNV128_MULT(_s); _s ^= _b;
#endif

inline static void
gu_fnv128a (const void* buf, ssize_t const len, uint128_t* seed)
{
    const uint8_t* bp = (const uint8_t*)buf;
    const uint8_t* const be = bp + len;

    /* this manual loop unrolling seems to be essential */
    while (bp <= be - 8)
    {
        GU_FNV128_ITERATION(*seed, *bp++);
        GU_FNV128_ITERATION(*seed, *bp++);
        GU_FNV128_ITERATION(*seed, *bp++);
        GU_FNV128_ITERATION(*seed, *bp++);
        GU_FNV128_ITERATION(*seed, *bp++);
        GU_FNV128_ITERATION(*seed, *bp++);
        GU_FNV128_ITERATION(*seed, *bp++);
        GU_FNV128_ITERATION(*seed, *bp++);
    }

    if (bp <= be - 4)
    {
        GU_FNV128_ITERATION(*seed, *bp++);
        GU_FNV128_ITERATION(*seed, *bp++);
        GU_FNV128_ITERATION(*seed, *bp++);
        GU_FNV128_ITERATION(*seed, *bp++);
    }

    if (bp <= be - 2)
    {
        GU_FNV128_ITERATION(*seed, *bp++);
        GU_FNV128_ITERATION(*seed, *bp++);
    }

    if (bp < be)
    {
        GU_FNV128_ITERATION(*seed, *bp++);
    }

    assert(be == bp);
}

#endif /* _gu_fnv_h_ */

