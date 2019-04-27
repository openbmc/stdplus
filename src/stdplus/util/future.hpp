#pragma once
#include <chrono>
#include <future>
#include <tuple>

namespace stdplus
{
namespace util
{

/** @brief Determines if a future is ready to be used without blocking
 *
 *  @param[in] future - The future we are checking
 *  @return True if ready to read
 */
template <typename T>
bool isReady(const std::future<T>& future)
{
    using namespace std::chrono_literals;
    return future.wait_for(0s) == std::future_status::ready;
}

/** @brief Determines if all futures in an iterable container are ready
 *         to be used without blocking
 *
 *  @param[in] futures - The futures we are checking
 *  @return True if all are ready to read
 */
template <template <typename...> typename C, typename... Cs,
          typename = typename C<Cs...>::const_iterator>
bool isReady(const C<Cs...>& futures)
{
    for (const auto& future : futures)
    {
        if (!isReady(future))
        {
            return false;
        }
    }
    return true;
}

/** @brief Determines if all futures are ready to be used without blocking
 *
 *  @param[in] futures... - The futures we are checking
 *  @return True if all are ready to read
 */
template <typename... Futures>
bool allReady(const Futures&... futures)
{
    return (... && isReady(futures));
}

/** @brief Determines if all futures in a tuple are ready to be used
 *         without blocking
 *
 *  @param[in] futures - The futures we are checking
 *  @return True if all are ready to read
 */
template <typename... Cs>
bool isReady(const std::tuple<Cs...>& futures)
{
    return std::apply(allReady<Cs...>, futures);
}

} // namespace util
} // namespace stdplus
