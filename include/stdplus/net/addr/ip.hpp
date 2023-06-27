#pragma once
#include <netinet/in.h>

#include <stdplus/hash.hpp>
#include <stdplus/numeric/endian.hpp>
#include <stdplus/numeric/str.hpp>
#include <stdplus/str/conv.hpp>
#include <stdplus/variant.hpp>

#include <algorithm>
#include <array>
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
    constexpr In4Addr() noexcept : In4AddrInner({.s4_addr = {}}) {}
    constexpr In4Addr(in_addr a) noexcept : In4Addr()
    {
        if (!std::is_constant_evaluated())
        {
            this->a = a;
            return;
        }
        auto arr = std::bit_cast<std::array<std::uint8_t, 4>>(a);
        std::copy(arr.begin(), arr.end(), s4_addr);
    }
    explicit constexpr In4Addr(std::initializer_list<uint8_t> a) noexcept :
        In4Addr()
    {
        std::copy(a.begin(), a.end(), s4_addr);
    }

    constexpr std::uint8_t byte(std::size_t i) const noexcept
    {
        return s4_addr[i];
    }

    constexpr void byte(std::size_t i, std::uint8_t v) noexcept
    {
        s4_addr[i] = v;
    }

    constexpr std::uint32_t word() const noexcept
    {
        if (!std::is_constant_evaluated())
        {
            return s4_addr32;
        }
        return std::bit_cast<std::uint32_t>(s4_addr);
    }

    constexpr void word(std::uint32_t v) noexcept
    {
        if (!std::is_constant_evaluated())
        {
            s4_addr32 = v;
            return;
        }
        auto arr = std::bit_cast<std::array<std::uint8_t, 4>>(v);
        std::copy(arr.begin(), arr.end(), s4_addr);
    }

    constexpr operator in_addr() const noexcept
    {
        if (!std::is_constant_evaluated())
        {
            return a;
        }
        return std::bit_cast<in_addr>(s4_addr);
    }

    constexpr bool operator==(in_addr rhs) const noexcept
    {
        return *this == In4Addr{rhs};
    }

    constexpr bool operator==(In4Addr rhs) const noexcept
    {
        return word() == rhs.word();
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
        auto n = bswap(ntoh(v.word()));
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
    constexpr In6Addr(in6_addr a) noexcept : in6_addr(a)
    {
        if (std::is_constant_evaluated())
        {
            std::copy(a.s6_addr, a.s6_addr + std::size(s6_addr), s6_addr);
        }
    }
    explicit constexpr In6Addr(std::initializer_list<uint8_t> a) noexcept :
        In6Addr()
    {
        std::copy(a.begin(), a.end(), s6_addr);
    }

    constexpr std::uint8_t byte(std::size_t i) const noexcept
    {
        return s6_addr[i];
    }

    constexpr void byte(std::size_t i, std::uint8_t v) noexcept
    {
        s6_addr[i] = v;
    }

    constexpr std::uint16_t hextet(std::size_t i) const noexcept
    {
        if (!std::is_constant_evaluated())
        {
            return s6_addr16[i];
        }
        std::uint8_t bytes[sizeof(std::uint16_t)];
        const auto start = s6_addr + i * std::size(bytes);
        std::copy(start, start + std::size(bytes), bytes);
        return std::bit_cast<std::uint16_t>(bytes);
    }

    constexpr void hextet(std::size_t i, std::uint16_t v) noexcept
    {
        if (!std::is_constant_evaluated())
        {
            s6_addr16[i] = v;
            return;
        }
        auto bytes = std::bit_cast<std::array<std::uint8_t, sizeof(v)>>(v);
        std::copy(bytes.begin(), bytes.end(), s6_addr + i * bytes.size());
    }

    constexpr std::uint32_t word(std::size_t i) const noexcept
    {
        if (!std::is_constant_evaluated())
        {
            return s6_addr32[i];
        }
        std::uint8_t bytes[sizeof(std::uint32_t)];
        const auto start = s6_addr + i * std::size(bytes);
        std::copy(start, start + std::size(bytes), bytes);
        return std::bit_cast<std::uint32_t>(bytes);
    }

    constexpr void word(std::size_t i, std::uint32_t v) noexcept
    {
        if (!std::is_constant_evaluated())
        {
            s6_addr32[i] = v;
            return;
        }
        auto bytes = std::bit_cast<std::array<std::uint8_t, sizeof(v)>>(v);
        std::copy(bytes.begin(), bytes.end(), s6_addr + i * bytes.size());
    }

    constexpr bool operator==(in6_addr rhs) const noexcept
    {
        if (!std::is_constant_evaluated())
        {
            return std::equal(s6_addr32, s6_addr32 + 4, rhs.s6_addr32);
        }
        return std::equal(s6_addr, s6_addr + 16, rhs.s6_addr);
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
        In6Addr ret = {};
        size_t i = 0;
        while (i < 8)
        {
            auto loc = sv.find(':');
            if (i == 6 && loc == sv.npos)
            {
                ret.word(3, fsip4(sv).word());
                return ret;
            }
            if (loc != 0 && !sv.empty())
            {
                ret.hextet(i++, hton(sti(sv.substr(0, loc))));
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
            ret.word(3, fsip4(sv.substr(loc == sv.npos ? 0 : loc + 1)).word());
            sv.remove_suffix(loc == sv.npos ? sv.size() : sv.size() - loc);
            j -= 2;
        }
        while (!sv.empty() && j > i)
        {
            auto loc = sv.rfind(':');
            ret.hextet(j--, hton(sti(sv.substr(loc == sv.npos ? 0 : loc + 1))));
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
        if (v.word(0) == 0 && v.word(1) == 0 && v.hextet(4) == 0 &&
            v.hextet(5) == 0xffff)
        {
            constexpr auto prefix = std::string_view("::ffff:");
            return ToStr<In4Addr>{}(
                std::copy(prefix.begin(), prefix.end(), buf),
                in_addr{v.word(3)});
        }

        size_t skip_start = 0;
        size_t skip_size = 0;
        {
            size_t new_start = 0;
            size_t new_size = 0;
            for (size_t i = 0; i < 9; ++i)
            {
                if (i < 8 && v.hextet(i) == 0)
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
            buf = ToHex{}(buf, ntoh(v.hextet(i)));
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

namespace detail
{

template <typename Addr, typename CharT, std::size_t N>
struct CompileInAddr
{
    CharT str[N - 1];
    Addr addr = {};
    bool valid = true;

    constexpr CompileInAddr(const CharT (&str)[N]) noexcept
    {
        std::copy(str, str + N - 1, this->str);
        std::basic_string_view<CharT> sv{this->str, N - 1};
        try
        {
            addr = fromStr<Addr>(sv);
        }
        catch (...)
        {
            valid = false;
        }
    }
};

template <typename CharT, std::size_t N>
struct CompileIn4Addr : CompileInAddr<In4Addr, CharT, N>
{
    constexpr CompileIn4Addr(const CharT (&str)[N]) noexcept :
        CompileInAddr<In4Addr, CharT, N>(str)
    {}
};

template <typename CharT, std::size_t N>
struct CompileIn6Addr : CompileInAddr<In6Addr, CharT, N>
{
    constexpr CompileIn6Addr(const CharT (&str)[N]) noexcept :
        CompileInAddr<In6Addr, CharT, N>(str)
    {}
};

template <typename CharT, std::size_t N>
struct CompileInAnyAddr
{
    CharT str[N - 1];
    union
    {
        stdplus::In4Addr addr4;
        stdplus::In6Addr addr6;
    } u;
    bool v4 = true;
    bool valid = true;

    constexpr CompileInAnyAddr(const CharT (&str)[N]) noexcept :
        u({.addr4 = {}})
    {
        std::copy(str, str + N - 1, this->str);
        std::basic_string_view<CharT> sv{this->str, N - 1};
        try
        {
            if (sv.find(':') == sv.npos)
            {
                u = {.addr4 = FromStr<In4Addr>{}(sv)};
                return;
            }
            u = {.addr6 = FromStr<In6Addr>{}(sv)};
            v4 = false;
        }
        catch (...)
        {
            valid = false;
        }
    }
};

} // namespace detail

inline namespace in_addr_literals
{

template <detail::CompileIn4Addr Str>
constexpr auto operator"" _ip4() noexcept
{
    static_assert(Str.valid, "stdplus::In4Addr");
    return Str.addr;
}

template <detail::CompileIn6Addr Str>
constexpr auto operator"" _ip6() noexcept
{
    static_assert(Str.valid, "stdplus::In6Addr");
    return Str.addr;
}

template <detail::CompileInAnyAddr Str>
constexpr auto operator"" _ip() noexcept
{
    static_assert(Str.valid, "stdplus::InAnyAddr");
    return Str.v4 ? InAnyAddr(Str.u.addr4) : InAnyAddr(Str.u.addr6);
}

} // namespace in_addr_literals

} // namespace stdplus

template <>
struct std::hash<stdplus::In4Addr>
{
    constexpr std::size_t operator()(stdplus::In4Addr addr) const noexcept
    {
        return stdplus::hashMulti(addr.word());
    }
};

template <>
struct std::hash<stdplus::In6Addr>
{
    constexpr std::size_t operator()(stdplus::In6Addr addr) const noexcept
    {
        if (std::is_constant_evaluated())
        {
            return stdplus::hashMulti(addr.s6_addr);
        }
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
