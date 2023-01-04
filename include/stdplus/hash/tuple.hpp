#pragma once
#include <stdplus/hash.hpp>
#include <tuple>

template <typename... Ts>
struct std::hash<std::tuple<Ts...>>
{
    constexpr auto operator()(const std::tuple<Ts...>& ts) const noexcept(
        noexcept(stdplus::hashMulti(std::declval<std::tuple<Ts...>>())))
    {
        return stdplus::hashMulti(ts);
    }
};
