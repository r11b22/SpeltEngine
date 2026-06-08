# SpeltEngine — static engine library
#
# Expected layout inside core/:
#   glad/include/   glad/src/
#   src/            (FileReader.cpp, Window.cpp, …)
#   include/        (FileReader.h,   Window.h,   …)
#   test/           (MainTest.cpp)
#   external/       (glm/ ...) <--- NEW

cmake_minimum_required(VERSION 3.15)

# Define the project name variable
set(PROJECT_NAME_VAR SpeltEngine)
project(${PROJECT_NAME_VAR} VERSION 1.0)

include(FetchContent)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# ---------------------------------------------------------------------------
# System packages
# ---------------------------------------------------------------------------
find_package(OpenGL REQUIRED)

# ---------------------------------------------------------------------------
# Dependencies
# ---------------------------------------------------------------------------
FetchContent_Declare(
        catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.7.1
)
FetchContent_MakeAvailable(catch2)

FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG        3.3.8
)
set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "")
set(GLFW_BUILD_TESTS    OFF CACHE INTERNAL "")
set(GLFW_BUILD_DOCS     OFF CACHE INTERNAL "")
set(GLFW_INSTALL        OFF CACHE INTERNAL "")
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
        assimp
        GIT_REPOSITORY https://github.com/assimp/assimp.git
        GIT_TAG        v5.4.3
)
set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS        OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL            OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ZLIB         ON  CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(assimp)

# GLM FetchContent block removed — using local vendored version in external/glm

FetchContent_Declare(
        stb
        GIT_REPOSITORY https://github.com/nothings/stb.git
        GIT_TAG        master
)
FetchContent_MakeAvailable(stb)
add_library(stb_image INTERFACE)
target_include_directories(stb_image INTERFACE ${stb_SOURCE_DIR})

