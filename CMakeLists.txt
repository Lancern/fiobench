cmake_minimum_required(VERSION 3.22)
project(fiobench)

find_library(liburing_shared
    NAMES liburing.so
    REQUIRED
)
message(STATUS "liburing.so found at " ${liburing_shared})

add_library(liburing SHARED IMPORTED)
set_target_properties(liburing PROPERTIES
    IMPORTED_LOCATION ${liburing_shared}
)

add_executable(fiobench
    src/bench.cpp
    src/main.cpp
    src/sync_io.cpp
    src/uring_io.cpp
    src/utils.cpp
)
target_compile_features(fiobench PRIVATE cxx_std_20)
target_link_libraries(fiobench PRIVATE liburing)
