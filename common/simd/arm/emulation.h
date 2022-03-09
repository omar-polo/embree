// Copyright 2009-2020 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

/* Make precision match SSE, at the cost of some performance */
#if !defined(__aarch64__)
#  define SSE2NEON_PRECISE_DIV 1
#  define SSE2NEON_PRECISE_SQRT 1
#endif

#include "sse2neon.h"

__forceinline __m128 _mm_abs_ps(__m128 a)
{
    return vabsq_f32(a);
}

__forceinline __m128 _mm_msub_ps(__m128 a, __m128 b, __m128 c)
{
    return vmlsq_f32(c, a, b);
}


// FMA

// Computes the fused multiply add product of 32-bit floating point numbers.
//
// Return Value
// Multiplies A and B, and adds C to the temporary result before returning it.
// https://software.intel.com/sites/landingpage/IntrinsicsGuide/#text=_mm_fmadd
__forceinline __m128 _mm_fmadd_ps(__m128 a, __m128 b, __m128 c)
{
#if defined(__aarch64__)
    return vreinterpretq_m128_f32(vfmaq_f32(vreinterpretq_f32_m128(c),
                                            vreinterpretq_f32_m128(b),
                                            vreinterpretq_f32_m128(a)));
#else
    return _mm_add_ps(_mm_mul_ps(a, b), c);
#endif
}

__forceinline __m128 _mm_fmsub_ps(__m128 a, __m128 b, __m128 c) {
   __m128 neg_c = vreinterpretq_m128_f32(vnegq_f32(vreinterpretq_f32_m128(c)));
  return _mm_fmadd_ps(a, b, neg_c);
}

__forceinline __m128 _mm_fnmadd_ps(__m128 a, __m128 b, __m128 c) {
#if defined(__aarch64__)
    return vreinterpretq_m128_f32(vfmsq_f32(vreinterpretq_f32_m128(c),
                                            vreinterpretq_f32_m128(b),
                                            vreinterpretq_f32_m128(a)));
#else
    return _mm_sub_ps(c, _mm_mul_ps(a, b));
#endif
}

__forceinline __m128 _mm_fnmsub_ps(__m128 a, __m128 b, __m128 c) {
  return vreinterpretq_m128_f32(vnegq_f32(vreinterpretq_f32_m128(_mm_fmadd_ps(a,b,c))));
}

// AVX2 emulation leverages Intel FMA defs above.  Include after them.
#if defined(NEON_AVX2_EMULATION)
#include "avx2neon.h"
#endif

/* Dummy defines for floating point control */
#define _MM_MASK_MASK 0x1f80
#define _MM_MASK_DIV_ZERO 0x200
// #define _MM_FLUSH_ZERO_ON 0x8000
#define _MM_MASK_DENORM 0x100
#define _MM_SET_EXCEPTION_MASK(x)
// #define _MM_SET_FLUSH_ZERO_MODE(x)

__forceinline int _mm_getcsr()
{
  return 0;
}

__forceinline void _mm_mfence()
{
  __sync_synchronize();
}

__forceinline __m128i _mm_load4epu8_epi32(__m128i *ptr)
{
    uint8x8_t  t0 = vld1_u8((uint8_t*)ptr);
    uint16x8_t t1 = vmovl_u8(t0);
    uint32x4_t t2 = vmovl_u16(vget_low_u16(t1));
    return vreinterpretq_s32_u32(t2);
}

__forceinline __m128i _mm_load4epu16_epi32(__m128i *ptr)
{
    uint16x8_t t0 = vld1q_u16((uint16_t*)ptr);
    uint32x4_t t1 = vmovl_u16(vget_low_u16(t0));
    return vreinterpretq_s32_u32(t1);
}

__forceinline __m128i _mm_load4epi8_f32(__m128i *ptr)
{
    int8x8_t    t0 = vld1_s8((int8_t*)ptr);
    int16x8_t   t1 = vmovl_s8(t0);
    int32x4_t   t2 = vmovl_s16(vget_low_s16(t1));
    float32x4_t t3 = vcvtq_f32_s32(t2);
    return vreinterpretq_s32_f32(t3);
}

__forceinline __m128i _mm_load4epu8_f32(__m128i *ptr)
{
    uint8x8_t   t0 = vld1_u8((uint8_t*)ptr);
    uint16x8_t  t1 = vmovl_u8(t0);
    uint32x4_t  t2 = vmovl_u16(vget_low_u16(t1));
    return vreinterpretq_s32_u32(t2);
}

__forceinline __m128i _mm_load4epi16_f32(__m128i *ptr)
{
    int16x8_t   t0 = vld1q_s16((int16_t*)ptr);
    int32x4_t   t1 = vmovl_s16(vget_low_s16(t0));
    float32x4_t t2 = vcvtq_f32_s32(t1);
    return vreinterpretq_s32_f32(t2);
}

#if defined(__aarch64__)
#include "../..//math/constants.h"
__forceinline int _mm_movemask_popcnt_ps(__m128 a)
{
    uint32x4_t t2 = vandq_u32(vreinterpretq_u32_f32(a), embree::movemask_mask);
    t2 = vreinterpretq_u32_u8(vcntq_u8(vreinterpretq_u8_u32(t2)));
    return vaddvq_u32(t2);

}
#endif
