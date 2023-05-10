#include <stdplus/fd/fmt.hpp>
#include <stdplus/fd/ops.hpp>

namespace stdplus
{
namespace fd
{

FormatBuffer::FormatBuffer(Fd& fd, size_t max) : fd(fd), max(max) {}

FormatBuffer::~FormatBuffer() noexcept(false)
{
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
