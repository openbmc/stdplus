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
    util::callCheckErrno("sigprocmask get", sigprocmask, SIG_BLOCK, nullptr,
                         &set);
    util::callCheckErrno("sigaddset", sigaddset, &set, signum);
    util::callCheckErrno("sigprocmask set", sigprocmask, SIG_BLOCK, &set,
                         nullptr);
}

} // namespace signal
} // namespace stdplus
