#pragma once
#include <fmt/format.h>
#include <stdplus/types.hpp>
#include <string_view>
#include <type_traits>

namespace stdplus
{
namespace util
{

/** @brief Copies data from a buffer into a copyable type
 *
 *  @param[in] data - The data buffer being copied from
 *  @return The copyable type with data populated
 */
template <typename T, typename Container>
T copyFrom(const Container& c)
{
    static_assert(std::is_trivially_copyable_v<T>);
    T ret;
    const size_t bytes = std::size(c) * sizeof(typename Container::value_type);
    if (bytes < sizeof(ret))
    {
        throw std::runtime_error(
            fmt::format("CopyFrom: {} < {}", bytes, sizeof(ret)));
    }
    std::memcpy(&ret, std::data(c), sizeof(ret));
    return ret;
}

/** @brief Extracts data from a buffer into a copyable type
 *         Updates the data buffer to show that data was removed
 *
 *  @param[in,out] data - The data buffer being extracted from
 *  @return The copyable type with data populated
 */
template <typename T, typename CharT>
T extract(std::basic_string_view<CharT>& data)
{
    T ret = copyFrom<T>(data);
    static_assert(sizeof(T) % sizeof(CharT) == 0);
    data.remove_prefix(sizeof(T) / sizeof(CharT));
    return ret;
}
#ifdef HAVE_SPAN
template <typename T, typename IntT,
          typename = std::enable_if_t<std::is_integral_v<IntT>>>
T extract(span<const IntT>& data)
{
    T ret = copyFrom<T>(data);
    static_assert(sizeof(T) % sizeof(IntT) == 0);
    data = data.subspan(sizeof(T) / sizeof(IntT));
    return ret;
}
#endif

} // namespace util
} // namespace stdplus
