#pragma once
#include <stdplus/fd/base.hpp>

namespace stdplus
{
namespace fd
{
namespace ops
{

int fcntl_getfl(const Fd& fd);
void fcntl_setfl(const Fd& fd, int flags);

} // namespace ops
} // namespace fd
} // namespace stdplus
