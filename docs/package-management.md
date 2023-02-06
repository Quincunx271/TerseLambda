# Supported Package Managers

 * [DDS][dds]
 * [CMake Usage][cmake]


# DDS

TerseLambda can be used by [DDS](https://github.com/vector-of-bool/dds).
A pre-made DDS repo for TerseLambda can be found at https://github.com/Quincunx271/dds-repos.
See [this example](/examples/dds) for a complete example.
These steps assume you wish to use that repo:

  * Add the repo to DDS:

    ```bash
    dds pkg repo add "https://quincunx271.github.io/dds-repos/public"
    ```

  * Tell DDS that you want to use the package:

    ```json5
      // In your package.json5
      depends: [
        'terse-lambda@1.0.0'
      ]
      // In your library.json5
      uses: [
        'quincunx271/terse-lambda'
      ]
    ```

# CMake

CMake isn't a package manager, but TerseLambda can be consumed via the standard ways of
using CMake packages. That is, you can use TerseLambda via `add_subdirectory(...)` and also
via `find_package(tl ${VERSION} REQUIRED)`.

See [the FetchContent example](/examples/cmake-fetchcontent) and [the find_package example](/examples/cmake-findpackage)
for complete examples.


  [dds]: #dds
  [cmake]: #cmake
