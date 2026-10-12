#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <stdplus/fd/atomic.hpp>
#include <stdplus/fd/managed.hpp>
#include <stdplus/util/cexec.hpp>

#include <cstdlib>
#include <filesystem>
#include <format>
#include <system_error>
#include <utility>

namespace stdplus
{
namespace fd
{

static std::string makeTmpName(const std::filesystem::path& filename)
{
    auto name = filename.filename();
    auto path = filename.parent_path() /
                std::format(".{}.XXXXXX", name.native());
    return path.native();
}

static int mktemp(std::string& tmpl)
{
    return CHECK_ERRNO(mkostemp(tmpl.data(), O_CLOEXEC), [&](int error) {
        throw std::system_error(error, std::generic_category(),
                                std::format("mkostemp({})", tmpl));
    });
}

AtomicWriter::AtomicWriter(const std::filesystem::path& filename, int mode,
                           std::string_view tmpl) :
    filename(filename), mode(mode),
    tmpname(!tmpl.empty() ? std::string(tmpl) : makeTmpName(filename)),
    fd(mktemp(tmpname))
{}

AtomicWriter::AtomicWriter(AtomicWriter&& other) noexcept :
    filename(std::move(other.filename)), mode(other.mode),
    tmpname(std::move(other.tmpname)), fd(std::move(other.fd))
{
    // We don't want to cleanup twice
    other.tmpname.clear();
}

AtomicWriter& AtomicWriter::operator=(AtomicWriter&& other) noexcept
{
    if (this != &other)
    {
        cleanup();
        filename = std::move(other.filename);
        mode = other.mode;
        tmpname = std::move(other.tmpname);
        fd = std::move(other.fd);

        // We don't want to cleanup twice
        other.tmpname.clear();
    }
    return *this;
}

AtomicWriter::~AtomicWriter()
{
    cleanup();
}

void AtomicWriter::commit(bool allow_copy)
{
    try
    {
        CHECK_ERRNO(fsync(get()), "fsync");
        CHECK_ERRNO(fchmod(get(), mode), "fchmod");
        // We want the file to be closed before renaming it
        {
            auto ifd = std::move(fd);
        }
        try
        {
            std::filesystem::rename(tmpname, filename);
            tmpname.clear();
        }
        catch (const std::filesystem::filesystem_error& e)
        {
            if (!allow_copy || e.code() != std::errc::cross_device_link)
            {
                throw;
            }
            auto dst_tmp = makeTmpName(filename);
            int rfd = mktemp(dst_tmp);
            try
            {
                ManagedFd dst_fd(std::move(rfd));
                std::filesystem::copy_file(
                    tmpname, dst_tmp,
                    std::filesystem::copy_options::overwrite_existing);
                CHECK_ERRNO(fsync(dst_fd.get()), "fsync");
                CHECK_ERRNO(fchmod(dst_fd.get(), mode), "fchmod");
                {
                    auto ifd = std::move(dst_fd);
                }
                std::filesystem::rename(dst_tmp, filename);
            }
            catch (...)
            {
                std::error_code ec;
                std::filesystem::remove(dst_tmp, ec);
                throw;
            }
            cleanup();
        }
    }
    catch (...)
    {
        // We do this to ensure that any crashes that might happen before the
        // destructor don't cause us to leave stale files.
        cleanup();
        throw;
    }
}

int AtomicWriter::get() const
{
    return fd.get();
}

void AtomicWriter::cleanup() noexcept
{
    if (!tmpname.empty())
    {
        // Ensure the FD is closed prior to removing the file
        {
            auto ifd = std::move(fd);
        }
        std::error_code ec;
        std::filesystem::remove(tmpname, ec);
        tmpname.clear();
    }
}

} // namespace fd
} // namespace stdplus
