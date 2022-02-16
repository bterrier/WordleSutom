

enable_testing()

add_executable(test main.cpp)
target_link_libraries(
  test
  gtest_main
  LibWordleSutom
)

include(GoogleTest)
gtest_discover_tests(test)
