#include <stdplus/zstring.hpp>

namespace stdplus
{

#ifdef NDEBUG
#define zstring_debug_instance(char_t)
#else
#define zstring_debug_instance(char_t)                                         \
    template std::size_t detail::zstring_validate<char_t>(                     \
        const char_t* str, std::size_t min, std::size_t max)
#endif
#define zstring_instance(char_t)                                               \
    template std::ptrdiff_t detail::zstring_find_term<char_t>(                 \
        const char_t* str, std::size_t min, std::size_t max) noexcept;         \
    zstring_debug_instance(char_t);                                            \
    template class basic_zstring<char_t>;                                      \
    template class basic_zstring<const char_t>
zstring_instance(char);
zstring_instance(char8_t);
zstring_instance(char16_t);
zstring_instance(char32_t);
zstring_instance(wchar_t);
#undef zstring_instance
#undef zstring_debug_instance

} // namespace stdplus
