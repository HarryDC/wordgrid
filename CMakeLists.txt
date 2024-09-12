cmake_minimum_required(VERSION 3.12)
project(WordGrid)
include(FetchContent)

# Generate compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Dependencies
set(RAYLIB_VERSION 5.0)

FetchContent_Declare(
    raylib
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    URL https://github.com/raysan5/raylib/archive/ed702673ea0ff49da2ca494686814376ad040941.zip # 9/12/2024
    FIND_PACKAGE_ARGS
)

FetchContent_Declare(
    raygui
    DOWNLOAD_EXTRACT_TIMESTAMP OFF
    GIT_REPOSITORY https://github.com/raysan5/raygui.git
    SOURCE_SUBDIR projects/CMake
    FIND_PACKAGE_ARGS
)

FetchContent_Declare(
    unity
    GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
    FIND_PACKAGE_ARGS
)

FetchContent_MakeAvailable(raylib raygui unity)

add_subdirectory(src)

if (NOT "${PLATFORM}" STREQUAL "Web")
    add_subdirectory(test)
endif()

