#pragma once
#include <stdplus/fd/base.hpp>
#include <stdplus/handle/managed.hpp>
#include <stdplus/internal/sys.hpp>

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
 *  @brief Holds references to file descriptors
 *  @details Provides RAII semantics for file descriptors
 */
class ManagedFd : public detail::ManagedFdHandle, public Fd
{
  public:
    /** @brief Holds the input file descriptor
     *         Becomes the sole owner of the file descriptor
     *
     *  @param[in] fd  - File descriptor to hold
     */
    explicit ManagedFd(int&& fd,
                       const internal::Sys* sys = &internal::sys_impl) noexcept;

    explicit ManagedFd(DupableFd&& other) noexcept;
    explicit ManagedFd(const DupableFd& other);
    ManagedFd& operator=(DupableFd&& other) noexcept;
    ManagedFd& operator=(const DupableFd& other);

    inline int getVal() const override
    {
        return value();
    }

    inline const internal::Sys* getSys() const noexcept override
    {
        return sys;
    }

  private:
    const internal::Sys* sys;
};

} // namespace fd
} // namespace stdplus
