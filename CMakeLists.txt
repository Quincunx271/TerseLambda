cmake_minimum_required(VERSION 3.10)

list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

project(terse-lambda
  VERSION 0.1.0
  LANGUAGES CXX
  DESCRIPTION "Terse lambdas for C++"
)

include(is_root)
is_root(is_root_project)

option(BUILD_TESTING "Build tests" ON)

find_package(Boost REQUIRED)

add_library(tl INTERFACE)
add_library(tl::tl ALIAS tl)
target_include_directories(tl
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(tl
  INTERFACE
    Boost::boost
)
target_compile_features(tl
  INTERFACE
    cxx_std_17
)

if(BUILD_TESTING AND "${is_root_project}")
  enable_testing()
  add_subdirectory(tests)
endif()

include(GNUInstallDirs)
set(INSTALL_CONFIGDIR "${CMAKE_INSTALL_LIBDIR}/cmake/tl")

install(TARGETS tl
  EXPORT tl-targets
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
)

install(EXPORT tl-targets
  FILE tl-targets.cmake
  NAMESPACE tl::
  DESTINATION "${INSTALL_CONFIGDIR}"
)

install(DIRECTORY include/
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

include(CMakePackageConfigHelpers)

# Not architecture sensitive
set(CMAKE_SIZEOF_VOID_P)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/tlConfigVersion.cmake"
  VERSION "${PROJECT_VERSION}"
  COMPATIBILITY SameMinorVersion
)

configure_package_config_file(
  cmake/tlConfig.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/tlConfig.cmake"
  INSTALL_DESTINATION "${INSTALL_CONFIGDIR}"
)

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/tlConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/tlConfigVersion.cmake"
  DESTINATION "${INSTALL_CONFIGDIR}"
)
