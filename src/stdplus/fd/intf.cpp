#include <fmt/format.h>
#include <stdplus/exception.hpp>
#include <stdplus/fd/intf.hpp>

namespace stdplus
{

void Fd::readAll(stdplus::span<std::byte> data)
{
    while (data.size() > 0)
    {
        auto ret = read(data);
        if (ret.size() == 0)
        {
            throw exception::WouldBlock(
                fmt::format("readAll missing {}B", data.size()));
        }
        data = data.subspan(ret.size());
    }
}

void Fd::recvAll(stdplus::span<std::byte> data, int flags)
{
    while (data.size() > 0)
    {
        auto ret = recv(data, flags);
        if (ret.size() == 0)
        {
            throw exception::WouldBlock(
                fmt::format("recvAll missing {}B", data.size()));
        }
        data = data.subspan(ret.size());
    }
}

void Fd::writeAll(stdplus::span<const std::byte> data)
{
    while (data.size() > 0)
    {
        auto ret = write(data);
        if (ret.size() == 0)
        {
            throw exception::WouldBlock(
                fmt::format("writeAll left {}B", data.size()));
        }
        data = data.subspan(ret.size());
    }
}

void Fd::sendAll(stdplus::span<const std::byte> data, int flags)
{
    while (data.size() > 0)
    {
        auto ret = send(data, flags);
        if (ret.size() == 0)
        {
            throw exception::WouldBlock(
                fmt::format("sendAll left {}B", data.size()));
        }
        data = data.subspan(ret.size());
    }
}

} // namespace stdplus
