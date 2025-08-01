cmake_minimum_required(VERSION 3.16)
project(RedisClone)

set(CMAKE_CXX_STANDARD 20)

# Enable testing support
include(CTest)
enable_testing()

# Include your include src dirs and submods
include_directories(${CMAKE_SOURCE_DIR}/include)

add_subdirectory(third_party/Catch2)

# Add Boost and TBB
set(BOOST_INCLUDEDIR "/usr/include")
include_directories(${BOOST_INCLUDEDIR})
set(TBB_INCLUDEDIR "/usr/include")
include_directories(${TBB_INCLUDEDIR})





add_executable(redis_server
        src/main.cpp
        src/server.cpp
        src/kvstore.cpp
        src/parser.cpp
        src/commands.cpp
        src/snapshot.cpp
        src/expire.cpp
        src/pubsub.cpp
)


target_link_libraries(redis_server PRIVATE boost_serialization boost_system tbb pthread)


add_executable(redis_client
        src/client.cpp
        src/parser.cpp
)

option(BUILD_TESTS "Build tests" ON)
if(BUILD_TESTS)
    if(EXISTS "${CMAKE_SOURCE_DIR}/test/test_basic.cpp")
        add_executable(tests
                test/test_basic.cpp
                test/test_string.cpp
                test/test_expiration.cpp
                test/test_list.cpp
                test/test_set.cpp
                test/test_hash.cpp
                test/test_misc.cpp
                test/test_persistence.cpp
                src/kvstore.cpp
                src/parser.cpp
                src/server.cpp
                src/commands.cpp
                src/snapshot.cpp
                src/expire.cpp
                src/pubsub.cpp
        )
        target_link_libraries(tests PRIVATE Catch2::Catch2WithMain boost_serialization boost_system tbb pthread)
        include(Catch)
        catch_discover_tests(tests)
    endif()
endif()
