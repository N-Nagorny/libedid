cmake_minimum_required (VERSION 3.0)

cmake_policy(SET CMP0048 NEW)
project (edid VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ENABLE_JSON ON CACHE BOOL "Enable EDID <-> JSON and EDID -> NMOS Receiver Capabilities conversion functions")
set(BUILD_TESTS OFF CACHE BOOL "Enable building test applications")
set(RUN_TESTS OFF CACHE BOOL "Enable running test applications as a part of the building procedure")
set(LINUXHW_EDID_DIR "${CMAKE_SOURCE_DIR}/linuxhw_edid" CACHE PATH "Path to linuxhw/EDID GitHub repo")
set(BUILD_EXAMPLES OFF CACHE BOOL "Enable example applications")
set(BUILD_SHARED_LIBS ON CACHE BOOL "Build using shared libraries")

# JSON schemas library
if (ENABLE_JSON)
    set(JSON_SCHEMAS_HEADERS
        include/edid/json_schemas.hh
    )

    set(JSON_SCHEMAS_BASE_BLOCK_FOLDER base_block)
    set(JSON_SCHEMAS_CTA861_BLOCK_FOLDER cta861_block)
    set(JSON_SCHEMAS_COMMON_FOLDER common)

    set(JSON_SCHEMAS_FILES
        schemas/${JSON_SCHEMAS_COMMON_FOLDER}/ascii_string.json
        schemas/${JSON_SCHEMAS_COMMON_FOLDER}/detailed_timing_descriptor.json
        schemas/${JSON_SCHEMAS_COMMON_FOLDER}/display_range_limits.json
        schemas/${JSON_SCHEMAS_COMMON_FOLDER}/established_timings_3.json
        schemas/${JSON_SCHEMAS_COMMON_FOLDER}/edid.json

        schemas/${JSON_SCHEMAS_CTA861_BLOCK_FOLDER}/audio_data_block.json
        schemas/${JSON_SCHEMAS_CTA861_BLOCK_FOLDER}/colorimetry_data_block.json
        schemas/${JSON_SCHEMAS_CTA861_BLOCK_FOLDER}/cta861_block.json
        schemas/${JSON_SCHEMAS_CTA861_BLOCK_FOLDER}/hdmi_vsdb.json
        schemas/${JSON_SCHEMAS_CTA861_BLOCK_FOLDER}/hdr_static_metadata_data_block.json
        schemas/${JSON_SCHEMAS_CTA861_BLOCK_FOLDER}/speaker_allocation_data_block.json
        schemas/${JSON_SCHEMAS_CTA861_BLOCK_FOLDER}/unknown_data_block.json
        schemas/${JSON_SCHEMAS_CTA861_BLOCK_FOLDER}/video_capability_data_block.json
        schemas/${JSON_SCHEMAS_CTA861_BLOCK_FOLDER}/video_data_block.json
        schemas/${JSON_SCHEMAS_CTA861_BLOCK_FOLDER}/ycbcr420_capability_map_data_block.json

        schemas/${JSON_SCHEMAS_BASE_BLOCK_FOLDER}/base_block.json
        schemas/${JSON_SCHEMAS_BASE_BLOCK_FOLDER}/standard_timing.json
    )

    set_property(
        DIRECTORY
        APPEND
        PROPERTY CMAKE_CONFIGURE_DEPENDS
        ${JSON_SCHEMAS_FILES}
    )

    set(JSON_SCHEMAS_PATH_REGEX "schemas/([^/]+)/([^;]+)\\.json")
    set(JSON_SCHEMAS_SOURCE_REPLACE "${CMAKE_CURRENT_BINARY_DIR}/src/\\1/\\2.cc")
    string(REGEX REPLACE "${JSON_SCHEMAS_PATH_REGEX}(;|$)" "${JSON_SCHEMAS_SOURCE_REPLACE}\\3" JSON_SCHEMAS_SOURCES "${JSON_SCHEMAS_FILES}")

    foreach(JSON ${JSON_SCHEMAS_FILES})
        string(REGEX REPLACE "${JSON_SCHEMAS_PATH_REGEX}" "${JSON_SCHEMAS_SOURCE_REPLACE}" SOURCE "${JSON}")
        string(REGEX REPLACE "${JSON_SCHEMAS_PATH_REGEX}" "\\1" NS "${JSON}")
        string(REGEX REPLACE "${JSON_SCHEMAS_PATH_REGEX}" "\\2" VAR "${JSON}")
        string(MAKE_C_IDENTIFIER "${NS}" NS)
        string(MAKE_C_IDENTIFIER "${VAR}" VAR)

        file(WRITE "${SOURCE}.in" "\
// Auto-generated from: ${JSON}\n\
\n\
namespace Edid {\n\
namespace json_schemas {\n\
namespace ${NS} {\n\
const char* ${VAR} = R\"-auto-generated-("
        )

        file(READ "${JSON}" RAW)
        file(APPEND "${SOURCE}.in" "${RAW}")

        file(APPEND "${SOURCE}.in" ")-auto-generated-\";\n\
}  // ${NS}\n\
}  // json_schemas\n\
}  // Edid\n"
        )

        configure_file("${SOURCE}.in" "${SOURCE}" COPYONLY)
    endforeach()

    list(APPEND JSON_SCHEMAS_SOURCES src/json_schemas.cc)

    add_library(
        json_schemas STATIC
        ${JSON_SCHEMAS_HEADERS}
        ${JSON_SCHEMAS_SOURCES}
    )

    target_include_directories(json_schemas PRIVATE
        include/
    )

    set_target_properties(json_schemas PROPERTIES
        POSITION_INDEPENDENT_CODE ${BUILD_SHARED_LIBS}
    )
