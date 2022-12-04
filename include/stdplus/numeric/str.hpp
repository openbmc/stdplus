#pragma once
#include <algorithm>
#include <array>
#include <bit>
#include <cstdint>
#include <limits>
#include <stdplus/strconv.hpp>
#include <string>
#include <type_traits>

namespace stdplus
{

namespace detail
{

template <typename T>
constexpr auto pow(T t, uint8_t n) noexcept
{
    if (n == 0)
    {
        return T{1};
    }
    return t * pow(t, n - 1);
}

inline constexpr auto max_base = 36;

inline constexpr auto singleIntTable = []() {
    std::array<char, max_base> ret;
    for (int8_t i = 0; i < 10; ++i)
    {
        ret[i] = i + '0';
    }
    for (int8_t i = 0; i < 26; ++i)
    {
        ret[i + 10] = i + 'a';
    }
    static_assert(max_base == 36);
    return ret;
}();

template <uint8_t size, int8_t base>
inline constexpr auto multiIntTable = []() {
    static_assert(size > 1);
    std::array<char, pow<size_t>(base, size) * size> ret;
    for (size_t i = 0; i < pow<size_t>(base, size); ++i)
    {
        for (size_t j = 0; j < size; ++j)
        {
            ret[i * size + j] = singleIntTable[i / pow<size_t>(base, j) % base];
        }
    }
    return ret;
}();

inline constexpr auto multiSupport = []() {
    std::array<uint8_t, max_base> ret;
    std::fill(ret.begin(), ret.end(), 1);
    // Decimals are common enough and have slow division
    ret[10] = 2;
    return ret;
}();

} // namespace detail

template <uint8_t base, typename T, typename CharT = char>
struct IntToStr
{
    static_assert(base > 1 && base <= detail::max_base);
    static_assert(std::is_unsigned_v<T>);

    static inline constexpr size_t buf_size = []() {
        T v = std::numeric_limits<T>::max();
        uint8_t i = 0;
        for (; v != 0; ++i)
        {
            v /= base;
        }
        // Normalize to support multi-int
        auto div = detail::multiSupport[base];
        return (i + div - 1) / div * div;
    }();

    constexpr uint8_t reverseFill(CharT* buf, T v) const noexcept
    {
        uint8_t i = 0;
        constexpr auto multi = detail::multiSupport[base];
        do
        {
            if constexpr (std::popcount(base) == 1)
            {
                constexpr auto shift = std::countr_zero(base);
                constexpr auto mask = (1 << shift) - 1;
                buf[i++] = detail::singleIntTable[v & mask];
                v >>= shift;
            }
            else if constexpr (multi > 1)
            {
                constexpr auto div = detail::pow<size_t>(base, multi);
                auto begin =
                    &detail::multiIntTable<multi, base>[v % div * multi];
                std::copy(begin, begin + multi, buf + i);
                i += multi;
                v /= div;
            }
            else
            {
                buf[i++] = detail::singleIntTable[v % base];
                v /= base;
            }
        } while (v > 0);
        if constexpr (multi > 1)
        {
            while (i > 1 && buf[i - 1] == '0')
            {
                --i;
            }
        }
        return i;
    }

    constexpr CharT* operator()(CharT* buf, T v) const noexcept
    {
        uint8_t i = reverseFill(buf, v);
        std::reverse(buf, buf + i);
        return buf + i;
    }

    constexpr CharT* operator()(CharT* buf, T v,
                                uint8_t min_width) const noexcept
    {
        uint8_t i = reverseFill(buf, v);
        auto end = buf + std::max(i, min_width);
        std::fill(buf + i, end, '0');
        std::reverse(buf, end);
        return end;
    }
};

template <typename T, typename CharT>
struct ToStr<T, CharT, std::is_unsigned_v<T>>
{
    static constexpr size_t buf_size = IntToStr<10, T, CharT>::buf_size;

    constexpr CharT* operator()(CharT* buf, T v) const noexcept
    {
        return IntToStr<10, T, CharT>{}(buf, v);
    }
};

template <typename CharT, uint8_t base = 10, typename T>
constexpr std::enable_if_t<std::is_unsigned_v<T>, std::basic_string<CharT>>
    to_basic_string(T t)
{
    return std::basic_string<CharT>(ToStrBuf<T, CharT>{}(t));
}

template <uint8_t base = 10, typename T>
constexpr std::enable_if_t<std::is_unsigned_v<T>, std::string> to_string(T t)
{
    return to_basic_string<char>(t);
}

} // namespace stdplus
