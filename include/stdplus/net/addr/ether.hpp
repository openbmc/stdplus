#pragma once
#include <net/ethernet.h>

#include <stdplus/hash.hpp>
#include <stdplus/numeric/str.hpp>
#include <stdplus/str/conv.hpp>

#include <algorithm>
#include <stdexcept>
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

template <>
struct FromStr<EtherAddr>
{
    constexpr EtherAddr operator()(const auto& str) const
    {
        std::basic_string_view sv{str};
        constexpr StrToInt<16, std::uint8_t> sti;
        EtherAddr ret;
        if (sv.size() == 12 && sv.find(":") == sv.npos)
        {
            for (size_t i = 0; i < 6; ++i)
            {
                ret.ether_addr_octet[i] = sti(sv.substr(i * 2, 2));
            }
        }
        else
        {
            for (size_t i = 0; i < 5; ++i)
            {
                auto loc = sv.find(":");
                ret.ether_addr_octet[i] = sti(sv.substr(0, loc));
                sv.remove_prefix(loc == sv.npos ? sv.size() : loc + 1);
                if (sv.empty())
                {
                    throw std::invalid_argument("Missing mac data");
                }
            }
            ret.ether_addr_octet[5] = sti(sv);
        }
        return ret;
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
