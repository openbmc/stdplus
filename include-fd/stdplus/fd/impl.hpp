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

    std::span<std::byte> read(std::span<std::byte> buf) override;
    std::span<std::byte> recv(std::span<std::byte> buf,
                              RecvFlags flags) override;
    std::tuple<std::span<std::byte>, std::span<std::byte>>
        recvfrom(std::span<std::byte> buf, RecvFlags flags,
                 std::span<std::byte> sockaddr) override;
    std::span<const std::byte> write(std::span<const std::byte> data) override;
    std::span<const std::byte> send(std::span<const std::byte> data,
                                    SendFlags flags) override;
    std::span<const std::byte>
        sendto(std::span<const std::byte> data, SendFlags flags,
               std::span<const std::byte> sockaddr) override;
    size_t lseek(off_t offset, Whence whence) override;
    void truncate(off_t size) override;
    void bind(std::span<const std::byte> sockaddr) override;
    void connect(std::span<const std::byte> sockaddr) override;
    void listen(int backlog) override;
    std::optional<std::tuple<int, std::span<std::byte>>>
        accept(std::span<std::byte> sockaddr) override;
    void setsockopt(SockLevel level, SockOpt optname,
                    std::span<const std::byte> opt) override;
    int ioctl(unsigned long id, void* data) override;
    int constIoctl(unsigned long id, void* data) const override;
    void fcntlSetfd(FdFlags flags) override;
    FdFlags fcntlGetfd() const override;
    void fcntlSetfl(FileFlags flags) override;
    FileFlags fcntlGetfl() const override;

  protected:
    std::span<std::byte> mmap(std::span<std::byte> window, ProtFlags prot,
                              MMapFlags flags, off_t offset) override;
    void munmap(std::span<std::byte> window) override;
};

} // namespace fd

using fd::FdImpl;

} // namespace stdplus
