/*
PackedToPlanar.cpp

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


#include "PlanarTools.h"


PackedToPlanar::PackedToPlanar(PClip _child) : GVFmod(_child)
{
    convert = get_planar_converter(vi.pixel_type, vi.width);
    vi.pixel_type = vi_src.IsYUY2() ? VideoInfo::CS_YV16 : VideoInfo::CS_YV24;
}


PVideoFrame __stdcall PackedToPlanar::GetFrame(int n, IScriptEnvironment* env)
{
    auto src = child->GetFrame(n, env);
    if (!is_aligned_frame(src->GetReadPtr())) {
        auto alt = env->NewVideoFrame(vi_src);
        env->BitBlt(alt->GetWritePtr(), alt->GetPitch(), src->GetReadPtr(),
            src->GetPitch(), src->GetRowSize(), src->GetHeight());
        src = alt;
    }

    auto dst = env->NewVideoFrame(vi);

    convert(src->GetReadPtr(), vi.width, vi.height, src->GetPitch(),
        dst->GetWritePtr(PLANAR_Y), dst->GetPitch(PLANAR_Y),
        dst->GetWritePtr(PLANAR_U), dst->GetPitch(PLANAR_U),
        dst->GetWritePtr(PLANAR_V), dst->GetPitch(PLANAR_V));

    return dst;
}


AVSValue __cdecl PackedToPlanar::
create(AVSValue args, void* user_data, IScriptEnvironment* env)
{
    PClip clip = args[0].AsClip();
    if (clip->GetVideoInfo().IsPlanar()) {
        env->ThrowError("PackedToPlanar: input is already planar format.");
    }

    return new PackedToPlanar(clip);
}


ExtractPlane::ExtractPlane(PClip _child, int _plane, IScriptEnvironment* env)
    : GVFmod(_child),
    plane(_plane)
{
    extract = get_extractor(vi.pixel_type, vi.width, plane);
    vi.pixel_type = VideoInfo::CS_Y8;
    if (vi_src.pixel_type == VideoInfo::CS_YUY2 && plane > 0) {
        vi.width /= 2;
    }
    vi.sample_type = 0;
    vi.audio_samples_per_second = 0;
    vi.nchannels = 0;
    vi.num_audio_samples = 0;
}


PVideoFrame __stdcall ExtractPlane::GetFrame(int n, IScriptEnvironment* env)
{
    auto src = child->GetFrame(n, env);
    if (!is_aligned_frame(src->GetReadPtr())) {
        auto alt = env->NewVideoFrame(vi_src);
        env->BitBlt(alt->GetWritePtr(), alt->GetPitch(), src->GetReadPtr(),
            src->GetPitch(), src->GetRowSize(), src->GetHeight());
        src = alt;
    }

    auto dst = env->NewVideoFrame(vi);

    extract(src->GetReadPtr(), vi_src.width, vi_src.height, src->GetPitch(),
        dst->GetWritePtr(), dst->GetPitch());

    return dst;
}


AVSValue __cdecl ExtractPlane::
create(AVSValue args, void* user_data, IScriptEnvironment* env)
{
    PClip clip = args[0].AsClip();
    const VideoInfo& vi = clip->GetVideoInfo();
    if (vi.IsPlanar()) {
        env->ThrowError("ExtractPlane: Input is unsupported format.");
    }

    int plane = args[1].AsInt(!vi.IsRGB32() ? 0 : 3);
    if (plane < 0 || (!vi.IsRGB32() && plane > 2) || plane > 3) {
        env->ThrowError("ExtractPlane: Invalid plane was specified.");
    }

    return new ExtractPlane(clip, plane, env);
}

