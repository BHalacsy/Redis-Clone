cmake_minimum_required(VERSION 3.16)
project(RedisClone)

set(CMAKE_CXX_STANDARD 20)

# Declare this early so it's available before anything depends on it
option(ENABLE_TESTING "Enable testing" ON)

include_directories(include)

add_executable(redis_clone
        src/main.cpp
        src/server.cpp
        src/client.cpp
        src/kvstore.cpp
)

add_executable(tests
        test/test_basic.cpp
        src/kvstore.cpp
)

if(ENABLE_TESTING)
    include(CTest)
    enable_testing()

    add_subdirectory(third_party/Catch2)
    target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
    include(Catch)
    catch_discover_tests(tests)
endif()
