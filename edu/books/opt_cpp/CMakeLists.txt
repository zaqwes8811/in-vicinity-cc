# https://stackoverflow.com/questions/58807225/google-benchmark-as-a-cmake-dependency
# https://cmake.org/cmake/help/latest/command/find_package.html
project(opt_cpp)
cmake_minimum_required(VERSION 3.10)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_LIST_DIR}/../cmake" CACHE STRING "Modules for CMake" FORCE)

include_directories(${CMAKE_SOURCE_DIR}/3rdparty/llvm-project/llvm/include)
include_directories(${CMAKE_SOURCE_DIR}/3rdparty/llvm-project/llvm_autogen/include)

find_package(benchmark REQUIRED)
include_directories(${benchmark_INCLUDE_DIRS})
link_libraries(${benchmark_LIBRARIES} pthread)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

# https://stackoverflow.com/questions/26749461/how-to-determine-the-boost-library-names-for-cmake
find_package(Boost 1.51.0 REQUIRED)

add_executable(opt_cpp
        ${CMAKE_SOURCE_DIR}/projects/all_benches.cpp
        ch4_benchmark.cc
        )

target_link_libraries(opt_cpp
        ${GTEST_BOTH_LIBRARIES}
        pthread dl m rt
        )

