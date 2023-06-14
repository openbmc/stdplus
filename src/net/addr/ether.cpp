#include <stdplus/net/addr/ether.hpp>

namespace stdplus
{
template char* ToStr<EtherAddr>::operator()(char*, EtherAddr) const noexcept;
template EtherAddr FromStr<EtherAddr>::operator()(std::string_view) const;
} // namespace stdplus
