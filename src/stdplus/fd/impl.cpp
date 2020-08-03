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

using namespace std::literals::string_view_literals;

stdplus::span<std::byte> FdImpl::read(stdplus::span<std::byte> buf)
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

stdplus::span<std::byte> FdImpl::recv(stdplus::span<std::byte> buf, int flags)
{
    ssize_t amt = ::recv(get(), buf.data(), buf.size(), flags);
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

stdplus::span<const std::byte>
    FdImpl::write(stdplus::span<const std::byte> data)
{
    ssize_t amt = ::write(get(), data.data(), data.size());
    if (amt == -1)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            return {};
        }
        throw stdplus::util::makeSystemError(errno, "write");
    }
    return data.subspan(0, amt);
}

stdplus::span<const std::byte> FdImpl::send(stdplus::span<const std::byte> data,
                                            int flags)
{
    ssize_t amt = ::send(get(), data.data(), data.size(), flags);
    if (amt == -1)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            return {};
        }
        throw stdplus::util::makeSystemError(errno, "send");
    }
    return data.subspan(0, amt);
}

static std::string_view whenceStr(int whence)
{
    switch (whence)
    {
        case SEEK_SET:
            return "set"sv;
        case SEEK_CUR:
            return "cur"sv;
        case SEEK_END:
            return "end"sv;
        default:
            return "Unknown whence"sv;
    }
}

size_t FdImpl::lseek(off_t offset, int whence)
{
    return CHECK_ERRNO(::lseek(get(), offset, whence),
                       fmt::format("lseek {}B {}", offset, whenceStr(whence)));
}

void FdImpl::truncate(off_t size)
{
    CHECK_ERRNO(::ftruncate(get(), size), fmt::format("ftruncate {}B", size));
}

void FdImpl::bind(stdplus::span<const std::byte> sockaddr)
{
    CHECK_ERRNO(
        ::bind(get(), reinterpret_cast<const struct sockaddr*>(sockaddr.data()),
               sockaddr.size()),
        "bind");
}

void FdImpl::setsockopt(int level, int optname,
                        stdplus::span<const std::byte> opt)
{
    CHECK_ERRNO(::setsockopt(get(), level, optname, opt.data(), opt.size()),
                "setsockopt");
}

int FdImpl::ioctl(unsigned long id, void* data)
{
    return CHECK_ERRNO(::ioctl(get(), id, data),
                       fmt::format("ioctl {:#x}", id));
}

void FdImpl::fcntl_setfl(int flags)
{
    CHECK_ERRNO(::fcntl(get(), F_SETFL, flags), "fcntl setfl");
}

} // namespace stdplus
