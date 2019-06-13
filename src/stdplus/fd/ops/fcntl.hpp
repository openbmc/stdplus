#pragma once
#include <stdplus/fd/base.hpp>

namespace stdplus
{
namespace fd
{
namespace ops
{

/** @brief Sets the NONBLOCK flag on the open file based on the input value
 *
 *  @param[in] fd    - The file descriptor to change the flag on
 *  @param[in] value - The new NONBLOCK value
 */
void setFileNonblock(const Fd& fd, bool value);

} // namespace ops
} // namespace fd
} // namespace stdplus
