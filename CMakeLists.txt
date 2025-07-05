cmake_minimum_required(VERSION 3.16)
project(RedisClone)

set(CMAKE_CXX_STANDARD 20)

# Enable testing support
include(CTest)
enable_testing()

# Include your include src dirs and submods
include_directories(${CMAKE_SOURCE_DIR}/include)
add_subdirectory(third_party/Catch2)

# Manually specify Boost paths
set(BOOST_INCLUDEDIR "/usr/include/boost")
set(BOOST_LIBRARYDIR "/usr/lib")

include_directories(${BOOST_INCLUDEDIR})
link_directories(${BOOST_LIBRARYDIR})

add_executable(redis_server
        src/main.cpp
        src/server.cpp
        src/kvstore.cpp
        src/parser.cpp
        src/commands.cpp
)

target_link_libraries(redis_server PRIVATE boost_serialization boost_system)

add_executable(redis_client
        src/client.cpp
        src/parser.cpp
)

add_executable(tests
        test/test_basic.cpp
        test/test_advanced.cpp
        src/kvstore.cpp
        src/parser.cpp
        src/server.cpp
        src/commands.cpp
)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain boost_serialization)
include(Catch)
catch_discover_tests(tests)