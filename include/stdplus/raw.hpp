#pragma once
#include <fmt/format.h>

#include <algorithm>
#include <span>
#include <stdexcept>
#include <string_view>
#include <type_traits>

namespace stdplus
{
namespace raw
{

namespace detail
{

/** @brief Gets the datatype referenced in a container
 */
template <typename Container>
using dataType = std::remove_pointer_t<decltype(std::data(
    std::declval<std::add_lvalue_reference_t<Container>>()))>;

/** @brief Gets the sizetype referenced in a container
 */
template <typename Container>
using sizeType =
    decltype(std::size(std::declval<std::add_lvalue_reference_t<Container>>()));

/** @brief Determines if the container holds trivially copyable data
 */
template <typename Container>
inline constexpr bool trivialContainer =
    std::is_trivially_copyable_v<dataType<Container>>;

/** @brief Adds const to A if B is const
 */
template <typename A, typename B>
using copyConst =
    std::conditional_t<std::is_const_v<B>, std::add_const_t<A>, A>;

/** @brief Determines if a type is a container of data
 */
template <typename, typename = void>
inline constexpr bool hasData = false;
template <typename T>
inline constexpr bool hasData<T, std::void_t<dataType<T>, sizeType<T>>> = true;

} // namespace detail

/** @brief Compares two containers to see if their raw bytes are equal
 *
 *  @param[in] a - The first container
 *  @param[in] b - The second container
 *  @return True if they are the same, false otherwise
 */
template <typename A, typename B>
constexpr bool equal(const A& a, const B& b) noexcept
{
    static_assert(std::is_trivially_copyable_v<A>);
    static_assert(std::is_trivially_copyable_v<B>);
    static_assert(sizeof(A) == sizeof(B));
    const auto a_byte = reinterpret_cast<const std::byte*>(&a);
    const auto b_byte = reinterpret_cast<const std::byte*>(&b);
    return std::equal(a_byte, a_byte + sizeof(A), b_byte);
}

/** @brief Copies data from a buffer into a copyable type
 *
 *  @param[in] data - The data buffer being copied from
 *  @return The copyable type with data populated
 */
#define STDPLUS_COPY_FROM(func, comp)                                          \
    template <typename T, typename Container>                                  \
    constexpr T func(const Container& c)                                       \
    {                                                                          \
        static_assert(std::is_trivially_copyable_v<T>);                        \
        static_assert(detail::trivialContainer<Container>);                    \
        T ret;                                                                 \
        const size_t bytes = std::size(c) * sizeof(*std::data(c));             \
        if (bytes comp sizeof(ret))                                            \
        {                                                                      \
            throw std::runtime_error(                                          \
                fmt::format(#func ": {} < {}", bytes, sizeof(ret)));           \
        }                                                                      \
        const auto c_bytes = reinterpret_cast<const std::byte*>(std::data(c)); \
        const auto ret_bytes = reinterpret_cast<std::byte*>(&ret);             \
        std::copy(c_bytes, c_bytes + sizeof(ret), ret_bytes);                  \
        return ret;                                                            \
    }
STDPLUS_COPY_FROM(copyFrom, <)
STDPLUS_COPY_FROM(copyFromStrict, !=)
#undef STDPLUS_COPY_FROM

/** @brief If you can guarantee the underlying data is properly aligned
 *  for raw struct access this specifier is used to override compile checks. */
struct Aligned
{};
struct UnAligned
{};

/** @brief References the data from a buffer if aligned
 *
 *  @param[in] data - The data buffer being referenced
 *  @return The reference to the data in the new type
 */
#define STDPLUS_REF_FROM(func, comp)                                           \
    template <typename T, typename A = stdplus::raw::UnAligned,                \
              typename Container,                                              \
              typename Tp = detail::copyConst<T, detail::dataType<Container>>> \
    constexpr Tp& func(Container&& c)                                          \
    {                                                                          \
        static_assert(std::is_trivially_copyable_v<Tp>);                       \
        static_assert(detail::trivialContainer<Container>);                    \
        static_assert(std::is_same_v<A, Aligned> ||                            \
                      sizeof(*std::data(c)) % alignof(Tp) == 0);               \
        const size_t bytes = std::size(c) * sizeof(*std::data(c));             \
        if (bytes comp sizeof(Tp))                                             \
        {                                                                      \
            throw std::runtime_error(                                          \
                fmt::format(#func ": {} < {}", bytes, sizeof(Tp)));            \
        }                                                                      \
        return *reinterpret_cast<Tp*>(std::data(c));                           \
    }
STDPLUS_REF_FROM(refFrom, <)
STDPLUS_REF_FROM(refFromStrict, !=)
#undef STDPLUS_REF_FROM

/** @brief Extracts data from a buffer into a copyable type
 *         Updates the data buffer to show that data was removed
 *
 *  @param[in,out] data - The data buffer being extracted from
 *  @return The copyable type with data populated
 */
template <typename T, typename CharT>
constexpr T extract(std::basic_string_view<CharT>& data)
{
    T ret = copyFrom<T>(data);
    static_assert(sizeof(T) % sizeof(CharT) == 0);
    data.remove_prefix(sizeof(T) / sizeof(CharT));
    return ret;
}
template <typename T, typename IntT,
          typename = std::enable_if_t<std::is_trivially_copyable_v<IntT>>>
constexpr T extract(std::span<IntT>& data)
{
    T ret = copyFrom<T>(data);
    static_assert(sizeof(T) % sizeof(IntT) == 0);
    data = data.subspan(sizeof(T) / sizeof(IntT));
    return ret;
}

/** @brief Extracts data from a buffer as a reference if aligned
 *         Updates the data buffer to show that data was removed
 *
 *  @param[in,out] data - The data buffer being extracted from
 *  @return A reference to the data
 */
template <typename T, typename A = stdplus::raw::UnAligned, typename CharT>
constexpr const T& extractRef(std::basic_string_view<CharT>& data)
{
    const T& ret = refFrom<T, A>(data);
    static_assert(sizeof(T) % sizeof(CharT) == 0);
    data.remove_prefix(sizeof(T) / sizeof(CharT));
    return ret;
}
template <typename T, typename A = stdplus::raw::UnAligned, typename IntT,
          typename = std::enable_if_t<std::is_trivially_copyable_v<IntT>>,
          typename Tp = detail::copyConst<T, IntT>>
constexpr Tp& extractRef(std::span<IntT>& data)
{
    Tp& ret = refFrom<Tp, A>(data);
    static_assert(sizeof(Tp) % sizeof(IntT) == 0);
    data = data.subspan(sizeof(Tp) / sizeof(IntT));
    return ret; // NOLINT(clang-analyzer-cplusplus.InnerPointer)
}

/** @brief Returns the std::span referencing the data of the raw trivial type
 *         or of trivial types in a contiguous container.
 *
 *  @param[in] t - The trivial raw data
 *  @return A view over the input with the given output integral type
 */
template <typename CharT, typename T>
constexpr std::enable_if_t<!detail::hasData<T>, std::basic_string_view<CharT>>
    asView(const T& t) noexcept
{
    static_assert(std::is_trivially_copyable_v<T>);
    static_assert(sizeof(T) % sizeof(CharT) == 0);
    return {reinterpret_cast<const CharT*>(&t), sizeof(T) / sizeof(CharT)};
}

template <typename CharT, typename Container>
constexpr std::enable_if_t<detail::hasData<Container>,
                           std::basic_string_view<CharT>>
    asView(const Container& c) noexcept
{
    static_assert(detail::trivialContainer<Container>);
    static_assert(sizeof(*std::data(c)) % sizeof(CharT) == 0);
    return {reinterpret_cast<const CharT*>(std::data(c)),
            std::size(c) * sizeof(*std::data(c)) / sizeof(CharT)};
}

template <typename IntT, typename T,
          typename = std::enable_if_t<std::is_trivially_copyable_v<IntT>>,
          typename = std::enable_if_t<!detail::hasData<T>>,
          typename IntTp = detail::copyConst<IntT, T>>
constexpr std::span<IntTp> asSpan(T& t) noexcept
{
    static_assert(std::is_trivially_copyable_v<T>);
    static_assert(sizeof(T) % sizeof(IntTp) == 0);
    return {reinterpret_cast<IntTp*>(&t), sizeof(T) / sizeof(IntTp)};
}
template <typename IntT, typename Container,
          typename = std::enable_if_t<std::is_trivially_copyable_v<IntT>>,
          typename = std::enable_if_t<detail::hasData<Container>>,
          typename IntTp = detail::copyConst<IntT, detail::dataType<Container>>>
constexpr std::span<IntTp> asSpan(Container&& c) noexcept
{
    static_assert(detail::trivialContainer<Container>);
    static_assert(sizeof(*std::data(c)) % sizeof(IntTp) == 0);
    return {reinterpret_cast<IntTp*>(std::data(c)),
            std::size(c) * sizeof(*std::data(c)) / sizeof(IntTp)};
}

} // namespace raw
} // namespace stdplus
