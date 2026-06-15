# SpeltEngine — static engine library with vendored dependencies
cmake_minimum_required(VERSION 3.15)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(PROJECT_NAME_VAR SpeltEngine)
project(${PROJECT_NAME_VAR} VERSION 1.0)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# ---------------------------------------------------------------------------
# Options & Context Checks
# ---------------------------------------------------------------------------
set(SPELT_IS_ROOT_PROJECT FALSE)
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(SPELT_IS_ROOT_PROJECT TRUE)
endif()

option(SPELT_VENDORED_DEPS
        "Use vendored copies of GLFW/Assimp (disable if consuming project provides them)"
        ON)

option(SPELT_BUILD_TESTS
        "Build SpeltEngine tests"
        ${SPELT_IS_ROOT_PROJECT})

option(SPELT_BUILD_TEST_SCENE
        "Build the interactive SpeltEngine test scene sandbox"
        ${SPELT_IS_ROOT_PROJECT})

# ---------------------------------------------------------------------------
# System packages
# ---------------------------------------------------------------------------
find_package(OpenGL REQUIRED)

# ---------------------------------------------------------------------------
# Local Dependencies (Vendored via external/)
# ---------------------------------------------------------------------------

# 1. Catch2 (Only required if building engine tests)
if(SPELT_BUILD_TESTS AND NOT TARGET Catch2::Catch2WithMain)
    add_subdirectory(external/catch2)
endif()

# 2. GLFW + Assimp — vendored or system-provided
if(SPELT_VENDORED_DEPS)
    if(NOT TARGET glfw)
        set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
        set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
        set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
        set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)
        add_subdirectory(external/glfw)
    endif()

    if(NOT TARGET assimp)
        set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)
        set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
        set(ASSIMP_BUILD_TESTS        OFF CACHE BOOL "" FORCE)
        set(ASSIMP_INSTALL            OFF CACHE BOOL "" FORCE)
        set(ASSIMP_BUILD_ZLIB         ON  CACHE BOOL "" FORCE)
        add_subdirectory(external/assimp)
    endif()
else()
    find_package(glfw3 REQUIRED)
    find_package(assimp REQUIRED)
endif()

# 3. STB (Header-only target creation)
if(NOT TARGET stb_image)
    add_library(stb_image INTERFACE)
    target_include_directories(stb_image INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/external/stb)
endif()

# ---------------------------------------------------------------------------
# Automatically Gather Source and Include Files
# ---------------------------------------------------------------------------
# Collects all source files (.cpp, .c) and header files (.h, .hpp) recursively
file(GLOB_RECURSE ENGINE_SOURCES
    "src/*.cpp"
    "src/*.c"
)

file(GLOB_RECURSE ENGINE_HEADERS
    "include/*.h"
    "include/*.hpp"
)

# Keep the manually configured glad components separate as per original structure
set(GLAD_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/glad/include")
set(GLAD_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/glad/src/glad.c")

# ---------------------------------------------------------------------------
# Static library target
# ---------------------------------------------------------------------------
add_library(${PROJECT_NAME_VAR} STATIC
        ${GLAD_SOURCES}
        ${ENGINE_SOURCES}
        ${ENGINE_HEADERS}
)

target_compile_features(${PROJECT_NAME_VAR} PUBLIC cxx_std_20)
set_target_properties(${PROJECT_NAME_VAR} PROPERTIES CXX_EXTENSIONS OFF)

target_compile_definitions(${PROJECT_NAME_VAR} PUBLIC
        GLM_ENABLE_EXPERIMENTAL
        STB_IMAGE_IMPLEMENTATION
        _USE_MATH_DEFINES
)

# ---------------------------------------------------------------------------
# Include paths
# ---------------------------------------------------------------------------
target_include_directories(${PROJECT_NAME_VAR} PUBLIC
        $<BUILD_INTERFACE:${GLAD_INCLUDE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/external> # Gives access to external/glm
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME_VAR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME_VAR}/glad>
)

# ---------------------------------------------------------------------------
# Link Libraries
# ---------------------------------------------------------------------------
target_link_libraries(${PROJECT_NAME_VAR}
        PUBLIC  OpenGL::GL
        PRIVATE $<BUILD_INTERFACE:glfw>
        $<BUILD_INTERFACE:assimp>
        $<BUILD_INTERFACE:stb_image>
)

# Linux requires Threads + X11
if(UNIX AND NOT APPLE)
    find_package(Threads REQUIRED)
    target_link_libraries(${PROJECT_NAME_VAR} PRIVATE Threads::Threads X11)
endif()

# ---------------------------------------------------------------------------
# Install Infrastructure (Skipped if used as a sub-project elsewhere)
# ---------------------------------------------------------------------------
if(SPELT_IS_ROOT_PROJECT)
    install(TARGETS ${PROJECT_NAME_VAR}
            EXPORT  ${PROJECT_NAME_VAR}Targets
            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

    install(DIRECTORY include/
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME_VAR}
    )
    install(DIRECTORY ${GLAD_INCLUDE_DIR}/
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME_VAR}/glad
    )
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/external/glm
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME_VAR}
    )

    set(${PROJECT_NAME_VAR}_CMAKE_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME_VAR}")

    install(EXPORT ${PROJECT_NAME_VAR}Targets
            FILE        ${PROJECT_NAME_VAR}Targets.cmake
            NAMESPACE   ${PROJECT_NAME_VAR}::
            DESTINATION ${${PROJECT_NAME_VAR}_CMAKE_DIR}
    )

    write_basic_package_version_file(
            "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME_VAR}ConfigVersion.cmake"
            VERSION       ${PROJECT_VERSION}
            COMPATIBILITY SameMajorVersion
    )

    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME_VAR}Config.cmake"
            "include(CMakeFindDependencyMacro)\n"
            "find_dependency(OpenGL REQUIRED)\n"
            "include(\"\\${CMAKE_CURRENT_LIST_DIR}/${PROJECT_NAME_VAR}Targets.cmake\")\n"
    )

    install(FILES
            "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME_VAR}Config.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME_VAR}ConfigVersion.cmake"
            DESTINATION ${${PROJECT_NAME_VAR}_CMAKE_DIR}
    )
endif()

# ---------------------------------------------------------------------------
# Tests
# ---------------------------------------------------------------------------
if(SPELT_BUILD_TESTS)
    add_subdirectory(test)
endif()

if(SPELT_BUILD_TEST_SCENE)
    add_subdirectory(test_scene)
endif()
