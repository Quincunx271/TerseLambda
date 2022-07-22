add_executable(test.tl tl.test.cpp)
target_link_libraries(test.tl
  PRIVATE
    tl::tl
)

add_test(test.tl test.tl)

add_executable(test.gcc-reproducer gcc-reproducer.test.cpp)
target_link_libraries(test.gcc-reproducer
  PRIVATE
    tl::tl
)

add_test(test.gcc-reproducer test.gcc-reproducer)
