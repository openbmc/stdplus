#pragma once
#include <stdplus/fd/base.hpp>

namespace stdplus
{
namespace fd
{
namespace ops
{

/** @brief Sets the CLOEXEC flag on the file descriptor based on the input value
 *
 *  @param[in] fd    - The file descriptor to change the flag on
 *  @param[in] value - The new CLOEXEC value
 */
void setFdCloexec(const Fd& fd, bool value);

} // namespace ops
} // namespace fd
} // namespace stdplus
