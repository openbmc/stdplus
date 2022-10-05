#include <array>
#include <execinfo.h>
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

void stackTrackHandler(int sig)
{
    std::array<void*, 20> array;
    size_t size = backtrace(array.data(), array.size());
    fprintf(stderr, "Error: signal %d:\n", sig);
    backtrace_symbols_fd(array.data(), size, STDERR_FILENO);
    exit(1);
}

void stackTrace()
{
    ::signal(SIGSEGV, stackTrackHandler);
}

} // namespace signal
} // namespace stdplus
