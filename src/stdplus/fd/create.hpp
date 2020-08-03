#pragma once
#include <stdplus/fd/dupable.hpp>

namespace stdplus
{
namespace fd
{

DupableFd open(const char* pathname, int options, mode_t mode = 0);
DupableFd socket(int domain, int type, int protocol);

} // namespace fd
} // namespace stdplus
