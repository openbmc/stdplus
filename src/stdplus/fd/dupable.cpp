#include <stdplus/fd/dupable.hpp>
#include <stdplus/util/cexec.hpp>
#include <utility>

namespace stdplus
{
namespace fd
{
namespace detail
{

int ref(const int& fd, const internal::Sys*& sys)
{
    return util::callCheckErrno("dup", &internal::Sys::dup, sys, fd);
}

} // namespace detail

DupableFd::DupableFd(const int& fd, const internal::Sys* sys) :
    detail::DupableFdHandle(fd, sys), sys(sys)
{
}

DupableFd::DupableFd(int&& fd, const internal::Sys* sys) noexcept :
    detail::DupableFdHandle(std::move(fd), sys), sys(sys)
{
}

} // namespace fd
} // namespace stdplus
