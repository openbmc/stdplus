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
            auto r =
                (fd.*fun)(data.subspan(total), std::forward<Args>(args)...);
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
static std::span<Byte>
    opAligned(const char* name, Fun&& fun, Fd& fd, size_t align,
              std::span<Byte> data, Args&&... args)
{
    std::size_t total = 0;
    try
    {
        do
        {
            auto r =
                (fd.*fun)(data.subspan(total), std::forward<Args>(args)...);
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

std::span<const std::byte> sendAligned(
    Fd& fd, size_t align, std::span<const std::byte> data, SendFlags flags)
{
    return opAligned("sendAligned", &Fd::send, fd, align, data, flags);
}

constexpr std::size_t maxStrideB = 65536;

void readAll(Fd& fd, function_view<std::span<std::byte>(size_t req)> resize)
{
    std::size_t strideB = 256;
    std::size_t totalB = 0;
    auto doRead = [&]() {
        auto buf = resize(totalB + strideB);
        auto r = fd.read(buf.subspan(totalB));
        if (r.size() == 0)
        {
            throw exception::WouldBlock("readAll");
        }
        totalB += r.size();
    };
    auto validateSize = [&]() {
        auto buf = resize(totalB);
        if (totalB != buf.size())
        {
            throw exception::Incomplete(
                std::format("readAll extra {}B", buf.size() - totalB));
        }
    };
    try
    {
        while (strideB < maxStrideB)
        {
            doRead();
            if (totalB >= strideB)
            {
                strideB = strideB << 1;
            }
        }
        while (true)
        {
            doRead();
        }
    }
    catch (const exception::Eof&)
    {
        validateSize();
        return;
    }
    catch (const std::system_error&)
    {
        validateSize();
        throw;
    }
}

std::span<std::byte> readAllFixed(Fd& fd, size_t align,
                                  std::span<std::byte> buf)
{
    std::size_t totalB = 0;
    auto validateSize = [&]() {
        if (totalB % align != 0)
        {
            throw exception::Incomplete(std::format(
                "readAllFixed partial {}B/{}B", totalB % align, align));
        }
    };
    try
    {
        while (totalB < buf.size())
        {
            auto r = fd.read(
                buf.subspan(totalB, std::min(maxStrideB, buf.size() - totalB)));
            if (r.size() == 0)
            {
                throw exception::WouldBlock("readAllFixed");
            }
            totalB += r.size();
        }
        std::byte b;
        auto r = fd.read(std::span(&b, 1));
        if (r.size() == 0)
        {
            throw exception::WouldBlock("readAllFixed");
        }
        throw std::system_error(
            std::make_error_code(std::errc::value_too_large),
            "readAllFixed overflow");
    }
    catch (const exception::Eof& e)
    {
        validateSize();
        return buf.subspan(0, totalB);
    }
    catch (const std::system_error& e)
    {
        validateSize();
        throw;
    }
}

void verifyExact(size_t expected, size_t actual)
{
    if (expected != actual)
    {
        throw exception::WouldBlock("verifyExact");
    }
}

} // namespace detail
} // namespace fd
} // namespace stdplus
