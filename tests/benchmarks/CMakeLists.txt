set(BENCHMARK_ENABLE_TESTING OFF)
include(FetchContent)
FetchContent_Declare(
  benchmark
  GIT_REPOSITORY https://github.com/google/benchmark.git
  GIT_TAG v1.6.1
  GIT_SHALLOW TRUE
  GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(benchmark)

add_executable(mbenchmark main.cpp)
target_link_libraries(
  mbenchmark
  benchmark::benchmark
  benchmark::benchmark_main
  LibWordleSutom
)
