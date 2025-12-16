cmake_minimum_required(VERSION 3.20)

project(lo21-splendor-duel LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
qt_standard_project_setup()

find_program(CLANG_FORMAT clang-format)
if(CLANG_FORMAT)
  add_custom_target(format
    COMMAND ${CLANG_FORMAT} -i
            ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/*.h
  )
endif()

file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_SOURCE_DIR}/src/*.cpp"
  "${CMAKE_SOURCE_DIR}/src/*.h"
)

add_executable(lo21-splendor-duel
  main.cpp
  main.h
  ${PROJECT_SOURCES}
)

target_include_directories(lo21-splendor-duel PRIVATE
  "${CMAKE_SOURCE_DIR}/src"
  "${CMAKE_SOURCE_DIR}/src/Qt/headers"
  "${CMAKE_SOURCE_DIR}/src/core/headers"
)

include(FetchContent)

FetchContent_Declare(nlohmann_json
  URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

FetchContent_Populate(nlohmann_json)

add_library(nlohmann_json INTERFACE)
add_library(nlohmann_json::nlohmann_json ALIAS nlohmann_json)
target_include_directories(nlohmann_json INTERFACE
  "${nlohmann_json_SOURCE_DIR}/include"
)

target_link_libraries(lo21-splendor-duel PRIVATE
  Qt6::Widgets
  Qt6::Core
  nlohmann_json::nlohmann_json
)
