#pragma once

#include <stdplus/str/conv.hpp>

#include <compare>
#include <string>
#include <string_view>

namespace stdplus
{

/** @class Hostname
 *  @brief Represents a validated DNS hostname or FQDN
 *  @details Validates hostnames according to RFC 952, RFC 1123, and RFC 1035.
 *           Uses a finite state machine for efficient, zero-allocation
 * validation.
 *
 *           Supports both fully qualified domain names (FQDN) and local
 * hostnames. Requires at least one alphabetic character to distinguish from
 *           numeric identifiers like IP addresses or port numbers.
 *
 *  Valid examples:
 *    - "localhost"
 *    - "example.com"
 *    - "sub.example.com"
 *    - "my-server.example.com"
 *    - "example.com." (trailing dot represents DNS root)
 *
 *  Invalid examples:
 *    - "123456" (pure numeric)
 *    - "-example.com" (starts with hyphen)
 *    - "example-.com" (label ends with hyphen)
 *    - "example..com" (consecutive dots)
 *    - "192.168.1" (all numeric labels)
 */
class Hostname
{
  private:
    std::string str;

  public:
    /** @brief Constructs a Hostname from a string view
     *  @param[in] hostname - The hostname string to validate
     *  @throws std::invalid_argument if hostname is invalid
     */
    explicit Hostname(std::string_view hostname);

    /** @brief Returns the hostname as a string view
     *  @return The validated hostname (without trailing dot if present)
     */
    std::string_view getHostname() const noexcept;

    /** @brief Three-way comparison operator
     *  @details Provides all comparison operators (==, !=, <, <=, >, >=)
     *           through a single spaceship operator implementation
     */
    auto operator<=>(const Hostname& other) const noexcept = default;
};

/** @brief Converts a string to a Hostname */
template <>
struct FromStr<Hostname>
{
    template <typename CharT>
    constexpr Hostname operator()(std::basic_string_view<CharT> sv) const
    {
        return Hostname(std::string_view(sv.data(), sv.size()));
    }
};

/** @brief Converts a Hostname to a string */
template <>
struct ToStr<Hostname>
{
    using type = Hostname;
    static inline constexpr std::size_t buf_size = 253;

    template <typename CharT>
    constexpr CharT* operator()(CharT* buf, const Hostname& v) const noexcept
    {
        auto sv = v.getHostname();
        return std::copy(sv.begin(), sv.end(), buf);
    }
};

} // namespace stdplus
