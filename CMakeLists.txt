cmake_minimum_required (VERSION 2.8)
project (edid LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ENABLE_JSON ON CACHE BOOL "Enable EDID <-> JSON and EDID -> NMOS Receiver Capabilities conversion functions")

set(SOURCES
    src/base_block.cc
    src/common.cc
    src/cta_data_block.cc
    src/cta861_block.cc
    src/edid.cc
    src/timing_modes.cc
)

if (ENABLE_JSON)
    find_package(nlohmann_json 3.2.0 REQUIRED)
    list(APPEND SOURCES src/bcp00501.cc)
    list(APPEND SOURCES src/json.cc)
endif()

set(HEADERS
    include/
)

add_library(${PROJECT_NAME} STATIC ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC
    ${HEADERS}
)

if (ENABLE_JSON)
    target_compile_definitions(${PROJECT_NAME} PRIVATE ENABLE_JSON)
    target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)
endif()

install(DIRECTORY include/ DESTINATION include)
install(TARGETS ${PROJECT_NAME} DESTINATION lib)

# Test application

include(GoogleTest)

set(TEST_HEADERS
    test/include/
)

set(TEST_SOURCES
    test/src/common.cc
    test/src/test_runner.cc
)

if (ENABLE_JSON)
    list(APPEND TEST_SOURCES test/src/bcp00501_tests.cc)
    list(APPEND TEST_SOURCES test/src/json_tests.cc)
endif()

add_executable(test-runner ${TEST_SOURCES})

target_include_directories(test-runner PUBLIC
    ${TEST_HEADERS}
)

if (ENABLE_JSON)
    target_compile_definitions(test-runner PRIVATE ENABLE_JSON)
endif()

target_link_libraries(test-runner PRIVATE
    ${PROJECT_NAME}
    gtest
)

# Example applications

set(WORKSHOP_SOURCES
    examples/edid-workshop/src/main.cc
)

add_executable(edid-workshop ${WORKSHOP_SOURCES})

target_link_libraries(edid-workshop PRIVATE
    ${PROJECT_NAME}
)
