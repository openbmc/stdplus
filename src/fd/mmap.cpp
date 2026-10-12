#include <stdplus/exception.hpp>
#include <stdplus/fd/mmap.hpp>
#include <stdplus/util/cexec.hpp>

namespace stdplus
{
namespace fd
{

MMap::MMap(const Fd& fd, size_t window_size, ProtFlags prot, MMapFlags flags,
           off_t offset) :
    mapping(fd.mmap(nullptr, window_size, prot, flags, offset), fd)
{}
MMap::MMap(const Fd& fd, std::span<std::byte> window, ProtFlags prot,
           MMapFlags flags, off_t offset) :
    mapping(fd.mmap(window.data(), window.size(), prot, flags, offset), fd)
{}

std::span<std::byte> MMap::get() const
{
    return *mapping;
}

void MMap::drop(std::span<std::byte>&& mapping,
                std::reference_wrapper<const Fd>& fd) noexcept
{
    stdplus::exception::ignore([&]() { fd.get().munmap(mapping); })();
}

} // namespace fd
} // namespace stdplus
