#pragma once
#include <cstring>
#include <string>
#include <string_view>
#include <utility>

namespace stdplus
{
namespace util
{
namespace detail
{

template <typename... Views>
void strAppendViews(std::string& dst, Views... views)
{
    dst.reserve((dst.size() + ... + views.size()));
    (dst.append(views), ...);
}

} // namespace detail

/** @brief Converts the string into its underlying nul-terminated c-str
 *
 *  @param[in] str - The string reference
 *  @return The c-str
 */
template <typename Str, typename = std::enable_if_t<
                            std::is_same_v<std::remove_cv_t<Str>, std::string>>>
auto cStr(Str& str)
{
    return str.data();
}
template <
    typename Str,
    typename = std::enable_if_t<
        std::is_pointer_v<Str> &&
        std::is_same_v<std::remove_cv_t<std::remove_pointer_t<Str>>, char>>>
auto cStr(Str str)
{
    return str;
}

/** @brief Appends multiple strings to the end of the destination string
 *         in the most optimal way for the given inputs.
 *
 *  @param[in, out] dst - The string being appended to
 *  @param[in] ...strs  - An arbitrary number of strings to concatenate
 */
template <typename... Strs>
void strAppend(std::string& dst, const Strs&... strs)
{
    detail::strAppendViews(dst, std::string_view(strs)...);
}

/** @brief Concatenates multiple strings together in the most optimal
 *         way for the given inputs.
 *
 *  @param[in] ...strs - An arbitrary number of strings to concatenate
 *  @return The concatenated result string
 */
template <typename... Strs>
std::string strCat(const Strs&... strs)
{
    std::string ret;
    strAppend(ret, strs...);
    return ret;
}
template <typename... Strs>
std::string strCat(std::string&& in, const Strs&... strs)
{
    std::string ret = std::move(in);
    strAppend(ret, strs...);
    return ret;
}

} // namespace util
} // namespace stdplus
