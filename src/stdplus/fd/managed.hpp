#pragma once
#include <stdplus/fd/base.hpp>
#include <stdplus/handle/managed.hpp>
#include <stdplus/internal/sys.hpp>
#include <tuple>

namespace stdplus
{
namespace fd
{

namespace detail
{

/** @brief Closes the file descriptor when dropping an Fd handle
 *
 *  @param[in] fd - File descriptor to close
 */
void drop(int&& fd, const internal::Sys*& sys);

using ManagedFdHandle = Managed<int, const internal::Sys*>::Handle<drop>;

} // namespace detail

class DupableFd;

/** @class ManagedFd
 *  @brief Holds references to unique, non-dupable file descriptors
 *  @details Provides RAII semantics for file descriptors
 */
class ManagedFd : public Fd
{
  public:
    /** @brief Holds the input file descriptor
     *         Becomes the sole owner of the file descriptor
     *
     *  @param[in] fd  - File descriptor to hold
     */
    explicit ManagedFd(int&& fd,
                       const internal::Sys* sys = &internal::sys_impl) noexcept;

    ManagedFd(DupableFd&& other) noexcept;
    ManagedFd(const DupableFd& other);
    ManagedFd& operator=(DupableFd&& other) noexcept;
    ManagedFd& operator=(const DupableFd& other);

    inline int value() const override
    {
        return handle.value();
    }

    inline const internal::Sys* sys() const noexcept override
    {
        return std::get<0>(handle.data());
    }

  private:
    detail::ManagedFdHandle handle;
};

} // namespace fd
} // namespace stdplus
