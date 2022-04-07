#pragma once
#include <span>
#define STDPLUS_SPAN_TYPE std::span

namespace stdplus
{

template <typename... Args>
using span = STDPLUS_SPAN_TYPE<Args...>;

} // namespace stdplus
