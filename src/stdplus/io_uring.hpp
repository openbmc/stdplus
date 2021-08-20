#pragma once

#include <liburing.h>

#include <stdplus/fd/managed.hpp>
#include <stdplus/handle/managed.hpp>

#include <chrono>
#include <optional>
#include <vector>

namespace stdplus
{

/** @brief Converts a chrono duration into a kernel duration */
__kernel_timespec chronoToKTS(std::chrono::nanoseconds t) noexcept;

class IoUring
{
  public:
    struct CQEHandler
    {
        CQEHandler() = default;
        CQEHandler(CQEHandler&&) = delete;
        CQEHandler& operator=(CQEHandler&&) = delete;
        CQEHandler(const CQEHandler&) = delete;
        CQEHandler& operator=(const CQEHandler&) = delete;
        virtual ~CQEHandler() = default;

        virtual void handleCQE(io_uring_cqe&) noexcept = 0;
    };

    class FileHandle
    {
      public:
        inline operator int() const
        {
            return *slot;
        }

      private:
        explicit FileHandle(unsigned slot, IoUring& ring);

        static void drop(unsigned&& slot, IoUring*& ring);

        Managed<unsigned, IoUring*>::Handle<drop> slot;

        friend class IoUring;
    };

    explicit IoUring(size_t queue_size = 10, int flags = 0);
    explicit IoUring(size_t queue_size, io_uring_params& params);

    IoUring(IoUring&&) = delete;
    IoUring& operator=(IoUring&&) = delete;
    IoUring(const IoUring&) = delete;
    IoUring& operator=(const IoUring&) = delete;
    ~IoUring();

    /** @brief Reserves an additional number of file descriptor slots
     *
     *  @param[in] num - The number of slots to register
     *  @throws std::system_error if the allocation fails
     */
    void reserveFiles(size_t num);

    /** @brief Registers a file descriptor with a slot on the ring
     *
     *  @param[in] fd - The file descriptor to register
     *  @throws std::system_error if the allocation fails
     *  @return A handle to the registered file on the ring
     */
    [[nodiscard]] FileHandle registerFile(int fd);

    /** @brief Get current list of files descriptors registered on the ring.
     * Note this view potentially expires when registrations change. */
    inline stdplus::span<const int> getFiles() const noexcept
    {
        return files;
    }

    /** @brief Gets an unused SQE from the ring
     *
     *  @throws std::system_error if the allocation fails
     *  @return An SQE on the ring
     */
    io_uring_sqe& getSQE();

    /** @brief Associates the SQE with a user provided callback handler
     *
     *  @param[in] sqe - The SQE that we want to register
     *  @param[in] h   - The handler which will be run when the CQE comes back
     */
    void setHandler(io_uring_sqe& sqe, CQEHandler* h) noexcept;

    /** @brief Cancels the outstanding request associated with a handler
     *
     *  @param[in] h - The handler associated with the request
     */
    void cancelHandler(CQEHandler& h);

    /** @brief Submits all outstanding SQEs to the kernel
     *
     *  @throws std::system_error if the submission fails
     */
    void submit();

    /** @brief Non-blocking process all outstanding CQEs */
    void process() noexcept;

    /** @brief Waits for new CQEs to become available */
    void wait();
    void wait(std::chrono::nanoseconds timeout);

    /** @brief Returns the EventFD associated with the ring
     *         A new descriptor is created if it does not yet exist
     *
     *  @throws std::system_error if constructing the event fd fails
     *  @return A reference to the descriptor
     */
    stdplus::ManagedFd& getEventFd();

    /** @brief Non-blocking process all outstanding eventFd events
     *         Should be used instead of process() to clear eventFd events.
     */
    void processEvents();

  private:
    io_uring ring;
    std::optional<stdplus::ManagedFd> event_fd;
    std::vector<CQEHandler*> handlers;
    std::vector<int> files;
    size_t filesAllocated = 0;

    void dropHandler(CQEHandler* h, io_uring_cqe& cqe) noexcept;
    void setFile(unsigned slot, int fd) noexcept;
    void updateFile(unsigned slot, int fd);
};

} // namespace stdplus
