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


#include <map>
#include <tuple>
#include "PlanarTools.h"
#include "proc_functions/packed_to_planar.h"



packed_to_planar get_planar_converter(int pixel_type, int width) noexcept
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


PackedToPlanar::PackedToPlanar(PClip _child) : GVFmod(_child)
{
    convert = get_planar_converter(vi.pixel_type, vi.width);
    vi.pixel_type = vi_src.IsYUY2() ? VideoInfo::CS_YV16 : VideoInfo::CS_YV24;

    child->SetCacheHints(CACHE_NOTHING, 0);
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




extract_plane get_extractor(int pixel_type, int width, int plane, bool ssse3) noexcept
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



ExtractPlane::ExtractPlane(PClip _child, int _plane, IScriptEnvironment* env)
    : GVFmod(_child),
    plane(_plane)
{
    bool ssse3 = (env->GetCPUFlags() & CPUF_SSSE3) != 0;
    extract = get_extractor(vi.pixel_type, vi.width, plane, ssse3);
    vi.pixel_type = VideoInfo::CS_Y8;
    if (vi_src.pixel_type == VideoInfo::CS_YUY2 && plane > 0) {
        vi.width /= 2;
    }
    vi.sample_type = 0;
    vi.audio_samples_per_second = 0;
    vi.nchannels = 0;
    vi.num_audio_samples = 0;

    child->SetCacheHints(CACHE_NOTHING, 0);
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

