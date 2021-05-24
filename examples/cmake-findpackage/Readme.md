# Consuming TerseLambda via CMake's find_package

This example project shows how to use TerseLambda via [CMake's find_package command](https://cmake.org/cmake/help/latest/command/find_package.html).
See [the package management documentation](/docs/package-management.md#cmake).

To use TerseLambda, you can use the following:

```cmake
find_package(tl 1.0.0 REQUIRED)

# Later, when you define a target that depends on TerseLambda:
target_link_libraries(myTarget
  PRIVATE tl::tl
)
```

Note that for this to work, TerseLambda must be installed in a location that CMake can find it.
Some package managers know how to do this for you.
To do this manually, specify the installation directory when configuring TerseLambda
with `-DCMAKE_INSTALL_PREFIX=/path/to/install` and run `cmake --build . --target install` to install
at that directory.
When configuring your code, pass this installation path to `CMAKE_PREFIX_PATH`:

```bash
# For TerseLambda
mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=$PWD/install ..
cmake --build . --target install

# In your own project's configuration
cmake -DCMAKE_PREFIX_PATH=/path/to/TerseLambda/build/install ..
```
