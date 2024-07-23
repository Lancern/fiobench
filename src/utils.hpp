#pragma once

namespace fiobench {

class FileHandle {
public:
  explicit FileHandle(int fd) noexcept : fd_{fd} {}

  FileHandle(const FileHandle &) = delete;
  FileHandle(FileHandle &&another) noexcept;

  ~FileHandle() noexcept;

  FileHandle &operator=(const FileHandle &) = delete;
  FileHandle &operator=(FileHandle &&another) noexcept;

  [[nodiscard]] int get() const noexcept { return fd_; }

  void reset() noexcept;

private:
  int fd_;
};

} // namespace fiobench
