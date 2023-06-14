#pragma once
#include <net/ethernet.h>

#include <stdplus/hash.hpp>

#include <algorithm>
#include <variant>

namespace stdplus
{

struct EtherAddr : ether_addr
{
    constexpr EtherAddr() noexcept : ether_addr() {}
    constexpr EtherAddr(ether_addr other) noexcept : ether_addr(other) {}
    explicit constexpr EtherAddr(std::initializer_list<uint8_t> a) noexcept :
        ether_addr()
    {
        std::copy(a.begin(), a.end(), ether_addr_octet);
    }

    constexpr bool operator==(ether_addr rhs) const noexcept
    {
        return std::equal(ether_addr_octet, ether_addr_octet + 6,
                          rhs.ether_addr_octet);
    }
};

} // namespace stdplus

template <>
struct std::hash<stdplus::EtherAddr>
{
    constexpr std::size_t operator()(ether_addr addr) const noexcept
    {
        return stdplus::hashMulti(addr.ether_addr_octet);
    }
};
