#include <unistd.h>

#include <stdplus/fd/dupable.hpp>
#include <stdplus/fd/managed.hpp>
#include <stdplus/fd/ops.hpp>
#include <stdplus/util/cexec.hpp>

#include <utility>

namespace stdplus
{
namespace fd
{
namespace detail
{

void drop(int&& fd)
{
    CHECK_ERRNO(close(fd), "close");
}

} // namespace detail

ManagedFd::ManagedFd() noexcept : handle(std::nullopt) {}

ManagedFd::ManagedFd(int&& fd) : handle(std::move(fd))
{
    fd::setFdFlags(*this, fd::getFdFlags(*this).set(fd::FdFlag::CloseOnExec));
}

ManagedFd::ManagedFd(DupableFd&& other) noexcept :
    handle(static_cast<detail::ManagedFdHandle&&>(other.handle))
{}

ManagedFd::ManagedFd(const DupableFd& other) : ManagedFd(DupableFd(other)) {}

ManagedFd& ManagedFd::operator=(DupableFd&& other) noexcept
{
    handle = static_cast<detail::ManagedFdHandle&&>(other.handle);
    return *this;
}

ManagedFd& ManagedFd::operator=(const DupableFd& other)
{
    return *this = DupableFd(other);
}

int ManagedFd::release()
{
    return handle.release();
}

int ManagedFd::get() const
{
    return handle.value();
}

ManagedFd::operator bool() const noexcept
{
    return static_cast<bool>(handle);
}

} // namespace fd
} // namespace stdplus
