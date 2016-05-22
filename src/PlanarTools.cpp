/*
PlanarTools.cpp

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


const AVS_Linkage* AVS_linkage = nullptr;

extern "C" __declspec(dllexport) const char* __stdcall
AvisynthPluginInit3(IScriptEnvironment* env, const AVS_Linkage* const vectors)
{
    AVS_linkage = vectors;

    env->AddFunction("Transpose", "c", Transpose::create, nullptr);
    env->AddFunction("PackedToPlanar", "c", PackedToPlanar::create, nullptr);
    env->AddFunction("ExtractPlane", "c[plane]i", ExtractPlane::create, nullptr);
    env->AddFunction("PlanarToPacked", "c", PlanarToPacked::create, nullptr);
    env->AddFunction("PlanarToPacked", "ccc", Yx3ToPacked::create, nullptr);
    env->AddFunction("PlanarToRGB32", "cc", PlanarToBGRA::create, nullptr);
    env->AddFunction("PlanarToRGB32", "cccc", Yx4ToBGRA::create, nullptr);
    env->AddFunction("RGBToRGB", "c", PackedRGBToRGB::create, nullptr);

    if (env->FunctionExists("SetFilterMTMode")) {
        auto env2 = static_cast<IScriptEnvironment2*>(env);
        env2->SetFilterMTMode("PackedToPlanar", MT_NICE_FILTER, true);
        env2->SetFilterMTMode("PlanarToPacked", MT_NICE_FILTER, true);
        env2->SetFilterMTMode("PackedToRGB32", MT_NICE_FILTER, true);
        env2->SetFilterMTMode("ExtractPlane", MT_NICE_FILTER, true);
        env2->SetFilterMTMode("RGBToRGB", MT_NICE_FILTER, true);
    }

    return "PlanarTools ver." PLANAR_TOOLS_VERSION " by OKA Motofumi";
}
