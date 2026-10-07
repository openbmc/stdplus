#pragma once
#include <stdplus/fd/intf.hpp>

namespace stdplus
{
namespace fd
{

class FdImpl : public Fd
{
  public:
    virtual int get() const = 0;

    std::span<std::byte> read(std::span<std::byte> buf) const override;
    std::span<std::byte> recv(std::span<std::byte> buf,
                              RecvFlags flags) const override;
    std::tuple<std::span<std::byte>, std::span<std::byte>> recvfrom(
        std::span<std::byte> buf, RecvFlags flags,
        std::span<std::byte> sockaddr) const override;
    std::span<const std::byte> write(
        std::span<const std::byte> data) const override;
    std::span<const std::byte> send(std::span<const std::byte> data,
                                    SendFlags flags) const override;
    std::span<const std::byte> sendto(
        std::span<const std::byte> data, SendFlags flags,
        std::span<const std::byte> sockaddr) const override;
    size_t lseek(off_t offset, Whence whence) const override;
    void truncate(off_t size) const override;
    void bind(std::span<const std::byte> sockaddr) const override;
    void connect(std::span<const std::byte> sockaddr) const override;
    void listen(int backlog) const override;
    std::optional<std::tuple<int, std::span<std::byte>>> accept(
        std::span<std::byte> sockaddr) const override;
    void setsockopt(SockLevel level, SockOpt optname,
                    std::span<const std::byte> opt) const override;
    int ioctl(unsigned long id, void* data) const override;
    int constIoctl(unsigned long id, void* data) const override;
    void fcntlSetfd(FdFlags flags) const override;
    FdFlags fcntlGetfd() const override;
    void fcntlSetfl(FileFlags flags) const override;
    FileFlags fcntlGetfl() const override;

  protected:
    std::span<std::byte> mmap(std::byte* window, std::size_t size,
                              ProtFlags prot, MMapFlags flags,
                              off_t offset) const override;
    void munmap(std::span<std::byte> window) const override;
};

} // namespace fd

using fd::FdImpl;

} // namespace stdplus
