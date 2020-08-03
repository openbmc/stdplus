#pragma once
#include <stdplus/fd/impl.hpp>
#include <stdplus/handle/managed.hpp>

namespace stdplus
{
namespace detail
{

/** @brief Closes the file descriptor when dropping an Fd handle
 *
 *  @param[in] fd - File descriptor to close
 */
void drop(int&& fd);

using ManagedFdHandle = Managed<int>::Handle<drop>;

/** @brief Sets or clears the CLOEXEC flag on the Fd
 *
 *  @param[in] fd  - File descriptor to set
 *  @param[in] val - New CLOEXEC value
 */
void setCloExec(int fd, bool val);

} // namespace detail

class DupableFd;

/** @class ManagedFd
 *  @brief Holds references to unique, non-dupable file descriptors
 *  @details Provides RAII semantics for file descriptors
 */
class ManagedFd : public FdImpl
{
  public:
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

    int get() override;

  private:
    detail::ManagedFdHandle handle;
};

} // namespace stdplus
