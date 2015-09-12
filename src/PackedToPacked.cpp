/*
PackedToPacked.cpp

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


PackedRGBToRGB::PackedRGBToRGB(PClip _child) : GVFmod(_child)
{
    convert = get_24_32_converter(vi.pixel_type, vi.width);
    vi.pixel_type = 
        vi.pixel_type == VideoInfo::CS_BGR32 ? VideoInfo::CS_BGR24
        : VideoInfo::CS_BGR32;
}


PVideoFrame __stdcall PackedRGBToRGB::GetFrame(int n, IScriptEnvironment* env)
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
        dst->GetWritePtr(), dst->GetPitch());

    return dst;

}


AVSValue __cdecl PackedRGBToRGB::
create(AVSValue args, void* user_data, IScriptEnvironment* env)
{
    PClip clip = args[0].AsClip();
    const VideoInfo& vi = clip->GetVideoInfo();
    if (!vi.IsRGB()) {
        env->ThrowError("RGBToRGB: Input is not RGB.");
    }

    if (env->GetCPUFlags() & CPUF_SSSE3) {
        return new PackedRGBToRGB(clip);
    }

    const char* filter = vi.IsRGB24() ? "ConvertToRGB32" : "ConvertToRGB24";
    return env->Invoke(filter, args);
}
