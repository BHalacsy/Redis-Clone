cmake_minimum_required(VERSION 3.16)
project(redis_clone)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directory
include_directories(include)


add_executable(redis_server
        src/main.cpp
        src/client.cpp
        src/server.cpp
        src/kvstore.cpp
)


# Testing Setup
enable_testing()

# Fetch Catch2 (header-only)
include(FetchContent)
FetchContent_Declare(
        catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.5.4 # Or the latest stable
)
FetchContent_MakeAvailable(catch2)

# Test binary
add_executable(tests
        test/test_basic.cpp
        src/kvstore.cpp
)

target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)

# Register with ctest
include(CTest)
include(Catch)
catch_discover_tests(tests)
