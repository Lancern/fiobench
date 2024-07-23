#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace fiobench {

struct DataFileInfo {
  std::filesystem::path path;
  std::uint64_t size;
};

class FioBenchmark {
public:
  [[nodiscard]] static std::unique_ptr<FioBenchmark> createSyncIO() noexcept;
  [[nodiscard]] static std::unique_ptr<FioBenchmark> createUringIO() noexcept;

  [[nodiscard]] static std::unique_ptr<FioBenchmark> create(std::string_view name) noexcept;

  FioBenchmark(const FioBenchmark &) = delete;
  FioBenchmark(FioBenchmark &&) = delete;

  virtual ~FioBenchmark() noexcept = default;

  FioBenchmark &operator=(const FioBenchmark &) = delete;
  FioBenchmark &operator=(FioBenchmark &&) = delete;

  [[nodiscard]] std::string_view getName() const noexcept { return name_; }

  void runReadBenchmark(const std::filesystem::path &data_dir);

protected:
  explicit FioBenchmark(std::string name) noexcept;

  virtual std::vector<std::unique_ptr<std::byte[]>>
  doRead(std::span<const DataFileInfo> data_files) = 0;

private:
  std::string name_;
};

} // namespace fiobench
