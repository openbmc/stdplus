#include <fcntl.h>

#include <stdplus/fd/dupable.hpp>
#include <stdplus/fd/ops.hpp>
#include <stdplus/util/cexec.hpp>

#include <utility>

namespace stdplus
{
namespace fd
{
namespace detail
{

int ref(const int& fd)
{
    return CHECK_ERRNO(fcntl(fd, F_DUPFD_CLOEXEC, fd), "fcntl dupfd_cloexec");
}

} // namespace detail

DupableFd::DupableFd() noexcept : handle(std::nullopt) {}

DupableFd::DupableFd(const int& fd) : handle(fd) {}

DupableFd::DupableFd(int&& fd) : handle(std::move(fd))
{
    fd::setFdFlags(*this, fd::getFdFlags(*this).set(fd::FdFlag::CloseOnExec));
}

int DupableFd::release()
{
    return handle.release();
}

int DupableFd::get() const
{
    return handle.value();
}

} // namespace fd
} // namespace stdplus
