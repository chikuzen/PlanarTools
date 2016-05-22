/*
proc_planar_to_packed.cpp

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


#ifndef PLANAR_TO_PACKED_H
#define PLANAR_TO_PACKED_H


#include "simd.h"


template <int MODE>
static void __stdcall
planar_to_yuy2(int width, int height, const uint8_t* srcpy, int pitch_y,
               const uint8_t* srcpu, int pitch_u, const uint8_t* srcpv,
               int pitch_v, uint8_t* dstp, int dst_pitch) noexcept
{
    const int w = (width + 7) / 16 * 16;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < w; x += 16) {
            __m128i y = load_reg(srcpy + x);
            __m128i u = load_l(srcpu + x / 2);
            __m128i v = load_l(srcpv + x / 2);

            __m128i uv = unpacklo8(u, v);
            __m128i yuv0 = unpacklo8(y, uv);
            __m128i yuv1 = unpackhi8(y, uv);

            stream_reg(dstp + 2 * x, yuv0);
            stream_reg(dstp + 2 * x + 16, yuv1);
        }
        if (MODE > 0) {
            __m128i y = load_reg(srcpy + w);
            __m128i u = load_l(srcpu + w / 2);
            __m128i v = load_l(srcpv + w / 2);

            __m128i uv = unpacklo8(u, v);
            __m128i yuv0 = unpacklo8(y, uv);
            stream_reg(dstp + 2 * w, yuv0);
        }
        srcpy += pitch_y;
        srcpu += pitch_u;
        srcpv += pitch_v;
        dstp += dst_pitch;
    }
}


static __forceinline void
runpack_x6(__m128i& a, __m128i& b, __m128i& c, __m128i& d, __m128i& e, __m128i& f)
{
    __m128i a0 = runpacklo8(a, b);
    __m128i b0 = runpacklo8(c, d);
    __m128i c0 = runpacklo8(e, f);
    __m128i d0 = runpackhi8(a, b);
    __m128i e0 = runpackhi8(c, d);
    __m128i f0 = runpackhi8(e, f);

    __m128i a1 = runpacklo8(a0, b0);
    __m128i b1 = runpacklo8(c0, d0);
    __m128i c1 = runpacklo8(e0, f0);
    __m128i d1 = runpackhi8(a0, b0);
    __m128i e1 = runpackhi8(c0, d0);
    __m128i f1 = runpackhi8(e0, f0);

    __m128i a2 = runpacklo8(a1, b1);
    __m128i b2 = runpacklo8(c1, d1);
    __m128i c2 = runpacklo8(e1, f1);
    __m128i d2 = runpackhi8(a1, b1);
    __m128i e2 = runpackhi8(c1, d1);
    __m128i f2 = runpackhi8(e1, f1);

    __m128i a3 = runpacklo8(a2, b2);
    __m128i b3 = runpacklo8(c2, d2);
    __m128i c3 = runpacklo8(e2, f2);
    __m128i d3 = runpackhi8(a2, b2);
    __m128i e3 = runpackhi8(c2, d2);
    __m128i f3 = runpackhi8(e2, f2);

    a = runpacklo8(a3, b3);
    b = runpacklo8(c3, d3);
    c = runpacklo8(e3, f3);
    d = runpackhi8(a3, b3);
    e = runpackhi8(c3, d3);
    f = runpackhi8(e3, f3);
}


template <int MODE>
static void __stdcall
planar_to_bgr24(int width, int height, const uint8_t* srcpg, int pitch_g,
                const uint8_t* srcpb, int pitch_b, const uint8_t* srcpr,
                int pitch_r, uint8_t* dstp, int dst_pitch) noexcept
{
    const int w = (width + 5) / 32 * 32;
    dstp += dst_pitch * (height - 1);

    for (int y = 0; y < height; ++y) {
        __m128i s0, s1, s2, s3, s4, s5;
        for (int x = 0; x < w; x += 32) {
            s0 = load_reg(srcpb + x);
            s1 = load_reg(srcpb + x + 16);
            s2 = load_reg(srcpg + x);
            s3 = load_reg(srcpg + x + 16);
            s4 = load_reg(srcpr + x);
            s5 = load_reg(srcpr + x + 16);

            runpack_x6(s0, s1, s2, s3, s4, s5);

            stream_reg(dstp + 3 * x, s0);
            stream_reg(dstp + 3 * x + 16, s1);
            stream_reg(dstp + 3 * x + 32, s2);
            stream_reg(dstp + 3 * x + 48, s3);
            stream_reg(dstp + 3 * x + 64, s4);
            stream_reg(dstp + 3 * x + 80, s5);
        }

        if (MODE > 0) {
            s0 = load_reg(srcpb + w);
            s2 = load_reg(srcpg + w);
            s4 = load_reg(srcpr + w);
            if (MODE > 3) {
                s1 = load_reg(srcpb + w + 16);
                s3 = load_reg(srcpg + w + 16);
                s5 = load_reg(srcpr + w + 16);
            } else {
                s1 = s0;
                s3 = s2;
                s5 = s4;
            }

            runpack_x6(s0, s1, s2, s3, s4, s5);

            stream_reg(dstp + 3 * w, s0);
            if (MODE > 1) {
                stream_reg(dstp + 3 * w + 16, s1);
                if (MODE > 2) {
                    stream_reg(dstp + 3 * w + 32, s2);
                    if (MODE > 3) {
                        stream_reg(dstp + 3 * w + 48, s3);
                        if (MODE > 4) {
                            stream_reg(dstp + 3 * w + 64, s4);
                        }
                    }
                }
            }
        }
        srcpb += pitch_b;
        srcpg += pitch_g;
        srcpr += pitch_r;
        dstp -= dst_pitch;
    }
}


template <int MODE>
static void __stdcall
planar_to_bgr24_ssse3(int width, int height, const uint8_t* srcpg, int pitch_g,
                      const uint8_t* srcpb, int pitch_b, const uint8_t* srcpr,
                      int pitch_r, uint8_t* dstp, int dst_pitch) noexcept
{
    const int w = (width + 5) / 16 * 16;
    static const __m128i mask0 =
        _mm_setr_epi8(0, 6, 11, 1, 7, 12, 2, 8, 13, 3, 9, 14, 4, 10, 15, 5);
    static const __m128i mask1 =
        _mm_setr_epi8(5, 11, 0, 6, 12, 1, 7, 13, 2, 8, 14, 3, 9, 15, 4, 10);
    static const __m128i mask2 =
        _mm_setr_epi8(10, 0, 5, 11, 1, 6, 12, 2, 7, 13, 3, 8, 14, 4, 9, 15);

    dstp += dst_pitch * (height - 1);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < w; x += 16) {
            __m128i sb, sg, sr, t;

            sb = load_reg(srcpb + x);
            sg = load_reg(srcpg + x);
            sr = load_reg(srcpr + x);

            t = _mm_alignr_epi8(sr, _mm_slli_si128(sg, 11), 11);
            t = _mm_alignr_epi8(t, _mm_slli_si128(sb, 10), 10);
            t = _mm_shuffle_epi8(t, mask0);
            stream_reg(dstp + 3 * x, t);

            t = _mm_alignr_epi8(_mm_srli_si128(sr, 5), _mm_slli_si128(sg, 5), 10);
            t = _mm_alignr_epi8(t, _mm_slli_si128(sb, 5), 11);
            t = _mm_shuffle_epi8(t, mask1);
            stream_reg(dstp + 3 * x + 16, t);

            t = _mm_alignr_epi8(_mm_srli_si128(sr, 10), sg, 11);
            t = _mm_alignr_epi8(t, sb, 11);
            t = _mm_shuffle_epi8(t, mask2);
            stream_reg(dstp + 3 * x + 32, t);
        }
        if (MODE > 0) {
            for (int x = w; x < width; ++x) {
                dstp[3 * x + 0] = srcpb[x];
                dstp[3 * x + 1] = srcpg[x];
                dstp[3 * x + 2] = srcpr[x];
            }
        }
        srcpb += pitch_b;
        srcpg += pitch_g;
        srcpr += pitch_r;
        dstp -= dst_pitch;
    }
}

static __forceinline void
unpack_x4(__m128i& a, __m128i& b, __m128i& c, __m128i& d)
{
    __m128i ab0 = unpacklo8(a, b);
    __m128i ab1 = unpackhi8(a, b);
    __m128i cd0 = unpacklo8(c, d);
    __m128i cd1 = unpackhi8(c, d);

    a = unpacklo16(ab0, cd0);
    b = unpackhi16(ab0, cd0);
    c = unpacklo16(ab1, cd1);
    d = unpackhi16(ab1, cd1);

}


template <int MODE>
static void __stdcall
planar_to_bgr32(int width, int height, const uint8_t* srcpg, int pitch_g,
                const uint8_t* srcpb, int pitch_b, const uint8_t* srcpr,
                int pitch_r, const uint8_t* srcpa, int pitch_a, uint8_t* dstp,
                int dst_pitch) noexcept
{
    const int w = (width + 3) / 16 * 16;
    dstp += dst_pitch * (height - 1);

    for (int y = 0; y < height; ++y) {
        __m128i sb, sg, sr, sa;
        for (int x = 0; x < w; x += 16) {
            sb = load_reg(srcpb + x);
            sg = load_reg(srcpg + x);
            sr = load_reg(srcpr + x);
            sa = load_reg(srcpa + x);

            unpack_x4(sb, sg, sr, sa);

            stream_reg(dstp + 4 * x, sb);
            stream_reg(dstp + 4 * x + 16, sg);
            stream_reg(dstp + 4 * x + 32, sr);
            stream_reg(dstp + 4 * x + 48, sa);
        }
        if (MODE > 0) {
            sb = load_reg(srcpb + w);
            sg = load_reg(srcpb + w);
            sr = load_reg(srcpb + w);
            sa = load_reg(srcpb + w);

            unpack_x4(sb, sg, sr, sa);

            stream_reg(dstp + 4 * w, sb);
            if (MODE > 1) {
                stream_reg(dstp + 4 * w + 16, sg);
                if (MODE > 2) {
                    stream_reg(dstp + 4 * w + 32, sr);
                }
            }
        }
        srcpb += pitch_b;
        srcpg += pitch_g;
        srcpr += pitch_r;
        srcpa += pitch_a;
        dstp -= dst_pitch;
    }
}

#endif
