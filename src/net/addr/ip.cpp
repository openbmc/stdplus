#include <stdplus/net/addr/ip.hpp>

namespace stdplus
{
template In4Addr FromStr<In4Addr>::operator()(std::string_view) const;
template char* ToStr<In4Addr>::operator()(char*, In4Addr) const noexcept;
} // namespace stdplus
