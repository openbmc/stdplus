#include <stdplus/numeric/str.hpp>

namespace stdplus::detail
{
template char* uintToStr<16>(char*, uintptr_t, uint8_t) noexcept;
template char* uintToStr<10>(char*, uintptr_t, uint8_t) noexcept;
template uintptr_t strToUInt<16>(std::string_view);
template uintptr_t strToUInt<10>(std::string_view);
} // namespace stdplus::detail
