/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * Copyright 2021, Wabi.
 */

/**
 * @file
 * COVAH Library.
 * Gadget Vault.
 */

#ifndef __CLI_MATH_C__
#define __CLI_MATH_C__

#include <float.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "CLI_assert.h"
#include "CLI_math_inline.h"

#if defined(__ARM_NEON) && defined(WITH_SSE2NEON)
#  define SSE2NEON_PRECISE_MINMAX 1
#  define SSE2NEON_PRECISE_DIV 1
#  define SSE2NEON_PRECISE_SQRT 1
#  include <sse2neon.h>
#  define CLI_HAVE_SSE2
#elif defined(__SSE2__)
#  include <emmintrin.h>
#  define CLI_HAVE_SSE2
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __GNUC__
#  define UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#  define UNLIKELY(x) (x)
#endif

MINLINE float pow2f(float x)
{
  return x * x;
}
MINLINE float pow3f(float x)
{
  return pow2f(x) * x;
}
MINLINE float pow4f(float x)
{
  return pow2f(pow2f(x));
}
MINLINE float pow5f(float x)
{
  return pow4f(x) * x;
}
MINLINE float pow7f(float x)
{
  return pow2f(pow3f(x)) * x;
}

MINLINE float sqrt3f(float f)
{
  if (UNLIKELY(f == 0.0f)) {
    return 0.0f;
  }
  else if (UNLIKELY(f < 0.0f)) {
    return -(float)(exp(log(-f) / 3.0));
  }
  else {
    return (float)(exp(log(f) / 3.0));
  }
}

MINLINE double sqrt3d(double d)
{
  if (UNLIKELY(d == 0.0)) {
    return 0.0;
  }
  else if (UNLIKELY(d < 0.0)) {
    return -exp(log(-d) / 3.0);
  }
  else {
    return exp(log(d) / 3.0);
  }
}

MINLINE float sqrtf_signed(float f)
{
  return (f >= 0.0f) ? sqrtf(f) : -sqrtf(-f);
}

MINLINE float saacos(float fac)
{
  if (UNLIKELY(fac <= -1.0f)) {
    return (float)M_PI;
  }
  else if (UNLIKELY(fac >= 1.0f)) {
    return 0.0f;
  }
  else {
    return acosf(fac);
  }
}

MINLINE float saasin(float fac)
{
  if (UNLIKELY(fac <= -1.0f)) {
    return (float)-M_PI / 2.0f;
  }
  else if (UNLIKELY(fac >= 1.0f)) {
    return (float)M_PI / 2.0f;
  }
  else {
    return asinf(fac);
  }
}

MINLINE float sasqrt(float fac)
{
  if (UNLIKELY(fac <= 0.0f)) {
    return 0.0f;
  }
  else {
    return sqrtf(fac);
  }
}

MINLINE float saacosf(float fac)
{
  if (UNLIKELY(fac <= -1.0f)) {
    return (float)M_PI;
  }
  else if (UNLIKELY(fac >= 1.0f)) {
    return 0.0f;
  }
  else {
    return acosf(fac);
  }
}

MINLINE float saasinf(float fac)
{
  if (UNLIKELY(fac <= -1.0f)) {
    return (float)-M_PI / 2.0f;
  }
  else if (UNLIKELY(fac >= 1.0f)) {
    return (float)M_PI / 2.0f;
  }
  else {
    return asinf(fac);
  }
}

MINLINE float sasqrtf(float fac)
{
  if (UNLIKELY(fac <= 0.0f)) {
    return 0.0f;
  }
  else {
    return sqrtf(fac);
  }
}

MINLINE float interpf(float target, float origin, float fac)
{
  return (fac * target) + (1.0f - fac) * origin;
}

MINLINE double interpd(double target, double origin, double fac)
{
  return (fac * target) + (1.0f - fac) * origin;
}

MINLINE float ratiof(float min, float max, float pos)
{
  float range = max - min;
  return range == 0 ? 0 : ((pos - min) / range);
}

MINLINE double ratiod(double min, double max, double pos)
{
  double range = max - min;
  return range == 0 ? 0 : ((pos - min) / range);
}

MINLINE float scalenorm(float a, float b, float x)
{
  CLI_assert(x <= 1 && x >= 0);
  return (x * (b - a)) + a;
}

