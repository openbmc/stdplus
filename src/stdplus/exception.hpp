#pragma once
#include <system_error>

namespace stdplus
{
namespace exception
{

struct WouldBlock : public std::system_error
{
    WouldBlock(const char* what);
    WouldBlock(const std::string& what);
};

struct Eof : public std::system_error
{
    Eof(const char* what);
    Eof(const std::string& what);
};

} // namespace exception
} // namespace stdplus
