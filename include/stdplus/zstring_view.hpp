#pragma once
#include <stdexcept>
#include <stdplus/zstring.hpp>
#include <string>
#include <string_view>
#include <type_traits>

namespace stdplus
{

/**
 * @brief Provides a class for expressing a string_view that is guaranteed to
 * have nul termination. Most of the functions of this class are identical to
 * std::string_view with needed modifier functions removed to guarantee safety.
 */
template <typename CharT, typename Traits = std::char_traits<CharT>>
class basic_zstring_view;

namespace detail
{

template <typename CharT, typename Traits>
inline constexpr auto
    unsafe_zstring_view(std::basic_string_view<CharT, Traits> sv) noexcept
{
    using SV = basic_zstring_view<CharT, Traits>;
    return SV(typename SV::unsafe(), sv);
}

template <typename CharT, std::size_t N>
struct compile_zstring_view
{
    CharT data[N] = {};
    bool valid = true;

    constexpr compile_zstring_view(const CharT (&str)[N]) noexcept
    {
        for (size_t i = 0; i < N - 1; ++i)
        {
            if (str[i] == '\0')
            {
                valid = false;
            }
            data[i] = str[i];
        }
        if (str[N - 1] != '\0')
        {
            valid = false;
        }
    }

    constexpr auto getzsv() const noexcept
    {
        return unsafe_zstring_view(std::basic_string_view<CharT>(data, N - 1));
    }
};

} // namespace detail

template <typename CharT, typename Traits>
class basic_zstring_view
{
  private:
    using string_view_base = std::basic_string_view<CharT, Traits>;
    using zstring_base = basic_zstring<const CharT, Traits>;

  public:
    using traits_type = string_view_base::traits_type;
    using value_type = string_view_base::value_type;
    using pointer = string_view_base::pointer;
    using const_pointer = string_view_base::const_pointer;
    using reference = string_view_base::reference;
    using const_reference = string_view_base::const_reference;
    using iterator = string_view_base::iterator;
    using const_iterator = string_view_base::const_iterator;
    using reverse_iterator = string_view_base::reverse_iterator;
    using const_reverse_iterator = string_view_base::const_reverse_iterator;
    using size_type = string_view_base::size_type;
    using difference_type = string_view_base::difference_type;

    static constexpr size_type npos = string_view_base::npos;

    template <
        typename T, size_type N,
        std::enable_if_t<std::is_same_v<value_type, std::remove_cvref_t<T>>,
                         bool> = true>
    constexpr basic_zstring_view(T (&str)[N])
#ifdef NDEBUG
        noexcept
#endif
        :
        sv(str, N - 1)
    {
#ifndef NDEBUG
        for (size_type i = 0; i < N - 1; ++i)
        {
            if (str[i] == '\0')
            {
                throw std::invalid_argument("stdplus::zstring_view");
            }
        }
        if (str[N] != '\0')
        {
            throw std::invalid_argument("stdplus::zstring_view");
        }
#endif
    }
    template <typename T, std::enable_if_t<std::is_pointer_v<T>, bool> = true>
    inline constexpr basic_zstring_view(T str) noexcept : sv(str)
    {
    }
    template <typename T,
              std::enable_if_t<
                  std::is_same_v<std::basic_string<value_type, Traits,
                                                   typename T::allocator_type>,
                                 std::remove_cvref_t<T>>,
                  bool> = true>
    constexpr basic_zstring_view(T& str)
#ifdef NDEBUG
        noexcept
#endif
        :
        sv(str.data())
    {
#ifndef NDEBUG
        if (str.find('\0') != npos)
        {
            throw std::invalid_argument("stdplus::zstring_view");
        }
#endif
    }
    template <
        typename T,
        std::enable_if_t<std::is_same_v<value_type, std::remove_const_t<T>>,
                         bool> = true>
    inline constexpr basic_zstring_view(basic_zstring<T, Traits> str) noexcept :
        sv(str.data())
    {
    }

    inline constexpr operator string_view_base() const noexcept
    {
        return sv;
    }

    inline constexpr operator zstring_base() const noexcept
    {
        return zstring_base(data());
    }

    inline constexpr const_iterator begin() const noexcept
    {
        return sv.begin();
    }
    inline constexpr const_iterator end() const noexcept
    {
        return sv.end();
    }
    inline constexpr const_reverse_iterator rbegin() const noexcept
    {
        return sv.rbegin();
    }
    inline constexpr const_reverse_iterator rend() const noexcept
    {
        return sv.rend();
    }

    inline constexpr const_reference operator[](size_type pos) const noexcept
    {
        return sv[pos];
    }
    inline constexpr const_reference at(size_type pos) const
    {
        return sv.at(pos);
    }
    inline constexpr const_reference front() const noexcept
    {
        return sv.front();
    }
    inline constexpr const_reference back() const noexcept
    {
        return sv.back();
    }
    inline constexpr const_pointer data() const noexcept
    {
        return sv.data();
    }

    inline constexpr size_type size() const noexcept
    {
        return sv.size();
    }
    inline constexpr size_type length() const noexcept
    {
        return sv.length();
    }
    inline constexpr size_type max_size() noexcept
    {
        return sv.max_size();
    }
    [[nodiscard]] inline constexpr bool empty() const noexcept
    {
        return sv.empty();
    }

    inline constexpr void swap(basic_zstring_view& v) noexcept
    {
        sv.swap(v.sv);
    }

