cmake_minimum_required(VERSION 3.20.1)

project(QLang VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(LLVM 18.1 REQUIRED CONFIG)

message(STATUS "LLVM version ${LLVM_PACKAGE_VERSION}")
message(STATUS "LLVM config ${LLVM_DIR}")

execute_process(
        COMMAND ${LLVM_TOOLS_BINARY_DIR}/llvm-config --libs
        OUTPUT_VARIABLE LLVM_LIBS
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(LLVM_LIBS "${LLVM_LIBS} -lws2_32")
endif ()

message(STATUS "LLVM libraries ${LLVM_LIBS}")
message(STATUS "LLVM definitions ${LLVM_DEFINITIONS}")

file(GLOB_RECURSE src "src/*.cpp" "include/*.hpp")
add_library(q ${src})
target_compile_definitions(q PUBLIC ${LLVM_DEFINITIONS})
target_include_directories(q PUBLIC "include" ${LLVM_INCLUDE_DIRS})
target_link_directories(q PUBLIC ${LLVM_LIBRARY_DIR})
target_link_libraries(q PUBLIC ${LLVM_LIBS})

file(GLOB_RECURSE qsrc "q/src/*.cpp" "q/include/*.hpp")
add_executable(qlang ${qsrc})
target_include_directories(qlang PRIVATE "q/include")
target_link_libraries(qlang PRIVATE q)

if (CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    add_custom_command(TARGET qlang POST_BUILD
            COMMAND ${CMAKE_OBJCOPY} --only-keep-debug $<TARGET_FILE:qlang> $<TARGET_FILE:qlang>.dbg
            COMMAND ${CMAKE_OBJCOPY} --strip-debug $<TARGET_FILE:qlang>
            COMMAND ${CMAKE_OBJCOPY} --add-gnu-debuglink=$<TARGET_FILE:qlang>.dbg $<TARGET_FILE:qlang>
    )
endif ()

install(TARGETS q qlang)

file(GLOB std_headers "example/std/*.qh")
install(FILES ${std_headers} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/q)
