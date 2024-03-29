#pragma once
#include <fmt/core.h>
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

    constexpr bool operator==(EtherAddr rhs) const noexcept
    {
        return *this == static_cast<ether_addr&>(rhs);
    }

    constexpr bool isMulticast() const noexcept
    {
        return ether_addr_octet[0] & 1;
    }

    constexpr bool isUnicast() const noexcept
    {
        return !(*this == EtherAddr{} || isMulticast());
    }
};

template <>
struct ToStr<EtherAddr>
{
    using type = EtherAddr;
    using ToHex = IntToStr<16, uint8_t>;
    // 6 octets * 2 hex chars + 5 separators
    static inline constexpr uint8_t buf_size = 15 + ToHex::buf_size;

    template <typename CharT>
    constexpr CharT* operator()(CharT* buf, EtherAddr v) const noexcept
    {
        for (auto ptr = buf + 2; ptr < buf + buf_size; ptr += 3)
        {
            *ptr = ':';
        }
        auto ptr = buf;
        for (std::size_t i = 0; i < 6; ++i, ptr += 3)
        {
            ToHex{}(ptr, v.ether_addr_octet[i], 2);
        }
        return buf + buf_size;
    }
};

template <>
struct FromStr<EtherAddr>
{
    template <typename CharT>
    constexpr EtherAddr operator()(std::basic_string_view<CharT> sv) const
    {
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

template <typename CharT>
struct fmt::formatter<stdplus::EtherAddr, CharT> :
    stdplus::Format<stdplus::ToStr<stdplus::EtherAddr>, CharT>
{};

namespace std
{
template <typename T, typename CharT>
struct formatter;

template <typename CharT>
struct formatter<stdplus::EtherAddr, CharT> :
    stdplus::Format<stdplus::ToStr<stdplus::EtherAddr>, CharT>
{};
} // namespace std
