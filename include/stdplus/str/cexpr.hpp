#pragma once
#include <algorithm>
#include <array>
#include <string_view>

namespace stdplus
{

template <auto f>
consteval auto cexprToStrArr()
{
    std::array<typename decltype(f())::value_type, std::size(f()) + 1> ret;
    {
        auto res = f();
        std::copy(std::begin(res), std::end(res), ret.begin());
        ret[ret.size() - 1] = '\0';
    }
    return ret;
}

template <auto f>
inline constexpr auto cexprStrArr = cexprToStrArr<f>();

template <auto f>
consteval auto cexprToSv()
{
    constexpr auto& d = cexprStrArr<f>;
    return std::basic_string_view(d.begin(), d.end() - 1);
}

template <auto f>
inline constexpr auto cexprSv = cexprToSv<f>();

} // namespace stdplus
