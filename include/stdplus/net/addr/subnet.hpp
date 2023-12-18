#include <fmt/core.h>

#include <stdplus/net/addr/ip.hpp>
#include <stdplus/numeric/endian.hpp>
#include <stdplus/numeric/str.hpp>
#include <stdplus/str/conv.hpp>

#include <bit>
#include <limits>
#include <type_traits>

namespace stdplus
{
namespace detail
{

// AddressSan doesn't understand our masking of shift UB
__attribute__((no_sanitize("undefined"))) constexpr uint32_t
    addr32Mask(std::ptrdiff_t pfx) noexcept
{
    // NOLINTNEXTLINE(clang-analyzer-core.UndefinedBinaryOperatorResult)
    auto v = ~uint32_t{0} << (32 - pfx);
    // Positive prefix check + mask to handle UB when the left shift becomes
    // more than 31 bits
    return hton(static_cast<uint32_t>(-int32_t{pfx > 0}) & v);
}

constexpr In4Addr addrToSubnet(In4Addr a, std::size_t pfx) noexcept
{
    return In4Addr{in_addr{a.word() & addr32Mask(pfx)}};
}

constexpr In6Addr addrToSubnet(In6Addr a, std::size_t pfx, std::size_t i = 0,
                               std::size_t s = 0, In6Addr ret = {}) noexcept
{
    if (s + 32 < pfx)
    {
        ret.word(i, a.word(i));
        return addrToSubnet(a, pfx, i + 1, s + 32, ret);
    }
    ret.word(i, a.word(i) & addr32Mask(pfx - s));
    return ret;
}

constexpr InAnyAddr addrToSubnet(InAnyAddr a, std::size_t pfx) noexcept
{
    return std::visit([&](auto av) { return InAnyAddr{addrToSubnet(av, pfx)}; },
                      a);
}

constexpr bool subnetContains(auto, auto, std::size_t) noexcept
{
    return false;
}

template <typename T>
constexpr bool subnetContains(T l, T r, std::size_t pfx) noexcept
{
    return addrToSubnet(l, pfx) == addrToSubnet(r, pfx);
}

constexpr bool subnetContains(InAnyAddr l, auto r, std::size_t pfx) noexcept
{
    return std::visit([&](auto v) { return detail::subnetContains(v, r, pfx); },
                      l);
}

constexpr std::size_t addrBits(auto a) noexcept
{
    return sizeof(a) << 3;
}

void invalidSubnetPfx(std::size_t pfx);

template <typename Addr_, typename Pfx_>
class Subnet46
{
  public:
    using Addr = Addr_;
    using Pfx = Pfx_;

  private:
    static inline constexpr std::size_t maxPfx = sizeof(Addr) * 8;
    static_assert(std::is_unsigned_v<Pfx> && std::is_integral_v<Pfx>);
    static_assert(std::numeric_limits<Pfx>::max() >= maxPfx);

    Addr addr;
    Pfx pfx;

  public:
    constexpr Subnet46(Addr addr, Pfx pfx) : addr(addr), pfx(pfx)
    {
        if (addrBits(addr) < pfx)
        {
            invalidSubnetPfx(pfx);
        }
    }

    constexpr auto getAddr() const noexcept
    {
        return addr;
    }

    constexpr auto getPfx() const noexcept
    {
        return pfx;
    }

    constexpr bool operator==(Subnet46 rhs) const noexcept
    {
        return addr == rhs.addr && pfx == rhs.pfx;
    }

    constexpr Addr network() const noexcept
    {
        return addrToSubnet(addr, pfx);
    }

    constexpr bool contains(Addr addr) const noexcept
    {
        return addrToSubnet(this->addr, pfx) == addrToSubnet(addr, pfx);
    }
};

} // namespace detail

template <typename T>
constexpr T pfxToMask(std::uint8_t pfx);

template <>
constexpr In4Addr pfxToMask<In4Addr>(std::uint8_t pfx)
{
    return in_addr{detail::addr32Mask(pfx)};
}

constexpr std::uint8_t maskToPfx(In4Addr mask)
{
    uint32_t x = ntoh(mask.word());
    if ((~x & (~x + 1)) != 0)
    {
        throw std::invalid_argument("Invalid netmask");
    }
    return 32 - std::countr_zero(x);
}

using Subnet4 = detail::Subnet46<In4Addr, std::uint8_t>;
using Subnet6 = detail::Subnet46<In6Addr, std::uint8_t>;

class SubnetAny;

template <typename T>
struct IsSubnet : std::false_type
{};

template <typename Addr, typename Pfx>
struct IsSubnet<detail::Subnet46<Addr, Pfx>> : std::true_type
{};

template <>
struct IsSubnet<SubnetAny> : std::true_type
{};

template <typename T>
concept Subnet = IsSubnet<T>::value;

class SubnetAny
{
  public:
    using Addr = InAnyAddr;
    using Pfx = std::uint8_t;

  private:
    Addr addr;
    Pfx pfx;

  public:
    constexpr SubnetAny(auto addr, Pfx pfx) : addr(addr), pfx(pfx)
    {
        if (detail::addrBits(addr) < pfx)
        {
            detail::invalidSubnetPfx(pfx);
        }
    }
    constexpr SubnetAny(InAnyAddr addr, Pfx pfx) : addr(addr), pfx(pfx)
    {
        if (std::visit([](auto v) { return detail::addrBits(v); }, addr) < pfx)
        {
            detail::invalidSubnetPfx(pfx);
        }
    }

