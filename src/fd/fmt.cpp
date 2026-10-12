#include <stdplus/fd/fmt.hpp>
#include <stdplus/fd/ops.hpp>

#include <exception>

namespace stdplus
{
namespace fd
{

FormatBuffer::FormatBuffer(const Fd& fd, size_t max) : fd(fd), max(max) {}

FormatBuffer::~FormatBuffer() noexcept(false)
{
    if (std::uncaught_exceptions() > 0)
    {
        try
        {
            flush();
        }
        catch (...)
        {}
        return;
    }
    flush();
}

void FormatBuffer::flush()
{
    if (buf.size() > 0)
    {
        writeExact(fd, buf);
        buf.clear();
    }
}

void FormatBuffer::writeIfNeeded()
{
    if (buf.size() >= max)
    {
        flush();
    }
}

} // namespace fd
} // namespace stdplus
