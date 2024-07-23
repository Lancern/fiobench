#include "utils.hpp"

#include <unistd.h>
#include <utility>

namespace fiobench {

namespace {

constexpr int INVALID_FD = -1;

} // namespace

FileHandle::FileHandle(FileHandle &&another) noexcept
    : fd_{std::exchange(another.fd_, INVALID_FD)} {}

FileHandle::~FileHandle() noexcept { reset(); }

FileHandle &FileHandle::operator=(FileHandle &&another) noexcept {
  if (this == &another) {
    return *this;
  }

  reset();
  fd_ = std::exchange(another.fd_, INVALID_FD);

  return *this;
}

void FileHandle::reset() noexcept {
  if (fd_ != INVALID_FD) {
    ::close(fd_);
    fd_ = INVALID_FD;
  }
}

} // namespace fiobench
