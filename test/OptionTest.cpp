#include "Error/Option.hpp"
#include "catch2/catch_test_macros.hpp"

using namespace Spelt;

TEST_CASE("Create Option Value"){
    Option<int> option = Option<int>::createValue(1);
    REQUIRE(option.isValue());
    REQUIRE_FALSE(option.isNone());
}
TEST_CASE("Create Option None"){
    Option<int> option = Option<int>::createNone();
    REQUIRE_FALSE(option.isValue());
    REQUIRE(option.isNone());
}
TEST_CASE("Get Option Value"){
    Option<int> option = Option<int>::createValue(1);
    REQUIRE(option.isValue());
    REQUIRE(option.value() == 1);
}
TEST_CASE("Option Value Match"){
    Option<int> option = Option<int>::createValue(1);
    bool reached = false;
    option.match(
        [&reached](int val) {reached = true; },
        []() { FAIL("Match on value option gave None");}
    );
    REQUIRE(reached);
}
TEST_CASE("Option None Match"){
    Option<int> option = Option<int>::createNone();
    bool reached = false;
    option.match(
        [](int val) {FAIL("Match on none option gave Value");},
        [&reached]() { reached = true; }
    );
    REQUIRE(reached);
}
TEST_CASE("Option valueOr"){
    Option<int> valueOption = Option<int>::createValue(0);
    REQUIRE(valueOption.valueOr(2) == 0);
    Option<int> noneOption = Option<int>::createNone();
    REQUIRE(noneOption.valueOr(1) == 1);
}

TEST_CASE("Option::andThen chains on a value and short-circuits on none", "[Option][andThen]") {
    Option<int> value = Value(4);
    Option<int> none = None{};

    SECTION("invokes func and returns its Option when there is a value") {
        auto result = value.andThen([](int& v) { return Option<int>::createValue(v * 2); });

        REQUIRE(result.isValue());
        REQUIRE(result.value() == 8);
    }

    SECTION("short-circuits to an empty Option without invoking func") {
        auto result = none.andThen([](int& v) { return Option<int>::createValue(v * 2); });

        REQUIRE(result.isNone());
    }

    SECTION("a chained func can itself produce None") {
        auto result = value.andThen([](int&) { return Option<int>::createNone(); });

        REQUIRE(result.isNone());
    }
}
