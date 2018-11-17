#include <stdplus/fd/dupable.hpp>
#include <stdplus/fd/managed.hpp>
#include <stdplus/util/cexec.hpp>
#include <utility>

namespace stdplus
{
namespace fd
{
namespace detail
{

void drop(int&& fd, const internal::Sys*& sys)
{
    util::callCheckErrno("close", &internal::Sys::close, sys, fd);
}

} // namespace detail

ManagedFd::ManagedFd(int&& fd, const internal::Sys* sys) noexcept :
    detail::ManagedFdHandle(std::move(fd), sys), sys(sys)
{
}

ManagedFd::ManagedFd(DupableFd&& other) noexcept :
    detail::ManagedFdHandle(static_cast<detail::ManagedFdHandle&&>(other)),
    sys(other.getSys())
{
}

ManagedFd::ManagedFd(const DupableFd& other) : ManagedFd(DupableFd(other))
{
}

ManagedFd& ManagedFd::operator=(DupableFd&& other) noexcept
{
    static_cast<detail::ManagedFdHandle&>(*this) =
        static_cast<detail::ManagedFdHandle&&>(other);
    return *this;
}

ManagedFd& ManagedFd::operator=(const DupableFd& other)
{
    return *this = DupableFd(other);
}

} // namespace fd
} // namespace stdplus
