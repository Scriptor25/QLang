cmake_minimum_required(VERSION 3.20.1)

project(QLang)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

execute_process(
    COMMAND ${LLVM_TOOLS_BINARY_DIR}/llvm-config --libs core orcjit native
    OUTPUT_VARIABLE LLVM_LIBS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

message(STATUS "Using LLVM Libs: ${LLVM_LIBS}")

file(GLOB_RECURSE src "src/*.cpp" "include/*.hpp")
add_library(q ${src})
target_compile_definitions(q PUBLIC ${LLVM_DEFINITIONS})
target_include_directories(q PUBLIC "include" ${LLVM_INCLUDE_DIRS})
target_link_libraries(q PUBLIC ${LLVM_LIBS})

file(GLOB_RECURSE qsrc "q/src/*.cpp" "q/include/*.hpp")
add_executable(qlang ${qsrc})
target_include_directories(qlang PRIVATE "q/include")
target_link_libraries(qlang PRIVATE q)

install(TARGETS q qlang)
