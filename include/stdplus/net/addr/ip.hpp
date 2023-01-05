#pragma once
#include <netinet/in.h>

#include <stdplus/hash.hpp>
#include <stdplus/variant.hpp>

#include <algorithm>
#include <variant>

namespace stdplus
{
namespace detail
{
struct In4AddrInner
{
    union
    {
        in_addr a;
        uint32_t s4_addr32;
        uint8_t s4_addr[4];
    };
};
static_assert(sizeof(In4AddrInner) == sizeof(in_addr));
} // namespace detail

struct In4Addr : detail::In4AddrInner
{
    constexpr In4Addr() noexcept : detail::In4AddrInner() {}
    constexpr In4Addr(in_addr a) noexcept : detail::In4AddrInner({a}) {}
    explicit constexpr In4Addr(std::initializer_list<uint8_t> a) noexcept :
        detail::In4AddrInner()
    {
        std::copy(a.begin(), a.end(), s4_addr);
    }

    constexpr operator in_addr() const noexcept
    {
        return a;
    }

    constexpr bool operator==(in_addr rhs) const noexcept
    {
        return a.s_addr == rhs.s_addr;
    }

    constexpr bool operator==(In4Addr rhs) const noexcept
    {
        return a.s_addr == rhs.a.s_addr;
    }
};

struct In6Addr : in6_addr
{
    constexpr In6Addr() noexcept : in6_addr() {}
    constexpr In6Addr(in6_addr a) noexcept : in6_addr(a) {}
    explicit constexpr In6Addr(std::initializer_list<uint8_t> a) noexcept :
        in6_addr()
    {
        std::copy(a.begin(), a.end(), s6_addr);
    }

    constexpr bool operator==(in6_addr rhs) const noexcept
    {
        return std::equal(s6_addr32, s6_addr32 + 4, rhs.s6_addr32);
    }

    constexpr bool operator==(In6Addr rhs) const noexcept
    {
        return *this == static_cast<in6_addr&>(rhs);
    }
};

namespace detail
{
using InAnyAddrV = std::variant<In4Addr, In6Addr>;
}

struct InAnyAddr : detail::InAnyAddrV
{
    constexpr InAnyAddr(in_addr a) noexcept : detail::InAnyAddrV(In4Addr{a}) {}
    constexpr InAnyAddr(In4Addr a) noexcept : detail::InAnyAddrV(a) {}
    constexpr InAnyAddr(in6_addr a) noexcept : detail::InAnyAddrV(In6Addr{a}) {}
    constexpr InAnyAddr(In6Addr a) noexcept : detail::InAnyAddrV(a) {}

    constexpr bool operator==(auto rhs) const noexcept
    {
        return variantEqFuzzy(*this, rhs);
    }
};

} // namespace stdplus

template <>
struct std::hash<stdplus::In4Addr>
{
    constexpr std::size_t operator()(in_addr addr) const noexcept
    {
        return stdplus::hashMulti(addr.s_addr);
    }
};

template <>
struct std::hash<stdplus::In6Addr>
{
    constexpr std::size_t operator()(in6_addr addr) const noexcept
    {
        return stdplus::hashMulti(addr.s6_addr32);
    }
};

template <>
struct std::hash<stdplus::InAnyAddr>
{
    inline std::size_t operator()(stdplus::InAnyAddr a) const noexcept
    {
        return std::hash<stdplus::detail::InAnyAddrV>{}(a);
    }
};
