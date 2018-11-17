#pragma once
#include <stdplus/internal/sys.hpp>

namespace stdplus
{
namespace fd
{

/** @brief An interface to retrieve a stored file descriptor
 *         and underlying libc implementation.
 */
class Fd
{
  public:
    virtual ~Fd() = default;

    /** @brief Gets the integer represting the file descriptor
     *  @return The file descriptor number.
     */
    virtual int getValue() const = 0;

    /** @brief Gets the interface used for making calls on the descriptor
     *  @return The interface.
     */
    virtual const internal::Sys* getSys() const noexcept = 0;
};

} // namespace fd
} // namespace stdplus
