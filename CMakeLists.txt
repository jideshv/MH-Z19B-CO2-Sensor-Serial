# Minimum required for FetchContent is 3.11
cmake_minimum_required(VERSION 3.12)

project(MH-Z19B)

# Make sure C++ 11 compiler is used
set (CMAKE_CXX_STANDARD 11)

# Get the 1.8.0 release of GTest
include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.8.0
)

FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
  FetchContent_Populate(googletest)
  add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
endif()

enable_testing()

add_subdirectory(src)
add_subdirectory(tests)
