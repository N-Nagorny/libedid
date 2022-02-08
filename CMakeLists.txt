cmake_minimum_required (VERSION 2.8)
project (edid LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    src/base_block.cc
    src/common.cc
    src/cta861_block.cc
    src/edid.cc
)

set(HEADERS
    include/
)

file(GLOB PUBLIC_HEADERS
    "include/*.hh"
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC
    ${HEADERS}
)

set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${PUBLIC_HEADERS}")

install(TARGETS ${PROJECT_NAME} PUBLIC_HEADER)

# Test application

include(GoogleTest)

set(TEST_SOURCES
    test/src/test_runner.cc
)

add_executable(test_runner ${TEST_SOURCES})

target_link_libraries(test_runner PRIVATE
    ${PROJECT_NAME}
    gtest
)
