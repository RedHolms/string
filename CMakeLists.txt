cmake_minimum_required(VERSION 3.8)

project("string")

add_executable("string" "test.cpp")

set_property(TARGET "string" PROPERTY CXX_STANDARD 17)