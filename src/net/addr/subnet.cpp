#include <fmt/format.h>

#include <stdplus/net/addr/subnet.hpp>

#include <stdexcept>

namespace stdplus::detail
{

void invalidSubnetPfx(std::size_t pfx)
{
    throw std::invalid_argument(fmt::format("Invalid subnet prefix {}", pfx));
}

template class Subnet46<In4Addr, uint8_t>;
template class Subnet46<In6Addr, uint8_t>;

template char* SubnetToStr<Subnet4>::operator()(char*, Subnet4) const noexcept;
template char* SubnetToStr<Subnet6>::operator()(char*, Subnet6) const noexcept;
template char* SubnetToStr<SubnetAny>::operator()(char*,
                                                  SubnetAny) const noexcept;

} // namespace stdplus::detail
