#include <fmt/format.h>

#include <stdplus/net/addr/subnet.hpp>

#include <stdexcept>

namespace stdplus
{

namespace detail
{

void invalidSubnetPfx(std::size_t pfx)
{
    throw std::invalid_argument(fmt::format("Invalid subnet prefix {}", pfx));
}

template class Subnet46<In4Addr, uint8_t>;
template class Subnet46<In6Addr, uint8_t>;

} // namespace detail

template Subnet4 FromStr<Subnet4>::operator()(std::string_view) const;
template Subnet6 FromStr<Subnet6>::operator()(std::string_view) const;
template SubnetAny FromStr<SubnetAny>::operator()(std::string_view) const;

template char* ToStr<Subnet4>::operator()(char*, Subnet4) const noexcept;
template char* ToStr<Subnet6>::operator()(char*, Subnet6) const noexcept;
template char* ToStr<SubnetAny>::operator()(char*, SubnetAny) const noexcept;

} // namespace stdplus
