#pragma once
#include <chrono>
#include <future>

namespace stdplus
{
namespace util
{

template <typename T>
bool isReady(const std::future<T>& future)
{
    using namespace std::chrono_literals;
    return future.wait_for(0s) == std::future_status::ready;
}

template <template <typename> typename C, typename T>
bool isReady(const C<std::future<T>>& futures)
{
    for (const std::future<T>& future : futures)
    {
        if (!isReady(future))
        {
            return false;
        }
    }
    return true;
}

} // namespace util
} // namespace stdplus
