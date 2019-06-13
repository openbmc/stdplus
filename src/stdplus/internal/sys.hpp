#pragma once

namespace stdplus
{
namespace internal
{

/** @class Sys
 *  @brief Overridable direct syscall interface
 */
class Sys
{
  public:
    virtual ~Sys() = default;

    virtual int dup(int oldfd) const = 0;
    virtual int close(int fd) const = 0;
    virtual int open(const char* pathname, int flags) const = 0;
};

/** @class SysImpl
 *  @brief syscall concrete implementation
 *  @details Passes through all calls to the normal linux syscalls
 */
class SysImpl : public Sys
{
  public:
    int dup(int oldfd) const override;
    int close(int fd) const override;
    int open(const char* pathname, int flags) const override;
};

/** @brief Default instantiation of sys */
extern SysImpl sys_impl;

} // namespace internal
} // namespace stdplus
