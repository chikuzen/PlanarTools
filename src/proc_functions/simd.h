/*
simd.h

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


#include <immintrin.h>

#pragma warning(disable:4752)

#define SFINLINE static __forceinline


SFINLINE __m128i operator &(const __m128i& x, const __m128i& y)
{
    return _mm_and_si128(x, y);
}

SFINLINE __m128i operator |(const __m128i& x, const __m128i& y)
{
    return _mm_or_si128(x, y);
}

template <int N>
SFINLINE __m128i lshift(const __m128i& x)
{
    return _mm_srli_si128(x, N);
}

template <int N>
SFINLINE __m128i rshift(const __m128i& x)
{
    return _mm_slli_si128(x, N);
}

SFINLINE __m128i load_reg(__m128i* x)
{
    return _mm_load_si128(x);
}

SFINLINE void stream_reg(__m128i* addr, __m128i& x)
{
    _mm_stream_si128(addr, x);
}

SFINLINE void store_reg(__m128i* addr, __m128i& x)
{
    _mm_store_si128(addr, x);
}

SFINLINE __m128i unpacklo8(const __m128i& x, const __m128i& y)
{
    return _mm_unpacklo_epi8(x, y);
}

SFINLINE __m128i unpackhi8(const __m128i& x, const __m128i& y)
{
    return _mm_unpackhi_epi8(x, y);
}

SFINLINE __m128i unpacklo16(const __m128i& x, const __m128i& y)
{
    return _mm_unpacklo_epi16(x, y);
}

SFINLINE __m128i unpackhi16(const __m128i& x, const __m128i& y)
{
    return _mm_unpackhi_epi16(x, y);
}

SFINLINE __m128i unpacklo32(const __m128i& x, const __m128i& y)
{
    return _mm_unpacklo_epi32(x, y);
}

SFINLINE __m128i unpackhi32(const __m128i& x, const __m128i& y)
{
    return _mm_unpackhi_epi32(x, y);
}

SFINLINE __m128i unpacklo64(const __m128i& x, const __m128i& y)
{
    return _mm_unpacklo_epi64(x, y);
}

SFINLINE __m128i unpackhi64(const __m128i& x, const __m128i& y)
{
    return _mm_unpackhi_epi64(x, y);
}

SFINLINE __m128i runpacklo8(const __m128i& x, const __m128i& y)
{
    static const __m128i mask = _mm_set1_epi16(0xFF);
    return _mm_packus_epi16(x & mask, y & mask);
}

SFINLINE __m128i runpackhi8(const __m128i& x, const __m128i& y)
{
    return _mm_packus_epi16(_mm_srli_epi16(x, 8), _mm_srli_epi16(y, 8));
}

SFINLINE __m128i movehl(const __m128i& x)
{
    __m128 t = _mm_castsi128_ps(x);
    return _mm_castps_si128(_mm_movehl_ps(t, t));
}

SFINLINE __m128i movelh(const __m128i& x)
{
    __m128 t = _mm_castsi128_ps(x);
    return _mm_castps_si128(_mm_movelh_ps(t, t));
}

SFINLINE __m128i movehl(const __m128i& x, const __m128i& y)
{
    return _mm_castps_si128(_mm_movehl_ps(_mm_castsi128_ps(x), _mm_castsi128_ps(y)));
}

SFINLINE __m128i movelh(const __m128i& x, const __m128i& y)
{
    __m128 t = _mm_castsi128_ps(x);
    return _mm_castps_si128(_mm_movelh_ps(_mm_castsi128_ps(x), _mm_castsi128_ps(y)));
}
