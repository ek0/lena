cmake_minimum_required(VERSION 3.12)
include(GenerateExportHeader) 

project(lena VERSION 0.1)

# Putting binaries in the output folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${PROJECT_BINARY_DIR}/bin)

# General poject settings
option(LENA_BUILD_SHARED_LIB "Build shared library" OFF)

if(LENA_BUILD_SHARED_LIB)
    add_library("lena" SHARED)
else()
    add_library("lena" STATIC)
endif()

# -----------------------------------------------------------------------------
# Generating configuration macros
# -----------------------------------------------------------------------------
generate_export_header("lena" BASE_NAME "LENA" 
                              EXPORT_FILE_NAME "lena_export_config.h")
target_include_directories("lena"
    PUBLIC "include" ${PROJECT_BINARY_DIR})

target_sources("lena" PRIVATE "${CMAKE_CURRENT_LIST_DIR}/src/lena.cpp"
                              "${CMAKE_CURRENT_LIST_DIR}/src/lenamanager.cpp"
                              "${CMAKE_CURRENT_LIST_DIR}/src/lenathreadtracer.cpp")
add_executable("test_lena" "${CMAKE_CURRENT_LIST_DIR}/tests/test_lena.cpp")
target_include_directories("test_lena" PRIVATE "${CMAKE_CURRENT_LIST_DIR}/include")
target_link_libraries("test_lena" PUBLIC "lena")