#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <stdplus/exception.hpp>
#include <stdplus/fd/impl.hpp>
#include <stdplus/util/cexec.hpp>

#include <format>
#include <string_view>

namespace stdplus
{
namespace fd
{

using namespace std::literals::string_view_literals;

template <typename Byte>
static std::span<Byte> fret(std::span<Byte> buf, const char* name, ssize_t r)
{
    if (r == -1)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            return {};
        }
        throw util::makeSystemError(errno, name);
    }
    else if (r == 0 && buf.size() > 0)
    {
        throw exception::Eof(name);
    }
    return buf.subspan(0, r);
}

std::span<std::byte> FdImpl::read(std::span<std::byte> buf)
{
    return fret(buf, "read", ::read(get(), buf.data(), buf.size()));
}

std::span<std::byte> FdImpl::recv(std::span<std::byte> buf, RecvFlags flags)
{
    return fret(buf, "recv",
                ::recv(get(), buf.data(), buf.size(), static_cast<int>(flags)));
}

std::tuple<std::span<std::byte>, std::span<std::byte>> FdImpl::recvfrom(
    std::span<std::byte> buf, RecvFlags flags, std::span<std::byte> sockaddr)
{
    socklen_t sockaddrsize = sockaddr.size();
    auto ret =
        fret(buf, "recvfrom",
             ::recvfrom(get(), buf.data(), buf.size(), static_cast<int>(flags),
                        reinterpret_cast<struct sockaddr*>(sockaddr.data()),
                        &sockaddrsize));
    return std::make_tuple(ret, sockaddr.subspan(0, sockaddrsize));
}

std::span<const std::byte> FdImpl::write(std::span<const std::byte> data)
{
    return fret(data, "write", ::write(get(), data.data(), data.size()));
}

std::span<const std::byte> FdImpl::send(std::span<const std::byte> data,
                                        SendFlags flags)
{
    return fret(data, "send",
                ::send(get(), data.data(), data.size(),
                       static_cast<int>(flags)));
}

std::span<const std::byte>
    FdImpl::sendto(std::span<const std::byte> data, SendFlags flags,
                   std::span<const std::byte> sockaddr)
{
    return fret(
        data, "sendto",
        ::sendto(get(), data.data(), data.size(), static_cast<int>(flags),
                 reinterpret_cast<const struct sockaddr*>(sockaddr.data()),
                 sockaddr.size()));
}

static std::string_view whenceStr(Whence whence)
{
    switch (whence)
    {
        case Whence::Set:
            return "set"sv;
        case Whence::Cur:
            return "cur"sv;
        case Whence::End:
            return "end"sv;
        default:
            return "Unknown whence"sv;
    }
}

size_t FdImpl::lseek(off_t offset, Whence whence)
{
    return CHECK_ERRNO(::lseek(get(), offset, static_cast<int>(whence)),
                       std::format("lseek {}B {}", offset, whenceStr(whence)));
}

void FdImpl::truncate(off_t size)
{
    CHECK_ERRNO(::ftruncate(get(), size), std::format("ftruncate {}B", size));
}

void FdImpl::bind(std::span<const std::byte> sockaddr)
{
    CHECK_ERRNO(
        ::bind(get(), reinterpret_cast<const struct sockaddr*>(sockaddr.data()),
               sockaddr.size()),
        "bind");
}

void FdImpl::connect(std::span<const std::byte> sockaddr)
{
    CHECK_ERRNO(
        ::connect(get(),
                  reinterpret_cast<const struct sockaddr*>(sockaddr.data()),
                  sockaddr.size()),
        "connect");
}

void FdImpl::listen(int backlog)
{
    CHECK_ERRNO(::listen(get(), backlog), "listen");
}

std::optional<std::tuple<int, std::span<std::byte>>>
    FdImpl::accept(std::span<std::byte> sockaddr)
{
    socklen_t len = sockaddr.size();
    auto fd = ::accept(
        get(), reinterpret_cast<struct sockaddr*>(sockaddr.data()), &len);
    if (fd == -1)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            return std::nullopt;
        }
        throw util::makeSystemError(errno, "accept");
    }
    return std::make_tuple(fd, sockaddr.subspan(0, len));
}

void FdImpl::setsockopt(SockLevel level, SockOpt optname,
                        std::span<const std::byte> opt)
{
    CHECK_ERRNO(::setsockopt(get(), static_cast<int>(level),
                             static_cast<int>(optname), opt.data(), opt.size()),
                "setsockopt");
}

int FdImpl::ioctl(unsigned long id, void* data)
{
    return constIoctl(id, data);
}

int FdImpl::constIoctl(unsigned long id, void* data) const
{
    return CHECK_ERRNO(::ioctl(get(), id, data),
                       std::format("ioctl {:#x}", id));
}

void FdImpl::fcntlSetfd(FdFlags flags)
{
    CHECK_ERRNO(::fcntl(get(), F_SETFD, static_cast<int>(flags)),
                "fcntl setfd");
}

FdFlags FdImpl::fcntlGetfd() const
{
    return FdFlags(CHECK_ERRNO(::fcntl(get(), F_GETFD), "fcntl getfd"));
}

void FdImpl::fcntlSetfl(FileFlags flags)
{
    CHECK_ERRNO(::fcntl(get(), F_SETFL, static_cast<int>(flags)),
                "fcntl setfl");
}

FileFlags FdImpl::fcntlGetfl() const
{
    return FileFlags(CHECK_ERRNO(::fcntl(get(), F_GETFL), "fcntl getfl"));
}

std::span<std::byte> FdImpl::mmap(std::span<std::byte> window, ProtFlags prot,
                                  MMapFlags flags, off_t offset)
{
    auto ret = ::mmap(window.data(), window.size(), static_cast<int>(prot),
                      static_cast<int>(flags), get(), offset);
    if (ret == MAP_FAILED)
    {
        util::doError(errno, "mmap");
    }
    return {reinterpret_cast<std::byte*>(ret), window.size()};
}

void FdImpl::munmap(std::span<std::byte> window)
{
    CHECK_ERRNO(::munmap(window.data(), window.size()), "munmap");
}

} // namespace fd
} // namespace stdplus
