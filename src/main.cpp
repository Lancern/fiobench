#include <iostream>
#include <filesystem>

#include "bench.hpp"

int main(int argc, char *argv[]) {
  if (argc != 3) {
    std::cerr << "wrong number of arguments." << std::endl;
    return 1;
  }

  std::filesystem::path data_dir{argv[1]};
  auto benchmark = fiobench::FioBenchmark::create(argv[2]);
  if (!benchmark) {
    std::cerr << "unknown benchmark name." << std::endl;
    return 1;
  }

  benchmark->runReadBenchmark(data_dir);

  return 0;
}
