#pragma once
#include <net/ethernet.h>

#include <stdplus/hash.hpp>

#include <algorithm>
#include <variant>

namespace stdplus
{

struct EtherAddr : ether_addr
{
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
