#pragma once
#include <stdplus/str/conv.hpp>

#include <algorithm>
#include <array>
#include <bit>
#include <concepts>
#include <cstdint>
#include <limits>
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

template <uint8_t size, uint8_t base>
inline constexpr auto multiIntTable = []() {
    static_assert(size > 1);
    static_assert(base <= maxBase);
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

template <uint8_t size, uint8_t base>
constexpr void intStrWrite(auto str, auto v) noexcept
{
    if constexpr (size > 1)
    {
        auto it = &detail::multiIntTable<size, base>[v * size];
        std::copy(it, it + size, str);
    }
    else
    {
        *str = detail::singleIntTable[v];
    }
}

inline constexpr auto multiIntSupport = []() {
    std::array<uint8_t, maxBase> ret;
    std::fill(ret.begin(), ret.end(), 1);
    // Decimals are common enough and have slow division
    ret[10] = 2;
    return ret;
}();

template <uint8_t base, typename T, typename CharT>
constexpr CharT* uintToStr(CharT* buf, T v, uint8_t min_width) noexcept
{
    static_assert(std::is_unsigned_v<T>);
    uint8_t i = 0;
    constexpr auto multi = detail::multiIntSupport[base];
    do
    {
        constexpr auto div = detail::pow<size_t>(base, multi);
        if constexpr (std::popcount(base) == 1)
        {
            constexpr auto shift = std::countr_zero(div);
            constexpr auto mask = (1 << shift) - 1;
            intStrWrite<multi, base>(buf + i, v & mask);
            v >>= shift;
        }
        else
        {
            intStrWrite<multi, base>(buf + i, v % div);
            v /= div;
        }
        i += multi;
    } while (v > 0);
    if constexpr (multi > 1)
    {
        const auto width = std::max<uint8_t>(min_width, 1);
        while (i > width && buf[i - 1] == '0')
        {
            --i;
        }
    }
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

    static inline constexpr size_t buf_size = []() {
        T v = std::numeric_limits<T>::max();
        uint8_t i = 0;
        for (; v != 0; ++i)
        {
            v /= base;
        }
        // Normalize to support multi-int
        auto div = detail::multiIntSupport[base];
        return (i + div - 1) / div * div + std::is_signed_v<T>;
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

template <std::integral T>
struct ToStr<T> : IntToStr<10, T>
{
    using type = T;
};

template <typename CharT, uint8_t base = 10, std::integral T>
constexpr auto to_basic_string(T t)
{
    return std::basic_string<CharT>(ToStrHandle<ToStr<T>, CharT>{}(t));
}

template <uint8_t base = 10, std::integral T>
constexpr auto to_string(T t)
{
    return to_basic_string<char>(t);
}

} // namespace stdplus
