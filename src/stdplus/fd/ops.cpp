#include <algorithm>
#include <stdexcept>
#include <stdplus/fd/exception.hpp>
#include <stdplus/fd/ops.hpp>
#include <stdplus/util/cexec.hpp>
#include <string>
#include <system_error>

namespace stdplus
{
namespace fd
{
namespace ops
{

using internal::Sys;
using util::callCheckErrno;

namespace detail
{

size_t read(const Fd& fd, void* buf, size_t count)
{
    ssize_t ret = fd.getSys()->read(fd.getVal(), buf, count);
    if (ret == 0)
    {
        throw Eof("read");
    }
    if (ret > 0)
    {
        return ret;
    }
    if (errno == EWOULDBLOCK || errno == EAGAIN)
    {
        return 0;
    }
    throw std::system_error(errno, std::generic_category(), "read");
}

} // namespace detail

DupableFd open(const char* pathname, int flags, const Sys* sys)
{
    return DupableFd(callCheckErrno("open", &Sys::open, sys, pathname, flags),
                     sys);
}

std::string readAll(const Fd& fd)
{
    std::string ret;
    try
    {
        while (readAppend(fd, ret, detail::read_block) > 0)
        {
        }
    }
    catch (const Eof& e)
    {
        return ret;
    }
    throw std::runtime_error("readAll: Never got EOF");
}

std::string readAll(const Fd& fd, size_t max)
{
    std::string ret;
    try
    {
        size_t read_size;
        do
        {
            read_size = std::min(max - ret.size(), detail::read_block);
        } while (readAppend(fd, ret, read_size) > 0);
    }
    catch (const Eof& e)
    {
        return ret;
    }
    throw std::runtime_error("readAll: Never got EOF");
}

int fcntl_getfl(const Fd& fd)
{
    return callCheckErrno("fcntl getfl", &Sys::fcntl_getfl, fd.getSys(),
                          fd.getVal());
}

void fcntl_setfl(const Fd& fd, int flags)
{
    callCheckErrno("fcntl setfl", &Sys::fcntl_setfl, fd.getSys(), fd.getVal(),
                   flags);
}

} // namespace ops
} // namespace fd
} // namespace stdplus
