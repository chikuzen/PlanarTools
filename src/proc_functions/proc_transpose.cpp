/*
proc_transpose.h

This file is part of PlanarTools

Copyright (C) 2015 OKA Motofumi

Author: OKA Motofumi (chikuzen.mo at gmail dot com)

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02111, USA.
*/


#include "proc_functions.h"
#include "simd.h"

#if 0
static void __stdcall
transpose_8x8(const uint8_t* srcp, uint8_t* dstp, const int width,
              const int height, const int src_pitch, const int dst_pitch)
{
    const int h = height / 8 * 8;
    const int w = width / 8 * 8;

    const uint8_t* s = srcp;

    for (int y = 0; y < h; y += 8) {
        uint8_t* d = dstp + y;
        for (int x = 0; x < w; x += 8) {
            __m128i s0 = _mm_loadl_epi64((__m128i*)(s + x + 0 * src_pitch));
            __m128i s1 = _mm_loadl_epi64((__m128i*)(s + x + 1 * src_pitch));
            __m128i s2 = _mm_loadl_epi64((__m128i*)(s + x + 2 * src_pitch));
            __m128i s3 = _mm_loadl_epi64((__m128i*)(s + x + 3 * src_pitch));
            __m128i s4 = _mm_loadl_epi64((__m128i*)(s + x + 4 * src_pitch));
            __m128i s5 = _mm_loadl_epi64((__m128i*)(s + x + 5 * src_pitch));
            __m128i s6 = _mm_loadl_epi64((__m128i*)(s + x + 6 * src_pitch));
            __m128i s7 = _mm_loadl_epi64((__m128i*)(s + x + 7 * src_pitch));

            __m128i ab07 = unpacklo8(s0, s1);
            __m128i cd07 = unpacklo8(s2, s3);
            __m128i ef07 = unpacklo8(s4, s5);
            __m128i gh07 = unpacklo8(s6, s7);

            __m128i abcd03 = unpacklo16(ab07, cd07);
            __m128i efgh03 = unpacklo16(ef07, gh07);
            __m128i abcd47 = unpackhi16(ab07, cd07);
            __m128i efgh47 = unpackhi16(ef07, gh07);

            __m128i abcdefgh01 = unpacklo32(abcd03, efgh03);
            __m128i abcdefgh23 = unpackhi32(abcd03, efgh03);
            __m128i abcdefgh45 = unpacklo32(abcd47, efgh47);
            __m128i abcdefgh67 = unpackhi32(abcd47, efgh47);


            _mm_storel_epi64((__m128i*)(d + 0 * dst_pitch), abcdefgh01);
            _mm_storel_epi64((__m128i*)(d + 1 * dst_pitch), movehl(abcdefgh01));
            _mm_storel_epi64((__m128i*)(d + 2 * dst_pitch), abcdefgh23);
            _mm_storel_epi64((__m128i*)(d + 3 * dst_pitch), movehl(abcdefgh23));
            _mm_storel_epi64((__m128i*)(d + 4 * dst_pitch), abcdefgh45);
            _mm_storel_epi64((__m128i*)(d + 5 * dst_pitch), movehl(abcdefgh45));
            _mm_storel_epi64((__m128i*)(d + 6 * dst_pitch), abcdefgh67);
            _mm_storel_epi64((__m128i*)(d + 7 * dst_pitch), movehl(abcdefgh67));

            d += dst_pitch * 8;
        }
        s += src_pitch * 8;
    }

    
    for (int y = h; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            dstp[y + x * dst_pitch] = srcp[x + y * src_pitch];
        }
    }

    if (w != width) {
        for (int y = 0; y < height; ++y) {
            for (int x = w; x < width; ++x) {
                dstp[y + x * dst_pitch] = srcp[x + y * src_pitch];
            }
        }
    }

}
#endif

