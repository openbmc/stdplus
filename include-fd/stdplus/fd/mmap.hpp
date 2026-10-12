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
    MMap(const Fd& fd, size_t window_size, ProtFlags prot, MMapFlags flags,
         off_t offset);
    MMap(const Fd& fd, std::span<std::byte> window, ProtFlags prot,
         MMapFlags flags, off_t offset);

    std::span<std::byte> get() const;

  private:
    static void drop(std::span<std::byte>&&,
                     std::reference_wrapper<const Fd>&) noexcept;
    Managed<std::span<std::byte>,
            std::reference_wrapper<const Fd>>::Handle<drop>
        mapping;
};

} // namespace fd
} // namespace stdplus
