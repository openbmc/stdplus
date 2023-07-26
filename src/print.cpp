#include <stdplus/print.hpp>

#include <cstdio>
#include <system_error>

namespace stdplus
{
void prints(std::FILE* stream, std::string_view data)
{
    int r = std::fwrite(data.data(), sizeof(char), data.size(), stream);
    if (r < 0)
    {
        throw std::system_error(errno, std::generic_category());
    }
}
} // namespace stdplus
