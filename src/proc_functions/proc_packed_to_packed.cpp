/*
proc_packed_to_packed.cpp

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
#include "proc_functions.h"
#include "simd.h"

#pragma warning(disable:4309)


template <int MODE>
static void __stdcall
bgr24_to_bgr32(const uint8_t* srcp, int width, int height, int src_pitch,
uint8_t* dstp, int dst_pitch)
{
    const int w = (width + 3) / 16 * 16;
    int mod = width - w;

    const __m128i m0 =
        _mm_setr_epi8(0, 1, 2, 0x80, 3, 4, 5, 0x80, 6, 7, 8, 0x80, 9, 10, 11, 0x80);
    const __m128i m1 =
        _mm_setr_epi8(4, 5, 6, 0x80, 7, 8, 9, 0x80, 10, 11, 12, 0x80, 13, 14, 15, 0x80);
    const __m128i alpha =
        _mm_set1_epi32(0xFF000000);

    for (int y = 0; y < height; ++y) {
        __m128i s0, s1, t, d;
        for (int x = 0; x < w; x += 16) {
            s0 = load_reg((__m128i*)(srcp + 3 * x) + 0);
            d = _mm_shuffle_epi8(s0, m0);
            stream_reg((__m128i*)(dstp + 4 * x) + 0, d | alpha);

            s1 = load_reg((__m128i*)(srcp + 3 * x) + 1);
            t = _mm_alignr_epi8(s1, s0, 12);
            d = _mm_shuffle_epi8(t, m0);
            stream_reg((__m128i*)(dstp + 4 * x) + 1, d | alpha);

            s0 = load_reg((__m128i*)(srcp + 3 * x) + 2);
            t = _mm_alignr_epi8(s0, s1, 8);
            d = _mm_shuffle_epi8(t, m0);
            stream_reg((__m128i*)(dstp + 4 * x) + 2, d | alpha);

            d = _mm_shuffle_epi8(s0, m1);
            stream_reg((__m128i*)(dstp + 4 * x) + 3, d | alpha);
        }
        if (MODE > 0) {
            s0 = load_reg((__m128i*)(srcp + 3 * w));
            d = _mm_shuffle_epi8(s0, m0);
            stream_reg((__m128i*)(dstp + 4 * w), d | alpha);

            if (MODE > 1) {
                s1 = MODE > 2 ? load_reg((__m128i*)(srcp + 3 * w) + 1) : s0;
                t = _mm_alignr_epi8(s1, s0, 12);
                d = _mm_shuffle_epi8(t, m0);
                stream_reg((__m128i*)(dstp + 4 * w) + 1, d | alpha);
            }
            if (MODE > 3) {
                s0 = MODE > 4 ? load_reg((__m128i*)(srcp + 3 * w) + 2) : s1;
                t = _mm_alignr_epi8(s0, s1, 8);
                d = _mm_shuffle_epi8(t, m0);
                stream_reg((__m128i*)(dstp + 4 * w) + 2, d | alpha);
            }
        }
        srcp += src_pitch;
        dstp += dst_pitch;
    }
}


template <int MODE>
static void __stdcall
bgr32_to_bgr24(const uint8_t* srcp, int width, int height, int src_pitch,
uint8_t* dstp, int dst_pitch)
{
    int w = (width + 3) / 16 * 16;
    int mod = width - w;

    const __m128i m0 =
        _mm_setr_epi8(3, 7, 11, 15, 0, 1, 2, 4, 5, 6, 8, 9, 10, 12, 13, 14);
    const __m128i m1 =
        _mm_setr_epi8(0, 1, 2, 4, 5, 6, 8, 9, 10, 12, 13, 14, 3, 7, 11, 15);

    for (int y = 0; y < height; ++y) {
        __m128i s0, s1, d;
        for (int x = 0; x < w; x += 16) {
            s0 = _mm_shuffle_epi8(load_reg((__m128i*)(srcp + 4 * x) + 0), m0);
            s1 = _mm_shuffle_epi8(load_reg((__m128i*)(srcp + 4 * x) + 1), m1);
            d = _mm_alignr_epi8(s1, s0, 4);
            stream_reg((__m128i*)(dstp + 3 * x), d);

            s0 = _mm_slli_si128(s1, 4);
            s1 = _mm_shuffle_epi8(load_reg((__m128i*)(srcp + 4 * x) + 2), m1);
            d = _mm_alignr_epi8(s1, s0, 8);
            stream_reg((__m128i*)(dstp + 3 * x) + 1, d);

            s0 = _mm_slli_si128(s1, 4);
            s1 = _mm_shuffle_epi8(load_reg((__m128i*)(srcp + 4 * x) + 3), m1);
            d = _mm_alignr_epi8(s1, s0, 12);
            stream_reg((__m128i*)(dstp + 3 * x) + 2, d);
        }
        if (MODE > 0) {
            s0 = load_reg((__m128i*)(srcp + 4 * w) + 0);
            if (mod == 1) {
                d = _mm_shuffle_epi8(s0, m1);
                stream_reg((__m128i*)(dstp + 3 * w), d);
            } else {
                s0 = _mm_shuffle_epi8(s0, m0);
                s1 = load_reg((__m128i*)(srcp + 4 * w) + 1);
                if (MODE == 2) {
                    d = _mm_alignr_epi8(s1, s0, 4);
                    stream_reg((__m128i*)(dstp + 3 * w), d);
                } else {
                    s1 = _mm_shuffle_epi8(s1, m1);
                    d = _mm_alignr_epi8(s1, s0, 4);
                    stream_reg((__m128i*)(dstp + 3 * w), d);
                    if (mod == 3){
                        d = _mm_srli_si128(s1, 4);
                        stream_reg((__m128i*)(dstp + 3 * w) + 1, d);
                    } else {
                        s0 = _mm_slli_si128(s1, 4);
                        s1 = load_reg((__m128i*)(srcp + 4 * w) + 2);
                        s1 = _mm_shuffle_epi8(s1, m1);
                        d = _mm_alignr_epi8(s1, s0, 8);
                        stream_reg((__m128i*)(dstp + 3 * w) + 1, d);
                        if (MODE == 5) {
                            d = _mm_srli_si128(s1, 8);
                            stream_reg((__m128i*)(dstp + 3 * w) + 2, d);
                        }
                    }
                }
            }
        }
        srcp += src_pitch;
        dstp += dst_pitch;
    }
}


packed_to_packed get_24_32_converter(int pixel_type, int width)
{
    using std::make_pair;
    std::map<std::pair<int, int>, packed_to_packed> func;

    func[make_pair(VideoInfo::CS_BGR24, 0)] = bgr24_to_bgr32<0>;
    func[make_pair(VideoInfo::CS_BGR24, 1)] = bgr24_to_bgr32<1>;
    func[make_pair(VideoInfo::CS_BGR24, 2)] = bgr24_to_bgr32<2>;
    func[make_pair(VideoInfo::CS_BGR24, 3)] = bgr24_to_bgr32<3>;
    func[make_pair(VideoInfo::CS_BGR24, 4)] = bgr24_to_bgr32<4>;
    func[make_pair(VideoInfo::CS_BGR24, 5)] = bgr24_to_bgr32<5>;

    func[make_pair(VideoInfo::CS_BGR32, 0)] = bgr32_to_bgr24<0>;
    func[make_pair(VideoInfo::CS_BGR32, 1)] = bgr32_to_bgr24<1>;
    func[make_pair(VideoInfo::CS_BGR32, 2)] = bgr32_to_bgr24<2>;
    func[make_pair(VideoInfo::CS_BGR32, 3)] = bgr32_to_bgr24<3>;
    func[make_pair(VideoInfo::CS_BGR32, 4)] = bgr32_to_bgr24<4>;
    func[make_pair(VideoInfo::CS_BGR32, 5)] = bgr32_to_bgr24<5>;

    int w = width - (width + 3) / 16 * 16;
    int mode;
    if (pixel_type == VideoInfo::CS_BGR24) {
        mode = w > 10 ? 5 : w > 8 ? 4 : w > 5 ? 3 : w > 4 ? 2 : w > 0 ? 1 : 0;
    } else {
        mode = w > 10 ? 5 : w > 8 ? 4 : w > 5 ? 3 : w == 5 ? 2 : w > 0 ? 1 : 0;
    }

    return func[make_pair(pixel_type, mode)];
}
