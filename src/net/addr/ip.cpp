#include <stdplus/net/addr/ip.hpp>

namespace stdplus
{
template In4Addr FromStr<In4Addr>::operator()(std::string_view) const;
template char* ToStr<In4Addr>::operator()(char*, In4Addr) const noexcept;
template In6Addr FromStr<In6Addr>::operator()(std::string_view) const;
template char* ToStr<In6Addr>::operator()(char*, In6Addr) const noexcept;
template InAnyAddr FromStr<InAnyAddr>::operator()(std::string_view) const;
template char* ToStr<InAnyAddr>::operator()(char*, InAnyAddr) const noexcept;
} // namespace stdplus
