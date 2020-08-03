#include <fmt/format.h>
#include <stdplus/exception.hpp>
#include <stdplus/fd/ops.hpp>
#include <utility>

namespace stdplus
{
namespace fd
{
namespace detail
{

template <typename Fun, typename Byte, typename... Args>
static void opExact(const char* name, Fun&& fun, Fd& fd, span<Byte> data,
                    Args&&... args)
{
    while (data.size() > 0)
    {
        auto ret = (fd.*fun)(data, std::forward<Args>(args)...);
        if (ret.size() == 0)
        {
            throw exception::WouldBlock(
                fmt::format("{} missing {}B", name, data.size()));
        }
        data = data.subspan(ret.size());
    }
}

void readExact(Fd& fd, span<std::byte> data)
{
    opExact("readExact", &Fd::read, fd, data);
}

void recvExact(Fd& fd, span<std::byte> data, int flags)
{
    opExact("recvExact", &Fd::recv, fd, data, flags);
}

void writeExact(Fd& fd, span<const std::byte> data)
{
    opExact("writeExact", &Fd::write, fd, data);
}

void sendExact(Fd& fd, span<const std::byte> data, int flags)
{
    opExact("sendExact", &Fd::send, fd, data, flags);
}

template <typename Fun, typename Byte, typename... Args>
static span<Byte> opAligned(const char* name, Fun&& fun, Fd& fd, size_t align,
                            span<Byte> data, Args&&... args)
{
    span<Byte> ret;
    do
    {
        auto r =
            (fd.*fun)(data.subspan(ret.size()), std::forward<Args>(args)...);
        if (ret.size() != 0 && r.size() == 0)
        {
            throw exception::WouldBlock(
                fmt::format("{} is {}B/{}B", name, ret.size() % align, align));
        }
        ret = data.subspan(0, ret.size() + r.size());
    } while (ret.size() % align != 0);
    return ret;
}

span<std::byte> readAligned(Fd& fd, size_t align, span<std::byte> buf)
{
    return opAligned("readAligned", &Fd::read, fd, align, buf);
}

span<std::byte> recvAligned(Fd& fd, size_t align, span<std::byte> buf,
                            int flags)
{
    return opAligned("recvAligned", &Fd::recv, fd, align, buf, flags);
}

span<const std::byte> writeAligned(Fd& fd, size_t align,
                                   span<const std::byte> data)
{
    return opAligned("writeAligned", &Fd::write, fd, align, data);
}

span<const std::byte> sendAligned(Fd& fd, size_t align,
                                  span<const std::byte> data, int flags)
{
    return opAligned("sendAligned", &Fd::send, fd, align, data, flags);
}

} // namespace detail
} // namespace fd
} // namespace stdplus
