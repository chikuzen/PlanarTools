#include "proc_functions.h"
#include "simd.h"


static __forceinline void
unpack_x6(__m128i& a, __m128i& b, __m128i& c, __m128i& d, __m128i& e, __m128i& f)
{
    __m128i a0 = unpacklo8(a, d);
    __m128i b0 = unpackhi8(a, d);
    __m128i c0 = unpacklo8(b, e);
    __m128i d0 = unpackhi8(b, e);
    __m128i e0 = unpacklo8(c, f);
    __m128i f0 = unpackhi8(c, f);

    __m128i a1 = unpacklo8(a0, d0);
    __m128i b1 = unpackhi8(a0, d0);
    __m128i c1 = unpacklo8(b0, e0);
    __m128i d1 = unpackhi8(b0, e0);
    __m128i e1 = unpacklo8(c0, f0);
    __m128i f1 = unpackhi8(c0, f0);

    __m128i a2 = unpacklo8(a1, d1);
    __m128i b2 = unpackhi8(a1, d1);
    __m128i c2 = unpacklo8(b1, e1);
    __m128i d2 = unpackhi8(b1, e1);
    __m128i e2 = unpacklo8(c1, f1);
    __m128i f2 = unpackhi8(c1, f1);

    __m128i a3 = unpacklo8(a2, d2);
    __m128i b3 = unpackhi8(a2, d2);
    __m128i c3 = unpacklo8(b2, e2);
    __m128i d3 = unpackhi8(b2, e2);
    __m128i e3 = unpacklo8(c2, f2);
    __m128i f3 = unpackhi8(c2, f2);

    a = unpacklo8(a3, d3);
    b = unpackhi8(a3, d3);
    c = unpacklo8(b3, e3);
    d = unpackhi8(b3, e3);
    e = unpacklo8(c3, f3);
    f = unpackhi8(c3, f3);
}


static void __stdcall
bgr24_to_yv24(const uint8_t* srcp, int width, int height, int src_pitch,
              uint8_t* dstpg, int pitch_g, uint8_t* dstpb, int pitch_b,
              uint8_t* dstpr, int pitch_r)
{
    const int w = (width + 5) / 32 * 32;
    srcp += src_pitch * (height - 1);

    const int mod = width - w;
    const int w1 = mod > 5 ? 1 : 0;
    const int w2 = mod > 10 ? 2 : 0;
    const int w3 = mod > 16 ? 3 : 0;
    const int w4 = mod > 21 ? 4 : 0;


    __m128i s0, s1, s2, s3, s4, s5;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < w; x += 32) {
            s0 = load_reg((__m128i*)(srcp + 3 * x) + 0);
            s1 = load_reg((__m128i*)(srcp + 3 * x) + 1);
            s2 = load_reg((__m128i*)(srcp + 3 * x) + 2);
            s3 = load_reg((__m128i*)(srcp + 3 * x) + 3);
            s4 = load_reg((__m128i*)(srcp + 3 * x) + 4);
            s5 = load_reg((__m128i*)(srcp + 3 * x) + 5);

            unpack_x6(s0, s1, s2, s3, s4, s5);

            stream_reg((__m128i*)(dstpb + x) + 0, s0);
            stream_reg((__m128i*)(dstpb + x) + 1, s1);
            stream_reg((__m128i*)(dstpg + x) + 0, s2);
            stream_reg((__m128i*)(dstpg + x) + 1, s3);
            stream_reg((__m128i*)(dstpr + x) + 0, s4);
            stream_reg((__m128i*)(dstpr + x) + 1, s5);
        }

        if (mod > 0) {
            s0 = load_reg((__m128i*)(srcp + 3 * w) +  0);
            s1 = load_reg((__m128i*)(srcp + 3 * w) + w1);
            s2 = load_reg((__m128i*)(srcp + 3 * w) + w2);
            s3 = load_reg((__m128i*)(srcp + 3 * w) + w3);
            s4 = load_reg((__m128i*)(srcp + 3 * w) + w4);
            s5 = s4;

            unpack_x6(s0, s1, s2, s3, s4, s5);

            stream_reg((__m128i*)(dstpb + w), s0);
            stream_reg((__m128i*)(dstpg + w), s2);
            stream_reg((__m128i*)(dstpr + w), s4);
            if (w3 > 0) {
                stream_reg((__m128i*)(dstpb + w + 16), s1);
                stream_reg((__m128i*)(dstpg + w + 16), s3);
                stream_reg((__m128i*)(dstpr + w + 16), s5);
            }
        }
        srcp -= src_pitch;
        dstpb += pitch_b;
        dstpg += pitch_g;
        dstpr += pitch_r;
    }
}


