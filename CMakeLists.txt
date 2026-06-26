cmake_minimum_required(VERSION 3.16)

project(AnimEngine
    VERSION 0.1.0
    DESCRIPTION "A lightweight C++ runtime focused on real-time animation systems"
    LANGUAGES CXX
)

# ---- Global settings ----
# Only set global defaults when built as the top-level project, so these
# do not leak into a parent project that pulls us in via add_subdirectory.
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)

    # Collect build artifacts into predictable locations.
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endif()

# ---- Options ----
option(ANIMENGINE_BUILD_TESTS "Build unit tests" OFF)

# ---- Library target ----
# GLOB is convenient while bootstrapping; switch to an explicit source list
# once the file layout stabilizes.
file(GLOB_RECURSE ANIMENGINE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_library(animengine ${ANIMENGINE_SOURCES})
add_library(AnimEngine::animengine ALIAS animengine)

target_include_directories(animengine
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_features(animengine PUBLIC cxx_std_17)

# Warnings plus a single fixed optimization profile: -O2 with debug symbols.
# This keeps the build fast while remaining debuggable, regardless of any
# CMAKE_BUILD_TYPE the caller might set.
if(MSVC)
    target_compile_options(animengine PRIVATE /W4 /O2 /Zi)
else()
    target_compile_options(animengine PRIVATE -Wall -Wextra -Wpedantic -O2 -g)
endif()

# ---- Subdirectories ----
if(ANIMENGINE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
