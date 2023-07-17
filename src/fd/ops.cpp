#include <stdplus/exception.hpp>
#include <stdplus/fd/ops.hpp>

#include <format>
#include <utility>

namespace stdplus
{
namespace fd
{
namespace detail
{

template <typename Fun, typename Byte, typename... Args>
static void opExact(const char* name, Fun&& fun, Fd& fd, std::span<Byte> data,
                    Args&&... args)
{
    while (data.size() > 0)
    {
        auto ret = (fd.*fun)(data, std::forward<Args>(args)...);
        if (ret.size() == 0)
        {
            throw exception::WouldBlock(
                std::format("{} missing {}B", name, data.size()));
        }
        data = data.subspan(ret.size());
    }
}

void readExact(Fd& fd, std::span<std::byte> data)
{
    opExact("readExact", &Fd::read, fd, data);
}

void recvExact(Fd& fd, std::span<std::byte> data, RecvFlags flags)
{
    opExact("recvExact", &Fd::recv, fd, data, flags);
}

void writeExact(Fd& fd, std::span<const std::byte> data)
{
    opExact("writeExact", &Fd::write, fd, data);
}

void sendExact(Fd& fd, std::span<const std::byte> data, SendFlags flags)
{
    opExact("sendExact", &Fd::send, fd, data, flags);
}

template <typename Fun, typename Byte, typename... Args>
static std::span<Byte> opAligned(const char* name, Fun&& fun, Fd& fd,
                                 size_t align, std::span<Byte> data,
                                 Args&&... args)
{
    std::span<Byte> ret;
    do
    {
        auto r = (fd.*fun)(data.subspan(ret.size()),
                           std::forward<Args>(args)...);
        if (ret.size() != 0 && r.size() == 0)
        {
            throw exception::WouldBlock(
                std::format("{} is {}B/{}B", name, ret.size() % align, align));
        }
        ret = data.subspan(0, ret.size() + r.size());
    } while (ret.size() % align != 0);
    return ret;
}

std::span<std::byte> readAligned(Fd& fd, size_t align, std::span<std::byte> buf)
{
    return opAligned("readAligned", &Fd::read, fd, align, buf);
}

std::span<std::byte> recvAligned(Fd& fd, size_t align, std::span<std::byte> buf,
                                 RecvFlags flags)
{
    return opAligned("recvAligned", &Fd::recv, fd, align, buf, flags);
}

std::span<const std::byte> writeAligned(Fd& fd, size_t align,
                                        std::span<const std::byte> data)
{
    return opAligned("writeAligned", &Fd::write, fd, align, data);
}

std::span<const std::byte> sendAligned(Fd& fd, size_t align,
                                       std::span<const std::byte> data,
                                       SendFlags flags)
{
    return opAligned("sendAligned", &Fd::send, fd, align, data, flags);
}

} // namespace detail
} // namespace fd
} // namespace stdplus
