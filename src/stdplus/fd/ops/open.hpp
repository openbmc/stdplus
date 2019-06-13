#pragma once
#include <stdplus/fd/dupable.hpp>
#include <stdplus/internal/sys.hpp>

namespace stdplus
{
namespace fd
{
namespace ops
{

DupableFd open(const char* pathname, int flags,
               const internal::Sys* sys = &internal::sys_impl);

} // namespace ops
} // namespace fd
} // namespace stdplus
