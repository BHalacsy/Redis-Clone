cmake_minimum_required(VERSION 3.16)
project(redis_clone)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

set(SRC_FILES
        src/main.cpp
        src/client.cpp
        src/server.cpp
        src/kvstore.cpp
)

# Build main Redis server
add_executable(redis_server ${SRC_FILES})



enable_testing()

# Add Catch2 (header-only - place it in test/catch.hpp or use FetchContent)
include(FetchContent)
FetchContent_Declare(
        catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.5.4 # Latest as of June 2025
)
FetchContent_MakeAvailable(catch2)

add_executable(tests
        test/test_basic.cpp
        src/kvstore.cpp
)

target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
