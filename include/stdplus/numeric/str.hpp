#pragma once
#include <algorithm>
#include <array>
#include <bit>
#include <cstdint>
#include <limits>
#include <stdplus/str/conv.hpp>
#include <string>
#include <type_traits>

namespace stdplus
{

namespace detail
{

inline constexpr auto maxBase = 36;

inline constexpr auto singleIntTable = []() {
    std::array<char, maxBase> ret;
    for (int8_t i = 0; i < 10; ++i)
    {
        ret[i] = i + '0';
    }
    for (int8_t i = 0; i < 26; ++i)
    {
        ret[i + 10] = i + 'a';
    }
    static_assert(maxBase == 36);
    return ret;
}();

template <uint8_t base, typename T, typename CharT>
constexpr CharT* uintToStr(CharT* buf, T v, uint8_t min_width) noexcept
{
    static_assert(std::is_unsigned_v<T>);
    uint8_t i = 0;
    do
    {
        if constexpr (std::popcount(base) == 1)
        {
            constexpr auto shift = std::countr_zero(base);
            constexpr auto mask = (1 << shift) - 1;
            buf[i] = detail::singleIntTable[v & mask];
            v >>= shift;
        }
        else
        {
            buf[i] = detail::singleIntTable[v % base];
            v /= base;
        }
        i += 1;
    } while (v > 0);
    auto end = buf + std::max(i, min_width);
    std::fill(buf + i, end, '0');
    std::reverse(buf, end);
    return end;
}

template <uint8_t base, typename T, typename CharT>
constexpr CharT* intToStr(CharT* buf, T v, uint8_t min_width) noexcept
{
    static_assert(std::is_integral_v<T>);
    if constexpr (std::is_signed_v<T>)
    {
        if (v < 0)
        {
            *(buf++) = '-';
            v = -v;
        }
    }
    return uintToStr<base>(buf, std::make_unsigned_t<T>(v), min_width);
}

} // namespace detail

template <uint8_t base, typename T>
struct IntToStr
{
    static_assert(base > 1 && base <= detail::maxBase);
    static_assert(std::is_integral_v<T>);

    static inline constexpr size_t bufSize = []() {
        T v = std::numeric_limits<T>::max();
        uint8_t i = 0;
        for (; v != 0; ++i)
        {
            v /= base;
        }
        return i + std::is_signed_v<T>;
    }();

    template <typename CharT>
    constexpr CharT* operator()(CharT* buf, T v,
                                uint8_t min_width = 0) const noexcept
    {
        using ptr_t =
            std::conditional_t<std::is_signed_v<T>, intptr_t, uintptr_t>;
        return detail::intToStr<
            base, std::conditional_t<sizeof(T) <= sizeof(ptr_t), ptr_t, T>>(
            buf, v, min_width);
    }
};

template <typename T>
struct ToStr<T, std::is_integral_v<T>>
    : ToStrFixed<ToStr<T>, T, IntToStr<10, T>::bufSize>
{
    template <typename CharT>
    constexpr CharT* conv(CharT* buf, T v) const noexcept
    {
        return IntToStr<10, T>{}(buf, v);
    }
};

template <typename CharT, uint8_t base = 10, typename T>
constexpr std::enable_if_t<std::is_integral_v<T>, std::basic_string<CharT>>
    to_basic_string(T t)
{
    return std::basic_string<CharT>(ToStrFinal<T, CharT>{}(t));
}

template <uint8_t base = 10, typename T>
constexpr std::enable_if_t<std::is_integral_v<T>, std::string> to_string(T t)
{
    return to_basic_string<char>(t);
}

} // namespace stdplus
