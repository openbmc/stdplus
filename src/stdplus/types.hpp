#pragma once
#if __has_include(<nonstd/span.hpp>)
#include <nonstd/span.hpp>
#define HAVE_SPAN nonstd
#endif

namespace stdplus
{

#ifdef HAVE_SPAN
template <typename... Args>
using span = HAVE_SPAN::span<Args...>;
#endif

} // namespace stdplus
