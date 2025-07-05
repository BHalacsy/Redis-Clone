cmake_minimum_required(VERSION 3.16)
project(RedisClone)

set(CMAKE_CXX_STANDARD 20)

# Enable testing support
include(CTest)
enable_testing()

# Include your include and src dirs
include_directories(${CMAKE_SOURCE_DIR}/include)

set(BOOST_ROOT "path/to/boost")
set(BOOST_INCLUDEDIR "path/to/boost/include")
set(BOOST_LIBRARYDIR "path/to/boost/lib")
find_package(Boost 1.71 REQUIRED COMPONENTS serialization system)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
else()
    message(FATAL_ERROR "Boost not found. Please check BOOST_ROOT, BOOST_INCLUDEDIR, and BOOST_LIBRARYDIR.")
endif()

add_executable(redis_server
        src/main.cpp
        src/server.cpp
        src/kvstore.cpp
        src/parser.cpp
        src/commands.cpp
)

target_link_libraries(redis_server PRIVATE Boost::serialization Boost::system)

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
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain Boost::serialization) #both catch and boost for tests

include(Catch)
catch_discover_tests(tests)
