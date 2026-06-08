# CGVCCore tests
include(FetchContent)

FetchContent_Declare(
        catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v3.7.1
)
FetchContent_MakeAvailable(catch2)


add_executable(tests
        MainTest.cpp
        SceneTest.cpp
)

# CGVCCore carries all headers and libraries transitively
target_link_libraries(tests PRIVATE CGVCCore Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(tests)
