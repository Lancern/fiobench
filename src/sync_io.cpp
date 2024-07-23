#include <cerrno>
#include <cstddef>
#include <memory>
#include <system_error>
#include <utility>
#include <vector>

#include <fcntl.h>
#include <unistd.h>

#include "bench.hpp"
#include "utils.hpp"

namespace fiobench {

namespace {

class SyncIOBenchmark final : public FioBenchmark {
public:
  SyncIOBenchmark() noexcept : FioBenchmark("syncio") {}

protected:
  std::vector<std::unique_ptr<std::byte[]>>
  doRead(std::span<const DataFileInfo> data_files) override {
    std::vector<std::unique_ptr<std::byte[]>> data;
    data.reserve(data_files.size());
    for (const auto &info : data_files) {
      auto file_data = read(info);
      data.push_back(std::move(file_data));
    }
    return data;
  }

private:
  [[nodiscard]] std::unique_ptr<std::byte[]> read(const DataFileInfo &info) {
    auto raw_fd = ::open(info.path.c_str(), O_RDONLY);
    if (raw_fd == -1) {
      auto ec = errno;
      throw std::system_error{ec, std::system_category(), "open"};
    }

    FileHandle fd{raw_fd};

    std::unique_ptr<std::byte[]> buffer{new std::byte[info.size]};
    std::uint64_t bytes_read = 0;
    while (bytes_read < info.size) {
      auto read_count = ::read(fd.get(), buffer.get() + bytes_read, info.size - bytes_read);
      if (read_count == -1) {
        auto ec = errno;
        throw std::system_error{ec, std::system_category(), "read"};
      }
      bytes_read += read_count;
    }

    return buffer;
  }
};

} // namespace

std::unique_ptr<FioBenchmark> FioBenchmark::createSyncIO() noexcept {
  return std::make_unique<SyncIOBenchmark>();
}

} // namespace fiobench