MINLINE float power_of_2(float val)
{
  return (float)pow(2.0, ceil(log((double)val) / M_LN2));
}

MINLINE int is_power_of_2_i(int n)
{
  return (n & (n - 1)) == 0;
}

MINLINE int power_of_2_max_i(int n)
{
  if (is_power_of_2_i(n)) {
    return n;
  }

  do {
    n = n & (n - 1);
  } while (!is_power_of_2_i(n));

  return n * 2;
}

MINLINE int power_of_2_min_i(int n)
{
  while (!is_power_of_2_i(n)) {
    n = n & (n - 1);
  }

  return n;
}

MINLINE unsigned int power_of_2_max_u(unsigned int x)
{
  x -= 1;
  x |= (x >> 1);
  x |= (x >> 2);
  x |= (x >> 4);
  x |= (x >> 8);
  x |= (x >> 16);
  return x + 1;
}

MINLINE unsigned int power_of_2_min_u(unsigned int x)
{
  x |= (x >> 1);
  x |= (x >> 2);
  x |= (x >> 4);
  x |= (x >> 8);
  x |= (x >> 16);
  return x - (x >> 1);
}

MINLINE unsigned int log2_floor_u(unsigned int x)
{
  return x <= 1 ? 0 : 1 + log2_floor_u(x >> 1);
}

MINLINE unsigned int log2_ceil_u(unsigned int x)
{
  if (is_power_of_2_i((int)x)) {
    return log2_floor_u(x);
  }
  else {
    return log2_floor_u(x) + 1;
  }
}

#define _round_clamp_fl_impl(arg, ty, min, max) \
  { \
    float r = floorf(arg + 0.5f); \
    if (UNLIKELY(r <= (float)min)) { \
      return (ty)min; \
    } \
    else if (UNLIKELY(r >= (float)max)) { \
      return (ty)max; \
    } \
    else { \
      return (ty)r; \
    } \
  }

#define _round_clamp_db_impl(arg, ty, min, max) \
  { \
    double r = floor(arg + 0.5); \
    if (UNLIKELY(r <= (double)min)) { \
      return (ty)min; \
    } \
    else if (UNLIKELY(r >= (double)max)) { \
      return (ty)max; \
    } \
    else { \
      return (ty)r; \
    } \
  }

#define _round_fl_impl(arg, ty) \
  { \
    return (ty)floorf(arg + 0.5f); \
  }
#define _round_db_impl(arg, ty) \
  { \
    return (ty)floor(arg + 0.5); \
  }

MINLINE signed char round_fl_to_char(float a){_round_fl_impl(a, signed char)} MINLINE
  unsigned char round_fl_to_uchar(float a){_round_fl_impl(a, unsigned char)} MINLINE
  short round_fl_to_short(float a){_round_fl_impl(a, short)} MINLINE
  unsigned short round_fl_to_ushort(float a){_round_fl_impl(a, unsigned short)} MINLINE
  int round_fl_to_int(float a){_round_fl_impl(a, int)} MINLINE
  unsigned int round_fl_to_uint(float a){_round_fl_impl(a, unsigned int)}

MINLINE signed char round_db_to_char(double a){_round_db_impl(a, signed char)} MINLINE
  unsigned char round_db_to_uchar(double a){_round_db_impl(a, unsigned char)} MINLINE
  short round_db_to_short(double a){_round_db_impl(a, short)} MINLINE
  unsigned short round_db_to_ushort(double a){_round_db_impl(a, unsigned short)} MINLINE
  int round_db_to_int(double a){_round_db_impl(a, int)} MINLINE unsigned int round_db_to_uint(double a)
{
  _round_db_impl(a, unsigned int)
}

#undef _round_fl_impl
#undef _round_db_impl

MINLINE signed char round_fl_to_char_clamp(float a){
  _round_clamp_fl_impl(a, signed char, SCHAR_MIN, SCHAR_MAX)} MINLINE
  unsigned char round_fl_to_uchar_clamp(float a){
    _round_clamp_fl_impl(a, unsigned char, 0, UCHAR_MAX)} MINLINE
  short round_fl_to_short_clamp(float a){_round_clamp_fl_impl(a, short, SHRT_MIN, SHRT_MAX)} MINLINE
  unsigned short round_fl_to_ushort_clamp(float a){
    _round_clamp_fl_impl(a, unsigned short, 0, USHRT_MAX)} MINLINE
  int round_fl_to_int_clamp(float a){_round_clamp_fl_impl(a, int, INT_MIN, INT_MAX)} MINLINE
  unsigned int round_fl_to_uint_clamp(float a){_round_clamp_fl_impl(a, unsigned int, 0, UINT_MAX)}

