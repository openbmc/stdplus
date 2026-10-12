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
    CHECK_ERRNO(sigemptyset(&set), "sigemptyset");
    CHECK_ERRNO(sigaddset(&set, signum), "sigaddset");
    CHECK_ERRNO(sigprocmask(SIG_BLOCK, &set, nullptr), "sigprocmask set");
}

} // namespace signal
} // namespace stdplus
