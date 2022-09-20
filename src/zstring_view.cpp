#include <stdplus/zstring_view.hpp>

namespace stdplus
{

template class basic_zstring_view<char>;
template class basic_zstring_view<char8_t>;
template class basic_zstring_view<char16_t>;
template class basic_zstring_view<char32_t>;
template class basic_zstring_view<wchar_t>;

} // namespace stdplus
