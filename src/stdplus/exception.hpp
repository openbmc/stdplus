#pragma once
#include <stdexcept>
#include <string>
#include <string_view>

namespace stdplus
{
namespace exception
{

class Eof : public std::runtime_error
{
  public:
    Eof(std::string_view prefix) :
        std::runtime_error(std::string{prefix} + ": EOF")
    {
    }
};

} // namespace exception
} // namespace stdplus
