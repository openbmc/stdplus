#pragma once
#include <algorithm>
#include <array>
#include <bit>
#include <cstdint>
#include <limits>
#include <string>
#include <type_traits>

namespace stdplus
{

namespace detail
{

inline constexpr auto intLookup = []() {
    std::array<char, 36> ret;
    for (int8_t i = 0; i < 10; ++i)
    {
        ret[i] = i + '0';
    }
    for (int8_t i = 0; i < 26; ++i)
    {
        ret[i + 10] = i + 'a';
    }
    return ret;
}();

}

template <typename T, uint8_t base>
struct IntToStr
{
    static_assert(base > 1 && base <= 36);
    static_assert(std::is_unsigned_v<T>);

    static constexpr uint8_t buf_size = []() {
        T v = std::numeric_limits<T>::max();
        uint8_t i = 0;
        for (; v != 0; ++i)
        {
            v /= base;
        }
        return i;
    }();

    constexpr uint8_t reverseFill(char* buf, T v) const noexcept
    {
        uint8_t i = 0;
        do
        {
            if constexpr (std::popcount(base) == 1)
            {
                buf[i++] = detail::intLookup[v & 0xf];
                v >>= 4;
            }
            else
            {
                buf[i++] = detail::intLookup[v % base];
                v /= base;
            }
        } while (v > 0);
        return i;
    }

    constexpr char* operator()(char* buf, T v) const noexcept
    {
        uint8_t i = reverseFill(buf, v);
        std::reverse(buf, buf + i);
        return buf + i;
    }

    constexpr char* operator()(char* buf, T v, uint8_t min_width) const noexcept
    {
        uint8_t i = reverseFill(buf, v);
        auto end = buf + std::max(i, min_width);
        std::fill(buf + i, end, '0');
        std::reverse(buf, end);
        return end;
    }
};

template <uint8_t base = 10, typename T>
constexpr std::string to_string(T t)
{
    IntToStr<T, base> enc;
    std::array<char, enc.buf_size> buf;
    return std::string(buf.data(), enc(buf.data(), t));
}

} // namespace stdplus