# ---------------------------------------------------------------------------
# glad
# ---------------------------------------------------------------------------
set(GLAD_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/glad/include")
set(GLAD_SOURCES     "${CMAKE_CURRENT_SOURCE_DIR}/glad/src/glad.c"
        include/Defaults/Camera/FirstPersonCamera.h
        src/Defaults/Camera/FirstPersonCamera.cpp
        include/Lighting/AmbientLight.h)

# ---------------------------------------------------------------------------
# Static library
# ---------------------------------------------------------------------------
add_library(${PROJECT_NAME_VAR} STATIC
        ${GLAD_SOURCES}
        src/FileReader.cpp
        include/FileReader.h
        src/Window.cpp
        include/Window.h
        include/Buffer/Buffer.h
        src/Buffer/Buffer.cpp
        include/Shaders/ShaderProgram.h
        src/Shaders/ShaderProgram.cpp
        src/Renderer/Renderer.cpp
        include/Renderer/Renderer.h
        src/Mesh.cpp
        include/VertexArray.h
        src/VertexArray.cpp
        include/Renderer/IDrawable.h
        include/Mesh.h
        src/ModelLoader.cpp
        include/Transform/Transform.h
        src/Transform/Transform.cpp
        src/Texture.cpp
        include/Texture.h
        include/ModelLoader.h
        src/Camera.cpp
        include/Camera.h
        src/InputManager.cpp
        include/InputManager.h
        include/FrameBuffer.h
        include/Scene/Scene.h
        src/App.cpp
        include/App.h
        src/Scene/Scene.cpp
        include/PostProcessing/PostProcessingPipeline.h
        src/PostProcessing/PostProcessingPipeline.cpp
        include/PostProcessing/PostProcessingEffect.h
        src/PostProcessing/PostProcessingEffect.cpp
        include/PostProcessing/PostProcessingComputeUnit.h
        src/PostProcessing/PostProcessingComputeUnit.cpp
        include/PostProcessing/PostProcessingGroup.h
        src/PostProcessing/PostProcessingGroup.cpp
        include/Strings/ShaderSource.h
        include/Scene/Object.h
        include/Scene/ObjectID.h
        src/Scene/Object.cpp
        include/Scene/SceneHierarchyComponent.h
        src/Scene/SceneHierarchyComponent.cpp
        src/Defaults/PostProcessing/Bloom.cpp
        include/Defaults/PostProcessing/Bloom.h
        include/Defaults/PostProcessing/Blur.h
        src/Defaults/PostProcessing/Blur.cpp
        include/PickBuffer.h
        src/PickBuffer.cpp
        include/Transform/ITransformable.h
        include/Defaults/Objects/Drawables/MeshObject.h
        include/Defaults/Objects/TransformableObject.h
        src/Defaults/Objects/TransformableObject.cpp
        src/Defaults/Objects/Drawables/MeshObject.cpp
        include/Defaults/Objects/Drawables/Clickables/ClickableObject.h
        src/Defaults/Objects/Drawables/Clickables/ClickableObject.cpp
        src/Lighting/PointLight.cpp
        src/Lighting/AmbientLight.cpp
        src/Renderer/RenderQueue.cpp
        include/Renderer/RenderQueue.h
        include/Defaults/Objects/Drawables/Clickables/ClickableMesh.h
        src/Defaults/Objects/Drawables/Clickables/ClickableMesh.cpp
        include/Utilities/Random.h
        src/Shaders/ComputeShader.cpp
        include/Shaders/ComputeShader.h
        include/Buffer/BufferMap.h
)

target_compile_definitions(${PROJECT_NAME_VAR} PUBLIC
        GLM_ENABLE_EXPERIMENTAL
        STB_IMAGE_IMPLEMENTATION
        _USE_MATH_DEFINES
)

# ---------------------------------------------------------------------------
# Include paths
#   BUILD_INTERFACE  — used when building inside this repo (or via add_subdirectory)
#   INSTALL_INTERFACE — used by external projects after cmake --install
# ---------------------------------------------------------------------------
target_include_directories(${PROJECT_NAME_VAR} PUBLIC
        $<BUILD_INTERFACE:${GLAD_INCLUDE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/external> # Gives access to external/glm/glm.hpp during build
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME_VAR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME_VAR}/glad>
)

# Explicitly ensure the FetchContent headers are ONLY used during the build phase
target_include_directories(${PROJECT_NAME_VAR} PRIVATE
        $<BUILD_INTERFACE:${glfw_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${assimp_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${stb_SOURCE_DIR}>
)

# OpenGL is PUBLIC because consumers need it transitively (it's an installed
# system target and safe to export).
# The FetchContent deps (glfw, assimp, stb_image) are PRIVATE.
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
# Install: library binary
# ---------------------------------------------------------------------------
install(TARGETS ${PROJECT_NAME_VAR}
        EXPORT  ${PROJECT_NAME_VAR}Targets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# Install public headers (preserves subdirectory structure)
install(DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME_VAR}
)
install(DIRECTORY ${GLAD_INCLUDE_DIR}/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME_VAR}/glad
)

# Install downloaded GLM headers alongside your engine headers
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/external/glm
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME_VAR}
)

# ---------------------------------------------------------------------------
# Install: CMake package config files
#   After install, external projects can do:
#     find_package(SpeltEngine REQUIRED)
#     target_link_libraries(MyApp PRIVATE SpeltEngine::SpeltEngine)
# ---------------------------------------------------------------------------
set(${PROJECT_NAME_VAR}_CMAKE_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME_VAR}")

install(EXPORT ${PROJECT_NAME_VAR}Targets
        FILE        ${PROJECT_NAME_VAR}Targets.cmake
        NAMESPACE   ${PROJECT_NAME_VAR}::
        DESTINATION ${${PROJECT_NAME_VAR}_CMAKE_DIR}
)

# Version file — enables find_package(SpeltEngine 1.0 REQUIRED)
write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME_VAR}ConfigVersion.cmake"
        VERSION       ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
)

# Config file — resolves OpenGL before pulling in the exported targets
# GLM is no longer required here since it installs directly with SpeltEngine
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME_VAR}Config.cmake"
        "include(CMakeFindDependencyMacro)\n"
        "find_dependency(OpenGL REQUIRED)\n"
        "include(\"\${CMAKE_CURRENT_LIST_DIR}/${PROJECT_NAME_VAR}Targets.cmake\")\n"
)

install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME_VAR}Config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME_VAR}ConfigVersion.cmake"
        DESTINATION ${${PROJECT_NAME_VAR}_CMAKE_DIR}
)

# ---------------------------------------------------------------------------
# Tests
# ---------------------------------------------------------------------------
add_subdirectory(test)