#pragma once
#include <stdplus/hash.hpp>

#include <array>

template <typename T, std::size_t N>
struct std::hash<std::array<T, N>>
{
    constexpr auto operator()(const std::array<T, N>& ts) const
        noexcept(noexcept(stdplus::hashMulti(std::declval<std::array<T, N>>())))
    {
        return stdplus::hashMulti(ts);
    }
};

template <typename T, std::size_t N>
struct std::hash<T[N]>
{
    constexpr auto operator()(const T (&ts)[N]) const
        noexcept(noexcept(stdplus::hashMulti(std::declval<T[N]>())))
    {
        return stdplus::hashMulti(ts);
    }
};
