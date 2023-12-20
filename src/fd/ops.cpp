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
    std::size_t total = 0;
    try
    {
        while (total < data.size())
        {
            auto r = (fd.*fun)(data.subspan(total),
                               std::forward<Args>(args)...);
            if (r.size() == 0)
            {
                throw exception::WouldBlock(
                    std::format("{} missing", name, data.size()));
            }
            total += r.size();
        }
    }
    catch (const std::system_error&)
    {
        if (total != 0)
        {
            throw exception::Incomplete(
                std::format("{} is {}B/{}B", name, total, data.size()));
        }
        throw;
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

void sendtoExact(Fd& fd, std::span<const std::byte> data, SendFlags flags,
                 std::span<const std::byte> addr)
{
    auto r = fd.sendto(data, flags, addr);
    if (r.size() == 0)
    {
        throw exception::WouldBlock("sendto");
    }
    if (r.size() < data.size())
    {
        throw exception::Incomplete("sendto");
    }
}

template <typename Fun, typename Byte, typename... Args>
static std::span<Byte> opAligned(const char* name, Fun&& fun, Fd& fd,
                                 size_t align, std::span<Byte> data,
                                 Args&&... args)
{
    std::size_t total = 0;
    try
    {
        do
        {
            auto r = (fd.*fun)(data.subspan(total),
                               std::forward<Args>(args)...);
            if (total != 0 && r.size() == 0)
            {
                throw exception::Incomplete(
                    std::format("{} is {}B/{}B", name, total % align, align));
            }
            total += r.size();
        } while (total % align != 0);
    }
    catch (const std::system_error&)
    {
        if (total % align)
        {
            throw exception::Incomplete(
                std::format("{} is {}B/{}B", name, total % align, align));
        }
        throw;
    }
    return std::span<Byte>(data.data(), total);
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
