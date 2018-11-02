add_executable(test.tl tl.test.cpp)
target_link_libraries(test.tl
  PRIVATE
    tl::tl
)

add_test(test.tl test.tl)
