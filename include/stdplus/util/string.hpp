#pragma once
#include <string>
#include <string_view>
#include <utility>

namespace stdplus
{
namespace util
{
namespace detail
{

template <typename CharT, typename Traits, typename Alloc, typename... Views>
constexpr void strAppendViews(std::basic_string<CharT, Traits, Alloc>& dst,
                              Views... views)
{
    dst.reserve((dst.size() + ... + views.size()));
    (dst.append(views), ...);
}

template <typename CharT,
          typename Traits = std::basic_string_view<CharT>::traits_type>
struct DedSV : std::basic_string_view<CharT, Traits>
{
    template <typename... Args>
    constexpr DedSV(Args&&... args) :
        std::basic_string_view<CharT, Traits>(std::forward<Args>(args)...)
    {
    }
};

template <typename CharT>
DedSV(const CharT*) -> DedSV<CharT>;

template <typename CharT, typename Traits, typename Alloc>
DedSV(const std::basic_string<CharT, Traits, Alloc>&) -> DedSV<CharT, Traits>;

template <typename CharT, typename Traits>
DedSV(std::basic_string_view<CharT, Traits>) -> DedSV<CharT, Traits>;

} // namespace detail

/** @brief Appends multiple strings to the end of the destination string
 *         in the most optimal way for the given inputs.
 *
 *  @param[in, out] dst - The string being appended to
 *  @param[in] ...strs  - An arbitrary number of strings to concatenate
 */
template <typename CharT, typename Traits, typename Alloc, typename... Strs>
constexpr void strAppend(std::basic_string<CharT, Traits, Alloc>& dst,
                         const Strs&... strs)
{
    detail::strAppendViews(dst, detail::DedSV(strs)...);
}

/** @brief Concatenates multiple strings together in the most optimal
 *         way for the given inputs.
 *
 *  @param[in] ...strs - An arbitrary number of strings to concatenate
 *  @return The concatenated result string
 */
template <typename CharT = char,
          typename Traits = std::basic_string<CharT>::traits_type,
          typename Alloc = std::basic_string<CharT>::allocator_type,
          typename... Strs>
constexpr std::basic_string<CharT, Traits, Alloc> strCat(const Strs&... strs)
{
    std::basic_string<CharT, Traits, Alloc> ret;
    strAppend(ret, strs...);
    return ret;
}
template <typename CharT, typename Traits, typename Alloc, typename... Strs>
constexpr std::basic_string<CharT, Traits, Alloc>
    strCat(std::basic_string<CharT, Traits, Alloc>&& in, const Strs&... strs)
{
    std::basic_string<CharT, Traits, Alloc> ret = std::move(in);
    strAppend(ret, strs...);
    return ret;
}

} // namespace util
} // namespace stdplus
