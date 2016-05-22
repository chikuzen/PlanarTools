/*
packed_to_packed.h

This file is part of PlanarTools

Copyright (C) 2015-2016 OKA Motofumi

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


#ifndef PACKED_TO_PACKED_H
#define PACKED_TO_PACKED_H

#include "simd.h"



template <int MODE>
static void __stdcall
bgr24_to_bgr32(const uint8_t* srcp, int width, int height, int src_pitch,
               uint8_t* dstp, int dst_pitch) noexcept
{
    const int w = (width + 3) / 16 * 16;
    int mod = width - w;

    const __m128i m0 =
        _mm_setr_epi8(0, 1, 2, -1, 3, 4, 5, -1, 6, 7, 8, -1, 9, 10, 11, -1);
    const __m128i m1 =
        _mm_setr_epi8(4, 5, 6, -1, 7, 8, 9, -1, 10, 11, 12, -1, 13, 14, 15, -1);
    const __m128i alpha =
        _mm_set1_epi32(0xFF000000);

    for (int y = 0; y < height; ++y) {
        __m128i s0, s1, t, d;
        for (int x = 0; x < w; x += 16) {
            s0 = load_reg(srcp + 3 * x + 0);
            d = _mm_shuffle_epi8(s0, m0);
            stream_reg(dstp + 4 * x + 0, d | alpha);

            s1 = load_reg(srcp + 3 * x + 16);
            t = _mm_alignr_epi8(s1, s0, 12);
            d = _mm_shuffle_epi8(t, m0);
            stream_reg(dstp + 4 * x + 16, d | alpha);

            s0 = load_reg(srcp + 3 * x + 32);
            t = _mm_alignr_epi8(s0, s1, 8);
            d = _mm_shuffle_epi8(t, m0);
            stream_reg(dstp + 4 * x + 32, d | alpha);

            d = _mm_shuffle_epi8(s0, m1);
            stream_reg(dstp + 4 * x + 48, d | alpha);
        }
        if (MODE > 0) {
            s0 = load_reg(srcp + 3 * w);
            d = _mm_shuffle_epi8(s0, m0);
            stream_reg(dstp + 4 * w, d | alpha);

            if (MODE > 1) {
                s1 = MODE > 2 ? load_reg(srcp + 3 * w + 16) : s0;
                t = _mm_alignr_epi8(s1, s0, 12);
                d = _mm_shuffle_epi8(t, m0);
                stream_reg(dstp + 4 * w + 16, d | alpha);
            }
            if (MODE > 3) {
                s0 = MODE > 4 ? load_reg(srcp + 3 * w + 32) : s1;
                t = _mm_alignr_epi8(s0, s1, 8);
                d = _mm_shuffle_epi8(t, m0);
                stream_reg(dstp + 4 * w + 32, d | alpha);
            }
        }
        srcp += src_pitch;
        dstp += dst_pitch;
    }
}


template <int MODE>
static void __stdcall
bgr32_to_bgr24(const uint8_t* srcp, int width, int height, int src_pitch,
               uint8_t* dstp, int dst_pitch) noexcept
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
            s0 = _mm_shuffle_epi8(load_reg(srcp + 4 * x +  0), m0);
            s1 = _mm_shuffle_epi8(load_reg(srcp + 4 * x + 16), m1);
            d = _mm_alignr_epi8(s1, s0, 4);
            stream_reg(dstp + 3 * x, d);

            s0 = _mm_slli_si128(s1, 4);
            s1 = _mm_shuffle_epi8(load_reg(srcp + 4 * x + 32), m1);
            d = _mm_alignr_epi8(s1, s0, 8);
            stream_reg(dstp + 3 * x + 16, d);

            s0 = _mm_slli_si128(s1, 4);
            s1 = _mm_shuffle_epi8(load_reg(srcp + 4 * x + 48), m1);
            d = _mm_alignr_epi8(s1, s0, 12);
            stream_reg(dstp + 3 * x + 32, d);
        }
        if (MODE > 0) {
            s0 = load_reg(srcp + 4 * w);
            if (mod == 1) {
                d = _mm_shuffle_epi8(s0, m1);
                stream_reg(dstp + 3 * w, d);
            } else {
                s0 = _mm_shuffle_epi8(s0, m0);
                s1 = load_reg(srcp + 4 * w + 16);
                if (MODE == 2) {
                    d = _mm_alignr_epi8(s1, s0, 4);
                    stream_reg(dstp + 3 * w, d);
                } else {
                    s1 = _mm_shuffle_epi8(s1, m1);
                    d = _mm_alignr_epi8(s1, s0, 4);
                    stream_reg(dstp + 3 * w, d);
                    if (mod == 3){
                        d = _mm_srli_si128(s1, 4);
                        stream_reg(dstp + 3 * w + 16, d);
                    } else {
                        s0 = _mm_slli_si128(s1, 4);
                        s1 = load_reg(srcp + 4 * w + 32);
                        s1 = _mm_shuffle_epi8(s1, m1);
                        d = _mm_alignr_epi8(s1, s0, 8);
                        stream_reg(dstp + 3 * w + 16, d);
                        if (MODE == 5) {
                            d = _mm_srli_si128(s1, 8);
                            stream_reg(dstp + 3 * w + 32, d);
                        }
                    }
                }
            }
        }
        srcp += src_pitch;
        dstp += dst_pitch;
    }
}


#endif
