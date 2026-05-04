cmake_minimum_required(VERSION 3.20)

project(NativeFileDialog C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

add_library(NativeFileDialog STATIC
  src/nfd_common.c
)

if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_sources(NativeFileDialog PRIVATE src/nfd_gtk.c)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_sources(NativeFileDialog PRIVATE src/nfd_win.cpp)
else()
    message(FATAL_ERROR "Platform is not supported yet")
endif()

target_include_directories(NativeFileDialog PUBLIC src/include)
