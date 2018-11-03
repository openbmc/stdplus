#include <signal.h>
#include <stdplus/signal.hpp>
#include <system_error>

namespace stdplus
{
namespace signal
{

void block(int signum)
{
    sigset_t set;
    if (sigprocmask(SIG_BLOCK, nullptr, &set) < 0)
    {
        throw std::system_error(errno, std::generic_category(),
                                "sigprocmask get");
    }

    if (sigaddset(&set, signum) < 0)
    {
        throw std::system_error(errno, std::generic_category(), "sigaddset");
    }

    if (sigprocmask(SIG_BLOCK, &set, nullptr) < 0)
    {
        throw std::system_error(errno, std::generic_category(),
                                "sigprocmask set");
    }
}

} // namespace signal
} // namespace stdplus
