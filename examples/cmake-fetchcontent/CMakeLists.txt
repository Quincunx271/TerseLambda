cmake_minimum_required(VERSION 3.11)

project(Example
  LANGUAGES CXX
  DESCRIPTION "Example project for using TerseLambda via FetchContent"
)

set(TERSE_LAMBDA_COMMITISH v0.2.0 CACHE STRING "The commit/branch/tag to use")

include(FetchContent)
FetchContent_Declare(
  TerseLambda
  GIT_REPOSITORY https://github.com/Quincunx271/TerseLambda.git
  GIT_TAG ${TERSE_LAMBDA_COMMITISH}
)
FetchContent_MakeAvailable(TerseLambda)

# Target
add_executable(example src/example.main.cpp)
target_link_libraries(example PRIVATE tl::tl)

enable_testing()
add_test(example example)
