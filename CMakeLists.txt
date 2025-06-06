cmake_minimum_required(VERSION 3.16)
project(RedisClone)

set(CMAKE_CXX_STANDARD 20)

# Enable testing support
include(CTest)
enable_testing()

# Include your include and src dirs
include_directories(include)

add_executable(redis_clone
        src/main.cpp
        src/server.cpp
        src/client.cpp
        src/kvstore.cpp
)


add_subdirectory(third_party/Catch2)

add_executable(tests
        test/test_basic.cpp
        src/kvstore.cpp
)

target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)

# Automatically discover tests using Catch2's CTest integration
include(Catch)
catch_discover_tests(tests)
