#ifndef PROC_TRANSPOSE_H
#define PROC_TRANSPOSE_H

#include <cstdint>


using proc_transpose = void(__stdcall *)(
    const uint8_t* srcp, uint8_t* dstp, const int width,
    const int height, const int src_pitch, const int dst_pitch);


proc_transpose get_transpose_function(int pixel_type) noexcept;

#endif

