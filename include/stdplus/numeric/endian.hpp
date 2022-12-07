#pragma once
#include <algorithm>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace stdplus
{

namespace detail
{

template <typename T, uint8_t size = sizeof(T)>
struct BswapAlign
{
    using type = T;
};

template <typename T>
struct BswapAlign<T, 2>
{
    using type alignas(uint16_t) = T;
};

template <typename T>
struct BswapAlign<T, 4>
{
    using type alignas(uint32_t) = T;
};

template <typename T>
struct BswapAlign<T, 8>
{
    using type alignas(uint64_t) = T;
};

template <typename T>
constexpr T bswapInt(typename BswapAlign<T>::type n) noexcept
{
    static_assert(std::is_trivially_copyable_v<T>);
    if constexpr (sizeof(T) == 2)
    {
        reinterpret_cast<uint16_t&>(n) =
            __builtin_bswap16(reinterpret_cast<uint16_t&>(n));
    }
    else if constexpr (sizeof(T) == 4)
    {
        reinterpret_cast<uint32_t&>(n) =
            __builtin_bswap32(reinterpret_cast<uint32_t&>(n));
    }
    else if constexpr (sizeof(T) == 8)
    {
        reinterpret_cast<uint64_t&>(n) =
            __builtin_bswap64(reinterpret_cast<uint64_t&>(n));
    }
    else
    {
        auto b = reinterpret_cast<std::byte*>(&n);
        std::reverse(b, b + sizeof(n));
    }
    return n;
}

} // namespace detail

template <typename T>
constexpr T bswap(T n) noexcept
{
    return detail::bswapInt<T>(n);
}

template <typename T>
constexpr T hton(T n) noexcept
{
    if constexpr (std::endian::native == std::endian::big)
    {
        return n;
    }
    else if constexpr (std::endian::native == std::endian::little)
    {
        return bswap(n);
    }
    else
    {
        static_assert(std::is_same_v<T, void>);
    }
}

template <typename T>
constexpr T ntoh(T n) noexcept
{
    return hton(n);
}

} // namespace stdplus
