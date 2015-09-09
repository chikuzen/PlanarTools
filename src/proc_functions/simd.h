#include <emmintrin.h>

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

SFINLINE __m128i load_reg(const __m128i* x)
{
    return _mm_load_si128(x);
}

SFINLINE void stream_reg(__m128i* addr, __m128i& x)
{
    _mm_stream_si128(addr, x);
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