MINLINE signed char round_db_to_char_clamp(double a){
  _round_clamp_db_impl(a, signed char, SCHAR_MIN, SCHAR_MAX)} MINLINE
  unsigned char round_db_to_uchar_clamp(double a){
    _round_clamp_db_impl(a, unsigned char, 0, UCHAR_MAX)} MINLINE
  short round_db_to_short_clamp(double a){_round_clamp_db_impl(a, short, SHRT_MIN, SHRT_MAX)} MINLINE
  unsigned short round_db_to_ushort_clamp(double a){
    _round_clamp_db_impl(a, unsigned short, 0, USHRT_MAX)} MINLINE
  int round_db_to_int_clamp(double a){_round_clamp_db_impl(a, int, INT_MIN, INT_MAX)} MINLINE
  unsigned int round_db_to_uint_clamp(double a)
{
  _round_clamp_db_impl(a, unsigned int, 0, UINT_MAX)
}

#undef _round_clamp_fl_impl
#undef _round_clamp_db_impl

MINLINE int divide_round_i(int a, int b)
{
  return (2 * a + b) / (2 * b);
}

MINLINE int divide_floor_i(int a, int b)
{
  int d = a / b;
  int r = a % b;
  return r ? d - ((a < 0) ^ (b < 0)) : d;
}

MINLINE uint divide_ceil_u(uint a, uint b)
{
  return (a + b - 1) / b;
}

MINLINE int mod_i(int i, int n)
{
  return (i % n + n) % n;
}

MINLINE float fractf(float a)
{
  return a - floorf(a);
}

MINLINE float wrapf(float value, float max, float min)
{
  float range = max - min;
  return (range != 0.0f) ? value - (range * floorf((value - min) / range)) : min;
}

MINLINE float pingpongf(float value, float scale)
{
  if (scale == 0.0f) {
    return 0.0f;
  }
  return fabsf(fractf((value - scale) / (scale * 2.0f)) * scale * 2.0f - scale);
}

MINLINE int square_s(short a)
{
  return a * a;
}

MINLINE int square_i(int a)
{
  return a * a;
}

MINLINE unsigned int square_uint(unsigned int a)
{
  return a * a;
}

MINLINE int square_uchar(unsigned char a)
{
  return a * a;
}

MINLINE float square_f(float a)
{
  return a * a;
}

MINLINE double square_d(double a)
{
  return a * a;
}

MINLINE int cube_s(short a)
{
  return a * a * a;
}

MINLINE int cube_i(int a)
{
  return a * a * a;
}

MINLINE unsigned int cube_uint(unsigned int a)
{
  return a * a * a;
}

MINLINE int cube_uchar(unsigned char a)
{
  return a * a * a;
}

MINLINE float cube_f(float a)
{
  return a * a * a;
}

MINLINE double cube_d(double a)
{
  return a * a * a;
}

MINLINE float min_ff(float a, float b)
{
  return (a < b) ? a : b;
}
MINLINE float max_ff(float a, float b)
{
  return (a > b) ? a : b;
}

MINLINE float smoothminf(float a, float b, float c)
{
  if (c != 0.0f) {
    float h = max_ff(c - fabsf(a - b), 0.0f) / c;
    return min_ff(a, b) - h * h * h * c * (1.0f / 6.0f);
  }
  else {
    return min_ff(a, b);
  }
}

MINLINE double min_dd(double a, double b)
{
  return (a < b) ? a : b;
}
MINLINE double max_dd(double a, double b)
{
  return (a > b) ? a : b;
}

MINLINE int min_ii(int a, int b)
{
  return (a < b) ? a : b;
}
MINLINE int max_ii(int a, int b)
{
  return (b < a) ? a : b;
}

MINLINE uint min_uu(uint a, uint b)
{
  return (a < b) ? a : b;
}
MINLINE uint max_uu(uint a, uint b)
{
  return (b < a) ? a : b;
}