    template <Subnet T>
    constexpr SubnetAny(T o) noexcept : addr(o.getAddr()), pfx(o.getPfx())
    {}

    constexpr auto getAddr() const noexcept
    {
        return addr;
    }

    constexpr auto getPfx() const noexcept
    {
        return pfx;
    }

    template <Subnet T>
    constexpr bool operator==(T rhs) const noexcept
    {
        return addr == rhs.getAddr() && pfx == rhs.getPfx();
    }

    constexpr InAnyAddr network() const noexcept
    {
        return detail::addrToSubnet(addr, pfx);
    }

    constexpr bool contains(In4Addr addr) const noexcept
    {
        return detail::subnetContains(this->addr, addr, pfx);
    }
    constexpr bool contains(in_addr addr) const noexcept
    {
        return contains(In4Addr{addr});
    }
    constexpr bool contains(In6Addr addr) const noexcept
    {
        return detail::subnetContains(this->addr, addr, pfx);
    }
    constexpr bool contains(in6_addr addr) const noexcept
    {
        return contains(In6Addr{addr});
    }
    constexpr bool contains(InAnyAddr addr) const noexcept
    {
        return std::visit([&](auto v) { return contains(v); }, addr);
    }
};

template <Subnet Sub>
struct FromStr<Sub>
{
    template <typename CharT>
    constexpr Sub operator()(std::basic_string_view<CharT> sv) const
    {
        const auto pos = sv.rfind('/');
        if (pos == sv.npos)
        {
            throw std::invalid_argument("Invalid subnet");
        }
        return {FromStr<typename Sub::Addr>{}(sv.substr(0, pos)),
                StrToInt<10, typename Sub::Pfx>{}(sv.substr(pos + 1))};
    }
};

template <Subnet Sub>
struct ToStr<Sub>
{
    using type = Sub;
    using ToDec = IntToStr<10, typename Sub::Pfx>;
    // Addr + sep + 3 prefix chars
    static inline constexpr std::size_t buf_size =
        ToStr<typename Sub::Addr>::buf_size + 1 + ToDec::buf_size;

    template <typename CharT>
    constexpr CharT* operator()(CharT* buf, Sub v) const noexcept
    {
        buf = ToStr<typename Sub::Addr>{}(buf, v.getAddr());
        (buf++)[0] = '/';
        return ToDec{}(buf, v.getPfx());
    }
};

namespace detail
{

template <Subnet Sub>
struct CompileInAddrInt<Sub> : CompileInAddrInt<typename Sub::Addr>
{
    Sub::Pfx pfx = 0;

    template <typename CharT>
    consteval void compile(std::basic_string_view<CharT> sv) noexcept
    {
        const auto pos = sv.rfind('/');
        if (pos == sv.npos)
        {
            this->valid = false;
        }
        static_cast<CompileInAddrInt<typename Sub::Addr>&>(*this).compile(
            sv.substr(0, pos));
        try
        {
            pfx = StrToInt<10, typename Sub::Pfx>{}(sv.substr(pos + 1));
        }
        catch (...)
        {
            this->valid = false;
        }
    }
};

template <typename CharT, std::size_t N>
struct CompileSubnet4 : CompileInAddr<Subnet4, CharT, N>
{
    consteval CompileSubnet4(const CharT (&str)[N]) noexcept :
        CompileInAddr<Subnet4, CharT, N>(str)
    {}
};

template <typename CharT, std::size_t N>
struct CompileSubnet6 : CompileInAddr<Subnet6, CharT, N>
{
    consteval CompileSubnet6(const CharT (&str)[N]) noexcept :
        CompileInAddr<Subnet6, CharT, N>(str)
    {}
};

template <typename CharT, std::size_t N>
struct CompileSubnetAny : CompileInAddr<SubnetAny, CharT, N>
{
    consteval CompileSubnetAny(const CharT (&str)[N]) noexcept :
        CompileInAddr<SubnetAny, CharT, N>(str)
    {}
};

} // namespace detail

inline namespace subnet_literals
{

template <detail::CompileSubnet4 Str>
constexpr auto operator"" _sub4() noexcept
{
    static_assert(Str.valid, "stdplus::Subnet4");
    return Subnet4(Str.addr, Str.pfx);
}

template <detail::CompileSubnet6 Str>
constexpr auto operator"" _sub6() noexcept
{
    static_assert(Str.valid, "stdplus::Subnet6");
    return Subnet6(Str.addr, Str.pfx);
}

template <detail::CompileSubnetAny Str>
constexpr auto operator"" _sub() noexcept
{
    static_assert(Str.valid, "stdplus::SubnetAny");
    return Str.v4 ? SubnetAny(Str.u.addr4, Str.pfx)
                  : SubnetAny(Str.u.addr6, Str.pfx);
}

} // namespace subnet_literals
} // namespace stdplus

template <stdplus::Subnet Sub, typename CharT>
struct fmt::formatter<Sub, CharT> : stdplus::Format<stdplus::ToStr<Sub>, CharT>
{};

template <stdplus::Subnet Sub>
struct std::hash<Sub>
{
    constexpr std::size_t operator()(Sub addr) const noexcept
    {
        return stdplus::hashMulti(addr.getAddr(), addr.getPfx());
    }
};

namespace std
{
template <typename T, typename CharT>
struct formatter;

template <stdplus::Subnet Sub, typename CharT>
struct formatter<Sub, CharT> : stdplus::Format<stdplus::ToStr<Sub>, CharT>
{};
} // namespace std
