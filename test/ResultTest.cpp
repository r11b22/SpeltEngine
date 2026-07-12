#include "Error/Result.hpp"
#include "catch2/catch_test_macros.hpp"


using namespace Spelt;

TEST_CASE("Create Result Value"){
    Result<int, std::string> result = Result<int, std::string>::createValue(1);

    REQUIRE(result.isValue());
    REQUIRE_FALSE(result.isError());
}

TEST_CASE("Create Result Error"){
    Result<int, std::string> result = Result<int, std::string>::createError("lols");

    REQUIRE_FALSE(result.isValue());
    REQUIRE(result.isError());
}


TEST_CASE("Get Result Value"){
    Result<int, std::string> result = Result<int, std::string>::createValue(1);

    REQUIRE(result.isValue());

    REQUIRE(result.value() == 1);
}

TEST_CASE("Get Result Error"){
    Result<int, std::string> result = Result<int, std::string>::createError("");

    REQUIRE(result.isError());

    REQUIRE(result.error() == "");
}


TEST_CASE("Result Value Match"){
    Result<int, std::string> result = Result<int, std::string>::createValue(1);

    bool reached = false;
    result.match(
        [&reached](int val) {reached = true; },
        [](std::string err) { FAIL("Match on value result gave Error");}
    );

    REQUIRE(reached);
}

TEST_CASE("Result Error Match"){
    Result<int, std::string> result = Result<int, std::string>::createError("");

    bool reached = false;
    result.match(
        [](int val) {FAIL("Match on error result gave Value");},
        [&reached](std::string err) { reached = true; }
    );

    REQUIRE(reached);
}
