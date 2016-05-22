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
#include <avisynth.h>




proc_transpose get_transpose_function(int pixel_type);


packed_to_planar get_planar_converter(int pixel_type, int width);


extract_plane get_extractor(int pixel_type, int width, int plane, bool ssse3);


planar_to_packed get_packed_converter(int pixel_type, int width, bool ssse3);


planar_to_bgra get_bgra_converter(int width);


packed_to_packed get_24_32_converter(int pixel_type, int width);

#endif
