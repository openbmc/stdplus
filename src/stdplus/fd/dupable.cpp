#include <stdplus/fd/dupable.hpp>
#include <stdplus/fd/ops/fcntl.hpp>
#include <stdplus/fd/sys.hpp>
#include <stdplus/util/cexec.hpp>
#include <utility>

namespace stdplus
{
namespace fd
{
namespace detail
{

int ref(const int& fd, const Sys*& sys)
{
    return util::callCheckErrno("fcntl_dupfd_cloexec",
                                &Sys::fcntl_dupfd_cloexec, sys, fd);
}

} // namespace detail

DupableFd::DupableFd(const int& fd, const Sys* sys) : handle(fd, sys)
{
}

DupableFd::DupableFd(int&& fd, const Sys* sys) : handle(std::move(fd), sys)
{
    ops::setFdCloexec(*this, true);
}

} // namespace fd
} // namespace stdplus
