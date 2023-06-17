#pragma once
#include <netinet/in.h>

#include <stdplus/hash.hpp>
#include <stdplus/numeric/endian.hpp>
#include <stdplus/numeric/str.hpp>
#include <stdplus/str/conv.hpp>
#include <stdplus/variant.hpp>

#include <algorithm>
#include <stdexcept>
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

template <>
struct FromStr<In4Addr>
{
    template <typename CharT>
    constexpr In4Addr operator()(std::basic_string_view<CharT> sv) const
    {
        constexpr StrToInt<10, uint8_t> sti;
        uint32_t addr = {};
        for (size_t i = 0; i < 3; ++i)
        {
            auto loc = sv.find(".");
            addr |= sti(sv.substr(0, loc));
            addr <<= 8;
            sv.remove_prefix(loc == sv.npos ? sv.size() : loc + 1);
            if (sv.empty())
            {
                throw std::invalid_argument("Missing addr data");
            }
        }
        addr |= sti(sv);
        return in_addr{hton(addr)};
    }
};

template <>
struct ToStr<In4Addr>
{
    using type = In4Addr;
    using ToOct = IntToStr<10, uint8_t>;
    // 4 octets * 3 dec chars + 3 separators
    static constexpr std::size_t buf_size = 12 + ToOct::buf_size;

    template <typename CharT>
    constexpr CharT* operator()(CharT* buf, In4Addr v) const noexcept
    {
        auto n = bswap(ntoh(v.s4_addr32));
        for (size_t i = 0; i < 3; ++i)
        {
            buf = ToOct{}(buf, n & 0xff);
            (buf++)[0] = '.';
            n >>= 8;
        }
        return ToOct{}(buf, n & 0xff);
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

template <>
struct FromStr<In6Addr>
{
    template <typename CharT>
    constexpr In6Addr operator()(std::basic_string_view<CharT> sv) const
    {
        constexpr StrToInt<16, uint16_t> sti;
        constexpr FromStr<In4Addr> fsip4;
        in6_addr ret = {};
        size_t i = 0;
        while (i < 8)
        {
            auto loc = sv.find(':');
            if (i == 6 && loc == sv.npos)
            {
                ret.s6_addr32[3] = fsip4(sv).s4_addr32;
                return ret;
            }
            if (loc != 0 && !sv.empty())
            {
                ret.s6_addr16[i++] = hton(sti(sv.substr(0, loc)));
            }
            if (i < 8 && sv.size() > loc + 1 && sv[loc + 1] == ':')
            {
                sv.remove_prefix(loc + 2);
                break;
            }
            else if (sv.empty())
            {
                throw std::invalid_argument("IPv6 Data");
            }
            sv.remove_prefix(loc == sv.npos ? sv.size() : loc + 1);
        }
        if (sv.starts_with(':'))
        {
            throw std::invalid_argument("Extra separator");
        }
        size_t j = 7;
        if (!sv.empty() && i < 6 && sv.find('.') != sv.npos)
        {
            auto loc = sv.rfind(':');
            ret.s6_addr32[3] =
                fsip4(sv.substr(loc == sv.npos ? 0 : loc + 1)).s4_addr32;
            sv.remove_suffix(loc == sv.npos ? sv.size() : sv.size() - loc);
            j -= 2;
        }
        while (!sv.empty() && j > i)
        {
            auto loc = sv.rfind(':');
            ret.s6_addr16[j--] =
                hton(sti(sv.substr(loc == sv.npos ? 0 : loc + 1)));
            sv.remove_suffix(loc == sv.npos ? sv.size() : sv.size() - loc);
        }
        if (!sv.empty())
        {
            throw std::invalid_argument("Too much data");
        }
        return ret;
    }
};

template <>
struct ToStr<In6Addr>
{
    using type = In6Addr;
    using ToHex = IntToStr<16, uint16_t>;
    // 8 hextets * 4 hex chars + 7 separators
    static constexpr std::size_t buf_size = 35 + ToHex::buf_size;

    template <typename CharT>
    constexpr CharT* operator()(CharT* buf, In6Addr v) const noexcept
    {
        // IPv4 in IPv6 Addr
        if (v.s6_addr32[0] == 0 && v.s6_addr32[1] == 0 && v.s6_addr16[4] == 0 &&
            v.s6_addr16[5] == 0xffff)
        {
            constexpr auto prefix = std::string_view("::ffff:");
            return ToStr<In4Addr>{}(
                std::copy(prefix.begin(), prefix.end(), buf),
                in_addr{v.s6_addr32[3]});
        }

        size_t skip_start = 0;
        size_t skip_size = 0;
        {
            size_t new_start = 0;
            size_t new_size = 0;
            for (size_t i = 0; i < 9; ++i)
            {
                if (i < 8 && v.s6_addr16[i] == 0)
                {
                    if (new_start + new_size == i)
                    {
                        new_size++;
                    }
                    else
                    {
                        new_start = i;
                        new_size = 1;
                    }
                }
                else if (new_start + new_size == i && new_size > skip_size)
                {
                    skip_start = new_start;
                    skip_size = new_size;
                }
            }
        }
        for (size_t i = 0; i < 8; ++i)
        {
            if (i == skip_start && skip_size > 1)
            {
                if (i == 0)
                {
                    *(buf++) = ':';
                }
                *(buf++) = ':';
                i += skip_size - 1;
                continue;
            }
            buf = ToHex{}(buf, ntoh(v.s6_addr16[i]));
            if (i < 7)
            {
                *(buf++) = ':';
            }
        }
        return buf;
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

template <>
struct FromStr<InAnyAddr>
{
    template <typename CharT>
    constexpr InAnyAddr operator()(std::basic_string_view<CharT> sv) const
    {
        if (sv.find(':') == sv.npos)
        {
            return FromStr<In4Addr>{}(sv);
        }
        return FromStr<In6Addr>{}(sv);
    }
};

template <>
struct ToStr<InAnyAddr>
{
    using type = InAnyAddr;
    static constexpr std::size_t buf_size = std::max(ToStr<In4Addr>::buf_size,
                                                     ToStr<In6Addr>::buf_size);

    template <typename CharT>
    constexpr CharT* operator()(CharT* buf, InAnyAddr v) const noexcept
    {
        return std::visit([=](auto v) { return ToStr<decltype(v)>{}(buf, v); },
                          v);
    }
};

} // namespace stdplus

template <>
struct std::hash<stdplus::In4Addr>
{
    constexpr std::size_t operator()(stdplus::In4Addr addr) const noexcept
    {
        return stdplus::hashMulti(addr.s4_addr32);
    }
};

template <>
struct std::hash<stdplus::In6Addr>
{
    constexpr std::size_t operator()(stdplus::In6Addr addr) const noexcept
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

template <typename CharT>
struct fmt::formatter<stdplus::In4Addr, CharT> :
    stdplus::Format<stdplus::ToStr<stdplus::In4Addr>, CharT>
{};

template <typename CharT>
struct fmt::formatter<stdplus::In6Addr, CharT> :
    stdplus::Format<stdplus::ToStr<stdplus::In6Addr>, CharT>
{};

template <typename CharT>
struct fmt::formatter<stdplus::InAnyAddr, CharT> :
    stdplus::Format<stdplus::ToStr<stdplus::InAnyAddr>, CharT>
{};