endif()

# libedid

set(SOURCES
    src/base_block.cc
    src/common.cc
    src/cta_data_block.cc
    src/cta861_block.cc
    src/dtd.cc
    src/edid.cc
    src/hdmi_vendor_data_block.cc
    src/timing_modes.cc
)

if (ENABLE_JSON)
    find_package(nlohmann_json 3.2.0 REQUIRED)
    find_package(nlohmann_json_schema_validator REQUIRED)
    list(APPEND SOURCES src/bcp00501.cc)
    list(APPEND SOURCES src/json.cc)
endif()

set(HEADERS
    include/
)

add_library(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC
    ${HEADERS}
)

if (ENABLE_JSON)
    target_compile_definitions(${PROJECT_NAME} PRIVATE ENABLE_JSON)
    target_link_libraries(${PROJECT_NAME} PUBLIC
        json_schemas
        nlohmann_json_schema_validator
        nlohmann_json::nlohmann_json
    )
endif()

set_target_properties(
  ${PROJECT_NAME}
  PROPERTIES
  VERSION ${PROJECT_VERSION}
  SOVERSION ${PROJECT_VERSION_MAJOR}
)

install(DIRECTORY include/ DESTINATION include)
install(TARGETS ${PROJECT_NAME} DESTINATION lib)

# ---------- Test applications ----------

if (BUILD_TESTS)
    include(GoogleTest)

    # ---------- test-runner ----------

    set(TEST_HEADERS
        test/include/
    )

    set(TEST_SOURCES
        test/src/common.cc
        test/src/hdmi_vendor_data_block.cc
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

    if(RUN_TESTS)
        add_custom_command(TARGET test-runner POST_BUILD
            COMMAND ${CMAKE_BINARY_DIR}/test-runner
        )
    endif()

    install(TARGETS test-runner DESTINATION bin)

    # ---------- roundtrip-tester ----------

    add_executable(roundtrip-tester test/src/main_roundtrip.cc)

    target_link_libraries(roundtrip-tester PRIVATE
        ${PROJECT_NAME}
        gtest
    )

    if(RUN_TESTS)
        add_custom_command(TARGET roundtrip-tester POST_BUILD
            COMMAND ${CMAKE_SOURCE_DIR}/scripts/linuxhw_coverage.sh
            ARGS ${CMAKE_BINARY_DIR}/roundtrip-tester 41017 ${LINUXHW_EDID_DIR} ${CMAKE_BINARY_DIR}/test_result
        )
    endif()

    install(TARGETS roundtrip-tester DESTINATION bin)

    # ---------- json-roundtrip-tester ----------

    if (ENABLE_JSON)
        add_executable(json-roundtrip-tester test/src/main_json_roundtrip.cc)

        target_link_libraries(json-roundtrip-tester PRIVATE
            ${PROJECT_NAME}
            gtest
        )

        if(RUN_TESTS)
            add_custom_command(TARGET json-roundtrip-tester POST_BUILD
                COMMAND ${CMAKE_SOURCE_DIR}/scripts/linuxhw_coverage.sh
                ARGS ${CMAKE_BINARY_DIR}/json-roundtrip-tester 37020 ${LINUXHW_EDID_DIR} ${CMAKE_BINARY_DIR}/test_result_json
            )
        endif()

        install(TARGETS json-roundtrip-tester DESTINATION bin)
    endif()
endif()

# Example applications

if (BUILD_EXAMPLES)
    set(WORKSHOP_SOURCES
        examples/edid-workshop/src/main.cc
    )

    add_executable(edid-workshop ${WORKSHOP_SOURCES})

    target_link_libraries(edid-workshop PRIVATE
        ${PROJECT_NAME}
    )

    install(TARGETS edid-workshop DESTINATION bin)
endif()
