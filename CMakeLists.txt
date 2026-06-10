# SpeltEngine — static engine library with vendored dependencies
cmake_minimum_required(VERSION 3.15)

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
# glad
# ---------------------------------------------------------------------------
set(GLAD_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/glad/include")
set(GLAD_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/glad/src/glad.c"
        include/Defaults/Camera/FirstPersonCamera.h
        src/Defaults/Camera/FirstPersonCamera.cpp
        include/Lighting/AmbientLight.h)

# ---------------------------------------------------------------------------
# Static library target
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
        include/Material/Material.h
        src/Material/Material.cpp
        include/Renderer/IRenderPass.h
        include/Renderer/BasicRenderPass.h
        src/Renderer/BasicRenderPass.cpp
)

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
            "include(\"\${CMAKE_CURRENT_LIST_DIR}/${PROJECT_NAME_VAR}Targets.cmake\")\n"
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