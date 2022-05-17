#include <stdplus/fd/mmap.hpp>
#include <stdplus/util/cexec.hpp>

namespace stdplus
{
namespace fd
{

MMap::MMap(Fd& fd, std::span<std::byte> window, ProtFlags prot, MMapFlags flags,
           off_t offset) :
    mapping(fd.mmap(window, prot, flags, offset), fd)
{
}

std::span<std::byte> MMap::get() const
{
    return *mapping;
}

void MMap::drop(std::span<std::byte>&& mapping, std::reference_wrapper<Fd>& fd)
{
    fd.get().munmap(mapping);
}

} // namespace fd
} // namespace stdplus
