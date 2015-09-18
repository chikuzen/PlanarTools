/*
proc_packed_to_planar.cpp

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


#include <map>
#include <tuple>
#include "proc_functions.h"
#include "simd.h"


static __forceinline void
unpack_x6(__m128i& a, __m128i& b, __m128i& c, __m128i& d, __m128i& e, __m128i& f)
{
    __m128i a0 = unpacklo8(a, d);
    __m128i b0 = unpackhi8(a, d);
    __m128i c0 = unpacklo8(b, e);
    __m128i d0 = unpackhi8(b, e);
    __m128i e0 = unpacklo8(c, f);
    __m128i f0 = unpackhi8(c, f);

    __m128i a1 = unpacklo8(a0, d0);
    __m128i b1 = unpackhi8(a0, d0);
    __m128i c1 = unpacklo8(b0, e0);
    __m128i d1 = unpackhi8(b0, e0);
    __m128i e1 = unpacklo8(c0, f0);
    __m128i f1 = unpackhi8(c0, f0);

    a0 = unpacklo8(a1, d1);
    b0 = unpackhi8(a1, d1);
    c0 = unpacklo8(b1, e1);
    d0 = unpackhi8(b1, e1);
    e0 = unpacklo8(c1, f1);
    f0 = unpackhi8(c1, f1);

    a1 = unpacklo8(a0, d0);
    b1 = unpackhi8(a0, d0);
    c1 = unpacklo8(b0, e0);
    d1 = unpackhi8(b0, e0);
    e1 = unpacklo8(c0, f0);
    f1 = unpackhi8(c0, f0);

    a = unpacklo8(a1, d1);
    b = unpackhi8(a1, d1);
    c = unpacklo8(b1, e1);
    d = unpackhi8(b1, e1);
    e = unpacklo8(c1, f1);
    f = unpackhi8(c1, f1);
}


template <int MODE>
static void __stdcall
bgr24_to_yv24(const uint8_t* srcp, int width, int height, int src_pitch,
              uint8_t* dstpg, int pitch_g, uint8_t* dstpb, int pitch_b,
              uint8_t* dstpr, int pitch_r)
{
    const int w = (width + 5) / 32 * 32;
    srcp += src_pitch * (height - 1);

    for (int y = 0; y < height; ++y) {
        __m128i s0, s1, s2, s3, s4, s5;
        for (int x = 0; x < w; x += 32) {
            s0 = load_reg((__m128i*)(srcp + 3 * x) + 0);
            s1 = load_reg((__m128i*)(srcp + 3 * x) + 1);
            s2 = load_reg((__m128i*)(srcp + 3 * x) + 2);
            s3 = load_reg((__m128i*)(srcp + 3 * x) + 3);
            s4 = load_reg((__m128i*)(srcp + 3 * x) + 4);
            s5 = load_reg((__m128i*)(srcp + 3 * x) + 5);

            unpack_x6(s0, s1, s2, s3, s4, s5);

            stream_reg((__m128i*)(dstpb + x) + 0, s0);
            stream_reg((__m128i*)(dstpb + x) + 1, s1);
            stream_reg((__m128i*)(dstpg + x) + 0, s2);
            stream_reg((__m128i*)(dstpg + x) + 1, s3);
            stream_reg((__m128i*)(dstpr + x) + 0, s4);
            stream_reg((__m128i*)(dstpr + x) + 1, s5);
        }

        if (MODE > 0) {
            s0 = load_reg((__m128i*)(srcp + 3 * w) +  0);
            s1 = MODE > 1 ? load_reg((__m128i*)(srcp + 3 * w) + 1) : s0;
            s2 = MODE > 2 ? load_reg((__m128i*)(srcp + 3 * w) + 2) : s1;
            s3 = MODE > 3 ? load_reg((__m128i*)(srcp + 3 * w) + 3) : s2;
            s4 = MODE > 4 ? load_reg((__m128i*)(srcp + 3 * w) + 4) : s3;
            s5 = s4;

            unpack_x6(s0, s1, s2, s3, s4, s5);

            stream_reg((__m128i*)(dstpb + w), s0);
            stream_reg((__m128i*)(dstpg + w), s2);
            stream_reg((__m128i*)(dstpr + w), s4);
            if (MODE > 3) {
                stream_reg((__m128i*)(dstpb + w + 16), s1);
                stream_reg((__m128i*)(dstpg + w + 16), s3);
                stream_reg((__m128i*)(dstpr + w + 16), s5);
            }
        }
        srcp -= src_pitch;
        dstpb += pitch_b;
        dstpg += pitch_g;
        dstpr += pitch_r;
    }
}


static __forceinline void
unpack_x4(__m128i& a, __m128i& b, __m128i& c, __m128i& d)
{
    __m128i a0 = unpacklo8(a, c);
    __m128i b0 = unpackhi8(a, c);
    __m128i c0 = unpacklo8(b, d);
    __m128i d0 = unpackhi8(b, d);

    __m128i a1 = unpacklo8(a0, c0);
    __m128i b1 = unpackhi8(a0, c0);
    __m128i c1 = unpacklo8(b0, d0);
    __m128i d1 = unpackhi8(b0, d0);

    __m128i a2 = unpacklo8(a1, c1);
    __m128i b2 = unpackhi8(a1, c1);
    __m128i c2 = unpacklo8(b1, d1);
    __m128i d2 = unpackhi8(b1, d1);

    a = unpacklo8(a2, c2);
    b = unpackhi8(a2, c2);
    c = unpacklo8(b2, d2);
}

template <int MODE>
static void __stdcall
bgr32_to_yv24(const uint8_t* srcp, int width, int height, int src_pitch,
              uint8_t* dstpg, int pitch_g, uint8_t* dstpb, int pitch_b,
              uint8_t* dstpr, int pitch_r)
{
    const int w = (width + 3) / 16 * 16;
    srcp += src_pitch * (height - 1);

    for (int y = 0; y < height; ++y) {
        __m128i s0, s1, s2, s3;
        for (int x = 0; x < w; x += 16) {
            s0 = load_reg((__m128i*)(srcp + 4 * x) + 0);
            s1 = load_reg((__m128i*)(srcp + 4 * x) + 1);
            s2 = load_reg((__m128i*)(srcp + 4 * x) + 2);
            s3 = load_reg((__m128i*)(srcp + 4 * x) + 3);

            unpack_x4(s0, s1, s2, s3);

            stream_reg((__m128i*)(dstpb + x), s0);
            stream_reg((__m128i*)(dstpg + x), s1);
            stream_reg((__m128i*)(dstpr + x), s2);
        }
        if (MODE > 0) {
            s0 = load_reg((__m128i*)(srcp + 4 * w) + 0);
            s1 = MODE > 1 ? load_reg((__m128i*)(srcp + 4 * w) + 1) : s0;
            s2 = MODE > 2 ? load_reg((__m128i*)(srcp + 4 * w) + 2) : s1;
            s3 = s2;

            unpack_x4(s0, s1, s2, s3);

            stream_reg((__m128i*)(dstpb + w), s0);
            stream_reg((__m128i*)(dstpg + w), s1);
            stream_reg((__m128i*)(dstpr + w), s2);
        }
        srcp -= src_pitch;
        dstpb += pitch_b;
        dstpg += pitch_g;
        dstpr += pitch_r;
    }
}


static __forceinline void
unpack_x4_yuy2(__m128i& a, __m128i& b, __m128i& c, __m128i& d)
{
    __m128i a0 = unpacklo8(a, c);
    __m128i b0 = unpackhi8(a, c);
    __m128i c0 = unpacklo8(b, d);
    __m128i d0 = unpackhi8(b, d);

    __m128i a1 = unpacklo8(a0, c0);
    __m128i b1 = unpackhi8(a0, c0);
    __m128i c1 = unpacklo8(b0, d0);
    __m128i d1 = unpackhi8(b0, d0);

    __m128i a2 = unpacklo8(a1, c1);
    __m128i b2 = unpackhi8(a1, c1);
    __m128i c2 = unpacklo8(b1, d1);
    __m128i d2 = unpackhi8(b1, d1);

    __m128i a3 = unpacklo8(a2, c2);
    c = unpackhi8(a2, c2);
    __m128i c3 = unpacklo8(b2, d2);
    d = unpackhi8(b2, d2);

    a = unpacklo8(a3, c3);
    b = unpackhi8(a3, c3);
}

template <int MODE>
static void __stdcall
yuy2_to_yv16(const uint8_t* srcp, int width, int height, int src_pitch,
             uint8_t* dstpy, int pitch_y, uint8_t* dstpu, int pitch_u,
             uint8_t* dstpv, int pitch_v)
{
    const int w = (width + 7) / 32 * 32;

    for (int y = 0; y < height; ++y) {
        __m128i s0, s1, s2, s3;
        for (int x = 0; x < w; x += 32) {
            s0 = load_reg((__m128i*)(srcp + 2 * x) + 0);
            s1 = load_reg((__m128i*)(srcp + 2 * x) + 1);
            s2 = load_reg((__m128i*)(srcp + 2 * x) + 2);
            s3 = load_reg((__m128i*)(srcp + 2 * x) + 3);

            unpack_x4_yuy2(s0, s1, s2, s3);

            stream_reg((__m128i*)(dstpy + x), s0);
            stream_reg((__m128i*)(dstpy + x + 16), s1);
            stream_reg((__m128i*)(dstpu + x / 2), s2);
            stream_reg((__m128i*)(dstpv + x / 2), s3);
        }
        if (MODE > 0) {
            s0 = load_reg((__m128i*)(srcp + 2 * w) + 0);
            s1 = MODE > 1 ? load_reg((__m128i*)(srcp + 2 * w) + 1) : s0;
            s2 = MODE > 2 ? load_reg((__m128i*)(srcp + 2 * w) + 2) : s1;
            s3 = s2;

            unpack_x4_yuy2(s0, s1, s2, s3);

            stream_reg((__m128i*)(dstpy + w), s0);
            if (MODE > 2) {
                stream_reg((__m128i*)(dstpy + w + 16), s1);
            }
            stream_reg((__m128i*)(dstpu + w / 2), s2);
            stream_reg((__m128i*)(dstpv + w / 2), s3);
        }
        srcp += src_pitch;
        dstpy += pitch_y;
        dstpu += pitch_u;
        dstpv += pitch_v;
    }
}

packed_to_planar get_planar_converter(int pixel_type, int width)
{
    using std::make_pair;

    std::map<std::pair<int, int>, packed_to_planar> func;

    func[make_pair(VideoInfo::CS_BGR32, 0)] = bgr32_to_yv24<0>;
    func[make_pair(VideoInfo::CS_BGR32, 1)] = bgr32_to_yv24<1>;
    func[make_pair(VideoInfo::CS_BGR32, 2)] = bgr32_to_yv24<2>;
    func[make_pair(VideoInfo::CS_BGR32, 3)] = bgr32_to_yv24<3>;

    func[make_pair(VideoInfo::CS_BGR24, 0)] = bgr24_to_yv24<0>;
    func[make_pair(VideoInfo::CS_BGR24, 1)] = bgr24_to_yv24<1>;
    func[make_pair(VideoInfo::CS_BGR24, 2)] = bgr24_to_yv24<2>;
    func[make_pair(VideoInfo::CS_BGR24, 3)] = bgr24_to_yv24<3>;
    func[make_pair(VideoInfo::CS_BGR24, 4)] = bgr24_to_yv24<4>;
    func[make_pair(VideoInfo::CS_BGR24, 5)] = bgr24_to_yv24<5>;

    func[make_pair(VideoInfo::CS_YUY2, 0)] = yuy2_to_yv16<0>;
    func[make_pair(VideoInfo::CS_YUY2, 1)] = yuy2_to_yv16<1>;
    func[make_pair(VideoInfo::CS_YUY2, 2)] = yuy2_to_yv16<2>;
    func[make_pair(VideoInfo::CS_YUY2, 3)] = yuy2_to_yv16<3>;

    int mode;
    if (pixel_type == VideoInfo::CS_BGR32) {
        int w = width - (width + 3) / 16 * 16;
        mode = w > 8 ? 3 : w > 4 ? 2 : w > 0 ? 1 : 0;
    } else if (pixel_type == VideoInfo::CS_BGR24) {
        int w = width - (width + 5) / 32 * 32;
        mode = w > 21 ? 5 : w > 16 ? 4 : w > 10 ? 3 : w > 5 ? 2 : w > 0 ? 1 : 0;
    } else {
        int w = width - (width + 7) / 32 * 32;
        mode = w > 16 ? 3 : w > 8 ? 2 : w > 0 ? 1 : 0;
    }

    return func[make_pair(pixel_type, mode)];
}


template <int PLANE>
static __forceinline void
extract_x6_sse2(__m128i& a, __m128i& b, __m128i& c, __m128i& d, __m128i& e, __m128i& f)
{
    __m128i a0 = unpacklo8(a, d);
    __m128i b0 = unpackhi8(a, d);
    __m128i c0 = unpacklo8(b, e);
    __m128i d0 = unpackhi8(b, e);
    __m128i e0 = unpacklo8(c, f);
    __m128i f0 = unpackhi8(c, f);

    __m128i a1 = unpacklo8(a0, d0);
    __m128i b1 = unpackhi8(a0, d0);
    __m128i c1 = unpacklo8(b0, e0);
    __m128i d1 = unpackhi8(b0, e0);
    __m128i e1 = unpacklo8(c0, f0);
    __m128i f1 = unpackhi8(c0, f0);

    if (PLANE == 0) {
        a0 = unpacklo8(a1, d1);
        b0 = unpackhi8(a1, d1);
        c0 = unpackhi8(b1, e1);
        d0 = unpacklo8(c1, f1);
        e0 = unpacklo8(a0, c0);
        f0 = unpackhi8(b0, d0);
    } else if (PLANE == 1) {
        a0 = unpacklo8(a1, d1);
        b0 = unpacklo8(b1, e1);
        c0 = unpackhi8(b1, e1);
        d0 = unpackhi8(c1, f1);
        e0 = unpackhi8(a0, c0);
        f0 = unpacklo8(b0, d0);
    } else {
        a0 = unpackhi8(a1, d1);
        b0 = unpacklo8(b1, e1);
        c0 = unpacklo8(c1, f1);
        d0 = unpackhi8(c1, f1);
        e0 = unpacklo8(a0, c0);
        f0 = unpackhi8(b0, d0);
    }

    a = unpacklo8(e0, f0);
    b = unpackhi8(e0, f0);
}


template <int PLANE>
static __forceinline void
extract_x6_ssse3(__m128i& a, __m128i& b, __m128i& c, __m128i& d, __m128i& e, __m128i& f)
{
    static const __m128i m0 = _mm_setr_epi8(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 6, 9, 12, 15);
    static const __m128i m1 = _mm_setr_epi8(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 5, 8, 11, 14);
    static const __m128i m2 = _mm_setr_epi8(1, 4, 7, 10, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

    __m128i a0 = _mm_shuffle_epi8(a, PLANE == 0 ? m0 : PLANE == 1 ? m2 : m1);
    __m128i b0 = _mm_shuffle_epi8(b, PLANE == 0 ? m1 : PLANE == 1 ? m0 : m2);
    __m128i c0 = _mm_shuffle_epi8(c, PLANE == 0 ? m2 : PLANE == 1 ? m1 : m0);
    a = _mm_alignr_epi8(_mm_alignr_epi8(c0, b0, 11), a0, 10);

    a0 = _mm_shuffle_epi8(d, PLANE == 0 ? m0 : PLANE == 1 ? m2 : m1);
    b0 = _mm_shuffle_epi8(e, PLANE == 0 ? m1 : PLANE == 1 ? m0 : m2);
    c0 = _mm_shuffle_epi8(f, PLANE == 0 ? m2 : PLANE == 1 ? m1 : m0);
    b = _mm_alignr_epi8(_mm_alignr_epi8(c0, b0, 11), a0, 10);
}


template <int PLANE, int MODE, bool SSSE3>
static void __stdcall
extract_plane_bgr24(const uint8_t* srcp, int width, int height, int src_pitch,
                    uint8_t* dstp, int dst_pitch)
{
    const int w = (width + 5) / 32 * 32;
    srcp += src_pitch * (height - 1);

    __m128i s0, s1, s2, s3, s4, s5;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < w; x += 32) {
            s0 = load_reg((__m128i*)(srcp + 3 * x) + 0);
            s1 = load_reg((__m128i*)(srcp + 3 * x) + 1);
            s2 = load_reg((__m128i*)(srcp + 3 * x) + 2);
            s3 = load_reg((__m128i*)(srcp + 3 * x) + 3);
            s4 = load_reg((__m128i*)(srcp + 3 * x) + 4);
            s5 = load_reg((__m128i*)(srcp + 3 * x) + 5);

            if (SSSE3) {
                extract_x6_ssse3<PLANE>(s0, s1, s2, s3, s4, s5);
            } else {
                extract_x6_sse2<PLANE>(s0, s1, s2, s3, s4, s5);
            }

            stream_reg((__m128i*)(dstp + x), s0);
            stream_reg((__m128i*)(dstp + x + 16), s1);
        }
        if (MODE > 0) {
            s0 = load_reg((__m128i*)(srcp + 3 * w) + 0);
            s1 = MODE > 1 ? load_reg((__m128i*)(srcp + 3 * w) + 1) : s0;
            s2 = MODE > 2 ? load_reg((__m128i*)(srcp + 3 * w) + 2) : s1;
            s3 = MODE > 3 ? load_reg((__m128i*)(srcp + 3 * w) + 3) : s2;
            s4 = MODE > 4 ? load_reg((__m128i*)(srcp + 3 * w) + 4) : s3;
            s5 = s4;

            if (SSSE3) {
                extract_x6_ssse3<PLANE>(s0, s1, s2, s3, s4, s5);
            } else {
                extract_x6_sse2<PLANE>(s0, s1, s2, s3, s4, s5);
            }

            stream_reg((__m128i*)(dstp + w), s0);
            if (MODE > 3) {
                stream_reg((__m128i*)(dstp + w + 16), s1);
            }
        }
        srcp -= src_pitch;
        dstp += dst_pitch;
    }
}


template <int PLANE, int MODE>
static void __stdcall
extract_plane_bgr32(const uint8_t* srcp, int width, int height, int src_pitch,
                    uint8_t* dstp, int dst_pitch)
{
    const __m128i mask = _mm_set1_epi32(0xFF);

    const int w = (width + 3) / 16 * 16;
    srcp += src_pitch * (height - 1);

    for (int y = 0; y < height; ++y) {
        __m128i s0, s1, s2, s3, t0, t1;
        for (int x = 0; x < w; x += 16) {
            if (PLANE == 0) {
                s0 = mask & load_reg((__m128i*)(srcp + 4 * x) + 0);
                s1 = mask & load_reg((__m128i*)(srcp + 4 * x) + 1);
                s2 = mask & load_reg((__m128i*)(srcp + 4 * x) + 2);
                s3 = mask & load_reg((__m128i*)(srcp + 4 * x) + 3);
            } else {
                s0 = mask & _mm_srli_epi32(load_reg((__m128i*)(srcp + 4 * x) + 0), PLANE * 8);
                s1 = mask & _mm_srli_epi32(load_reg((__m128i*)(srcp + 4 * x) + 1), PLANE * 8);
                s2 = mask & _mm_srli_epi32(load_reg((__m128i*)(srcp + 4 * x) + 2), PLANE * 8);
                s3 = mask & _mm_srli_epi32(load_reg((__m128i*)(srcp + 4 * x) + 3), PLANE * 8);
            }

            t0 = _mm_packs_epi32(s0, s1);
            t1 = _mm_packs_epi32(s2, s3);
            t0 = _mm_packus_epi16(t0, t1);

            stream_reg((__m128i*)(dstp + x), t0);
        }
        if (MODE > 0) {
            if (PLANE == 0) {
                s0 = mask & load_reg((__m128i*)(srcp + 4 * w) + 0);
                s1 = MODE > 1 ? mask & load_reg((__m128i*)(srcp + 4 * w) + 1) : s0;
                s2 = MODE > 2 ? mask & load_reg((__m128i*)(srcp + 4 * w) + 2) : s0;
            } else {
                s0 = mask & _mm_srli_epi32(load_reg((__m128i*)(srcp + 4 * w) + 0), PLANE * 8);
                s1 = MODE > 1 ? mask & _mm_srli_epi32(load_reg((__m128i*)(srcp + 4 * w) + 1), PLANE * 8) : s0;
                s2 = MODE > 2 ? mask & _mm_srli_epi32(load_reg((__m128i*)(srcp + 4 * w) + 2), PLANE * 8) : s1;
            }

            t0 = _mm_packs_epi32(s0, s1);
            t1 = _mm_packs_epi32(s2, s2);
            t0 = _mm_packus_epi16(t0, t1);

            stream_reg((__m128i*)(dstp + w), t0);
        }
        srcp -= src_pitch;
        dstp += dst_pitch;
    }
}


template <int MODE>
static void __stdcall
extract_plane_yuy2_y(const uint8_t* srcp, int width, int height, int src_pitch,
                     uint8_t* dstp, int dst_pitch)
{
    const __m128i mask = _mm_set1_epi16(0xFF);

    const int w = (width + 7) / 16 * 16;

    const int mod = width - w;

    for (int y = 0; y < height; ++y) {
        __m128i s0, s1;
        for (int x = 0; x < w; x += 16) {
            s0 = mask & load_reg((__m128i*)(srcp + 2 * x));
            s1 = mask & load_reg((__m128i*)(srcp + 2 * x + 16));

            s0 = _mm_packus_epi16(s0, s1);

            stream_reg((__m128i*)(dstp + x), s0);
        }
        if (MODE > 0) {
            s0 = mask & load_reg((__m128i*)(srcp + 2 * w));

            s0 = _mm_packus_epi16(s0, s0);

            stream_reg((__m128i*)(dstp + w), s0);
        }
        srcp += src_pitch;
        dstp += dst_pitch;
    }
}


template <int PLANE, int MODE>
static void __stdcall
extract_plane_yuy2_uv(const uint8_t* srcp, int width, int height, int src_pitch,
                      uint8_t* dstp, int dst_pitch)
{
    const __m128i mask = _mm_set1_epi32(0xFF);

    const int w = (width + 7) / 32 * 32;

    for (int y = 0; y < height; ++y) {
        __m128i s0, s1, s2, s3;
        for (int x = 0; x < w; x += 32) {
            s0 = mask & _mm_srli_epi32(load_reg((__m128i*)(srcp + 2 * x) + 0), PLANE == 1 ? 8 : 24);
            s1 = mask & _mm_srli_epi32(load_reg((__m128i*)(srcp + 2 * x) + 1), PLANE == 1 ? 8 : 24);
            s2 = mask & _mm_srli_epi32(load_reg((__m128i*)(srcp + 2 * x) + 2), PLANE == 1 ? 8 : 24);
            s3 = mask & _mm_srli_epi32(load_reg((__m128i*)(srcp + 2 * x) + 3), PLANE == 1 ? 8 : 24);

            s0 = _mm_packs_epi32(s0, s1);
            s2 = _mm_packs_epi32(s2, s3);
            s0 = _mm_packus_epi16(s0, s2);

            stream_reg((__m128i*)(dstp + x / 2), s0);
        }
        if (MODE > 0) {
            s0 = mask & _mm_srli_epi32(load_reg((__m128i*)(srcp + 2 * w) + 0), PLANE == 1 ? 8 : 24);
            s1 = MODE > 1 ? mask & _mm_srli_epi32(load_reg((__m128i*)(srcp + 2 * w) + 1), PLANE == 1 ? 8 : 24) : s0;
            s2 = MODE > 2 ? mask & _mm_srli_epi32(load_reg((__m128i*)(srcp + 2 * w) + 2), PLANE == 1 ? 8 : 24) : s1;

            s0 = _mm_packs_epi32(s0, s1);
            s2 = _mm_packs_epi32(s2, s2);
            s0 = _mm_packus_epi16(s0, s2);

            stream_reg((__m128i*)(dstp + w / 2), s0);
        }
        srcp += src_pitch;
        dstp += dst_pitch;
    }
}



extract_plane get_extractor(int pixel_type, int width, int plane, bool ssse3)
{
    using std::make_tuple;

    std::map<std::tuple<int, int, int, bool>, extract_plane> func;

    func[make_tuple(VideoInfo::CS_BGR32, 0, 0, false)] = extract_plane_bgr32<0, 0>;
    func[make_tuple(VideoInfo::CS_BGR32, 0, 1, false)] = extract_plane_bgr32<0, 1>;
    func[make_tuple(VideoInfo::CS_BGR32, 0, 2, false)] = extract_plane_bgr32<0, 2>;
    func[make_tuple(VideoInfo::CS_BGR32, 0, 3, false)] = extract_plane_bgr32<0, 3>;

    func[make_tuple(VideoInfo::CS_BGR32, 1, 0, false)] = extract_plane_bgr32<1, 0>;
    func[make_tuple(VideoInfo::CS_BGR32, 1, 1, false)] = extract_plane_bgr32<1, 1>;
    func[make_tuple(VideoInfo::CS_BGR32, 1, 2, false)] = extract_plane_bgr32<1, 2>;
    func[make_tuple(VideoInfo::CS_BGR32, 1, 3, false)] = extract_plane_bgr32<1, 3>;

    func[make_tuple(VideoInfo::CS_BGR32, 2, 0, false)] = extract_plane_bgr32<2, 0>;
    func[make_tuple(VideoInfo::CS_BGR32, 2, 1, false)] = extract_plane_bgr32<2, 1>;
    func[make_tuple(VideoInfo::CS_BGR32, 2, 2, false)] = extract_plane_bgr32<2, 2>;
    func[make_tuple(VideoInfo::CS_BGR32, 2, 3, false)] = extract_plane_bgr32<2, 3>;

    func[make_tuple(VideoInfo::CS_BGR32, 3, 0, false)] = extract_plane_bgr32<3, 0>;
    func[make_tuple(VideoInfo::CS_BGR32, 3, 1, false)] = extract_plane_bgr32<3, 1>;
    func[make_tuple(VideoInfo::CS_BGR32, 3, 2, false)] = extract_plane_bgr32<3, 2>;
    func[make_tuple(VideoInfo::CS_BGR32, 3, 3, false)] = extract_plane_bgr32<3, 3>;

    func[make_tuple(VideoInfo::CS_BGR24, 0, 0, true)] = extract_plane_bgr24<0, 0, true>;
    func[make_tuple(VideoInfo::CS_BGR24, 0, 1, true)] = extract_plane_bgr24<0, 1, true>;
    func[make_tuple(VideoInfo::CS_BGR24, 0, 2, true)] = extract_plane_bgr24<0, 2, true>;
    func[make_tuple(VideoInfo::CS_BGR24, 0, 3, true)] = extract_plane_bgr24<0, 3, true>;
    func[make_tuple(VideoInfo::CS_BGR24, 0, 4, true)] = extract_plane_bgr24<0, 4, true>;
    func[make_tuple(VideoInfo::CS_BGR24, 0, 5, true)] = extract_plane_bgr24<0, 5, true>;

    func[make_tuple(VideoInfo::CS_BGR24, 1, 0, true)] = extract_plane_bgr24<1, 0, true>;
    func[make_tuple(VideoInfo::CS_BGR24, 1, 1, true)] = extract_plane_bgr24<1, 1, true>;
    func[make_tuple(VideoInfo::CS_BGR24, 1, 2, true)] = extract_plane_bgr24<1, 2, true>;
    func[make_tuple(VideoInfo::CS_BGR24, 1, 3, true)] = extract_plane_bgr24<1, 3, true>;
    func[make_tuple(VideoInfo::CS_BGR24, 1, 4, true)] = extract_plane_bgr24<1, 4, true>;
    func[make_tuple(VideoInfo::CS_BGR24, 1, 5, true)] = extract_plane_bgr24<1, 5, true>;

    func[make_tuple(VideoInfo::CS_BGR24, 2, 0, true)] = extract_plane_bgr24<0, 0, true>;
    func[make_tuple(VideoInfo::CS_BGR24, 2, 1, true)] = extract_plane_bgr24<0, 1, true>;
    func[make_tuple(VideoInfo::CS_BGR24, 2, 2, true)] = extract_plane_bgr24<0, 2, true>;
    func[make_tuple(VideoInfo::CS_BGR24, 2, 3, true)] = extract_plane_bgr24<0, 3, true>;
    func[make_tuple(VideoInfo::CS_BGR24, 2, 4, true)] = extract_plane_bgr24<0, 4, true>;
    func[make_tuple(VideoInfo::CS_BGR24, 2, 5, true)] = extract_plane_bgr24<0, 5, true>;

    func[make_tuple(VideoInfo::CS_BGR24, 0, 0, false)] = extract_plane_bgr24<0, 0, false>;
    func[make_tuple(VideoInfo::CS_BGR24, 0, 1, false)] = extract_plane_bgr24<0, 1, false>;
    func[make_tuple(VideoInfo::CS_BGR24, 0, 2, false)] = extract_plane_bgr24<0, 2, false>;
    func[make_tuple(VideoInfo::CS_BGR24, 0, 3, false)] = extract_plane_bgr24<0, 3, false>;
    func[make_tuple(VideoInfo::CS_BGR24, 0, 4, false)] = extract_plane_bgr24<0, 4, false>;
    func[make_tuple(VideoInfo::CS_BGR24, 0, 5, false)] = extract_plane_bgr24<0, 5, false>;

    func[make_tuple(VideoInfo::CS_BGR24, 1, 0, false)] = extract_plane_bgr24<1, 0, false>;
    func[make_tuple(VideoInfo::CS_BGR24, 1, 1, false)] = extract_plane_bgr24<1, 1, false>;
    func[make_tuple(VideoInfo::CS_BGR24, 1, 2, false)] = extract_plane_bgr24<1, 2, false>;
    func[make_tuple(VideoInfo::CS_BGR24, 1, 3, false)] = extract_plane_bgr24<1, 3, false>;
    func[make_tuple(VideoInfo::CS_BGR24, 1, 4, false)] = extract_plane_bgr24<1, 4, false>;
    func[make_tuple(VideoInfo::CS_BGR24, 1, 5, false)] = extract_plane_bgr24<1, 5, false>;

    func[make_tuple(VideoInfo::CS_BGR24, 2, 0, false)] = extract_plane_bgr24<2, 0, false>;
    func[make_tuple(VideoInfo::CS_BGR24, 2, 1, false)] = extract_plane_bgr24<2, 1, false>;
    func[make_tuple(VideoInfo::CS_BGR24, 2, 2, false)] = extract_plane_bgr24<2, 2, false>;
    func[make_tuple(VideoInfo::CS_BGR24, 2, 3, false)] = extract_plane_bgr24<2, 3, false>;
    func[make_tuple(VideoInfo::CS_BGR24, 2, 4, false)] = extract_plane_bgr24<2, 4, false>;
    func[make_tuple(VideoInfo::CS_BGR24, 2, 5, false)] = extract_plane_bgr24<2, 5, false>;

    func[make_tuple(VideoInfo::CS_YUY2, 0, 0, false)] = extract_plane_yuy2_y<0>;
    func[make_tuple(VideoInfo::CS_YUY2, 0, 1, false)] = extract_plane_yuy2_y<1>;

    func[make_tuple(VideoInfo::CS_YUY2, 1, 0, false)] = extract_plane_yuy2_uv<1, 0>;
    func[make_tuple(VideoInfo::CS_YUY2, 1, 1, false)] = extract_plane_yuy2_uv<1, 1>;
    func[make_tuple(VideoInfo::CS_YUY2, 1, 2, false)] = extract_plane_yuy2_uv<1, 2>;
    func[make_tuple(VideoInfo::CS_YUY2, 1, 3, false)] = extract_plane_yuy2_uv<1, 3>;

    func[make_tuple(VideoInfo::CS_YUY2, 2, 0, false)] = extract_plane_yuy2_uv<2, 0>;
    func[make_tuple(VideoInfo::CS_YUY2, 2, 1, false)] = extract_plane_yuy2_uv<2, 1>;
    func[make_tuple(VideoInfo::CS_YUY2, 2, 2, false)] = extract_plane_yuy2_uv<2, 2>;
    func[make_tuple(VideoInfo::CS_YUY2, 2, 3, false)] = extract_plane_yuy2_uv<2, 3>;

    int mode;
    if (pixel_type == VideoInfo::CS_BGR32) {
        int w = width - (width + 3) / 16 * 16;
        mode = w > 8 ? 3 : w > 4 ? 2 : w > 0 ? 1 : 0;
        ssse3 = false;
    } else if (pixel_type == VideoInfo::CS_BGR24) {
        int w = width - (width + 5) / 32 * 32;
        mode = w > 21 ? 5 : w > 16 ? 4 : w > 10 ? 3 : w > 5 ? 2 : w > 0 ? 1 : 0;
    } else {
        ssse3 = false;
        if (plane > 0) {
            int w = width - (width + 7) / 32 * 32;
            mode = w > 16 ? 3 : w > 8 ? 2 : w > 0 ? 1 : 0;
        } else {
            mode = width - (width + 7) / 16 * 16 > 0 ? 1 : 0;
        }
    }

    return func[make_tuple(pixel_type, plane, mode, ssse3)];
}