MINLINE float min_fff(float a, float b, float c)
{
  return min_ff(min_ff(a, b), c);
}
MINLINE float max_fff(float a, float b, float c)
{
  return max_ff(max_ff(a, b), c);
}

MINLINE int min_iii(int a, int b, int c)
{
  return min_ii(min_ii(a, b), c);
}
MINLINE int max_iii(int a, int b, int c)
{
  return max_ii(max_ii(a, b), c);
}

MINLINE float min_ffff(float a, float b, float c, float d)
{
  return min_ff(min_fff(a, b, c), d);
}
MINLINE float max_ffff(float a, float b, float c, float d)
{
  return max_ff(max_fff(a, b, c), d);
}

MINLINE int min_iiii(int a, int b, int c, int d)
{
  return min_ii(min_iii(a, b, c), d);
}
MINLINE int max_iiii(int a, int b, int c, int d)
{
  return max_ii(max_iii(a, b, c), d);
}

MINLINE size_t min_zz(size_t a, size_t b)
{
  return (a < b) ? a : b;
}
MINLINE size_t max_zz(size_t a, size_t b)
{
  return (b < a) ? a : b;
}

MINLINE char min_cc(char a, char b)
{
  return (a < b) ? a : b;
}
MINLINE char max_cc(char a, char b)
{
  return (b < a) ? a : b;
}

MINLINE int clamp_i(int value, int min, int max)
{
  return min_ii(max_ii(value, min), max);
}

MINLINE float clamp_f(float value, float min, float max)
{
  if (value > max) {
    return max;
  }
  else if (value < min) {
    return min;
  }
  return value;
}

MINLINE size_t clamp_z(size_t value, size_t min, size_t max)
{
  return min_zz(max_zz(value, min), max);
}

MINLINE int compare_ff(float a, float b, const float max_diff)
{
  return fabsf(a - b) <= max_diff;
}

MINLINE int compare_ff_relative(float a, float b, const float max_diff, const int max_ulps)
{
  union {
    float f;
    int i;
  } ua, ub;

  CLI_assert(sizeof(float) == sizeof(int));
  CLI_assert(max_ulps < (1 << 22));

  if (fabsf(a - b) <= max_diff) {
    return 1;
  }

  ua.f = a;
  ub.f = b;

  return ((ua.i < 0) != (ub.i < 0)) ? 0 : (abs(ua.i - ub.i) <= max_ulps) ? 1 : 0;
}

MINLINE float signf(float f)
{
  return (f < 0.0f) ? -1.0f : 1.0f;
}

MINLINE float compatible_signf(float f)
{
  if (f > 0.0f) {
    return 1.0f;
  }
  if (f < 0.0f) {
    return -1.0f;
  }
  else {
    return 0.0f;
  }
}

MINLINE int signum_i_ex(float a, float eps)
{
  if (a > eps) {
    return 1;
  }
  if (a < -eps) {
    return -1;
  }
  else {
    return 0;
  }
}

MINLINE int signum_i(float a)
{
  if (a > 0.0f) {
    return 1;
  }
  if (a < 0.0f) {
    return -1;
  }
  else {
    return 0;
  }
}

MINLINE int integer_digits_f(const float f)
{
  return (f == 0.0f) ? 0 : (int)floor(log10(fabs(f))) + 1;
}

MINLINE int integer_digits_d(const double d)
{
  return (d == 0.0) ? 0 : (int)floor(log10(fabs(d))) + 1;
}

MINLINE int integer_digits_i(const int i)
{
  return (int)log10((double)i) + 1;
}

#ifdef CLI_HAVE_SSE2

MALWAYS_INLINE __m128 _cli_math_fastpow(const int exp, const int e2coeff, const __m128 arg)
{
  __m128 ret;
  ret = _mm_mul_ps(arg, _mm_castsi128_ps(_mm_set1_epi32(e2coeff)));
  ret = _mm_cvtepi32_ps(_mm_castps_si128(ret));
  ret = _mm_mul_ps(ret, _mm_castsi128_ps(_mm_set1_epi32(exp)));
  ret = _mm_castsi128_ps(_mm_cvtps_epi32(ret));
  return ret;
}

