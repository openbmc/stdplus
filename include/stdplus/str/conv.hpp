#pragma once
#include <fmt/core.h>

#include <stdplus/str/buf.hpp>

#include <array>
#include <string_view>
#include <type_traits>

namespace stdplus
{

template <typename T>
struct ToStr;

template <typename T>
struct FromStr;

namespace detail
{

template <typename T>
concept ToStrStatic =
    !std::is_void_v<decltype(std::declval<T>().template operator()<char>(
        std::declval<typename T::type>()))>;

template <typename T>
concept ToStrFixed = !std::is_void_v<decltype(std::declval<T>()(
    std::declval<char*>(), std::declval<typename T::type>()))>;

} // namespace detail

template <typename T, typename CharT = char>
struct ToStrAdap
{};

template <typename T, typename CharT = char>
struct ToStrHandle
{
  private:
    stdplus::BasicStrBuf<CharT> buf;

  public:
    constexpr std::basic_string_view<CharT> operator()(const T::type& v)
    {
        buf.reset();
        T{}(buf, v);
        return buf;
    }
};

template <detail::ToStrStatic T, typename CharT>
struct ToStrHandle<T, CharT>
{
    static_assert(T::buf_size > 0);

    constexpr std::basic_string_view<CharT>
        operator()(const T::type& v) noexcept(
            noexcept(std::declval<T>().template operator()<CharT>(
                std::declval<typename T::type>())))
    {
        return T{}.template operator()<CharT>(v);
    }
};

template <detail::ToStrFixed T, typename CharT>
struct ToStrHandle<T, CharT>
{
  private:
    std::array<CharT, T::buf_size> buf;

  public:
    constexpr std::basic_string_view<CharT>
        operator()(const T::type& v) noexcept(noexcept(std::declval<T>()(
            std::declval<CharT*>(), std::declval<typename T::type>())))
    {
        return {buf.data(), T{}(buf.data(), v)};
    }
};

template <typename T, typename CharT>
struct Format
{
  private:
    fmt::formatter<std::basic_string_view<CharT>> formatter;

  public:
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(auto v, FormatContext& ctx) const
    {
        return formatter.format(ToStrHandle<T, CharT>{}(v), ctx);
    }
};

} // namespace stdplus
