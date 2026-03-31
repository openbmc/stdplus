#include <stdplus/net/addr/hostname.hpp>

#include <cstddef>
#include <stdexcept>
#include <string_view>

namespace stdplus
{

// Custom character classification (constexpr, no locale dependency)
// Ensures predictable, non-allocating compile-time execution
constexpr bool is_alpha(char c) noexcept
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

constexpr bool is_digit(char c) noexcept
{
    return (c >= '0' && c <= '9');
}

constexpr bool is_alnum(char c) noexcept
{
    return is_alpha(c) || is_digit(c);
}

/** @brief Validates a hostname using finite state machine (zero allocations)
 *  @details Validates hostnames according to RFC 952, RFC 1123, and RFC 1035.
 *           Uses a finite state machine for efficient, single-pass validation
 *           with no memory allocations.
 *
 *           Validation rules:
 *           - Total length: 1-253 characters (RFC 1035)
 *           - Label length: 1-63 characters per label (RFC 1035)
 *           - Labels must start and end with alphanumeric characters
 *           - Labels may contain hyphens in the middle
 *           - Dots separate labels
 *           - Trailing dot permitted (represents DNS root)
 *           - At least one alphabetic character required
 *           - Supports both FQDN (example.com) and local hostnames (localhost)
 *
 *  @param[in] hostname - The hostname to validate
 *  @return true if valid hostname, false otherwise
 */
[[nodiscard]] constexpr bool is_valid_hostname(
    std::string_view hostname) noexcept
{
    // RFC 1035: Total hostname length cannot exceed 253 characters
    if (hostname.empty() || hostname.length() > 253)
    {
        return false;
    }

    // Finite state machine states for hostname validation
    enum class State
    {
        Start,     // Initial state, or expecting first char of new label
        LabelChar, // Processing valid alphanumeric characters in label
        Hyphen,    // Just processed a hyphen
        Dot        // Just processed a dot separator
    };

    State state = State::Start;
    std::size_t label_len = 0;
    bool current_label_has_alpha =
        false;                  // Track if current label has alphabetic char
    bool has_any_alpha = false; // Track if entire hostname has any alpha

    for (char c : hostname)
    {
        if (c == '.')
        {
            // Fails if: starts with dot, consecutive dots (empty label),
            // or label ends in hyphen
            if (state == State::Start || state == State::Dot ||
                state == State::Hyphen)
            {
                return false;
            }
            // Reset for next label
            current_label_has_alpha = false;
            state = State::Dot;
            label_len = 0;
        }
        else if (c == '-')
        {
            // Fails if: label starts with a hyphen
            if (state == State::Start || state == State::Dot)
            {
                return false;
            }
            state = State::Hyphen;
            if (++label_len > 63)
            {
                return false; // RFC 1035: max label length is 63 characters
            }
        }
        else if (is_alnum(c))
        {
            // Track if this character is alphabetic
            if (is_alpha(c))
            {
                current_label_has_alpha = true;
                has_any_alpha = true;
            }
            state = State::LabelChar;
            if (++label_len > 63)
            {
                return false;
            }
        }
        else
        {
            // Reject any illegal characters (spaces, underscores, special
            // symbols)
            return false;
        }
    }

    // Hostname cannot end in a hyphen
    if (state == State::Hyphen)
    {
        return false;
    }

    // Hostname must end in a valid label or trailing dot
    if (state != State::LabelChar && state != State::Dot)
    {
        return false;
    }

    // Ensure hostname contains at least one alphabetic character
    // This rejects pure numeric hostnames (e.g., "123456", "192.168.1")
    // while allowing mixed alphanumeric (e.g., "server1", "192.example.com")
    if (!has_any_alpha)
    {
        return false;
    }

    // The final label must contain at least one alphabetic character
    // This ensures the TLD or single-label hostname is not purely numeric
    if (state == State::LabelChar && !current_label_has_alpha)
    {
        return false;
    }

    return true;
}

Hostname::Hostname(std::string_view hostname)
{
    if (!is_valid_hostname(hostname))
    {
        throw std::invalid_argument(
            "Invalid hostname format: " + std::string(hostname));
    }

    // Remove trailing dot if present (example.com. -> example.com)
    if (!hostname.empty() && hostname.back() == '.')
    {
        str = std::string(hostname.substr(0, hostname.length() - 1));
    }
    else
    {
        str = std::string(hostname);
    }
}

std::string_view Hostname::getHostname() const noexcept
{
    return str;
}

// Explicit template instantiations
template Hostname FromStr<Hostname>::operator()(std::string_view) const;
template char* ToStr<Hostname>::operator()(char*,
                                           const Hostname&) const noexcept;

} // namespace stdplus
