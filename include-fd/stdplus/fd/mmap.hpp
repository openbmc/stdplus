#pragma once
#include <stdplus/fd/intf.hpp>
#include <stdplus/handle/managed.hpp>

#include <cstddef>
#include <functional>
#include <span>

namespace stdplus
{
namespace fd
{

class MMap
{
  public:
    inline MMap(Fd& fd, size_t window_size, ProtFlags prot, MMapFlags flags,
                off_t offset) :
        MMap(fd,
             std::span<std::byte>{static_cast<std::byte*>(nullptr),
                                  window_size},
             prot, flags, offset)
    {}
    MMap(Fd& fd, std::span<std::byte> window, ProtFlags prot, MMapFlags flags,
         off_t offset);

    std::span<std::byte> get() const;

  private:
    static void drop(std::span<std::byte>&&, std::reference_wrapper<Fd>&);
    Managed<std::span<std::byte>, std::reference_wrapper<Fd>>::Handle<drop>
        mapping;
};

} // namespace fd
} // namespace stdplus
