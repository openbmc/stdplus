#include <signal.h>

#include <stdplus/signal.hpp>
#include <stdplus/util/cexec.hpp>

#include <system_error>

namespace stdplus
{
namespace signal
{

void block(int signum)
{
    sigset_t set;
    CHECK_ERRNO(sigprocmask(SIG_BLOCK, nullptr, &set), "sigprocmask get");
    CHECK_ERRNO(sigaddset(&set, signum), "sigaddset");
    CHECK_ERRNO(sigprocmask(SIG_BLOCK, &set, nullptr), "sigprocmask set");
}

} // namespace signal
} // namespace stdplus
