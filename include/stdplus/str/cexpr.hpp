#pragma once
#include <algorithm>
#include <array>
#include <string_view>

namespace stdplus
{

template <auto f, bool nul>
consteval auto cexprToStrArr()
{
    constexpr std::size_t size = f().size();
    std::array<typename decltype(f())::value_type, size + (nul ? 1 : 0)> ret;
    {
        auto res = f();
        std::copy(res.begin(), res.end(), ret.begin());
        if constexpr (nul)
        {
            ret[ret.size() - 1] = '\0';
        }
    }
    return ret;
}

template <auto f, bool nul>
inline constexpr auto cexprStrArr = cexprToStrArr<f, nul>();

template <auto f>
consteval auto cexprToSv()
{
    constexpr auto& d = cexprStrArr<f, /*nul=*/false>;
    return std::basic_string_view(d.begin(), d.end());
}

template <auto f>
inline constexpr auto cexprSv = cexprToSv<f>();

} // namespace stdplus
