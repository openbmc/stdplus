#pragma once

namespace stdplus
{
namespace fd
{

/** @class Sys
 *  @brief Overridable direct syscall interface
 */
class Sys
{
  public:
    virtual ~Sys() = default;

    virtual int close(int fd) const = 0;
    virtual int fcntl_dupfd_cloexec(int fd) const = 0;
    virtual int fcntl_setfd(int fd, int flags) const = 0;

    virtual int open(const char* pathname, int flags) const = 0;
    virtual int socket(int domain, int type, int protocol) const = 0;
};

/** @class SysImpl
 *  @brief syscall concrete implementation
 *  @details Passes through all calls to the normal linux syscalls
 */
class SysImpl : public virtual Sys
{
  public:
    int close(int fd) const override;
    int fcntl_dupfd_cloexec(int fd) const override;
    int fcntl_setfd(int fd, int flags) const override;

    int open(const char* pathname, int flags) const override;
    int socket(int domain, int type, int protocol) const override;
};

/** @brief Default instantiation of sys */
extern SysImpl sys_impl;

} // namespace fd
} // namespace stdplus
