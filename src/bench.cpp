#include "bench.hpp"

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <utility>
#include <vector>

namespace fiobench {

std::unique_ptr<FioBenchmark> FioBenchmark::create(std::string_view name) noexcept {
  if (name == "sync") {
    return createSyncIO();
  } else if (name == "uring") {
    return createUringIO();
  }

  return nullptr;
}

void FioBenchmark::runReadBenchmark(const std::filesystem::path &data_dir) {
  constexpr std::uint64_t MAX_IO_SIZE = 10ULL * 1024 * 1024 * 1024;

  std::vector<DataFileInfo> data_files;
  std::vector<std::uint64_t> data_file_sizes;
  std::uint64_t data_file_total_size = 0;
  for (const auto &entry : std::filesystem::directory_iterator{data_dir}) {
    if (!entry.is_regular_file()) {
      continue;
    }

    std::uint64_t file_size = entry.file_size();
    DataFileInfo info{entry.path(), file_size};
    data_files.push_back(std::move(info));
    data_file_sizes.push_back(file_size);

    data_file_total_size += file_size;
    if (data_file_total_size >= MAX_IO_SIZE) {
      break;
    }
  }

  if (data_files.empty()) {
    std::cout << "No data files found." << std::endl;
    return;
  }

  std::ranges::sort(data_file_sizes);
  auto min_file_size = data_file_sizes.front();
  auto max_file_size = data_file_sizes.back();
  auto mid_file_size = data_file_sizes[data_file_sizes.size() / 2];

  std::cout << "========== Benchmark " << name_ << " ==========\n";
  std::cout << data_files.size() << " data files\n";
  std::cout << DataSize{data_file_total_size} << " of data in total" << "\n";
  std::cout << "File sizes (min, mid, max): " << DataSize{min_file_size} << " "
            << DataSize{mid_file_size} << " " << DataSize{max_file_size} << std::endl;

  auto start_time = std::chrono::high_resolution_clock::now();
  auto data = doRead(data_files);
  auto end_time = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> dur_s = end_time - start_time;

  auto throughput = data_file_total_size / dur_s.count();

  std::cout << "IO operation completed in " << dur_s.count() << " seconds.\n";
  std::cout << "Throughput: " << DataSize{throughput} << "/s" << std::endl;
}

FioBenchmark::FioBenchmark(std::string name) noexcept : name_(std::move(name)) {}

} // namespace fiobench
