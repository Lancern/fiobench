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
  std::uint64_t data_file_size = 0;
  for (const auto &entry : std::filesystem::directory_iterator{data_dir}) {
    if (!entry.is_regular_file()) {
      continue;
    }

    DataFileInfo info{entry.path(), entry.file_size()};
    data_files.push_back(std::move(info));

    data_file_size += entry.file_size();
    if (data_file_size >= MAX_IO_SIZE) {
      break;
    }
  }

  std::cout << "========== Benchmark " << name_ << " ==========\n";
  std::cout << data_files.size() << " data files\n";
  std::cout << data_file_size << " bytes in total" << std::endl;

  auto start_time = std::chrono::high_resolution_clock::now();
  auto data = doRead(data_files);
  auto end_time = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> dur_s = end_time - start_time;

  auto throughput = data_file_size / dur_s.count() / 1024 / 1024;

  std::cout << "IO operation completed in " << dur_s.count() << " seconds.\n";
  std::cout << "Throughput: " << throughput << "MB/s" << std::endl;
}

FioBenchmark::FioBenchmark(std::string name) noexcept : name_(std::move(name)) {}

} // namespace fiobench
