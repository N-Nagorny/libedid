cmake_minimum_required (VERSION 2.8)
project (edid LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ENABLE_BCP00501 ON CACHE BOOL "Enable EDID to NMOS Receiver Capabilities conversion functions")

set(SOURCES
    src/base_block.cc
    src/common.cc
    src/cta861_block.cc
    src/edid.cc
    src/timing_modes.cc
)

if (ENABLE_BCP00501)
    find_package(nlohmann_json 3.2.0 REQUIRED)
    list(APPEND SOURCES src/bcp00501.cc)
endif()

set(HEADERS
    include/
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC
    ${HEADERS}
)

if (ENABLE_BCP00501)
    target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)
endif()

install(DIRECTORY include/ DESTINATION include)

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
