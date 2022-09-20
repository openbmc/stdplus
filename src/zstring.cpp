#include <stdplus/zstring.hpp>

namespace stdplus
{

#define zstring_instance(char_t)                                               \
    template class basic_zstring<char_t>;                                      \
    template class basic_zstring<const char_t>
zstring_instance(char);
zstring_instance(char8_t);
zstring_instance(char16_t);
zstring_instance(char32_t);
zstring_instance(wchar_t);
#undef zstring_instance

} // namespace stdplus
