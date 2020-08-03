#pragma once
#include <stdplus/fd/impl.hpp>
#include <stdplus/fd/managed.hpp>
#include <stdplus/handle/copyable.hpp>

namespace stdplus
{
namespace detail
{

int ref(const int& fd);

using DupableFdHandle = Copyable<int>::Handle<drop, ref>;

} // namespace detail

/** @class DupableFd
 *  @brief Holds references to file descriptors which can be dup'd
 *  @details Provides RAII semantics for file descriptors
 */
class DupableFd : public FdImpl
{
  public:
    /** @brief Duplicates and holds a file descriptor
     *         Does not automatically close the input descriptor
     *
     *  @param[in] fd  - File descriptor being duplicated
     *  @throws std::system_error for underlying syscall failures
     */
    explicit DupableFd(const int& fd);

    /** @brief Holds the input file descriptor
     *         Becomes the sole owner of the file descriptor
     *
     *  @param[in] fd  - File descriptor to hold
     */
    explicit DupableFd(int&& fd);

    /** @brief Unmanages the file descriptor and returns the value with
     *         ownership to the caller.
     *
     *  @return The file descriptor number
     */
    [[nodiscard]] int release();

    int get() override;

  private:
    friend class ManagedFd;
    detail::DupableFdHandle handle;
};

} // namespace stdplus
