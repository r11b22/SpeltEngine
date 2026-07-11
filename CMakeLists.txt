# SpeltEngine — static engine library with vendored dependencies
cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
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

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-fno-omit-frame-pointer)
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


option(ENABLE_PROFILING "Enable Tracy profiling" ON)

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
# Explicitly List Source and Include Files
# ---------------------------------------------------------------------------
# Manually list engine source/header files to avoid globbing.
set(ENGINE_SOURCES
        src/App.cpp

        src/Buffer/Buffer.cpp
        src/Buffer/ShaderStorageBuffer.cpp


        src/Camera.cpp

        src/Defaults/Camera/FirstPersonCamera.cpp
        src/Defaults/Objects/Drawables/MeshObject.cpp
        src/Defaults/Objects/Lighting/AmbientLight.cpp
        src/Defaults/Objects/Lighting/DirectionalLight.cpp
        src/Defaults/Objects/Lighting/PointLight.cpp
        src/Defaults/Objects/TransformableObject.cpp
        src/Defaults/PostProcessing/Bloom.cpp
        src/Defaults/PostProcessing/Blur.cpp

        src/FileReader.cpp

        src/FrameBuffer/MultisampledFrameBuffer.cpp

        src/Hashing/Hashing.cpp

        src/InputManager.cpp

        src/Lighting/LightManager.cpp

        src/Material/Material.cpp

        src/PostProcessing/PostProcessingComputeUnit.cpp
        src/PostProcessing/PostProcessingEffect.cpp
        src/PostProcessing/PostProcessingGroup.cpp
        src/PostProcessing/PostProcessingPipeline.cpp

        src/Renderer/RenderCommand.cpp
        src/Renderer/Renderer.cpp
        src/Renderer/RenderQueue.cpp
        src/Renderer/RenderState.cpp
        src/Renderer/RenderStateManager.cpp
        src/Renderer/RenderPass.cpp

        src/Renderer/Instancing/InstanceRenderer.cpp
        src/Renderer/Instancing/InstanceData.cpp

        src/Object/Object.cpp
        src/Object/ObjectRepository.cpp

        src/Serial/Json/Json.cpp
        src/Serial/Json/JsonLoader.cpp

        src/Scene/Scene.cpp
        src/Scene/SceneHierarchyComponent.cpp
        src/Scene/SceneHierarchy.cpp

        src/Shader/ComputeShader.cpp
        src/Shader/ShaderProgram.cpp
        src/Shader/ShaderUniform.cpp

        src/Texture/MultisamledTexture.cpp
        src/Texture/CubemapTexture.cpp
        src/Texture/Texture.cpp
        src/Texture/TextureLoader.cpp
        src/Texture/TextureAsset.cpp
        src/Texture/CubemapLoader.cpp
        src/Texture/CubemapAsset.cpp

        src/Transform/Transform.cpp

        src/VertexArray.cpp

        src/Window.cpp

        src/Asset/AssetManager.cpp
        src/Asset/Asset.cpp
        src/Asset/EmbeddedAsset.cpp

        src/Mesh/MeshReference.cpp
        src/Mesh/MeshAsset.cpp
        src/Mesh/Mesh.cpp
        src/Mesh/MeshLoader.cpp

)

set(ENGINE_HEADERS
        include/App.h

        include/Buffer/Buffer.h
        include/Buffer/BufferMap.h
        include/Buffer/ShaderStorageBuffer.hpp

        include/Camera.h

        include/DataStructures/SparseSet.hpp

        include/Defaults/Camera/FirstPersonCamera.h

        include/Defaults/Objects/Drawables/MeshObject.h
        include/Defaults/Objects/Lighting/AmbientLight.h
        include/Defaults/Objects/Lighting/DirectionalLight.h
        include/Defaults/Objects/Lighting/PointLight.h
        include/Defaults/Objects/TransformableObject.h

        include/Defaults/PostProcessing/Bloom.h
        include/Defaults/PostProcessing/Blur.h

        include/FileReader.h

        include/FrameBuffer/FrameBuffer.h
        include/FrameBuffer/MultisampledFrameBuffer.h

        include/Hashing/Hashing.hpp

        include/InputManager.h

        include/Lighting/AmbientLightData.h
        include/Lighting/DirectionalLightData.h
        include/Lighting/ILight.h
        include/Lighting/LightData.h
        include/Lighting/LightManager.h
        include/Lighting/PointLightData.h

        include/Material/Material.h

        include/Object/Object.h
        include/Object/ObjectID.h
        include/Object/ObjectType.h
        include/Object/ObjectRepository.h

        include/PostProcessing/PostProcessingComputeUnit.h
        include/PostProcessing/PostProcessingEffect.h
        include/PostProcessing/PostProcessingGroup.h
        include/PostProcessing/PostProcessingPipeline.h

        include/Renderer/IDrawable.h
        include/Renderer/IRenderable.h
        include/Renderer/RenderCommand.h
        include/Renderer/Renderer.h
        include/Renderer/RenderQueue.h
        include/Renderer/RenderState.h
        include/Renderer/RenderStateManager.h
        include/Renderer/RenderPass.hpp

        include/Renderer/Instancing/InstanceRenderer.hpp
        include/Renderer/Instancing/InstanceData.hpp
        include/Renderer/Instancing/InstanceLayout.hpp

        include/Serial/Json/Json.hpp
        include/Serial/Json/JsonLoader.hpp

        include/Scene/Scene.hpp
        include/Scene/SceneHierarchyComponent.h
        include/Scene/SceneHierarchy.h

        include/Shader/ComputeShader.h
        include/Shader/ShaderProgram.h
        include/Shader/ShaderUniform.h

        include/Strings/ShaderSource.h

        include/Texture/MultisampledTexture.h
        include/Texture/Texture.h
        include/Texture/TextureLoader.h
        include/Texture/TextureData.h
        include/Texture/TextureReference.hpp
        include/Texture/TextureAsset.hpp
        include/Texture/CubemapTexture.hpp
        include/Texture/CubemapTextureReference.hpp
        include/Texture/CubemapLoader.hpp
        include/Texture/CubemapAsset.hpp
        include/Texture/TextureUniform.hpp

        include/Transform/ITransformable.h
        include/Transform/Transform.h

        include/Utilities/Random.h

        include/VertexArray.h

        include/Window.h

        include/Asset/AssetManager.hpp
        include/Asset/AssetRepository.hpp
        include/Asset/AssetReference.hpp
        include/Asset/AssetLoader.hpp
        include/Asset/Asset.hpp
        include/Asset/EmbeddedAsset.hpp

        include/Mesh/MeshReference.hpp
        include/Mesh/MeshAsset.hpp
        include/Mesh/Mesh.h
        include/Mesh/MeshLoader.hpp

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
        $<BUILD_INTERFACE:assimp>
        PRIVATE $<BUILD_INTERFACE:glfw>
        $<BUILD_INTERFACE:stb_image>
)

# Tracy

if(ENABLE_PROFILING)
    # Add Tracy header paths
    target_include_directories(${PROJECT_NAME_VAR} PRIVATE external/tracy/public)

    # Add Tracy source file to compilation
    target_sources(${PROJECT_NAME_VAR} PRIVATE external/tracy/public/TracyClient.cpp)

    # Enable Tracy macros globally for your target
    target_compile_definitions(${PROJECT_NAME_VAR} PRIVATE TRACY_ENABLE)

    # Linux requires pthreads and dl
    target_link_libraries(${PROJECT_NAME_VAR} PRIVATE pthread dl)
endif()

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
