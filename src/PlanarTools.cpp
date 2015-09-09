
#include "PlanarTools.h"


const AVS_Linkage* AVS_linkage = nullptr;

extern "C" __declspec(dllexport) const char* __stdcall
AvisynthPluginInit3(IScriptEnvironment* env, const AVS_Linkage* const vectors)
{
    AVS_linkage = vectors;

    env->AddFunction("Transpose", "c", &Transpose::create, nullptr);
    env->AddFunction("PackedToPlanar", "c", &PackedToPlanar::create, nullptr);
    env->AddFunction("ExtractPlane", "c[plane]i", &ExtractPlane::create, nullptr);
    env->AddFunction("PlanarToPacked", "c", &PlanarToPacked::create, nullptr);
    env->AddFunction("PlanarToPacked", "ccc", &Yx3ToPacked::create, nullptr);
    env->AddFunction("PlanarToRGB32", "cc", &PlanarToBGRA::create, nullptr);
    env->AddFunction("PlanarToRGB32", "cccc", &Yx4ToBGRA::create, nullptr);

    return "PlanarTools ver." PLANAR_TOOLS_VERSION " by OKA Motofumi";
}
