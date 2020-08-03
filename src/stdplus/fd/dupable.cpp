#include <fcntl.h>
#include <stdplus/fd/dupable.hpp>
#include <stdplus/util/cexec.hpp>
#include <utility>

namespace stdplus
{
namespace detail
{

int ref(const int& fd)
{
    return CHECK_ERRNO(fcntl(fd, F_DUPFD_CLOEXEC, fd), "fcntl dupfd_cloexec");
}

} // namespace detail

DupableFd::DupableFd(const int& fd) : handle(fd)
{
}

DupableFd::DupableFd(int&& fd) : handle(std::move(fd))
{
    detail::setCloExec(fd, true);
}

int DupableFd::release()
{
    return handle.release();
}

int DupableFd::get()
{
    return handle.value();
}

} // namespace stdplus
