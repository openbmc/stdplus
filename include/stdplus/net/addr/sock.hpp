#pragma once
#include <fmt/core.h>
#include <netinet/in.h>
#include <sys/un.h>

#include <stdplus/net/addr/ip.hpp>
#include <stdplus/numeric/endian.hpp>
#include <stdplus/numeric/str.hpp>
#include <stdplus/str/conv.hpp>
#include <stdplus/variant.hpp>

#include <array>
#include <cstdint>
#include <format>
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

template <>
struct FromStr<Sock4Addr>
{
    template <typename CharT>
    constexpr Sock4Addr operator()(std::basic_string_view<CharT> sv) const
    {
        const auto pos = sv.rfind(':');
        if (pos == sv.npos)
        {
            throw std::invalid_argument("Invalid string for Sock4Addr");
        }
        return {FromStr<In4Addr>{}(sv.substr(0, pos)),
                StrToInt<10, std::uint16_t>{}(sv.substr(pos + 1))};
    }
};

template <>
struct ToStr<Sock4Addr>
{
    using type = Sock4Addr;
    using FromAddr = ToStr<In4Addr>;
    using FromDec = IntToStr<10, std::uint16_t>;
    // Addr + sep + port chars
    static inline constexpr std::size_t buf_size = FromAddr::buf_size + 1 +
                                                   FromDec::buf_size;

    template <typename CharT>
    constexpr CharT* operator()(CharT* buf, Sock4Addr v) const noexcept
    {
        buf = FromAddr{}(buf, v.addr);
        (buf++)[0] = ':';
        return FromDec{}(buf, v.port);
    }
};

template <>
struct FromStr<Sock6Addr>
{
    template <typename CharT>
    constexpr Sock6Addr operator()(std::basic_string_view<CharT> sv) const
    {
        const auto pos = sv.rfind(':');
        if (pos == sv.npos)
        {
            throw std::invalid_argument("Invalid string for Sock6Addr");
        }
        const auto v6seg = sv.substr(0, pos);
        if (!v6seg.starts_with('[') || !v6seg.ends_with(']'))
        {
            throw std::invalid_argument("Invalid string for Sock6Addr");
        }
        return {FromStr<In6Addr>{}(v6seg.substr(1, v6seg.size() - 2)),
                StrToInt<10, std::uint16_t>{}(sv.substr(pos + 1)), 0};
    }
};

template <>
struct ToStr<Sock6Addr>
{
    using type = Sock6Addr;
    using FromAddr = ToStr<In6Addr>;
    using FromDec = IntToStr<10, std::uint16_t>;
    // Addr + sep + port chars
    static inline constexpr std::size_t buf_size = FromAddr::buf_size + 1 +
                                                   FromDec::buf_size;

    template <typename CharT>
    constexpr CharT* operator()(CharT* buf, Sock6Addr v) const noexcept
    {
        (buf++)[0] = '[';
        buf = FromAddr{}(buf, v.addr);
        (buf++)[0] = ']';
        (buf++)[0] = ':';
        return FromDec{}(buf, v.port);
    }
};

namespace detail
{
static inline constexpr std::string_view upfx = "unix:";
}

template <>
struct FromStr<SockUAddr>
{
    template <typename CharT>
    constexpr SockUAddr operator()(std::basic_string_view<CharT> sv) const
    {
        if (sv.starts_with(detail::upfx))
        {
            sv = sv.substr(detail::upfx.size());
        }
        return SockUAddr{sv};
    }
};

template <>
struct ToStr<SockUAddr>
{
    using type = SockUAddr;
    static inline constexpr std::size_t buf_size = detail::upfx.size() +
                                                   SockUAddr::maxLen;

    template <typename CharT>
    constexpr CharT* operator()(CharT* buf, const SockUAddr& v) const noexcept
    {
        buf = std::copy(detail::upfx.begin(), detail::upfx.end(), buf);
        auto p = v.path();
        return std::copy(p.begin(), p.end(), buf);
    }
};

template <>
struct FromStr<SockAnyAddr>
{
    template <typename CharT>
    constexpr SockAnyAddr operator()(std::basic_string_view<CharT> sv) const
    {
        if (sv.starts_with('['))
        {
            return FromStr<Sock6Addr>{}(sv);
        }
        else if (sv.starts_with(detail::upfx))
        {
            return FromStr<SockUAddr>{}(sv);
        }
        return FromStr<Sock4Addr>{}(sv);
    }
};

template <>
struct ToStr<SockAnyAddr>
{
    using type = SockAnyAddr;
    static inline constexpr std::size_t buf_size =
        std::max({ToStr<Sock4Addr>::buf_size, ToStr<Sock6Addr>::buf_size,
                  ToStr<SockUAddr>::buf_size});

    template <typename CharT>
    constexpr CharT* operator()(CharT* buf, const SockAnyAddr& v) const noexcept
    {
        return std::visit(
            [buf]<typename T>(const T& t) { return ToStr<T>{}(buf, t); }, v);
    }
};

} // namespace stdplus

template <stdplus::SockAddr T, typename CharT>
struct fmt::formatter<T, CharT> : stdplus::Format<stdplus::ToStr<T>, CharT>
{};

template <stdplus::SockAddr T, typename CharT>
struct std::formatter<T, CharT> : stdplus::Format<stdplus::ToStr<T>, CharT>
{};
