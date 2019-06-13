#pragma once
#include <stdplus/fd/dupable.hpp>
#include <stdplus/internal/sys.hpp>

namespace stdplus
{
namespace fd
{
namespace ops
{

/** @brief Opens a new file descriptor for the given path and
 *         flags. Read open(2) for more details.
 *
 *  @param[in] pathname - The path of the file to open
 *  @param[in] flags - The flags passed to the system
 *  @return A managed file descriptor handle
 */
DupableFd open(const char* pathname, int flags,
               const internal::Sys* sys = &internal::sys_impl);

} // namespace ops
} // namespace fd
} // namespace stdplus
