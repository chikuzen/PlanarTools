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


#include "proc_functions.h"
#include "simd.h"

static void __stdcall
planar_to_yuy2(int width, int height, const uint8_t* srcpy, int pitch_y,
               const uint8_t* srcpu, int pitch_u, const uint8_t* srcpv,
               int pitch_v, uint8_t* dstp, int dst_pitch)
{
    const int w = (width + 7) / 16 * 16;
    const int mod = width - w;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < w; x += 16) {
            __m128i y = load_reg((__m128i*)(srcpy + x));
            __m128i u = _mm_loadl_epi64((__m128i*)(srcpu + x / 2));
            __m128i v = _mm_loadl_epi64((__m128i*)(srcpv + x / 2));

            __m128i uv = unpacklo8(u, v);
            __m128i yuv0 = unpacklo8(y, uv);
            __m128i yuv1 = unpackhi8(y, uv);

            stream_reg((__m128i*)(dstp + 2 * x), yuv0);
            stream_reg((__m128i*)(dstp + 2 * x + 16), yuv1);
        }
        if (mod > 0) {
            __m128i y = load_reg((__m128i*)(srcpy + w));
            __m128i u = _mm_loadl_epi64((__m128i*)(srcpu + w / 2));
            __m128i v = _mm_loadl_epi64((__m128i*)(srcpv + w / 2));

            __m128i uv = unpacklo8(u, v);
            __m128i yuv0 = unpacklo8(y, uv);
            stream_reg((__m128i*)(dstp + 2 * w), yuv0);
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


static void __stdcall
planar_to_bgr24(int width, int height, const uint8_t* srcpg, int pitch_g,
                const uint8_t* srcpb, int pitch_b, const uint8_t* srcpr,
                int pitch_r, uint8_t* dstp, int dst_pitch)
{
    const int w = (width + 5) / 32 * 32;
    dstp += dst_pitch * (height - 1);

    const int mod = width - w;

    for (int y = 0; y < height; ++y) {
        __m128i s0, s1, s2, s3, s4, s5;
        for (int x = 0; x < w; x += 32) {
            s0 = load_reg((__m128i *)(srcpb + x) + 0);
            s1 = load_reg((__m128i *)(srcpb + x) + 1);
            s2 = load_reg((__m128i *)(srcpg + x) + 0);
            s3 = load_reg((__m128i *)(srcpg + x) + 1);
            s4 = load_reg((__m128i *)(srcpr + x) + 0);
            s5 = load_reg((__m128i *)(srcpr + x) + 1);

            runpack_x6(s0, s1, s2, s3, s4, s5);

            stream_reg((__m128i*)(dstp + 3 * x) + 0, s0);
            stream_reg((__m128i*)(dstp + 3 * x) + 1, s1);
            stream_reg((__m128i*)(dstp + 3 * x) + 2, s2);
            stream_reg((__m128i*)(dstp + 3 * x) + 3, s3);
            stream_reg((__m128i*)(dstp + 3 * x) + 4, s4);
            stream_reg((__m128i*)(dstp + 3 * x) + 5, s5);
        }

        if (mod > 0) {
            s0 = load_reg((__m128i*)(srcpb + w));
            s2 = load_reg((__m128i*)(srcpg + w));
            s4 = load_reg((__m128i*)(srcpr + w));
            if (mod > 16) {
                s1 = load_reg((__m128i*)(srcpb + w) + 1);
                s3 = load_reg((__m128i*)(srcpg + w) + 1);
                s5 = load_reg((__m128i*)(srcpr + w) + 1);
            } else {
                s1 = s0;
                s3 = s2;
                s5 = s4;
            }

            runpack_x6(s0, s1, s2, s3, s4, s5);

            stream_reg((__m128i*)(dstp + 3 * w), s0);
            if (mod > 5) {
                stream_reg((__m128i*)(dstp + 3 * w) + 1, s1);
                if (mod > 10) {
                    stream_reg((__m128i*)(dstp + 3 * w) + 2, s2);
                    if (mod > 16) {
                        stream_reg((__m128i*)(dstp + 3 * w) + 3, s3);
                        if (mod > 21) {
                            stream_reg((__m128i*)(dstp + 3 * w) + 4, s4);
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


static void __stdcall
planar_to_bgr32(int width, int height, const uint8_t* srcpg, int pitch_g,
                const uint8_t* srcpb, int pitch_b, const uint8_t* srcpr,
                int pitch_r, const uint8_t* srcpa, int pitch_a, uint8_t* dstp,
                int dst_pitch)
{
    const int w = (width + 3) / 16 * 16;
    dstp += dst_pitch * (height - 1);

    const int mod = width - w;

    for (int y = 0; y < height; ++y) {
        __m128i sb, sg, sr, sa;
        for (int x = 0; x < w; x += 16) {
            sb = load_reg((__m128i*)(srcpb + x));
            sg = load_reg((__m128i*)(srcpg + x));
            sr = load_reg((__m128i*)(srcpr + x));
            sa = load_reg((__m128i*)(srcpa + x));

            unpack_x4(sb, sg, sr, sa);

            stream_reg((__m128i*)(dstp + 4 * x) + 0, sb);
            stream_reg((__m128i*)(dstp + 4 * x) + 1, sg);
            stream_reg((__m128i*)(dstp + 4 * x) + 2, sr);
            stream_reg((__m128i*)(dstp + 4 * x) + 3, sa);
        }
        if (mod > 0) {
            sb = load_reg((__m128i*)(srcpb + w));
            sg = load_reg((__m128i*)(srcpb + w));
            sr = load_reg((__m128i*)(srcpb + w));
            sa = load_reg((__m128i*)(srcpb + w));

            unpack_x4(sb, sg, sr, sa);

            stream_reg((__m128i*)(dstp + 4 * w), sb);
            if (mod > 4) {
                stream_reg((__m128i*)(dstp + 4 * w) + 1, sg);
                if (mod > 8) {
                    stream_reg((__m128i*)(dstp + 4 * w) + 2, sr);
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


planar_to_packed get_packed_converter(int pixel_type)
{
    switch (pixel_type) {
    case VideoInfo::CS_BGR24:
        return planar_to_bgr24;
    case VideoInfo::CS_YUY2:
        return planar_to_yuy2;
    default:
        return nullptr;
    }
}

planar_to_bgra get_bgra_converter()
{
    return planar_to_bgr32;
}