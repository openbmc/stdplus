#pragma once

#include <stdplus/str/conv.hpp>

#include <string>
#include <string_view>

namespace stdplus
{

class Hostname
{
  public:
    explicit Hostname(std::string hostname);
    [[nodiscard]] const std::string& str() const noexcept;
    [[nodiscard]] static bool isValid(std::string_view hostname) noexcept;
    [[nodiscard]] bool operator==(const Hostname& other) const noexcept;
    [[nodiscard]] bool operator!=(const Hostname& other) const noexcept;

  private:
    std::string hostname_;
};

template <>
struct FromStr<Hostname>
{
    Hostname operator()(std::string_view hostname) const;
};

template <>
struct ToStr<Hostname>
{
    std::string operator()(const Hostname& hostname) const;
};

} // namespace stdplus
