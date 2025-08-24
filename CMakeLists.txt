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


# --- Sample of clang libtooling ---
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

# Example: generate metadata
set(DEVICE_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/lib_device.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/lib2_device.cpp
)
add_library(sample__device SHARED ${DEVICE_SRCS})

set(META_SRCS "")
foreach(SRC_FILE IN LISTS DEVICE_SRCS)
    get_filename_component(SRC_NAME "${SRC_FILE}" NAME_WE)
    set(META_FILE "${CMAKE_CURRENT_BINARY_DIR}/${SRC_NAME}.meta.cpp")
    list(APPEND META_SRCS "${META_FILE}")

    add_custom_command(OUTPUT ${META_FILE}
        COMMAND $<TARGET_FILE:my-annotation-matcher> ${SRC_FILE} -out ${META_FILE} -p ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating Meta data... (target: ${SRC_FILE})"
    )
endforeach()
add_custom_target(sample__generate_metadata
    DEPENDS ${META_SRCS}
)

add_library(sample__meta SHARED ${META_SRCS})
add_custom_command(TARGET sample__meta
    POST_BUILD
    COMMAND objcopy
        --input binary
        --output elf64-x86-64
        --rename-section .data=.sample.meta,alloc,load,readonly,data,contents
        --redefine-sym _binary_$<MAKE_C_IDENTIFIER:$<TARGET_FILE:sample__meta>>_start=sample_metadata_start
        --redefine-sym _binary_$<MAKE_C_IDENTIFIER:$<TARGET_FILE:sample__meta>>_end=sample_metadata_end
        --redefine-sym _binary_$<MAKE_C_IDENTIFIER:$<TARGET_FILE:sample__meta>>_size=sample_metadata_size
        $<TARGET_FILE:sample__meta> $<TARGET_FILE:sample__meta>.o
    COMMAND objcopy
        --add-section .note.GNU-stack=/dev/null
        --set-section-flags .note.GNU-stack=contents
        $<TARGET_FILE:sample__meta>.o $<TARGET_FILE:sample__meta>.o
)

set(HOST_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/main_host.cpp
)
add_executable(sample__host ${HOST_SRCS})
target_link_libraries(sample__host
    PRIVATE
        $<TARGET_FILE:sample__meta>.o
)
add_dependencies(sample__host
    sample__meta
)