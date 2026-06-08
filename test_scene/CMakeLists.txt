cmake_minimum_required(VERSION 3.20)
project(SpeltEngineTestScene VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---------------------------------------------------------------------------
# Core engine library (Only fetch if not already included as a sub-project)
# ---------------------------------------------------------------------------
if(NOT TARGET SpeltEngine)
    include(FetchContent)

    FetchContent_Declare(SpeltEngine
            GIT_REPOSITORY https://github.com/r11b22/SpeltEngine.git
            GIT_TAG        main
    )

    FetchContent_MakeAvailable(SpeltEngine)
endif()

# ---------------------------------------------------------------------------
# Application executable
# ---------------------------------------------------------------------------
add_executable(${PROJECT_NAME}
        main.cpp
        src/ToonScene.cpp
        include/ToonScene.h
)

target_include_directories(${PROJECT_NAME} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)


target_link_libraries(${PROJECT_NAME} PRIVATE SpeltEngine)