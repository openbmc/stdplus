#include <stdplus/exception.hpp>
#include <stdplus/fd/line.hpp>
#include <stdplus/fd/ops.hpp>

namespace stdplus
{
namespace fd
{

LineReader::LineReader(Fd& fd) : fd(fd)
{
}

const std::string* LineReader::readLine()
{
    if (hit_eof)
    {
        throw exception::Eof("readLine");
    }
    if (line_complete)
    {
        line.clear();
    }
    while (true)
    {
        if (buf_data.empty())
        {
            try
            {
                buf_data = read(fd, buf);
                if (buf_data.empty())
                {
                    return nullptr;
                }
            }
            catch (const exception::Eof&)
            {
                hit_eof = true;
                return &line;
            }
        }
        line_complete = false;
        for (size_t i = 0; i < buf_data.size(); ++i)
        {
            if (buf_data[i] == '\n')
            {
                line.insert(line.end(), buf_data.begin(), buf_data.begin() + i);
                buf_data = buf_data.subspan(i + 1);
                line_complete = true;
                return &line;
            }
        }
        line.insert(line.end(), buf_data.begin(), buf_data.end());
        buf_data = {};
    }
}

} // namespace fd
} // namespace stdplus
