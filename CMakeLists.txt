cmake_minimum_required(VERSION 3.20)
project(chatserver LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include paths
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(/usr/local/include)

# Find dependencies
find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)

# Enable tests and FetchContent for GoogleTest
enable_testing()
include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Optional: enable AddressSanitizer for Debug builds
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  message(STATUS "Debug build: enabling AddressSanitizer")
  add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
  add_link_options(-fsanitize=address)
endif()

# Collect sources
file(GLOB_RECURSE CHATSERVER_SOURCES
    ${CMAKE_SOURCE_DIR}/src/chatserver/*.cpp
)

# Exclude old server files if present
list(REMOVE_ITEM CHATSERVER_SOURCES
    ${CMAKE_SOURCE_DIR}/src/chatserver/transport/server/server.cpp
    ${CMAKE_SOURCE_DIR}/src/chatserver/transport/server/request_router.cpp
    ${CMAKE_SOURCE_DIR}/src/chatserver/transport/server/response_mapper.cpp
)

# Static library target
add_library(chatserver STATIC
    ${CHATSERVER_SOURCES}
)

# Make PIC for safety
set_target_properties(chatserver PROPERTIES POSITION_INDEPENDENT_CODE ON)

# Public link so consumers inherit OpenSSL and Threads
target_link_libraries(chatserver
    PUBLIC
        OpenSSL::SSL
        OpenSSL::Crypto
        Threads::Threads
)

# Server executable
add_executable(server
    src/main.cpp
)

target_link_libraries(server
    PRIVATE
        chatserver
        OpenSSL::SSL
        OpenSSL::Crypto
        /usr/local/lib/libpqxx.a
        pq
        pthread
        dl
)

# -------------------------
# GoogleTest targets
# -------------------------

# Smoke test
add_executable(test_gtest
    tests/gtest_example.cpp
)
target_include_directories(test_gtest PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(test_gtest PRIVATE chatserver GTest::gtest_main)
add_test(NAME unit_tests COMMAND test_gtest)

# Password hasher unit test
add_executable(password_hasher_test
    tests/password_hasher_test.cpp
)
target_include_directories(password_hasher_test PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(password_hasher_test
    PRIVATE
        chatserver
        GTest::gtest_main
        OpenSSL::SSL
        OpenSSL::Crypto
)
add_test(NAME password_hasher_test COMMAND password_hasher_test)

# Resource lifetime test (ensures shared_ptr capture keeps resource alive)
add_executable(resource_lifetime_test
    tests/resource_lifetime_test.cpp
)
target_include_directories(resource_lifetime_test PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(resource_lifetime_test
    PRIVATE
        chatserver
        GTest::gtest_main
)
add_test(NAME resource_lifetime_test COMMAND resource_lifetime_test)

# Death test to detect dangling captures (may be nondeterministic)
add_executable(detect_dangling_resource_test
    tests/detect_dangling_resource_test.cpp
)
target_include_directories(detect_dangling_resource_test PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(detect_dangling_resource_test
    PRIVATE
        chatserver
        GTest::gtest_main
)
add_test(NAME detect_dangling_resource_test COMMAND detect_dangling_resource_test)

message(STATUS "ChatServer build configured")

