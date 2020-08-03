#include <stdplus/exception.hpp>
#include <stdplus/fd/convenience.hpp>
#include <utility>

namespace stdplus
{
namespace fd
{

template <typename Fun, typename Byte, typename... Args>
static void opAll(const char* name, Fun&& fun, Fd& fd, stdplus::span<Byte> data,
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

void readAll(Fd& fd, stdplus::span<std::byte> data)
{
    opAll("readAll", &Fd::read, fd, data);
}

void recvAll(Fd& fd, stdplus::span<std::byte> data, int flags)
{
    opAll("recvAll", &Fd::recv, fd, data, flags);
}

void writeAll(Fd& fd, stdplus::span<const std::byte> data)
{
    opAll("writeAll", &Fd::write, fd, data);
}

void sendAll(Fd& fd, stdplus::span<const std::byte> data, int flags)
{
    opAll("sendAll", &Fd::send, fd, data, flags);
}

template <typename Fun, typename Byte, typename... Args>
static stdplus::span<Byte> opAligned(const char* name, Fun&& fun, Fd& fd,
                                     size_t align, stdplus::span<Byte> data,
                                     Args&&... args)
{
    stdplus::span<Byte> ret;
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

stdplus::span<std::byte> readAligned(Fd& fd, size_t align,
                                     stdplus::span<std::byte> buf)
{
    return opAligned("readAligned", &Fd::read, fd, align, buf);
}

stdplus::span<std::byte> recvAligned(Fd& fd, size_t align,
                                     stdplus::span<std::byte> buf, int flags)
{
    return opAligned("recvAligned", &Fd::recv, fd, align, buf, flags);
}

stdplus::span<const std::byte> writeAligned(Fd& fd, size_t align,
                                            stdplus::span<const std::byte> data)
{
    return opAligned("writeAligned", &Fd::write, fd, align, data);
}

stdplus::span<const std::byte> sendAligned(Fd& fd, size_t align,
                                           stdplus::span<const std::byte> data,
                                           int flags)
{
    return opAligned("sendAligned", &Fd::send, fd, align, data, flags);
}

} // namespace fd
} // namespace stdplus