static __forceinline void
unpack_x4(__m128i& a, __m128i& b, __m128i& c, __m128i& d)
{
    __m128i a0 = unpacklo8(a, c);
    __m128i b0 = unpackhi8(a, c);
    __m128i c0 = unpacklo8(b, d);
    __m128i d0 = unpackhi8(b, d);

    __m128i a1 = unpacklo8(a0, c0);
    __m128i b1 = unpackhi8(a0, c0);
    __m128i c1 = unpacklo8(b0, d0);
    __m128i d1 = unpackhi8(b0, d0);

    __m128i a2 = unpacklo8(a1, c1);
    __m128i b2 = unpackhi8(a1, c1);
    __m128i c2 = unpacklo8(b1, d1);
    __m128i d2 = unpackhi8(b1, d1);

    a = unpacklo8(a2, c2);
    b = unpackhi8(a2, c2);
    c = unpacklo8(b2, d2);
}


static void __stdcall
bgr32_to_yv24(const uint8_t* srcp, int width, int height, int src_pitch,
              uint8_t* dstpg, int pitch_g, uint8_t* dstpb, int pitch_b,
              uint8_t* dstpr, int pitch_r)
{
    const int w = (width + 3) / 16 * 16;
    srcp += src_pitch * (height - 1);

    const int mod = width - w;
    const int w1 = mod >  4 ? 1 : 0;
    const int w2 = mod >  8 ? 2 : 0;

    __m128i s0, s1, s2, s3;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < w; x += 16) {
            const __m128i* sx = (__m128i*)(srcp + 4 * x);
            s0 = load_reg(sx + 0);
            s1 = load_reg(sx + 1);
            s2 = load_reg(sx + 2);
            s3 = load_reg(sx + 3);

            unpack_x4(s0, s1, s2, s3);

            stream_reg((__m128i*)(dstpb + x), s0);
            stream_reg((__m128i*)(dstpg + x), s1);
            stream_reg((__m128i*)(dstpr + x), s2);
        }
        if (mod > 0) {
            const __m128i* sw = (__m128i*)(srcp + 4 * w);
            s0 = load_reg(sw +  0);
            s1 = load_reg(sw + w1);
            s2 = load_reg(sw + w2);
            s3 = s2;

            unpack_x4(s0, s1, s2, s3);

            stream_reg((__m128i*)(dstpb + w), s0);
            stream_reg((__m128i*)(dstpg + w), s1);
            stream_reg((__m128i*)(dstpr + w), s2);
        }
        srcp -= src_pitch;
        dstpb += pitch_b;
        dstpg += pitch_g;
        dstpr += pitch_r;
    }
}


static __forceinline void
unpack_x4_yuy2(__m128i& a, __m128i& b, __m128i& c, __m128i& d)
{
    __m128i a0 = unpacklo8(a, c);
    __m128i b0 = unpackhi8(a, c);
    __m128i c0 = unpacklo8(b, d);
    __m128i d0 = unpackhi8(b, d);

    __m128i a1 = unpacklo8(a0, c0);
    __m128i b1 = unpackhi8(a0, c0);
    __m128i c1 = unpacklo8(b0, d0);
    __m128i d1 = unpackhi8(b0, d0);

    __m128i a2 = unpacklo8(a1, c1);
    __m128i b2 = unpackhi8(a1, c1);
    __m128i c2 = unpacklo8(b1, d1);
    __m128i d2 = unpackhi8(b1, d1);

    __m128i a3 = unpacklo8(a2, c2);
    c = unpackhi8(a2, c2);
    __m128i c3 = unpacklo8(b2, d2);
    d = unpackhi8(b2, d2);

    a = unpacklo8(a3, c3);
    b = unpackhi8(a3, c3);
}


