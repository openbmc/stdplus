#pragma once
#include <stdplus/fd/base.hpp>
#include <stdplus/fd/managed.hpp>
#include <stdplus/handle/copyable.hpp>
#include <stdplus/internal/sys.hpp>

namespace stdplus
{
namespace fd
{

namespace detail
{

int ref(const int& fd, const internal::Sys*& sys);

using DupableFdHandle = Copyable<int, const internal::Sys*>::Handle<drop, ref>;

} // namespace detail

/** @class DupableFd
 *  @brief Holds references to file descriptors
 *  @details Provides RAII semantics for file descriptors
 */
class DupableFd : public detail::DupableFdHandle, public Fd
{
  public:
    /** @brief Duplicates and holds a file descriptor
     *         Does not automatically close the input descriptor
     *
     *  @param[in] fd  - File descriptor being duplicated
     *  @throws std::system_error for underlying syscall failures
     */
    explicit DupableFd(const int& fd,
                       const internal::Sys* sys = &internal::sys_impl);

    /** @brief Holds the input file descriptor
     *         Becomes the sole owner of the file descriptor
     *
     *  @param[in] fd  - File descriptor to hold
     */
    explicit DupableFd(int&& fd,
                       const internal::Sys* sys = &internal::sys_impl) noexcept;

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
