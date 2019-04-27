#pragma once
#include <chrono>
#include <future>

namespace stdplus
{
namespace util
{

template <typename... Args>
bool areAllReady(const Args&... args)
{
    using namespace std::chrono_literals;
    return (... && (args.wait_for(0s) == std::future_status::ready));
}

} // namespace util
} // namespace stdplus