static void __stdcall
yuy2_to_yv16(const uint8_t* srcp, int width, int height, int src_pitch,
             uint8_t* dstpy, int pitch_y, uint8_t* dstpu, int pitch_u,
             uint8_t* dstpv, int pitch_v)
{
    const int w = (width + 7) / 32 * 32;

    const int mod = width - w;
    const int w1 = mod > 8 ? 1 : 0;
    const int w2 = mod > 16 ? 2 : 0;

    for (int y = 0; y < height; ++y) {
        __m128i s0, s1, s2, s3;
        for (int x = 0; x < w; x += 32) {
            const __m128i* sx = (__m128i*)(srcp + 2 * x);
            s0 = load_reg(sx + 0);
            s1 = load_reg(sx + 1);
            s2 = load_reg(sx + 2);
            s3 = load_reg(sx + 3);

            unpack_x4_yuy2(s0, s1, s2, s3);

            stream_reg((__m128i*)(dstpy + x), s0);
            stream_reg((__m128i*)(dstpy + x + 16), s1);
            stream_reg((__m128i*)(dstpu + x / 2), s2);
            stream_reg((__m128i*)(dstpv + x / 2), s3);
        }
        if (mod > 0) {
            s0 = load_reg((__m128i*)(srcp + 2 * w) + 0);
            s1 = load_reg((__m128i*)(srcp + 2 * w) + w1);
            s2 = load_reg((__m128i*)(srcp + 2 * w) + w2);
            s3 = s2;

            unpack_x4_yuy2(s0, s1, s2, s3);

            stream_reg((__m128i*)(dstpy + w), s0);
            if (mod > 16) {
                stream_reg((__m128i*)(dstpy + w + 16), s1);
            }
            stream_reg((__m128i*)(dstpu + w / 2), s2);
            stream_reg((__m128i*)(dstpv + w / 2), s3);
        }
        srcp += src_pitch;
        dstpy += pitch_y;
        dstpu += pitch_u;
        dstpv += pitch_v;
    }
}

packed_to_planar get_planar_converter(int pixel_type)
{
    switch (pixel_type) {
    case VideoInfo::CS_YUY2:
        return yuy2_to_yv16;
    case VideoInfo::CS_BGR32:
        return bgr32_to_yv24;
    case VideoInfo::CS_BGR24:
        return bgr24_to_yv24;
    default:
        return nullptr;
    }
}


static void __stdcall
extract_plane_bgr24(const uint8_t* srcp, int width, int height, int src_pitch,
                    uint8_t* dstp, int dst_pitch, int plane)
{
    const int w = (width + 5) / 32 * 32;
    srcp += src_pitch * (height - 1);

    const int mod = width - w;
    const int w1 = mod > 5 ? 1 : 0;
    const int w2 = mod > 10 ? 2 : 0;
    const int w3 = mod > 16 ? 3 : 0;
    const int w4 = mod > 21 ? 4 : 0;

    __m128i s0, s1, s2, s3, s4, s5;
    __m128i& ss0 = plane == 0 ? s0 : plane == 1 ? s2 : s4;
    __m128i& ss1 = plane == 0 ? s1 : plane == 1 ? s3 : s5;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < w; x += 32) {
            const __m128i* sx = (__m128i*)(srcp + 3 * x);
            s0 = load_reg(sx + 0);
            s1 = load_reg(sx + 1);
            s2 = load_reg(sx + 2);
            s3 = load_reg(sx + 3);
            s4 = load_reg(sx + 4);
            s5 = load_reg(sx + 5);

            unpack_x6(s0, s1, s2, s3, s4, s5);

            stream_reg((__m128i*)(dstp + x), ss0);
            stream_reg((__m128i*)(dstp + x + 16), ss1);
        }
        if (mod > 0) {
            s0 = load_reg((__m128i*)(srcp + 3 * w) + 0);
            s1 = load_reg((__m128i*)(srcp + 3 * w) + 1);
            s2 = load_reg((__m128i*)(srcp + 3 * w) + 2);
            s3 = load_reg((__m128i*)(srcp + 3 * w) + 3);
            s4 = load_reg((__m128i*)(srcp + 3 * w) + 4);
            s5 = s4;

            unpack_x6(s0, s1, s2, s3, s4, s5);

            stream_reg((__m128i*)(dstp + w), ss0);
            if (w3 > 0) {
                stream_reg((__m128i*)(dstp + w + 16), ss1);
            }
        }
        srcp -= src_pitch;
        dstp += dst_pitch;
    }
}


