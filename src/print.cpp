#include <stdplus/print.hpp>

#include <cerrno>
#include <cstdio>
#include <system_error>

namespace stdplus
{
void prints(std::FILE* stream, std::string_view data)
{
    if (data.empty())
    {
        return;
    }
    std::fwrite(data.data(), sizeof(char), data.size(), stream);
    if (std::ferror(stream))
    {
        throw std::system_error(EIO, std::generic_category());
    }
}
} // namespace stdplus
