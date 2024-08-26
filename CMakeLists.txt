cmake_minimum_required(VERSION 3.20.1)

project(QLang)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE src "src/*.cpp" "include/*.hpp")
add_library(libq ${src})
target_include_directories(libq PUBLIC "include")

file(GLOB_RECURSE qsrc "q/src/*.cpp" "q/include/*.hpp")
add_executable(q ${qsrc})
target_include_directories(q PRIVATE "q/include")
target_link_libraries(q PRIVATE libq)
