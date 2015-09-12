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

    for (int y = 0; y < height; ++y) {
        __m128i s0, s1, t, d;
        for (int x = 0; x < w; x += 16) {
            s0 = load_reg((__m128i*)(srcp + 3 * x) + 0);
            d = _mm_shuffle_epi8(s0, m0);
            stream_reg((__m128i*)(dstp + 4 * x) + 0, d);

            s1 = load_reg((__m128i*)(srcp + 3 * x) + 1);
            t = _mm_alignr_epi8(s1, s0, 12);
            d = _mm_shuffle_epi8(t, m0);
            stream_reg((__m128i*)(dstp + 4 * x) + 1, d);

            s0 = load_reg((__m128i*)(srcp + 3 * x) + 2);
            t = _mm_alignr_epi8(s0, s1, 8);
            d = _mm_shuffle_epi8(t, m0);
            stream_reg((__m128i*)(dstp + 4 * x) + 2, d);
            
            d = _mm_shuffle_epi8(s0, m1);
            stream_reg((__m128i*)(dstp + 4 * x) + 3, d);
        }
        if (MODE > 0) {
            s0 = load_reg((__m128i*)(srcp + 3 * w));
            d = _mm_shuffle_epi8(s0, m0);
            stream_reg((__m128i*)(dstp + 4 * w), d);

            if (MODE > 1) {
                s1 = MODE > 2 ? load_reg((__m128i*)(srcp + 3 * w) + 1) : s0;
                t = _mm_alignr_epi8(s1, s0, 12);
                d = _mm_shuffle_epi8(t, m0);
                stream_reg((__m128i*)(dstp + 4 * w) + 1, d);
            }
            if (MODE > 3) {
                s0 = MODE > 4 ? load_reg((__m128i*)(srcp + 3 * w) + 2) : s1;
                t = _mm_alignr_epi8(s0, s1, 8);
                d = _mm_shuffle_epi8(t, m0);
                stream_reg((__m128i*)(dstp + 4 * w) + 2, d);
            }
        }
        srcp += src_pitch;
        dstp += dst_pitch;
    }
}


packed_to_packed get_24to32_converter(int width)
{
    packed_to_packed func[] = {
        bgr24_to_bgr32<0>,
        bgr24_to_bgr32<1>,
        bgr24_to_bgr32<2>,
        bgr24_to_bgr32<3>,
        bgr24_to_bgr32<4>,
        bgr24_to_bgr32<5>,
    };

    int w = width - (width + 3) / 16 * 16;
    int mode = w > 10 ? 5 : w > 8 ? 4 : w > 5 ? 3 : w > 4 ? 2 : w > 0 ? 1 : 0;
    return func[mode];
}
