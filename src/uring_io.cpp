#include "bench.hpp"

#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <system_error>
#include <vector>

#include <fcntl.h>
#include <liburing.h>
#include <liburing/io_uring.h>
#include <unistd.h>

namespace fiobench {

namespace {

class UringIOBenchmark final : public FioBenchmark {
public:
  UringIOBenchmark() noexcept : FioBenchmark{"uring"} {}

protected:
  std::vector<std::unique_ptr<std::byte[]>>
  doRead(std::span<const DataFileInfo> data_files) override {
    std::unique_ptr<::io_uring, UringDeleter> uring{new ::io_uring{}};
    if (auto ret = ::io_uring_queue_init(data_files.size(), uring.get(), 0); ret != 0) {
      throw std::system_error{-ret, std::system_category(), "io_uring_queue_init"};
    }

    std::vector<std::unique_ptr<std::byte[]>> data(data_files.size());

    // Submit all IO read requests to the ring.
    for (std::size_t i = 0; i < data_files.size(); ++i) {
      data[i].reset(new std::byte[data_files[i].size]);
      submitFileReadRequest(uring.get(), data_files[i], data[i].get());
    }

    // Process each IO completion notification.
    std::size_t ongoing_ioreq = data_files.size();
    while (ongoing_ioreq > 0) {
      if (processIoCompletion(uring.get())) {
        --ongoing_ioreq;
      }
    }

    return data;
  }

private:
  struct UringDeleter {
    void operator()(::io_uring *uring) const noexcept { ::io_uring_queue_exit(uring); }
  };

  struct UringUserData {
    const DataFileInfo *info;
    std::byte *buffer;
    int fd;
    std::uint64_t bytes_read;
  };

  void submitFileReadRequest(::io_uring *ring, const DataFileInfo &info, std::byte *buffer) {
    auto fd = ::open(info.path.c_str(), O_RDONLY);
    if (fd == -1) {
      auto ec = errno;
      throw std::system_error{ec, std::system_category(), "open"};
    }

    auto *user_data = new UringUserData{&info, buffer, fd, 0};
    submitFileReadRequest(ring, user_data);
  }

  void submitFileReadRequest(::io_uring *ring, UringUserData *user_data) {
    auto *read_buffer = user_data->buffer + user_data->bytes_read;
    auto read_size = user_data->info->size - user_data->bytes_read;

    auto *sqe = ::io_uring_get_sqe(ring);
    if (!sqe) {
      ::close(user_data->fd);
      delete user_data;
      throw std::runtime_error{"io_uring_get_sqe failed to allocate io_uring SQE"};
    }

    ::io_uring_prep_read(sqe, user_data->fd, read_buffer, read_size, 0);
    ::io_uring_sqe_set_data(sqe, user_data);

    if (auto ret = ::io_uring_submit(ring); ret < 0) {
      ::close(user_data->fd);
      delete user_data;
      throw std::system_error{-ret, std::system_category(), "io_uring_submit"};
    }
  }

  bool processIoCompletion(::io_uring *ring) {
    ::io_uring_cqe *cqe;
    if (auto ret = ::io_uring_wait_cqe(ring, &cqe); ret != 0) {
      throw std::system_error{-ret, std::system_category(), "io_uring_wait_cqe"};
    }
    if (cqe->res < 0) {
      throw std::system_error{-cqe->res, std::system_category(), "io_uring read"};
    }

    auto *user_data = reinterpret_cast<UringUserData *>(::io_uring_cqe_get_data(cqe));
    user_data->bytes_read += cqe->res;

    ::io_uring_cqe_seen(ring, cqe);

    if (user_data->bytes_read < user_data->info->size) {
      submitFileReadRequest(ring, user_data);
      return false;
    }

    ::close(user_data->fd);
    delete user_data;

    return true;
  }
};

} // namespace

std::unique_ptr<FioBenchmark> FioBenchmark::createUringIO() noexcept {
  return std::make_unique<UringIOBenchmark>();
}

} // namespace fiobench
