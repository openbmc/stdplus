#include <fcntl.h>
#include <fmt/format.h>
#include <stdplus/exception.hpp>
#include <stdplus/fd/impl.hpp>
#include <stdplus/util/cexec.hpp>
#include <string_view>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

namespace stdplus
{
namespace fd
{

using namespace std::literals::string_view_literals;

span<std::byte> FdImpl::read(span<std::byte> buf)
{
    ssize_t amt = ::read(get(), buf.data(), buf.size());
    if (amt == -1)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            return {};
        }
        throw util::makeSystemError(errno, "read");
    }
    else if (amt == 0)
    {
        throw exception::Eof("read");
    }
    return buf.subspan(0, amt);
}

span<std::byte> FdImpl::recv(span<std::byte> buf, RecvFlags flags)
{
    ssize_t amt =
        ::recv(get(), buf.data(), buf.size(), static_cast<int>(flags));
    if (amt == -1)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            return {};
        }
        throw util::makeSystemError(errno, "recv");
    }
    else if (amt == 0)
    {
        throw exception::Eof("recv");
    }
    return buf.subspan(0, amt);
}

span<const std::byte> FdImpl::write(span<const std::byte> data)
{
    ssize_t amt = ::write(get(), data.data(), data.size());
    if (amt == -1)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            return {};
        }
        throw util::makeSystemError(errno, "write");
    }
    return data.subspan(0, amt);
}

span<const std::byte> FdImpl::send(span<const std::byte> data, SendFlags flags)
{
    ssize_t amt =
        ::send(get(), data.data(), data.size(), static_cast<int>(flags));
    if (amt == -1)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            return {};
        }
        throw util::makeSystemError(errno, "send");
    }
    return data.subspan(0, amt);
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
                       fmt::format("lseek {}B {}", offset, whenceStr(whence)));
}

void FdImpl::truncate(off_t size)
{
    CHECK_ERRNO(::ftruncate(get(), size), fmt::format("ftruncate {}B", size));
}

void FdImpl::bind(span<const std::byte> sockaddr)
{
    CHECK_ERRNO(
        ::bind(get(), reinterpret_cast<const struct sockaddr*>(sockaddr.data()),
               sockaddr.size()),
        "bind");
}

void FdImpl::setsockopt(SockLevel level, SockOpt optname,
                        span<const std::byte> opt)
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
                       fmt::format("ioctl {:#x}", id));
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

} // namespace fd
} // namespace stdplus
