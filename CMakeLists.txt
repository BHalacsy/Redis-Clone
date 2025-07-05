cmake_minimum_required(VERSION 3.16)
project(RedisClone)

set(CMAKE_CXX_STANDARD 20)

# Enable testing support
include(CTest)
enable_testing()

# Include your include and src dirs
include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(redis_server
        src/main.cpp
        src/server.cpp
        src/kvstore.cpp
        src/parser.cpp
        src/commands.cpp
)

add_executable(redis_client
        src/client.cpp
        src/parser.cpp
)

add_subdirectory(third_party/Catch2)

add_executable(tests
        test/test_basic.cpp
        test/test_advanced.cpp
        src/kvstore.cpp
        src/parser.cpp
        src/server.cpp
        src/commands.cpp
)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)

# Automatically discover tests using Catch2's CTest integration
include(Catch)
catch_discover_tests(tests)
