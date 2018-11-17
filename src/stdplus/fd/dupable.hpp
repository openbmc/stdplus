#pragma once
#include <stdplus/fd/base.hpp>
#include <stdplus/fd/managed.hpp>
#include <stdplus/fd/sys.hpp>
#include <stdplus/handle/copyable.hpp>
#include <tuple>

namespace stdplus
{
namespace fd
{

namespace detail
{

int ref(const int& fd, const Sys*& sys);

using DupableFdHandle = Copyable<int, const Sys*>::Handle<drop, ref>;

} // namespace detail

/** @class DupableFd
 *  @brief Holds references to file descriptors which can be dup'd
 *  @details Provides RAII semantics for file descriptors
 */
class DupableFd : public Fd
{
  public:
    /** @brief Duplicates and holds a file descriptor
     *         Does not automatically close the input descriptor
     *
     *  @param[in] fd  - File descriptor being duplicated
     *  @throws std::system_error for underlying syscall failures
     */
    explicit DupableFd(const int& fd, const Sys* sys = &sys_impl);

    /** @brief Holds the input file descriptor
     *         Becomes the sole owner of the file descriptor
     *
     *  @param[in] fd  - File descriptor to hold
     */
    explicit DupableFd(int&& fd, const Sys* sys = &sys_impl);

    /** @brief Unmanages the file descriptor and returns the value with
     *         ownership to the caller.
     *
     *  @return The file descriptor number
     */
    inline int release()
    {
        return handle.release();
    }

    inline int getValue() const override
    {
        return handle.value();
    }

    inline const Sys* getSys() const noexcept override
    {
        return std::get<0>(handle.data());
    }

  private:
    friend class ManagedFd;
    detail::DupableFdHandle handle;
};

} // namespace fd
} // namespace stdplus
