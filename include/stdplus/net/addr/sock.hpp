#pragma once
#include <netinet/in.h>
#include <sys/un.h>

#include <stdplus/net/addr/ip.hpp>
#include <stdplus/numeric/endian.hpp>
#include <stdplus/variant.hpp>

#include <array>
#include <cstdint>
#include <string_view>

namespace stdplus
{

struct SockAddrBuf : sockaddr
{
    static inline constexpr std::size_t maxLen =
        std::max(
            {sizeof(sockaddr_in), sizeof(sockaddr_in6), sizeof(sockaddr_un)}) -
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

    constexpr SockAddrBuf buf() const noexcept
    {
        SockAddrBuf ret;
        reinterpret_cast<sockaddr_in&>(ret) = sockaddr();
        ret.len = sockaddrLen();
        return ret;
    }
    constexpr operator SockAddrBuf() const noexcept
    {
        return buf();
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

    constexpr SockAddrBuf buf() const noexcept
    {
        SockAddrBuf ret;
        reinterpret_cast<sockaddr_in6&>(ret) = sockaddr();
        ret.len = sockaddrLen();
        return ret;
    }
    constexpr operator SockAddrBuf() const noexcept
    {
        return buf();
    }
};

template <>
struct IsSockAddr<Sock6Addr> : std::true_type
{};

struct SockUAddr
{
    static inline constexpr std::size_t maxLen =
        std::size(sockaddr_un{}.sun_path);

    constexpr explicit SockUAddr(std::string_view path) : len_(path.size())
    {
        if (path.empty())
        {
            return;
        }
        bool abstract = path[0] == '@' || path[0] == '\0';
        // Abstract sockets are not null terminated but path sockets are
        if (path.size() >= maxLen + (abstract ? 1 : 0))
        {
            throw std::invalid_argument("Socket path too long");
        }
        if (!abstract && path.find('\0') != path.npos)
        {
            throw std::invalid_argument("Null bytes in non-abtract path");
        }
        buf_[0] = abstract ? '@' : path[0];
        std::copy(path.begin() + 1, path.end(), buf_.begin() + 1);
    }

    constexpr bool operator==(const SockUAddr& rhs) const noexcept
    {
        return path() == rhs.path();
    }

    constexpr std::string_view path() const noexcept
    {
        return {buf_.data(), len_};
    }

    constexpr sockaddr_un sockaddr() const noexcept
    {
        sockaddr_un ret;
        fill(ret);
        return ret;
    }
    constexpr operator sockaddr_un() const noexcept
    {
        return sockaddr();
    }

    constexpr std::size_t sockaddrLen() const noexcept
    {
        return sizeof(sockaddr_un{}.sun_family) + len_ +
               (len_ > 0 && buf_[0] != '@' ? 1 : 0);
    }

    constexpr SockAddrBuf buf() const noexcept
    {
        SockAddrBuf ret;
        fill(reinterpret_cast<sockaddr_un&>(ret));
        ret.len = sockaddrLen();
        return ret;
    }
    constexpr operator SockAddrBuf() const noexcept
    {
        return buf();
    }

  private:
    std::array<char, maxLen> buf_ = {};
    std::uint8_t len_;
    static_assert(maxLen <= std::numeric_limits<decltype(len_)>::max());

    constexpr void fill(sockaddr_un& v) const noexcept
    {
        v.sun_family = AF_UNIX;
        bool abstract = buf_[0] == '@';
        v.sun_path[0] = abstract ? '\0' : buf_[0];
        std::copy(buf_.begin() + 1, buf_.begin() + len_, v.sun_path + 1);
        if (abstract)
        {
            v.sun_path[len_] = '\0';
        }
    };
};

template <>
struct IsSockAddr<SockUAddr> : std::true_type
{};

namespace detail
{

using SockAnyAddrV = std::variant<Sock4Addr, Sock6Addr, SockUAddr>;

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

    constexpr SockAddrBuf sockaddr() const noexcept
    {
        return std::visit([](const auto& t) { return t.buf(); }, *this);
    }

    constexpr std::size_t sockaddrLen() const noexcept
    {
        return std::visit([](const auto& t) { return t.sockaddrLen(); }, *this);
    }

    constexpr SockAddrBuf buf() const noexcept
    {
        return sockaddr();
    }
    constexpr operator SockAddrBuf() const noexcept
    {
        return sockaddr();
    }
};

template <>
struct IsSockAddr<SockAnyAddr> : std::true_type
{};

} // namespace stdplus
