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

TEST_CASE("Result valueOr"){
    Result<int, std::string> valueResult = Result<int, std::string>::createValue(0);

    REQUIRE(valueResult.valueOr(2) == 0);

    Result<int, std::string> errorResult = Result<int, std::string>::createError("");

    REQUIRE(errorResult.valueOr(1) == 1);
}


TEST_CASE("Result::andThen chains on a value and forwards the error", "[Result][andThen]") {
    Result<int, std::string> ok = Value(10);
    Result<int, std::string> err = Error(std::string("bad input"));

    SECTION("invokes func and returns its Result when there is a value") {
        auto result = ok.andThen([](int& v) { return Result<int, std::string>::createValue(v / 2); });

        REQUIRE(result.isValue());
        REQUIRE(result.value() == 5);
    }

    SECTION("short-circuits and forwards the original error without invoking func") {
        auto result = err.andThen([](int& v) { return Result<int, std::string>::createValue(v / 2); });

        REQUIRE(result.isError());
        REQUIRE(result.error() == "bad input");
    }
}
