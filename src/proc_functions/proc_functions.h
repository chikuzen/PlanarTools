/*
proc_functions.h

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


#ifndef PROC_FUNCTIONS_H
#define PROC_FUNCTIONS_H

#include <cstdint>
#ifndef WI32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include "../avisynth.h"


using proc_transpose = void(__stdcall *)(
    const uint8_t* srcp, uint8_t* dstp, const int width,
    const int height, const int src_pitch, const int dst_pitch);


// dst mapping --- Y:YorG, U:UorB, V:VorR, Alpha:drop
using packed_to_planar = void(__stdcall *)(
    const uint8_t* srcp, int width, int height, int src_pitch, uint8_t* dstpy,
    int pitchy, uint8_t* dstpu, int pitchu, uint8_t* dstpv, int pitchv);


// src mapping --- 0:YorG, 1:UorB, 2:VorR
using planar_to_packed = void(__stdcall *)(
    int width, int height, const uint8_t* srcp0, int pitch0,
    const uint8_t* srcp1, int pitch1, const uint8_t* srcp2, int pitch2,
    uint8_t* dstp, int dst_pitch);


using planar_to_bgra = void(__stdcall *)(
    int width, int height, const uint8_t* srcpg, int pitchg,
    const uint8_t* srcpb, int pitchb, const uint8_t* srcpr, int pitchr,
    const uint8_t* srcpa, int pitcha, uint8_t* dstp, int dst_pitch);


// plane mapping --- 0:YorB, 1:UorG, 2;VorR, 3:Alpha
using extract_plane = void(__stdcall *)(
    const uint8_t* srcp, int width, int height, int src_pitch, uint8_t* dstp,
    int dst_pitch);


using packed_to_packed = void(__stdcall *)(
    const uint8_t* srcp, int width, int height, int src_pitch, uint8_t* dstp,
    int dst_pitch);


proc_transpose get_transpose_function(int pixel_type);


packed_to_planar get_planar_converter(int pixel_type, int width);


extract_plane get_extractor(int pixel_type, int width, int plane);


planar_to_packed get_packed_converter(int pixel_type, int width);


planar_to_bgra get_bgra_converter(int width);


packed_to_packed get_24to32_converter(int width);

#endif
