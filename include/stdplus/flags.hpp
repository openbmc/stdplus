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

    inline BitFlags() noexcept : val(0) {}
    explicit inline BitFlags(underlying val) noexcept : val(val) {}

    inline BitFlags& set(type flag) & noexcept
    {
        val |= std::to_underlying(flag);
        return *this;
    }
    inline BitFlags&& set(type flag) && noexcept
    {
        val |= std::to_underlying(flag);
        return std::move(*this);
    }

    inline BitFlags& unset(type flag) & noexcept
    {
        val &= ~std::to_underlying(flag);
        return *this;
    }
    inline BitFlags&& unset(type flag) && noexcept
    {
        val &= ~std::to_underlying(flag);
        return std::move(*this);
    }

    explicit inline operator underlying() const noexcept
    {
        return val;
    }

  private:
    underlying val;
};

} // namespace stdplus
