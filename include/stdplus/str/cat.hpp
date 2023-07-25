#pragma once
#include <string>
#include <string_view>
#include <utility>

namespace stdplus
{

template <typename CharT, std::size_t ObjSize, typename Alloc>
class BasicStrBuf;

namespace detail
{

template <typename CharT, typename Traits, typename Alloc, typename... CharTs>
constexpr void strAppend(std::basic_string<CharT, Traits, Alloc>& dst,
                         std::basic_string_view<CharTs>... strs)
{
    dst.reserve((dst.size() + ... + strs.size()));
    (dst.append(strs), ...);
}
template <typename CharT, std::size_t ObjSize, typename Alloc,
          typename... CharTs>
constexpr void strAppend(stdplus::BasicStrBuf<CharT, ObjSize, Alloc>& dst,
                         std::basic_string_view<CharTs>... strs)
{
    [[maybe_unused]] auto out = dst.append((strs.size() + ... + 0));
    ((out = std::copy(strs.begin(), strs.end(), out)), ...);
}

} // namespace detail

inline namespace util
{

/** @brief Appends multiple strings to the end of the destination string
 *         in the most optimal way for the given inputs.
 *
 *  @param[in, out] dst - The string being appended to
 *  @param[in] ...strs  - An arbitrary number of strings to concatenate
 */
constexpr void strAppend(auto& dst, const auto&... strs)
{
    detail::strAppend(dst, std::basic_string_view(strs)...);
}

/** @brief Concatenates multiple strings together in the most optimal
 *         way for the given inputs.
 *
 *  @param[in] ...strs - An arbitrary number of strings to concatenate
 *  @return The concatenated result string
 */
template <typename CharT = char,
          typename Traits = std::basic_string<CharT>::traits_type,
          typename Alloc = std::basic_string<CharT>::allocator_type>
constexpr auto strCat(const auto&... strs)
{
    std::basic_string<CharT, Traits, Alloc> ret;
    strAppend(ret, strs...);
    return ret;
}
template <typename CharT, typename Traits, typename Alloc>
constexpr auto strCat(std::basic_string<CharT, Traits, Alloc>&& in,
                      const auto&... strs)
{
    auto ret = std::move(in);
    strAppend(ret, strs...);
    return ret;
}

} // namespace util
} // namespace stdplus