static void __stdcall
extract_plane_bgr32(const uint8_t* srcp, int width, int height, int src_pitch,
                    uint8_t* dstp, int dst_pitch, int plane)
{
    const __m128i mask = _mm_set1_epi32(0xFF);

    const int w = (width + 3) / 16 * 16;
    srcp += src_pitch * (height - 1);

    const int mod = width - w;
    const int w1 = mod > 4 ? 1 : 0;
    const int w2 = mod > 8 ? 2 : 0;

    for (int y = 0; y < height; ++y) {
        __m128i s0, s1, s2, s3, t0, t1;
        for (int x = 0; x < w; x += 16) {
            const __m128i* sx = (__m128i*)(srcp + 4 * x);
            s0 = mask & _mm_srli_epi32(load_reg(sx + 0), plane * 8);
            s1 = mask & _mm_srli_epi32(load_reg(sx + 1), plane * 8);
            s2 = mask & _mm_srli_epi32(load_reg(sx + 2), plane * 8);
            s3 = mask & _mm_srli_epi32(load_reg(sx + 3), plane * 8);

            t0 = _mm_packs_epi32(s0, s1);
            t1 = _mm_packs_epi32(s2, s3);
            t0 = _mm_packus_epi16(t0, t1);

            stream_reg((__m128i*)(dstp + x), t0);
        }
        if (mod > 0) {
            const __m128i* sw = (__m128i*)(srcp + 4 * w);
            s0 = mask & _mm_srli_epi32(load_reg(sw + 0), plane * 8);
            s1 = mask & _mm_srli_epi32(load_reg(sw + w1), plane * 8);
            s2 = mask & _mm_srli_epi32(load_reg(sw + w2), plane * 8);

            t0 = _mm_packs_epi32(s0, s1);
            t1 = _mm_packs_epi32(s2, s2);
            t0 = _mm_packus_epi16(t0, t1);

            stream_reg((__m128i*)(dstp + w), t0);
        }
        srcp -= src_pitch;
        dstp += dst_pitch;
    }
}


static void __stdcall
extract_plane_yuy2_y(const uint8_t* srcp, int width, int height, int src_pitch,
                     uint8_t* dstp, int dst_pitch, int plane)
{
    const __m128i mask = _mm_set1_epi16(0xFF);

    const int w = (width + 7) / 16 * 16;

    const int mod = width - w;

    for (int y = 0; y < height; ++y) {
        __m128i s0, s1;
        for (int x = 0; x < w; x += 16) {
            s0 = mask & load_reg((__m128i*)(srcp + 2 * x));
            s1 = mask & load_reg((__m128i*)(srcp + 2 * x + 16));

            s0 = _mm_packus_epi16(s0, s1);

            stream_reg((__m128i*)(dstp + x), s0);
        }
        if (mod > 0) {
            s0 = mask & load_reg((__m128i*)(srcp + 2 * w));

            s0 = _mm_packus_epi16(s0, s0);

            stream_reg((__m128i*)(dstp + w), s0);
        }
        srcp += src_pitch;
        dstp += dst_pitch;
    }
}


static void __stdcall
extract_plane_yuy2_uv(const uint8_t* srcp, int width, int height, int src_pitch,
                      uint8_t* dstp, int dst_pitch, int plane)
{
    const __m128i mask = _mm_set1_epi32(0xFF);
    const int shift = plane == 1 ? 8 : 24;

    const int w = (width + 7) / 32 * 32;

    const int mod = width - w;
    const int w1 = mod > 8 ? 1 : 0;
    const int w2 = mod > 16 ? 2 : 0;

    for (int y = 0; y < height; ++y) {
        __m128i s0, s1, s2, s3;
        for (int x = 0; x < w; x += 32) {
            const __m128i* sx = (__m128i*)(srcp + 2 * x);
            s0 = mask & _mm_srli_epi32(load_reg(sx + 0), shift);
            s1 = mask & _mm_srli_epi32(load_reg(sx + 1), shift);
            s2 = mask & _mm_srli_epi32(load_reg(sx + 2), shift);
            s3 = mask & _mm_srli_epi32(load_reg(sx + 3), shift);

            s0 = _mm_packs_epi32(s0, s1);
            s2 = _mm_packs_epi32(s2, s3);
            s0 = _mm_packus_epi16(s0, s2);

            stream_reg((__m128i*)(dstp + x / 2), s0);
        }
        if (mod > 0) {
            const __m128i* sw = (__m128i*)(srcp + 2 * w);
            s0 = mask & _mm_srli_epi32(load_reg(sw + 0), shift);
            s1 = mask & _mm_srli_epi32(load_reg(sw + w1), shift);
            s2 = mask & _mm_srli_epi32(load_reg(sw + w2), shift);

            s0 = _mm_packs_epi32(s0, s1);
            s2 = _mm_packs_epi32(s2, s2);
            s0 = _mm_packus_epi16(s0, s2);

            stream_reg((__m128i*)(dstp + w / 2), s0);
        }
        srcp += src_pitch;
        dstp += dst_pitch;
    }
}



extract_plane get_extractor(int pixel_type, int plane)
{
    switch (pixel_type) {
    case VideoInfo::CS_BGR24:
        return extract_plane_bgr24;
    case VideoInfo::CS_BGR32:
        return extract_plane_bgr32;
    case VideoInfo::CS_YUY2:
        return plane == 0 ? extract_plane_yuy2_y : extract_plane_yuy2_uv;
    default:
        return nullptr;
    }
}
