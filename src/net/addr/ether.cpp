#include <stdplus/net/addr/ether.hpp>

namespace stdplus
{
template EtherAddr FromStr<EtherAddr>::operator()(std::string_view) const;
}
