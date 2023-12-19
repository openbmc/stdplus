#pragma once
#include <stdplus/concepts.hpp>

#include <concepts>
#include <type_traits>
#include <utility>

namespace stdplus
{

template <Enum T, std::integral I = std::underlying_type_t<T>>
class BitFlags
{
  public:
    using type = T;
    using underlying = I;

    constexpr BitFlags() noexcept : val(0) {}
    constexpr BitFlags(type t) noexcept : val(std::to_underlying(t)) {}
    explicit constexpr BitFlags(underlying val) noexcept : val(val) {}

    constexpr BitFlags& set(type flag) & noexcept
    {
        val |= std::to_underlying(flag);
        return *this;
    }
    constexpr BitFlags&& set(type flag) && noexcept
    {
        val |= std::to_underlying(flag);
        return std::move(*this);
    }

    constexpr BitFlags& unset(type flag) & noexcept
    {
        val &= ~std::to_underlying(flag);
        return *this;
    }
    constexpr BitFlags&& unset(type flag) && noexcept
    {
        val &= ~std::to_underlying(flag);
        return std::move(*this);
    }

    explicit constexpr operator underlying() const noexcept
    {
        return val;
    }

  private:
    underlying val;
};

} // namespace stdplus
