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


#include <map>
#include "PlanarTools.h"
#include "proc_functions/packed_to_packed.h"



packed_to_packed get_24_32_converter(int pixel_type, int width) noexcept
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


PackedRGBToRGB::PackedRGBToRGB(PClip _child) : GVFmod(_child)
{
    convert = get_24_32_converter(vi.pixel_type, vi.width);

    vi.pixel_type = 
        vi.pixel_type == VideoInfo::CS_BGR32 ? VideoInfo::CS_BGR24
        : VideoInfo::CS_BGR32;

    child->SetCacheHints(CACHE_NOTHING, 0);
}


PVideoFrame __stdcall PackedRGBToRGB::GetFrame(int n, IScriptEnvironment* env)
{
    auto src = child->GetFrame(n, env);
    if (!is_aligned_frame(src->GetReadPtr())) {
        env->MakeWritable(&src);
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

    return env->Invoke(vi.IsRGB24() ? "ConvertToRGB32" : "ConvertToRGB24", clip);
}
