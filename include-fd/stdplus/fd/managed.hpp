#pragma once
#include <stdplus/fd/impl.hpp>
#include <stdplus/handle/managed.hpp>

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
void drop(int&& fd);

using ManagedFdHandle = Managed<int>::Handle<drop>;

} // namespace detail

class DupableFd;

/** @class ManagedFd
 *  @brief Holds references to unique, non-dupable file descriptors
 *  @details Provides RAII semantics for file descriptors
 */
class ManagedFd : public FdImpl
{
  public:
    /** @brief Constructs an empty file descriptor */
    ManagedFd() noexcept;

    /** @brief Holds the input file descriptor
     *         Becomes the sole owner of the file descriptor
     *
     *  @param[in] fd  - File descriptor to hold
     */
    explicit ManagedFd(int&& fd);

    ManagedFd(DupableFd&& other) noexcept;
    ManagedFd(const DupableFd& other);
    ManagedFd& operator=(DupableFd&& other) noexcept;
    ManagedFd& operator=(const DupableFd& other);

    /** @brief Unmanages the file descriptor and returns the value with
     *         ownership to the caller.
     *
     *  @return The file descriptor number
     */
    [[nodiscard]] int release();

    int get() const override;
    explicit operator bool() const noexcept;

  private:
    detail::ManagedFdHandle handle;
};

} // namespace fd

using fd::ManagedFd;

} // namespace stdplus
