#pragma once
#include <algorithm>
#include <array>
#include <bit>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <stdplus/str/conv.hpp>
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

template <uint8_t size, uint8_t base>
inline constexpr auto multiIntTable = []() {
    static_assert(size > 1);
    static_assert(base <= max_base);
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
    std::array<uint8_t, max_base> ret;
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

inline constexpr auto charTable = []() {
    std::array<int8_t, 256> ret;
    std::fill(ret.begin(), ret.end(), -1);
    for (int8_t i = 0; i < 10; ++i)
    {
        ret[i + '0'] = i;
    }
    for (int8_t i = 0; i < 26; ++i)
    {
        ret[i + 'A'] = i + 10;
        ret[i + 'a'] = i + 10;
    }
    return ret;
}();

template <uint8_t base, typename T, typename CharT>
constexpr T strToUInt(std::basic_string_view<CharT> str)
{
    static_assert(std::is_unsigned_v<T>);
    if (str.empty())
    {
        throw std::invalid_argument("Empty Str");
    }
    constexpr auto max = std::numeric_limits<T>::max();
    T ret = 0;
    for (auto c : str)
    {
        constexpr auto cmax = 1 << (sizeof(CharT) << 3);
        if constexpr (detail::charTable.size() < cmax)
        {
            if (detail::charTable.size() <= c)
            {
                throw std::invalid_argument("Invalid numeral");
            }
        }
        auto v = detail::charTable[c];
        if (v < 0 || v >= base)
        {
            throw std::invalid_argument("Invalid numeral");
        }
        if constexpr (std::popcount(base) == 1)
        {
            constexpr auto shift = std::countr_zero(base);
            constexpr auto maxshift = max >> shift;
            if (ret > maxshift)
            {
                throw std::overflow_error("Integer Decode");
            }
            ret = (ret << shift) | v;
        }
        else
        {
            constexpr auto maxbase = max / base;
            if (ret > maxbase)
            {
                throw std::overflow_error("Integer Decode");
            }
            ret *= base;
            if (max - v < ret)
            {
                throw std::overflow_error("Integer Decode");
            }
            ret += v;
        }
    }
    return ret;
}

template <uint8_t base, typename T, typename CharT>
constexpr T strToInt(std::basic_string_view<CharT> str)
{
    static_assert(std::is_integral_v<T>);
    if constexpr (std::is_unsigned_v<T>)
    {
        return strToUInt<base, T>(str);
    }
    bool is_neg = str.starts_with('-');
    auto ret = strToUInt<base, std::make_unsigned_t<T>>(str.substr(is_neg));
    if (ret > std::numeric_limits<T>::max())
    {
        throw std::overflow_error("Integer Decode");
    }
    return is_neg ? -static_cast<T>(ret) : ret;
}

} // namespace detail

template <uint8_t base, typename T>
struct IntToStr
{
    static_assert(base > 1 && base <= detail::max_base);
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

template <typename T>
struct ToStr<T, std::is_integral_v<T>>
{
    static constexpr size_t buf_size = IntToStr<10, T>::buf_size;

    template <typename CharT>
    constexpr CharT* operator()(CharT* buf, T v) const noexcept
    {
        return IntToStr<10, T>{}(buf, v);
    }
};

template <typename CharT, uint8_t base = 10, typename T>
constexpr std::enable_if_t<std::is_integral_v<T>, std::basic_string<CharT>>
    to_basic_string(T t)
{
    return std::basic_string<CharT>(ToStrBuf<T, CharT>{}(t));
}

template <uint8_t base = 10, typename T>
constexpr std::enable_if_t<std::is_integral_v<T>, std::string> to_string(T t)
{
    return to_basic_string<char>(t);
}

template <uint8_t base, typename T>
struct StrToInt
{
    static_assert(base > 1 && base <= detail::max_base);
    static_assert(std::is_integral_v<T>);

    constexpr T operator()(const auto& str) const
    {
        using ptr_t =
            std::conditional_t<std::is_signed_v<T>, intptr_t, uintptr_t>;
        auto ret = detail::strToInt<
            base, std::conditional_t<sizeof(T) <= sizeof(ptr_t), ptr_t, T>>(
            std::basic_string_view(str));
        if (ret > std::numeric_limits<T>::max())
        {
            throw std::overflow_error("Integer Decode");
        }
        return ret;
    }
};

} // namespace stdplus
