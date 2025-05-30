#pragma once
#include <fmt/core.h>

#include <cstddef>
#include <format>
#include <limits>
#include <string>
#include <type_traits>
#ifndef NDEBUG
#include <stdexcept>
#endif

namespace stdplus
{
namespace detail
{

template <typename CharT>
constexpr std::ptrdiff_t zstring_find_term(const CharT* str, std::size_t min,
                                           std::size_t max) noexcept
{
    for (size_t i = 0; i < min; ++i)
    {
        if (str[i] == '\0')
        {
            return -1;
        }
    }
    for (size_t i = min; i < max; ++i)
    {
        if (str[i] == '\0')
        {
            return i;
        }
    }
    return -1;
}

#ifndef NDEBUG
template <typename CharT>
constexpr std::size_t zstring_validate(const CharT* str, std::size_t min,
                                       std::size_t max)
{
    auto ret = zstring_find_term(str, min, max);
    if (ret < 0)
    {
        throw std::invalid_argument("stdplus::zstring");
    }
    return ret;
}
#endif

template <typename T, typename CharT, typename Traits>
struct same_string : std::false_type
{};

template <typename CharT, typename Traits, typename Allocator>
struct same_string<std::basic_string<CharT, Traits, Allocator>, CharT, Traits> :
    std::true_type
{};

} // namespace detail

/**
 * @brief Provides a class for expressing an unbounded length string that has
 * a nul terminating character. This is different from zstring_view in that it
 * never computes the length, which is slightly cheaper if the string will just
 * be downcast to a `const char *`.
 */
template <typename CharT,
          typename Traits = std::char_traits<std::remove_const_t<CharT>>>
class basic_zstring
{
  private:
    using decay_t = std::remove_const_t<CharT>;

  public:
    using traits_type = Traits;
    using value_type = CharT;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;
    using size_type = std::size_t;

    template <typename T, size_type N>
    constexpr basic_zstring(T (&str)[N])
#ifdef NDEBUG
        noexcept
#endif
        : data_(str)
    {
#ifndef NDEBUG
        detail::zstring_validate(str, 0, N);
#endif
    }
    template <typename T, std::enable_if_t<std::is_pointer_v<T>, bool> = true>
    constexpr basic_zstring(T str) noexcept : data_(str)
    {}
    template <typename T,
              std::enable_if_t<detail::same_string<std::remove_cvref_t<T>,
                                                   decay_t, Traits>::value,
                               bool> = true>
    constexpr basic_zstring(T&& str)
#ifdef NDEBUG
        noexcept
#endif
        : data_(str.data())
    {
#ifndef NDEBUG
        detail::zstring_validate(str.data(), str.size(), str.size() + 1);
#endif
    }

    constexpr operator basic_zstring<const CharT, Traits>() const noexcept
    {
        return basic_zstring<const CharT, Traits>(data_);
    }

    constexpr reference operator[](size_type pos) const noexcept
    {
        return data_[pos];
    }
    constexpr reference front() const noexcept
    {
        return data_[0];
    }
    constexpr pointer data() const noexcept
    {
        return data_;
    }
    constexpr const_pointer c_str() const noexcept
    {
        return data_;
    }
    [[nodiscard]] constexpr bool empty() const noexcept
    {
        return data_ == nullptr || data_[0] == '\0';
    }

    constexpr basic_zstring suffix(size_type size) const noexcept
    {
        return data_ + size;
    }
    constexpr basic_zstring<const CharT, Traits> csuffix(
        size_type size) const noexcept
    {
        return data_ + size;
    }

    constexpr int compare(const_pointer other,
                          size_type other_size) const noexcept
    {
        for (size_t i = 0; i < other_size; ++i)
        {
            if (data_[i] == '\0')
            {
                return -1;
            }
            if (!Traits::eq(other[i], data_[i]))
            {
                return Traits::lt(other[i], data_[i]) ? 1 : -1;
            }
        }
        if (data_[other_size] == '\0')
        {
            return 0;
        }
        return 1;
    }
    constexpr int compare(const_pointer other) const noexcept
    {
        auto data = data_;
        while (true)
        {
            if (data[0] == '\0')
            {
                if (other[0] == '\0')
                {
                    return 0;
                }
                return -1;
            }
            if (other[0] == '\0')
            {
                return 1;
            }
            if (!Traits::eq(other[0], data[0]))
            {
                return Traits::lt(other[0], data[0]) ? 1 : -1;
            }
            data++;
            other++;
        }
    }

    template <typename CharT1>
    constexpr Traits::comparison_category operator<=>(
        basic_zstring<CharT1, Traits> rhs) const noexcept
    {
        return compare(rhs.data()) <=> 0;
    }
    template <typename T, std::enable_if_t<std::is_pointer_v<T>, bool> = true>
    constexpr Traits::comparison_category operator<=>(T rhs) const noexcept
    {
        return compare(rhs) <=> 0;
    }
    template <typename Allocator>
    constexpr Traits::comparison_category operator<=>(
        const std::basic_string<decay_t, Traits, Allocator>& rhs) const noexcept
    {
        return compare(rhs.data(), rhs.size()) <=> 0;
    }
    constexpr Traits::comparison_category operator<=>(
        std::basic_string_view<decay_t, Traits> rhs) const noexcept
    {
        return compare(rhs.data(), rhs.size()) <=> 0;
    }

    constexpr bool operator==(const auto& rhs) const noexcept
    {
        return (*this <=> rhs) == 0;
    }

  private:
    pointer data_;
};

template <typename CharT, typename Traits>
std::basic_ostream<std::remove_const_t<CharT>, Traits>& operator<<(
    std::basic_ostream<std::remove_const_t<CharT>, Traits>& os,
    basic_zstring<CharT, Traits> v)
{
    return os << v.c_str();
}

#define zstring_all(char_t, pfx)                                               \
    using pfx##zstring = basic_zstring<char_t>;                                \
    using const_##pfx##zstring = basic_zstring<const char_t>;
zstring_all(char, );
zstring_all(char8_t, u8);
zstring_all(char16_t, u16);
zstring_all(char32_t, u32);
zstring_all(wchar_t, w);
#undef zstring_all

} // namespace stdplus

namespace fmt
{
template <typename CharT, typename Traits>
struct formatter<stdplus::basic_zstring<CharT, Traits>,
                 std::remove_const_t<CharT>> :
    formatter<const CharT*, std::remove_const_t<CharT>>
{
    template <typename FormatContext>
    inline auto format(stdplus::basic_zstring<CharT, Traits> str,
                       FormatContext& ctx) const
    {
        return formatter<const CharT*, std::remove_const_t<CharT>>::format(
            str.c_str(), ctx);
    }
};
} // namespace fmt

namespace std
{
template <typename CharT, typename Traits>
struct formatter<stdplus::basic_zstring<CharT, Traits>,
                 std::remove_const_t<CharT>> :
    formatter<const CharT*, std::remove_const_t<CharT>>
{
    template <typename FormatContext>
    inline auto format(stdplus::basic_zstring<CharT, Traits> str,
                       FormatContext& ctx) const
    {
        return formatter<const CharT*, std::remove_const_t<CharT>>::format(
            str.c_str(), ctx);
    }
};
} // namespace std
