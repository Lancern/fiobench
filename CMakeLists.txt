cmake_minimum_required(VERSION 3.22)
project(fiobench)

add_executable(fiobench
    src/main.cpp
)
target_compile_features(fiobench PRIVATE cxx_std_20)
