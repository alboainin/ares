
cmake_minimum_required(VERSION 3.5)

project("ares" C)

# Set output directories.
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Check if compiler name.
message(${CMAKE_C_COMPILER_ID})

# Set build type definitions.
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-D__DEBUG__)
endif (CMAKE_BUILD_TYPE STREQUAL "Debug")
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_definitions(-D__RELEASE__)
endif (CMAKE_BUILD_TYPE STREQUAL "Release")

# Add project files
file(GLOB_RECURSE ares_hdrs "src/*.h")
file(GLOB_RECURSE ares_srcs "src/*.c")

include_directories("src/")

add_executable(ares ${ares_hdrs} ${ares_srcs})
