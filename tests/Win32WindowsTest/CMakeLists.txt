cmake_minimum_required(VERSION 3.8)

add_executable("Win32WindowsTest" "./main.cpp" "${PROJECT_SOURCE_DIR}/EString_Ansi.cpp" "${PROJECT_SOURCE_DIR}/EString_Streams.cpp")

set_property(TARGET Win32WindowsTest PROPERTY CXX_STANDARD 20)
target_include_directories(Win32WindowsTest PRIVATE ${PROJECT_SOURCE_DIR})
