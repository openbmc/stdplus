#include <netinet/in.h>

#include <stdplus/net/addr/ip.hpp>
#include <stdplus/numeric/endian.hpp>
#include <stdplus/variant.hpp>

#include <array>
#include <cstdint>

namespace stdplus
{

struct SockAnyBuf : sockaddr
{
    static inline constexpr std::size_t maxLen =
        std::max({sizeof(sockaddr_in), sizeof(sockaddr_in6)}) -
        sizeof(sockaddr);
    std::array<std::uint8_t, maxLen> buf;
    std::uint8_t len;
    static_assert(maxLen <= std::numeric_limits<decltype(len)>::max());
};

template <typename T>
struct IsSockAddr : std::false_type
{};

template <typename T>
concept SockAddr = IsSockAddr<T>::value;

struct Sock4Addr
{
    In4Addr addr;
    std::uint16_t port;

    constexpr bool operator==(Sock4Addr rhs) const noexcept
    {
        return addr == rhs.addr && port == rhs.port;
    }

    constexpr sockaddr_in sockaddr() const noexcept
    {
        return {.sin_family = AF_INET,
                .sin_port = stdplus::hton(port),
                .sin_addr = addr,
                .sin_zero = {}};
    }
    constexpr operator sockaddr_in() const noexcept
    {
        return sockaddr();
    }

    static constexpr std::size_t sockaddrLen() noexcept
    {
        return sizeof(sockaddr_in);
    }

    constexpr SockAnyBuf sockbuf() const noexcept
    {
        SockAnyBuf ret;
        reinterpret_cast<sockaddr_in&>(ret) = sockaddr();
        ret.len = sockaddrLen();
        return ret;
    }
    constexpr operator SockAnyBuf() const noexcept
    {
        return sockbuf();
    }
};

template <>
struct IsSockAddr<Sock4Addr> : std::true_type
{};

struct Sock6Addr
{
    In6Addr addr;
    std::uint16_t port;
    std::uint32_t scope;

    constexpr bool operator==(Sock6Addr rhs) const noexcept
    {
        return addr == rhs.addr && port == rhs.port && scope == rhs.scope;
    }

    constexpr sockaddr_in6 sockaddr() const noexcept
    {
        return {.sin6_family = AF_INET6,
                .sin6_port = stdplus::hton(port),
                .sin6_flowinfo = 0,
                .sin6_addr = addr,
                .sin6_scope_id = scope};
    }
    constexpr operator sockaddr_in6() const noexcept
    {
        return sockaddr();
    }

    static constexpr std::size_t sockaddrLen() noexcept
    {
        return sizeof(sockaddr_in6);
    }

    constexpr SockAnyBuf sockbuf() const noexcept
    {
        SockAnyBuf ret;
        reinterpret_cast<sockaddr_in6&>(ret) = sockaddr();
        ret.len = sockaddrLen();
        return ret;
    }
    constexpr operator SockAnyBuf() const noexcept
    {
        return sockbuf();
    }
};

template <>
struct IsSockAddr<Sock6Addr> : std::true_type
{};

namespace detail
{

using SockAnyAddrV = std::variant<Sock4Addr, Sock6Addr>;

} // namespace detail

struct SockAnyAddr : detail::SockAnyAddrV
{
    constexpr SockAnyAddr(auto&&... a) noexcept :
        detail::SockAnyAddrV(std::forward<decltype(a)>(a)...)
    {}

    template <SockAddr T>
    constexpr bool operator==(const T& rhs) const noexcept
    {
        return variantEqFuzzy(*this, rhs);
    }

    constexpr SockAnyBuf sockaddr() const noexcept
    {
        return std::visit([](const auto& t) { return t.sockbuf(); }, *this);
    }

    constexpr std::size_t sockaddrLen() const noexcept
    {
        return std::visit([](const auto& t) { return t.sockaddrLen(); }, *this);
    }

    constexpr SockAnyBuf sockbuf() const noexcept
    {
        return sockaddr();
    }
    constexpr operator SockAnyBuf() const noexcept
    {
        return sockaddr();
    }
};

template <>
struct IsSockAddr<SockAnyAddr> : std::true_type
{};

} // namespace stdplus
