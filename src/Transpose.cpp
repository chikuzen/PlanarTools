#include "PlanarTools.h"


Transpose::Transpose(PClip _child) : GVFmod(_child)
{
    num_planes = (vi.IsY8() || vi.IsRGB()) ? 1 : 3;
    vi.width = vi_src.height;
    vi.height = vi_src.width;
    proc_transpose = get_transpose_function(vi.pixel_type);
}


PVideoFrame __stdcall Transpose::GetFrame(int n, IScriptEnvironment* env)
{
    const int planes[] = { PLANAR_Y, PLANAR_U, PLANAR_V };

    auto src = child->GetFrame(n, env);
    if (!is_aligned_frame(src)) {
        auto alt = env->NewVideoFrame(vi_src);
        for (int i = 0; i < num_planes; ++i) {
            int p = planes[i];
            env->BitBlt(
                alt->GetWritePtr(p), alt->GetPitch(p), src->GetReadPtr(p),
                src->GetPitch(p), src->GetRowSize(p), src->GetHeight(p));
        }
        src = alt;
    }

    auto dst = env->NewVideoFrame(vi);

    for (int i = 0; i < num_planes; ++i) {
        int p = planes[i];
        proc_transpose(
            src->GetReadPtr(p), dst->GetWritePtr(p), src->GetRowSize(p),
            src->GetHeight(p), src->GetPitch(p), dst->GetPitch(p));
    }

    return dst;
}


AVSValue __cdecl Transpose::
create(AVSValue args, void* user_data, IScriptEnvironment* env)
{
    PClip clip = args[0].AsClip();
    const VideoInfo& vi = clip->GetVideoInfo();
    if (vi.IsRGB24() || vi.IsYUY2() || vi.IsYV16() || vi.IsYV411()) {
        env->ThrowError("Transpose: input is not supported format.");
    }

    return new Transpose(clip);
}

