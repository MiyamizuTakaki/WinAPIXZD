cmake_minimum_required(VERSION 3.24)
project(test1)

set(CMAKE_CXX_STANDARD 17)

add_executable(zd1 z1.cpp z3.cpp)
add_executable(zd2 z2.cpp resource.h)
add_executable(zd3 z3.cpp)
target_link_libraries(zd2 comdlg32)
