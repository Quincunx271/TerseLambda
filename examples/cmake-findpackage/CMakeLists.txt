cmake_minimum_required(VERSION 3.11)

project(Example
  LANGUAGES CXX
  DESCRIPTION "Example project for using TerseLambda via FetchContent"
)

set(TERSE_LAMBDA_VERSION 1.0.0 CACHE STRING "The commit/branch/tag to use")

find_package(tl ${TERSE_LAMBDA_VERSION} REQUIRED)

# Target
add_executable(example src/example.main.cpp)
target_link_libraries(example PRIVATE tl::tl)

enable_testing()
add_test(example example)