    inline constexpr size_type copy(pointer dest, size_type count,
                                    size_type pos = 0) const
    {
        return sv.copy(dest, count, pos);
    }
    inline constexpr string_view_base substr(size_type pos = 0,
                                             size_type count = npos) const
    {
        return sv.substr(pos, count);
    }

    inline constexpr int compare(string_view_base v) const noexcept
    {
        return sv.compare(v);
    }
    inline constexpr int compare(size_type pos1, size_type count1,
                                 string_view_base v) const
    {
        return sv.compare(pos1, count1, v);
    }
    inline constexpr int compare(size_type pos1, size_type count1,
                                 string_view_base v, size_type pos2,
                                 size_type count2) const
    {
        return sv.compare(pos1, count1, v, pos2, count2);
    }
    inline constexpr int compare(const_pointer s) const
    {
        return sv.compare(s);
    }
    inline constexpr int compare(size_type pos1, size_type count1,
                                 const_pointer s) const
    {
        return sv.compare(pos1, count1, s);
    }
    inline constexpr int compare(size_type pos1, size_type count1,
                                 const_pointer s, size_type count2) const
    {
        return sv.compare(pos1, count1, s, count2);
    }

#define zstring_view_has(func)                                                 \
    inline constexpr bool func(string_view_base sv) const noexcept             \
    {                                                                          \
        return sv.func(sv);                                                    \
    }                                                                          \
    inline constexpr bool func(value_type c) const noexcept                    \
    {                                                                          \
        return sv.func(c);                                                     \
    }                                                                          \
    inline constexpr bool func(const_pointer s) const                          \
    {                                                                          \
        return sv.func(s);                                                     \
    }
    zstring_view_has(starts_with);
    zstring_view_has(ends_with);
#undef zstring_view_has

#define zstring_view_find(func)                                                \
    inline constexpr size_type func(string_view_base v, size_type pos = 0)     \
        const noexcept                                                         \
    {                                                                          \
        return sv.func(v, pos);                                                \
    }                                                                          \
    inline constexpr size_type func(value_type ch, size_type pos = 0)          \
        const noexcept                                                         \
    {                                                                          \
        return sv.func(ch, pos);                                               \
    }                                                                          \
    inline constexpr size_type func(const_pointer s, size_type pos,            \
                                    size_type count) const                     \
    {                                                                          \
        return sv.func(s, pos, count);                                         \
    }                                                                          \
    inline constexpr size_type func(const_pointer s, size_type pos = 0) const  \
    {                                                                          \
        return sv.func(s, pos);                                                \
    }
    zstring_view_find(find);
    zstring_view_find(rfind);
    zstring_view_find(find_first_of);
    zstring_view_find(find_last_of);
    zstring_view_find(find_first_not_of);
    zstring_view_find(find_last_not_of);
#undef zstring_view_find

    inline constexpr const_pointer c_str() const noexcept
    {
        return sv.data();
    }
    constexpr basic_zstring_view suffix(size_type pos = 0) const
    {
        if (pos > sv.size())
        {
            throw std::out_of_range("stdplus::zstring_view");
        }
        return basic_zstring_view(unsafe(), sv.substr(pos));
    }

    template <typename CharTO, size_type N>
    inline constexpr bool operator==(const CharTO (&rhs)[N]) const noexcept
    {
        return *this == std::basic_string_view<CharTO, Traits>(rhs, N - 1);
    }
    constexpr bool
        operator==(std::basic_string_view<CharT, Traits> rhs) const noexcept
    {
        return size() == rhs.size() && compare(rhs) == 0;
    }
    template <typename CharTO, size_type N>
    inline constexpr Traits::comparison_category
        operator<=>(const CharTO (&rhs)[N]) const noexcept
    {
        return *this <=> std::basic_string_view<CharTO, Traits>(rhs, N - 1);
    }
    constexpr Traits::comparison_category
        operator<=>(std::basic_string_view<CharT, Traits> rhs) const noexcept
    {
        return compare(rhs) <=> 0;
    }

  private:
    string_view_base sv;

    struct unsafe
    {
    };
    inline constexpr basic_zstring_view(unsafe, string_view_base sv) noexcept :
        sv(sv)
    {
    }
    friend auto detail::unsafe_zstring_view<CharT, Traits>(string_view_base sv);
};

template <typename CharT, typename Traits>
std::basic_ostream<CharT, Traits>&
    operator<<(std::basic_ostream<CharT, Traits>& os,
               basic_zstring_view<CharT, Traits> v)
{
    return os << static_cast<std::basic_string_view<CharT, Traits>>(v);
}

namespace zstring_view_literals
{
template <detail::compile_zstring_view Str>
inline constexpr auto operator"" _zsv() noexcept
{
    static_assert(Str.valid, "stdplus::zstring_view");
    return Str.getzsv();
}
} // namespace zstring_view_literals

} // namespace stdplus

#define zstring_view_all(char_t, pfx)                                          \
    namespace stdplus                                                          \
    {                                                                          \
    using pfx##zstring_view = basic_zstring_view<char_t>;                      \
    }                                                                          \
    namespace std                                                              \
    {                                                                          \
    template <>                                                                \
    struct hash<stdplus::basic_zstring_view<char_t>>                           \
        : hash<basic_string_view<char_t>>                                      \
    {                                                                          \
    };                                                                         \
    }
zstring_view_all(char, );
zstring_view_all(char8_t, u8);
zstring_view_all(char16_t, u16);
zstring_view_all(char32_t, u32);
zstring_view_all(wchar_t, w);
#undef zstring_view_all
