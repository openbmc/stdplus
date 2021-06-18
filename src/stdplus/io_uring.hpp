#pragma once
#include <liburing.h>

#include <stdplus/fd/managed.hpp>

#include <optional>
#include <vector>

namespace stdplus
{

class IoUring
{
  public:
    struct CqeHandler
    {
        CqeHandler() = default;
        CqeHandler(CqeHandler&&) = delete;
        CqeHandler& operator=(CqeHandler&&) = delete;
        CqeHandler(const CqeHandler&) = delete;
        CqeHandler& operator=(const CqeHandler&) = delete;
        virtual ~CqeHandler() = default;

        virtual void handleCqe(io_uring_cqe&) noexcept = 0;
    };

    void setHandler(io_uring_sqe& sqe, CqeHandler* h) noexcept;
    void cancelHandler(CqeHandler& h);

    explicit IoUring(size_t queue_size = 10);
    IoUring(IoUring&&) = delete;
    IoUring& operator=(IoUring&&) = delete;
    IoUring(const IoUring&) = delete;
    IoUring& operator=(const IoUring&) = delete;
    ~IoUring();

    io_uring_sqe* getSqe();
    void submit();
    void process() noexcept;

    stdplus::ManagedFd& getEventFd();
    void processEvents();

  private:
    io_uring ring;
    std::optional<stdplus::ManagedFd> event_fd;
    std::vector<CqeHandler*> handlers;

    void dropHandler(CqeHandler* h, io_uring_cqe& cqe) noexcept;
};

} // namespace stdplus
