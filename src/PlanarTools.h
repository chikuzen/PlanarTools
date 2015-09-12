/*
PlanarTools.h

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


#ifndef PLANAR_TOOLS_H
#define PLANAR_TOOLS_H

#include <cstdint>
#include <initializer_list>
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include "avisynth.h"

#include "proc_functions/proc_functions.h"


#define PLANAR_TOOLS_VERSION "0.2.0"


class GVFmod : public GenericVideoFilter
{
protected:
    VideoInfo vi_src;

public:
    GVFmod(PClip _child) : GenericVideoFilter(_child) {
        memcpy(&vi_src, &vi, sizeof(VideoInfo));
    };
};

class Transpose : public GVFmod
{
    int num_planes;
    proc_transpose proc_transpose;

public:
    Transpose(PClip child);
    ~Transpose(){}
    PVideoFrame __stdcall GetFrame(int n, IScriptEnvironment* env);
    static AVSValue __cdecl create(
        AVSValue args, void* user_data, IScriptEnvironment* env);
};


class PackedToPlanar : public GVFmod
{
    packed_to_planar convert;

public:
    PackedToPlanar(PClip _child);
    ~PackedToPlanar(){}
    PVideoFrame __stdcall GetFrame(int n, IScriptEnvironment* env);
    static AVSValue __cdecl create(
        AVSValue args, void* user_data, IScriptEnvironment* env);
};


class ExtractPlane : public GVFmod
{
    int plane;
    extract_plane extract;

public:
    ExtractPlane(PClip child, int plane, IScriptEnvironment* env);
    ~ExtractPlane(){}
    PVideoFrame __stdcall GetFrame(int n, IScriptEnvironment* env);
    static AVSValue __cdecl create(
        AVSValue args, void* usar_data, IScriptEnvironment* env);
};


//YV24->BGR, YV16->YUY2
class PlanarToPacked : public GVFmod
{
    planar_to_packed convert;

public:
    PlanarToPacked(PClip child);
    ~PlanarToPacked(){}
    PVideoFrame __stdcall GetFrame(int n, IScriptEnvironment* env);
    static AVSValue __cdecl create(
        AVSValue args, void* user_data, IScriptEnvironment* env);
};


//Yx3->BGR/YUY2
class Yx3ToPacked : public GVFmod
{
    PClip clip1;
    PClip clip2;
    planar_to_packed convert;

public:
    Yx3ToPacked(PClip c0, PClip c1, PClip c2, int pix_type);
    ~Yx3ToPacked(){}
    PVideoFrame __stdcall GetFrame(int n, IScriptEnvironment* env);
    static AVSValue __cdecl create(
        AVSValue args, void* user_data, IScriptEnvironment* env);
};


//YV24+Y->BGRA
class PlanarToBGRA : public GVFmod
{
    PClip alpha;
    VideoInfo vi_a;
    planar_to_bgra convert;

public:
    PlanarToBGRA(PClip base, PClip alpha);
    ~PlanarToBGRA(){}
    PVideoFrame __stdcall GetFrame(int n, IScriptEnvironment* env);
    static AVSValue __cdecl create(
        AVSValue args, void* user_data, IScriptEnvironment* env);
};


//Yx4->BGRA
class Yx4ToBGRA : public GVFmod
{
    PClip blue;
    PClip red;
    PClip alpha;
    planar_to_bgra convert;

public:
    Yx4ToBGRA(PClip cg, PClip cb, PClip cr, PClip ca);
    ~Yx4ToBGRA(){}
    PVideoFrame __stdcall GetFrame(int n, IScriptEnvironment* env);
    static AVSValue __cdecl create(
        AVSValue args, void* user_data, IScriptEnvironment* env);
};


class PackedRGBToRGB : public GVFmod
{
    packed_to_packed convert;
public:
    PackedRGBToRGB(PClip child);
    ~PackedRGBToRGB(){}
    PVideoFrame __stdcall GetFrame(int n, IScriptEnvironment* env);
    static AVSValue __cdecl create(
        AVSValue args, void* user_data, IScriptEnvironment* env);
};


static inline bool
is_aligned_frame(PVideoFrame& f, int align=16)
{
    uintptr_t y = (uintptr_t)f->GetReadPtr(PLANAR_Y);
    uintptr_t u = (uintptr_t)f->GetReadPtr(PLANAR_U);
    uintptr_t v = (uintptr_t)f->GetReadPtr(PLANAR_V);
    return (y | u | v) & (align - 1) ? false : true;
}


static inline bool
is_aligned_frame(const uint8_t* p0, int align=16)
{
    return !((uintptr_t)p0 & (align - 1));
}

#endif
