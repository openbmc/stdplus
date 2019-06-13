#include <algorithm>
#include <stdexcept>
#include <stdplus/fd/exception.hpp>
#include <stdplus/fd/ops/read.hpp>
#include <string>
#include <system_error>

namespace stdplus
{
namespace fd
{
namespace ops
{
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

} // namespace ops
} // namespace fd
} // namespace stdplus
