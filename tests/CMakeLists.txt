cmake_minimum_required(VERSION 3.8)

# Configure GTest
include(FetchContent)
FetchContent_Declare(
  googletest
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
  URL "https://github.com/google/googletest/archive/f345b2ca6adb1b505049190867eedf24d3b5eaa3.zip"
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

include(GoogleTest)

add_executable(
  EStringTests

  "StringModifyingTests.cpp"
  "EncodingsTests.cpp"
  "OperatorsTests.cpp"
  "ChecksTests.cpp"
  "InitializingTests.cpp"
  
  "${PROJECT_SOURCE_DIR}/EString.cpp"
)

set_property(TARGET EStringTests PROPERTY CXX_STANDARD 20)

target_include_directories(EStringTests PRIVATE ${PROJECT_SOURCE_DIR})
target_link_libraries(EStringTests PRIVATE GTest::gtest_main)

gtest_discover_tests(EStringTests)
