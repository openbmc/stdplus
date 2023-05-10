#pragma once
#include <stdplus/fd/managed.hpp>

#include <filesystem>
#include <string>
#include <string_view>

namespace stdplus
{
namespace fd
{

class AtomicWriter : public stdplus::FdImpl
{
  public:
    AtomicWriter(const std::filesystem::path& filename, int mode,
                 std::string_view tmpl = {});
    AtomicWriter(AtomicWriter&& other);
    AtomicWriter& operator=(AtomicWriter&& other);
    ~AtomicWriter();

    void commit(bool allow_copy = false);

    inline const std::string& getTmpname() const
    {
        return tmpname;
    }

    int get() const override;

  private:
    std::filesystem::path filename;
    int mode;
    std::string tmpname;
    ManagedFd fd;

    void cleanup() noexcept;
};

} // namespace fd
} // namespace stdplus
