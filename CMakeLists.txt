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

set(BOOST_SERIALIZATION_LIB "/usr/lib/x86_64-linux-gnu/libboost_serialization.so.1.83.0")
set(BOOST_SYSTEM_LIB "/usr/lib/x86_64-linux-gnu/libboost_system.so.1.83.0")
set(TBB_LIB "/usr/lib/x86_64-linux-gnu/libtbb.so.12")

add_executable(redis_server
        src/main.cpp
        src/server.cpp
        src/kvstore.cpp
        src/parser.cpp
        src/commands.cpp
        src/snapshot.cpp
        src/expire.cpp
)

target_link_libraries(redis_server PRIVATE ${BOOST_SERIALIZATION_LIB} ${BOOST_SYSTEM_LIB} ${TBB_LIB})

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
        src/snapshot.cpp
        src/expire.cpp
)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain ${BOOST_SERIALIZATION_LIB} ${TBB_LIB})
include(Catch)
catch_discover_tests(tests)