# fiobench

Benchmarks for file IO performance.

> [!NOTE]
> Currently `fiobench` only implements file read benchmarks.

## Build

> [!NOTE]
> Currently `fiobench` can only be built for Linux.

### Install Prerequisites

`fiobench` requires the following prerequisites:

- A working C++ compiler that supports C++20.
- CMake with version 3.22 or later.
- `liburing`.

You could install these prerequisites on Ubuntu via:

```bash
apt install build-essential cmake liburing-dev
```

### Clone and Build

Clone the repository:

```bash
git clone https://github.com/Lancern/fiobench.git
cd fiobench
```

Create a build directory:

```bash
mkdir build
cd build
```

Build with `cmake`:

```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

## Usage

After build, you can find the `fiobench` binary directly under the build
directory. You can run it with:

```bash
./fiobench $data_dir $benchmark
```

- `$data_dir` is the path to the directory that will be used as the benchmark
  directory. For read benchmarks, this directory contains data files to be read.
- `$benchmark` is the name of the desired benchmark. Currently the following
  benchmarks are available:
  - `sync`: read out all data files sequentially in a synchronous manner.
  - `uring`: read out all data files in an asynchronous manner with
    [`io_uring`](https://man7.org/linux/man-pages/man7/io_uring.7.html).

## License

Published under the [MIT](./LICENSE) license.
