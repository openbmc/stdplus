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

template <typename T, size_t s = sizeof(T)>
struct BswapFast
{
    static inline constexpr bool fast = false;
};

template <typename T>
struct BswapFast<T, 2>
{
    static inline constexpr bool fast = true;
    using type = uint16_t;

    static constexpr uint16_t swapAligned(uint16_t n) noexcept
    {
        return __builtin_bswap16(n);
    }
};

template <typename T>
struct BswapFast<T, 4>
{
    static inline constexpr bool fast = true;
    using type = uint32_t;

    static constexpr uint32_t swapAligned(uint32_t n) noexcept
    {
        return __builtin_bswap32(n);
    }
};

template <typename T>
struct BswapFast<T, 8>
{
    static inline constexpr bool fast = true;
    using type = uint64_t;

    static constexpr uint64_t swapAligned(uint64_t n) noexcept
    {
        return __builtin_bswap64(n);
    }
};

} // namespace detail

template <typename T>
constexpr T bswap(T n) noexcept
{
    static_assert(std::is_trivially_copyable_v<T>);
    using F = detail::BswapFast<T>;
    if constexpr (sizeof(T) == 1)
    {}
    else if constexpr (F::fast)
    {
        if constexpr (alignof(T) % alignof(typename F::type) == 0)
        {
            n = F::swapAligned(n);
        }
        else
        {
            typename F::type v;
            auto i = reinterpret_cast<std::byte*>(&n);
            auto o = reinterpret_cast<std::byte*>(&v);
            std::copy(i, i + sizeof(T), o);
            v = F::swapAligned(v);
            std::copy(o, o + sizeof(T), i);
        }
    }
    else
    {
        auto b = reinterpret_cast<std::byte*>(&n);
        std::reverse(b, b + sizeof(n));
    }
    return n;
}

template <std::endian E, typename T>
constexpr T htoe(T t) noexcept
{
    if constexpr (std::endian::native == std::endian::big)
    {
        if constexpr (E == std::endian::big)
        {
            return t;
        }
        else if constexpr (E == std::endian::little)
        {
            return bswap(t);
        }
        else
        {
            static_assert(std::is_same_v<T, void>);
        }
    }
    else if constexpr (std::endian::native == std::endian::little)
    {
        if constexpr (E == std::endian::big)
        {
            return bswap(t);
        }
        else if constexpr (E == std::endian::little)
        {
            return t;
        }
        else
        {
            static_assert(std::is_same_v<T, void>);
        }
    }
    else
    {
        static_assert(std::is_same_v<T, void>);
    }
}

template <std::endian E, typename T>
constexpr T etoh(T t) noexcept
{
    return htoe<E>(t);
}

template <typename T>
constexpr T htob(T t) noexcept
{
    return htoe<std::endian::big>(t);
}

template <typename T>
constexpr T btoh(T t) noexcept
{
    return etoh<std::endian::big>(t);
}

template <typename T>
constexpr T htol(T t) noexcept
{
    return htoe<std::endian::little>(t);
}

template <typename T>
constexpr T ltoh(T t) noexcept
{
    return etoh<std::endian::little>(t);
}

template <typename T>
constexpr T hton(T t) noexcept
{
    return htoe<std::endian::big>(t);
}

template <typename T>
constexpr T ntoh(T t) noexcept
{
    return etoh<std::endian::big>(t);
}

template <typename T, std::endian E>
struct EndianPacked
{
    using value_type = T;

    constexpr EndianPacked() noexcept = default;
    constexpr EndianPacked(T t) noexcept
    {
        *this = t;
    }
    constexpr EndianPacked& operator=(T t) noexcept
    {
        data.n = htoe<E>(t);
        return *this;
    }

    constexpr T value() const noexcept
    {
        return etoh<E>(data.n);
    }

    constexpr operator T() const noexcept
    {
        return value();
    }

    struct
    {
        T n;
    } __attribute__((packed)) data;
};

using int8_ubt = EndianPacked<std::int8_t, std::endian::big>;
using int16_ubt = EndianPacked<std::int16_t, std::endian::big>;
using int32_ubt = EndianPacked<std::int32_t, std::endian::big>;
using int64_ubt = EndianPacked<std::int64_t, std::endian::big>;

using int8_ult = EndianPacked<std::int8_t, std::endian::little>;
using int16_ult = EndianPacked<std::int16_t, std::endian::little>;
using int32_ult = EndianPacked<std::int32_t, std::endian::little>;
using int64_ult = EndianPacked<std::int64_t, std::endian::little>;

using int8_unt = EndianPacked<std::int8_t, std::endian::big>;
using int16_unt = EndianPacked<std::int16_t, std::endian::big>;
using int32_unt = EndianPacked<std::int32_t, std::endian::big>;
using int64_unt = EndianPacked<std::int64_t, std::endian::big>;

using uint8_ubt = EndianPacked<std::uint8_t, std::endian::big>;
using uint16_ubt = EndianPacked<std::uint16_t, std::endian::big>;
using uint32_ubt = EndianPacked<std::uint32_t, std::endian::big>;
using uint64_ubt = EndianPacked<std::uint64_t, std::endian::big>;

using uint8_ult = EndianPacked<std::uint8_t, std::endian::little>;
using uint16_ult = EndianPacked<std::uint16_t, std::endian::little>;
using uint32_ult = EndianPacked<std::uint32_t, std::endian::little>;
using uint64_ult = EndianPacked<std::uint64_t, std::endian::little>;

using uint8_unt = EndianPacked<std::uint8_t, std::endian::big>;
using uint16_unt = EndianPacked<std::uint16_t, std::endian::big>;
using uint32_unt = EndianPacked<std::uint32_t, std::endian::big>;
static_assert(alignof(uint32_unt) == 1);
using uint64_unt = EndianPacked<std::uint64_t, std::endian::big>;

} // namespace stdplus
