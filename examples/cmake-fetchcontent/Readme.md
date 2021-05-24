# Consuming TerseLambda via CMake's FetchContent

This example project shows how to use TerseLambda via [CMake's FetchContent module](https://cmake.org/cmake/help/latest/module/FetchContent.html).
See [the package management documentation](/docs/package-management.md#cmake).

To use TerseLambda, you can use the following:

```cmake
include(FetchContent)
FetchContent_Declare(
  TerseLambda
  GIT_REPOSITORY https://github.com/Quincunx271/TerseLambda.git
  GIT_TAG v1.0.0-alpha.1
)
FetchContent_MakeAvailable(TerseLambda)

# Later, when you define a target that depends on TerseLambda:
target_link_libraries(myTarget
  PRIVATE tl::tl
)
```
