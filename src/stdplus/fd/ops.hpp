#pragma once
#include <iterator>
#include <optional>
#include <stdexcept>
#include <stdplus/fd/base.hpp>
#include <stdplus/fd/dupable.hpp>
#include <stdplus/internal/sys.hpp>
#include <string>
#include <type_traits>
#include <utility>

namespace stdplus
{
namespace fd
{
namespace ops
{
namespace detail
{

constexpr size_t read_block = 4096;

size_t read(const Fd& fd, void* buf, size_t count);

} // namespace detail

DupableFd open(const char* pathname, int flags,
               const internal::Sys* sys = &internal::sys_impl);

template <typename T, typename = std::enable_if_t<
                          std::is_trivially_copyable_v<typename T::value_type>>>
size_t readInto(const Fd& fd, T& t, size_t offset = 0)
{
    using V = typename T::value_type;

    size_t r = detail::read(fd, std::data(t) + offset,
                            (std::size(t) - offset) * sizeof(V));
    if (r % sizeof(V) != 0)
    {
        throw std::runtime_error("readInto: Misaligned read");
    }
    return r / sizeof(V);
}

template <typename T, typename = std::enable_if_t<
                          std::is_trivially_copyable_v<typename T::value_type>>>
size_t readAppend(const Fd& fd, T& t, size_t count)
{
    const size_t old_size = std::size(t);
    t.resize(old_size + count);
    size_t r = readInto(fd, t, old_size);
    t.resize(old_size + r);
    return r;
}

std::string readAll(const Fd& fd);
std::string readAll(const Fd& fd, size_t max);

template <typename T,
          typename = std::enable_if_t<std::is_trivially_copyable_v<T>>>
size_t readFull(const Fd& fd, T& t)
{
    size_t r = detail::read(fd, &t, sizeof(T));
    if (r == 0 || r == sizeof(T))
    {
        return r;
    }
    throw std::runtime_error("readFull: Not enough bytes");
}

template <typename T,
          typename = std::enable_if_t<std::is_trivially_copyable_v<T>>>
void readFull(const Fd& fd, std::optional<T>& maybeT)
{
    maybeT.emplace();
    size_t r = readFull(*maybeT);
    if (r == 0)
    {
        maybeT.reset();
    }
}

template <typename T,
          typename = std::enable_if_t<std::is_trivially_copyable_v<T>>>
std::optional<T> readFull(const Fd& fd)
{
    std::optional<T> ret;
    readFull(fd, ret);
    return ret;
}

int fcntl_getfl(const Fd& fd);
void fcntl_setfl(const Fd& fd, int flags);

} // namespace ops
} // namespace fd
} // namespace stdplus
