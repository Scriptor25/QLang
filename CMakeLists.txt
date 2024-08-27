cmake_minimum_required(VERSION 3.20.1)

project(QLang)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

list(REMOVE_ITEM LLVM_AVAILABLE_LIBS omptarget.rtl.amdgpu omptarget.rtl.cuda omptarget.rtl.x86_64 omptarget LTO Remarks)
message(STATUS "Libraries: ${LLVM_AVAILABLE_LIBS}")

file(GLOB_RECURSE src "src/*.cpp" "include/*.hpp")
add_library(libq ${src})
target_compile_definitions(libq PUBLIC ${LLVM_DEFINITIONS})
target_include_directories(libq PUBLIC "include" ${LLVM_INCLUDE_DIRS})
target_link_libraries(libq PUBLIC ${LLVM_AVAILABLE_LIBS})

file(GLOB_RECURSE qsrc "q/src/*.cpp" "q/include/*.hpp")
add_executable(q ${qsrc})
target_include_directories(q PRIVATE "q/include")
target_link_libraries(q PRIVATE libq)
