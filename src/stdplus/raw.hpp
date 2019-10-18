#pragma once
#include <fmt/format.h>
#include <stdplus/types.hpp>
#include <string_view>
#include <type_traits>

namespace stdplus
{
namespace raw
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
    const size_t bytes = std::size(c) * sizeof(*std::data(c));
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
#ifdef STDPLUS_SPAN_TYPE
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

/** @brief Returns a span of the data
 */
template <typename CharT, typename T,
          typename = std::enable_if_t<std::is_trivially_copyable_v<T>>>
std::basic_string_view<CharT> asView(const T& t) noexcept
{
    static_assert(sizeof(T) % sizeof(CharT) == 0);
    return {reinterpret_cast<const CharT*>(&t), sizeof(T) / sizeof(CharT)};
}
template <typename CharT, typename Container,
          typename = std::enable_if_t<!std::is_trivially_copyable_v<Container>>,
          typename = decltype(std::data(std::declval<Container>()))>
std::basic_string_view<CharT> asView(const Container& c) noexcept
{
    static_assert(sizeof(*std::data(c)) % sizeof(CharT) == 0);
    return {reinterpret_cast<const CharT*>(std::data(c)),
            std::size(c) * sizeof(*std::data(c)) / sizeof(CharT)};
}
#ifdef STDPLUS_SPAN_TYPE
template <typename IntT, typename T,
          typename = std::enable_if_t<std::is_integral_v<IntT>>,
          typename = std::enable_if_t<std::is_trivially_copyable_v<T>>>
span<IntT> asSpan(T& t) noexcept
{
    static_assert(sizeof(T) % sizeof(IntT) == 0);
    return {reinterpret_cast<IntT*>(&t), sizeof(T) / sizeof(IntT)};
}
template <typename IntT, typename Container,
          typename = std::enable_if_t<std::is_integral_v<IntT>>,
          typename = std::enable_if_t<!std::is_trivially_copyable_v<Container>>,
          typename = decltype(std::data(std::declval<Container>()))>
span<IntT> asSpan(Container& c) noexcept
{
    static_assert(sizeof(*std::data(c)) % sizeof(IntT) == 0);
    return {reinterpret_cast<IntT*>(std::data(c)),
            std::size(c) * sizeof(*std::data(c)) / sizeof(IntT)};
}
template <typename IntT, typename T,
          typename = std::enable_if_t<std::is_integral_v<IntT>>,
          typename = std::enable_if_t<std::is_trivially_copyable_v<T>>>
span<const IntT> asSpan(const T& t) noexcept
{
    static_assert(sizeof(T) % sizeof(IntT) == 0);
    return {reinterpret_cast<const IntT*>(&t), sizeof(T) / sizeof(IntT)};
}
template <typename IntT, typename Container,
          typename = std::enable_if_t<std::is_integral_v<IntT>>,
          typename = std::enable_if_t<!std::is_trivially_copyable_v<Container>>,
          typename = decltype(std::data(std::declval<Container>()))>
span<const IntT> asSpan(const Container& c) noexcept
{
    static_assert(sizeof(*std::data(c)) % sizeof(IntT) == 0);
    return {reinterpret_cast<const IntT*>(std::data(c)),
            std::size(c) * sizeof(*std::data(c)) / sizeof(IntT)};
}
#endif

} // namespace raw
} // namespace stdplus
