#include <stdplus/net/addr/hostname.hpp>

#include <algorithm>
#include <cctype>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace stdplus
{

Hostname::Hostname(std::string hostname) : hostname_(std::move(hostname))
{
    if (!isValid(hostname_))
    {
        throw std::invalid_argument("Invalid hostname: " + hostname_);
    }
}

const std::string& Hostname::str() const noexcept
{
    return hostname_;
}

bool Hostname::isValid(std::string_view hostname) noexcept
{
    if (hostname.empty() || hostname.length() > 253)
    {
        return false;
    }

    if (hostname.front() == '.')
    {
        return false;
    }

    std::string hostnameStr(hostname);
    if (hostnameStr.back() == '.')
    {
        hostnameStr.pop_back();
        if (hostnameStr.empty())
        {
            return false;
        }
    }

    std::vector<std::string> labels;
    std::stringstream ss(hostnameStr);
    std::string label;

    while (std::getline(ss, label, '.'))
    {
        labels.push_back(label);
    }

    if (labels.empty() ||
        std::find(labels.begin(), labels.end(), "") != labels.end())
    {
        return false;
    }

    for (size_t i = 0; i < labels.size(); ++i)
    {
        const auto& lbl = labels[i];

        if (lbl.empty() || lbl.length() > 63)
        {
            return false;
        }

        if (lbl.front() == '-' || lbl.back() == '-')
        {
            return false;
        }

        for (char c : lbl)
        {
            if (!std::isalnum(static_cast<unsigned char>(c)) && c != '-')
            {
                return false;
            }
        }

        if (i == labels.size() - 1)
        {
            bool hasLetter = std::any_of(lbl.begin(), lbl.end(), [](char c) {
                return std::isalpha(static_cast<unsigned char>(c));
            });

            if (!hasLetter)
            {
                return false;
            }
        }
    }

    return true;
}

bool Hostname::operator==(const Hostname& other) const noexcept
{
    return hostname_ == other.hostname_;
}

bool Hostname::operator!=(const Hostname& other) const noexcept
{
    return !(*this == other);
}

Hostname FromStr<Hostname>::operator()(std::string_view hostname) const
{
    return Hostname(std::string(hostname));
}

std::string ToStr<Hostname>::operator()(const Hostname& hostname) const
{
    return hostname.str();
}

} // namespace stdplus