static void __stdcall
transpose_16x8(const uint8_t* srcp, uint8_t* dstp, const int width,
               const int height, const int src_pitch, const int dst_pitch)
{
    const int h = height / 8 * 8;
    const int w = width / 16 * 16;

    const uint8_t* s = srcp;

    for (int y = 0; y < h; y += 8) {
        uint8_t* d = dstp + y;
        for (int x = 0; x < w; x += 16) {
            __m128i s0 = load_reg((__m128i*)(s + x + 0 * src_pitch));
            __m128i s1 = load_reg((__m128i*)(s + x + 1 * src_pitch));
            __m128i ab07 = unpacklo8(s0, s1);
            __m128i ab8F = unpackhi8(s0, s1);

            s0 = load_reg((__m128i*)(s + x + 2 * src_pitch));
            s1 = load_reg((__m128i*)(s + x + 3 * src_pitch));
            __m128i cd07 = unpacklo8(s0, s1);
            __m128i cd8F = unpackhi8(s0, s1);

            s0 = load_reg((__m128i*)(s + x + 4 * src_pitch));
            s1 = load_reg((__m128i*)(s + x + 5 * src_pitch));
            __m128i ef07 = unpacklo8(s0, s1);
            __m128i ef8F = unpackhi8(s0, s1);

            s0 = load_reg((__m128i*)(s + x + 6 * src_pitch));
            s1 = load_reg((__m128i*)(s + x + 7 * src_pitch));
            __m128i gh07 = unpacklo8(s0, s1);
            __m128i gh8F = unpackhi8(s0, s1);

            __m128i abcdCF = unpackhi16(ab8F, cd8F);
            __m128i efghCF = unpackhi16(ef8F, gh8F);

            __m128i abcdefghCD = unpacklo32(abcdCF, efghCF);
            _mm_storel_epi64((__m128i*)(d + 12 * dst_pitch), abcdefghCD);
            _mm_storel_epi64((__m128i*)(d + 13 * dst_pitch), movehl(abcdefghCD));
            __m128i abcdefghEF = unpackhi32(abcdCF, efghCF);
            _mm_storel_epi64((__m128i*)(d + 14 * dst_pitch), abcdefghEF);
            _mm_storel_epi64((__m128i*)(d + 15 * dst_pitch), movehl(abcdefghEF));

            __m128i abcd8B = unpacklo16(ab8F, cd8F);
            __m128i efgh8B = unpacklo16(ef8F, gh8F);

            __m128i abcdefgh89 = unpacklo32(abcd8B, efgh8B);
            _mm_storel_epi64((__m128i*)(d + 8 * dst_pitch), abcdefgh89);
            _mm_storel_epi64((__m128i*)(d + 9 * dst_pitch), movehl(abcdefgh89));
            __m128i abcdefghAB = unpackhi32(abcd8B, efgh8B);
            _mm_storel_epi64((__m128i*)(d + 10 * dst_pitch), abcdefghAB);
            _mm_storel_epi64((__m128i*)(d + 11 * dst_pitch), movehl(abcdefghAB));

            __m128i abcd47 = unpackhi16(ab07, cd07);
            __m128i efgh47 = unpackhi16(ef07, gh07);

            __m128i abcdefgh45 = unpacklo32(abcd47, efgh47);
            _mm_storel_epi64((__m128i*)(d + 4 * dst_pitch), abcdefgh45);
            _mm_storel_epi64((__m128i*)(d + 5 * dst_pitch), movehl(abcdefgh45));
            __m128i abcdefgh67 = unpackhi32(abcd47, efgh47);
            _mm_storel_epi64((__m128i*)(d + 6 * dst_pitch), abcdefgh67);
            _mm_storel_epi64((__m128i*)(d + 7 * dst_pitch), movehl(abcdefgh67));

            __m128i abcd03 = unpacklo16(ab07, cd07);
            __m128i efgh03 = unpacklo16(ef07, gh07);

            __m128i abcdefgh01 = unpacklo32(abcd03, efgh03);
            _mm_storel_epi64((__m128i*)(d + 0 * dst_pitch), abcdefgh01);
            _mm_storel_epi64((__m128i*)(d + 1 * dst_pitch), movehl(abcdefgh01));
            __m128i abcdefgh23 = unpackhi32(abcd03, efgh03);
            _mm_storel_epi64((__m128i*)(d + 2 * dst_pitch), abcdefgh23);
            _mm_storel_epi64((__m128i*)(d + 3 * dst_pitch), movehl(abcdefgh23));

            d += dst_pitch * 15;
        }
        s += src_pitch * 8;
    }


    for (int y = h; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            dstp[y + x * dst_pitch] = srcp[x + y * src_pitch];
        }
    }

    if (w != width) {
        for (int y = 0; y < height; ++y) {
            for (int x = w; x < width; ++x) {
                dstp[y + x * dst_pitch] = srcp[x + y * src_pitch];
            }
        }
    }

}


static void __stdcall
transpose_bgra(const uint8_t* srcp, uint8_t* dstp, const int rowsize,
               const int height, const int src_pitch, const int dst_pitch)
{
    const int h = height / 4 * 4;
    const int r = rowsize / 16 * 16;

    const uint8_t* s = srcp;

    for (int y = 0; y < h; y += 4) {
        uint8_t* d = dstp + 4 * y;
        for (int x = 0; x < r; x += 16) {
            __m128i s0 = load_reg((__m128i*)(s + x + 0 * src_pitch));
            __m128i s1 = load_reg((__m128i*)(s + x + 1 * src_pitch));
            __m128i s2 = load_reg((__m128i*)(s + x + 2 * src_pitch));
            __m128i s3 = load_reg((__m128i*)(s + x + 3 * src_pitch));

            __m128i ab01 = unpacklo32(s0, s1);
            __m128i ab23 = unpackhi32(s0, s1);
            __m128i cd01 = unpacklo32(s2, s3);
            __m128i cd23 = unpackhi32(s2, s3);

            __m128i abcd0 = unpacklo64(ab01, cd01);
            __m128i abcd1 = unpackhi64(ab01, cd01);
            __m128i abcd2 = unpacklo64(ab23, cd23);
            __m128i abcd3 = unpackhi64(ab23, cd23);

            store_reg((__m128i*)(d + 0 * dst_pitch), abcd0);
            store_reg((__m128i*)(d + 1 * dst_pitch), abcd1);
            store_reg((__m128i*)(d + 2 * dst_pitch), abcd2);
            store_reg((__m128i*)(d + 3 * dst_pitch), abcd3);

            d += 4 * dst_pitch;
        }
        s += 4 * src_pitch;
    }

    if (r == rowsize && h == height)
        return;

    int width = rowsize / 4;
    const uint32_t* s4 = (uint32_t*)srcp;
    uint32_t* d4 = (uint32_t*)dstp;
    int sp = src_pitch / 4;
    int dp = dst_pitch / 4;

    for (int y = h; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            d4[y + x * dp] = s4[x + y * sp];
        }
    }

    if (r < rowsize) {
        for (int y = 0; y < height; ++y) {
            for (int x = r / 4; x < width; ++x) {
                d4[y + x * dp] = s4[x + y * sp];
            }
        }
    }

}


proc_transpose get_transpose_function(int pixel_type)
{
    switch (pixel_type) {
    case VideoInfo::CS_BGR32:
        return transpose_bgra;
    case VideoInfo::CS_YUY2:
    case VideoInfo::CS_YV16:
    case VideoInfo::CS_YV411:
    case VideoInfo::CS_BGR24:
        return nullptr;
    default:
        return transpose_16x8;
    }
}
