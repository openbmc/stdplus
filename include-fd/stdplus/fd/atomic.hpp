#pragma once
#include <filesystem>
#include <stdplus/fd/managed.hpp>
#include <string>
#include <string_view>

namespace stdplus
{
namespace fd
{

namespace detail
{
extern bool tmpfile_support;
}

class AtomicWriter : public stdplus::FdImpl
{
  public:
    AtomicWriter(const std::filesystem::path& filename, int mode,
                 std::string_view tmpl = {});
    AtomicWriter(AtomicWriter&& other);
    AtomicWriter& operator=(AtomicWriter&& other);
    ~AtomicWriter();

    void commit(bool allow_copy = false);

    const std::string& getTmpname() const;

    int get() const override;

  private:
    std::filesystem::path filename;
    int mode;
    bool tmpexists;
    std::string tmpname;
    ManagedFd fd;

    void cleanup() noexcept;
};

} // namespace fd
} // namespace stdplus
