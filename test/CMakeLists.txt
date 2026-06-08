cmake_minimum_required(VERSION 3.15)

# Create the test executable
add_executable(tests
        MainTest.cpp
        SceneTest.cpp
)

# Link your engine target (SpeltEngine) and the Catch2 target.
# Catch2 was already added by the root CMakeLists.txt, so we just use it here!
target_link_libraries(tests PRIVATE
        SpeltEngine
        Catch2::Catch2WithMain
)

# Enable CTest discovery
include(CTest)
include(Catch)
catch_discover_tests(tests)