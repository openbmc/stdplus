#pragma once
#include <stdplus/fd/dupable.hpp>
#include <stdplus/fd/sys.hpp>
#include <string>

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
 *  @param[in] flags    - The flags passed to the system
 *  @param[in] sys      - The syscall implementation
 *  @return A managed file descriptor handle
 */
DupableFd open(const char* pathname, int flags, const Sys* sys = &sys_impl);
DupableFd open(const std::string& pathname, int flags,
               const Sys* sys = &sys_impl);

/** @brief Opens a new file descriptor for the given socket parameters
 *         Read socket(2) for more details.
 *
 *  @param[in] domain   - The socket domain
 *  @param[in] type     - The socket type
 *  @param[in] protocol - The socket protocol
 *  @param[in] sys      - The syscall implementation
 *  @return A managed file descriptor handle
 */
DupableFd socket(int domain, int type, int protocol,
                 const Sys* sys = &sys_impl);

} // namespace ops
} // namespace fd
} // namespace stdplus
