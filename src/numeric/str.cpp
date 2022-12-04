#include <stdplus/numeric/str.hpp>

namespace stdplus::detail
{
template char* uintToStr<16>(char*, uintptr_t, uint8_t) noexcept;
template char* uintToStr<10>(char*, uintptr_t, uint8_t) noexcept;
} // namespace stdplus::detail
