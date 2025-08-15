cmake_minimum_required(VERSION 3.20)

set(CMAKE_C_COMPILER             "/usr/bin/clang")
set(CMAKE_C_FLAGS                "-Wall -std=c99")
set(CMAKE_C_FLAGS_DEBUG          "-O0 -g")
set(CMAKE_C_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
set(CMAKE_C_FLAGS_RELEASE        "-O4 -DNDEBUG")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(clang-tooling-sample)

find_package( Clang REQUIRED CONFIG )



include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${CLANG_INCLUDE_DIRS})
add_executable(my-clang-check
    src/ClangCheck.cpp
)
target_link_libraries(my-clang-check
    PRIVATE
    clangAST
    clangFrontend
    clangTooling
)

add_executable(my-annotation-matcher
    src/AnnotationMatcher.cpp
)
target_link_libraries(my-annotation-matcher
    PRIVATE
    clangAST
    clangFrontend
    clangTooling
)

add_executable(sample main.cpp lib.cpp)