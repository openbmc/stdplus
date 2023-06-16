#include <fmt/core.h>

#include <stdplus/net/addr/ip.hpp>
#include <stdplus/numeric/endian.hpp>
#include <stdplus/numeric/str.hpp>
#include <stdplus/str/conv.hpp>

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
    return In4Addr{in_addr{a.s4_addr32 & addr32Mask(pfx)}};
}

constexpr In6Addr addrToSubnet(In6Addr a, std::size_t pfx, std::size_t i = 0,
                               std::size_t s = 0, In6Addr ret = {}) noexcept
{
    if (s + 32 < pfx)
    {
        ret.s6_addr32[i] = a.s6_addr32[i];
        return addrToSubnet(a, pfx, i + 1, s + 32, ret);
    }
    ret.s6_addr32[i] = a.s6_addr32[i] & addr32Mask(pfx - s);
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
    static constexpr inline std::size_t maxPfx = sizeof(Addr) * 8;
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

using Subnet4 = detail::Subnet46<In4Addr, std::uint8_t>;
using Subnet6 = detail::Subnet46<In6Addr, std::uint8_t>;

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

    template <typename T, typename S>
    constexpr SubnetAny(detail::Subnet46<T, S> o) noexcept :
        addr(o.getAddr()), pfx(o.getPfx())
    {}

    constexpr auto getAddr() const noexcept
    {
        return addr;
    }

    constexpr auto getPfx() const noexcept
    {
        return pfx;
    }

    template <typename T, typename S>
    constexpr bool operator==(detail::Subnet46<T, S> rhs) const noexcept
    {
        return addr == rhs.getAddr() && pfx == rhs.getPfx();
    }
    constexpr bool operator==(SubnetAny rhs) const noexcept
    {
        return addr == rhs.addr && pfx == rhs.pfx;
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

template <typename Subnet>
struct SubnetToStr
{
    using type = Subnet;
    using ToDec = IntToStr<10, typename Subnet::Pfx>;
    // Addr + sep + 3 prefix chars
    static constexpr std::size_t buf_size =
        ToStr<typename Subnet::Addr>::buf_size + 1 + ToDec::buf_size;

    template <typename CharT>
    constexpr CharT* operator()(CharT* buf, Subnet v) const noexcept
    {
        buf = ToStr<typename Subnet::Addr>{}(buf, v.getAddr());
        (buf++)[0] = '/';
        return ToDec{}(buf, v.getPfx());
    }
};

template <typename Addr, typename Pfx>
struct ToStr<detail::Subnet46<Addr, Pfx>> :
    SubnetToStr<detail::Subnet46<Addr, Pfx>>
{};

template <>
struct ToStr<SubnetAny> : SubnetToStr<SubnetAny>
{};

} // namespace stdplus

template <typename Addr, typename Pfx, typename CharT>
struct fmt::formatter<stdplus::detail::Subnet46<Addr, Pfx>, CharT> :
    stdplus::Format<stdplus::ToStr<stdplus::detail::Subnet46<Addr, Pfx>>, CharT>
{};

template <typename CharT>
struct fmt::formatter<stdplus::SubnetAny, CharT> :
    stdplus::Format<stdplus::ToStr<stdplus::SubnetAny>, CharT>
{};
