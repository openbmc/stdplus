#pragma once
#if __has_include(<nonstd/span.hpp>)
#define span_CONFIG_SELECT_SPAN span_SPAN_NONSTD
#include <nonstd/span.hpp>
#define STDPLUS_SPAN_TYPE nonstd::span
#endif

namespace stdplus
{

#ifdef STDPLUS_SPAN_TYPE
template <typename... Args>
using span = STDPLUS_SPAN_TYPE<Args...>;
#endif

} // namespace stdplus