MALWAYS_INLINE __m128 _cli_math_improve_5throot_solution(const __m128 old_result, const __m128 x)
{
  __m128 approx2 = _mm_mul_ps(old_result, old_result);
  __m128 approx4 = _mm_mul_ps(approx2, approx2);
  __m128 t = _mm_div_ps(x, approx4);
  __m128 summ = _mm_add_ps(_mm_mul_ps(_mm_set1_ps(4.0f), old_result), t); /* FMA. */
  return _mm_mul_ps(summ, _mm_set1_ps(1.0f / 5.0f));
}

MALWAYS_INLINE __m128 _cli_math_fastpow24(const __m128 arg)
{
  __m128 x = _cli_math_fastpow(0x3F4CCCCD, 0x4F55A7FB, arg);
  __m128 arg2 = _mm_mul_ps(arg, arg);
  __m128 arg4 = _mm_mul_ps(arg2, arg2);
  x = _cli_math_improve_5throot_solution(x, arg4);
  x = _cli_math_improve_5throot_solution(x, arg4);
  x = _cli_math_improve_5throot_solution(x, arg4);
  return _mm_mul_ps(x, _mm_mul_ps(x, x));
}

MALWAYS_INLINE __m128 _cli_math_fastpow512(const __m128 arg)
{
  __m128 xf = _cli_math_fastpow(0x3f2aaaab, 0x5eb504f3, arg);
  __m128 xover = _mm_mul_ps(arg, xf);
  __m128 xfm1 = _mm_rsqrt_ps(xf);
  __m128 x2 = _mm_mul_ps(arg, arg);
  __m128 xunder = _mm_mul_ps(x2, xfm1);
  /* sqrt2 * over + 2 * sqrt2 * under */
  __m128 xavg = _mm_mul_ps(_mm_set1_ps(1.0f / (3.0f * 0.629960524947437f) * 0.999852f),
                           _mm_add_ps(xover, xunder));
  xavg = _mm_mul_ps(xavg, _mm_rsqrt_ps(xavg));
  xavg = _mm_mul_ps(xavg, _mm_rsqrt_ps(xavg));
  return xavg;
}

MALWAYS_INLINE __m128 _cli_math_blend_sse(const __m128 mask, const __m128 a, const __m128 b)
{
  return _mm_or_ps(_mm_and_ps(mask, a), _mm_andnot_ps(mask, b));
}

#endif

MINLINE unsigned char unit_float_to_uchar_clamp(float val)
{
  return (
    unsigned char)(((val <= 0.0f) ? 0 : ((val > (1.0f - 0.5f / 255.0f)) ? 255 : ((255.0f * val) + 0.5f))));
}
#define unit_float_to_uchar_clamp(val) ((CHECK_TYPE_INLINE(val, float)), unit_float_to_uchar_clamp(val))

MINLINE unsigned short unit_float_to_ushort_clamp(float val)
{
  return (unsigned short)((val >= 1.0f - 0.5f / 65535) ? 65535 :
                          (val <= 0.0f)                ? 0 :
                                                         (val * 65535.0f + 0.5f));
}
#define unit_float_to_ushort_clamp(val) ((CHECK_TYPE_INLINE(val, float)), unit_float_to_ushort_clamp(val))

MINLINE unsigned char unit_ushort_to_uchar(unsigned short val)
{
  return (unsigned char)(((val) >= 65535 - 128) ? 255 : ((val) + 128) >> 8);
}
#define unit_ushort_to_uchar(val) ((CHECK_TYPE_INLINE(val, unsigned short)), unit_ushort_to_uchar(val))

#define unit_float_to_uchar_clamp_v3(v1, v2) \
  { \
    (v1)[0] = unit_float_to_uchar_clamp((v2[0])); \
    (v1)[1] = unit_float_to_uchar_clamp((v2[1])); \
    (v1)[2] = unit_float_to_uchar_clamp((v2[2])); \
  } \
  ((void)0)
#define unit_float_to_uchar_clamp_v4(v1, v2) \
  { \
    (v1)[0] = unit_float_to_uchar_clamp((v2[0])); \
    (v1)[1] = unit_float_to_uchar_clamp((v2[1])); \
    (v1)[2] = unit_float_to_uchar_clamp((v2[2])); \
    (v1)[3] = unit_float_to_uchar_clamp((v2[3])); \
  } \
  ((void)0)

#ifdef __cplusplus
}
#endif

#endif /* __CLI_MATH_C__ */