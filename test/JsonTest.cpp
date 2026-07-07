#include "Serial/Json/JsonLoader.hpp"
#include "catch2/catch_test_macros.hpp"
#include <stdexcept>
#include <variant>
#include <vector>
#include "Serial/Json/Json.hpp"

TEST_CASE("Create empty Json Object") {
    Json json{};
    REQUIRE(true);
}

TEST_CASE("Add and get a single data point") {
    Json json{};

    json.addData("key", 0.5);

    REQUIRE(json.getNumber("key") == 0.5);
    REQUIRE(json.tryGetNumber("key").has_value());
    REQUIRE(json.tryGetNumber("key").value() == 0.5);
    REQUIRE_THROWS_AS(json.getBool("key"), std::out_of_range);
    REQUIRE_FALSE(json.tryGetBool("key").has_value());
}

TEST_CASE("Null value") {
    Json json{};

    REQUIRE(std::holds_alternative<std::monostate>(json.getData("any key")));
}

TEST_CASE("Recursive data"){
    Json root{};

    Json child{};
    child.addData("data", "value");

    root.addData("child1", child);

    REQUIRE(root.tryGetObject("child1").has_value());
    REQUIRE(root.getObject("child1").getString("data") == "value");
    REQUIRE(root.getObject("child1").tryGetString("data").has_value());
}

TEST_CASE("Json Arrays"){
    Json root{};

    Json child1{};
    child1.addData("data", 1.0);
    Json child2{};
    child2.addData("data", 2.0);
    Json child3{};
    child3.addData("data", 3.0);

    JsonArray arr;

    arr.addData(child1);
    arr.addData(child2);
    arr.addData(child3);

    root.addData("array", arr);

    REQUIRE(root.getArray("array").getObject(0).getNumber("data") == 1.0);
    REQUIRE(root.getArray("array").getObject(1).getNumber("data") == 2.0);
    REQUIRE(root.getArray("array").getObject(2).getNumber("data") == 3.0);
}


TEST_CASE("Loading: Empty Json Object"){
    JsonLoader loader{};

    Json root = loader.getJson("{}");

    REQUIRE(std::holds_alternative<std::monostate>(root.getData("")));
}

TEST_CASE("Loading: Single data Json Object") {
    JsonLoader loader{};

    Json root = loader.getJson("{\"data\": 512}");
    REQUIRE(root.getNumber("data") == 512);

    root = loader.getJson("{\"data\": true}");
    REQUIRE(root.getBool("data") == true);

    root = loader.getJson("{\"data\": \"string\"}");
    REQUIRE(root.getString("data") == "string");

    root = loader.getJson("{\"data\": null}");
    REQUIRE(std::holds_alternative<std::monostate>(root.getData("data")));
}

TEST_CASE("Loading: Nested Json") {
    JsonLoader loader{};

    Json root = loader.getJson("{\"data\": {\"value\" : 512}}");

    REQUIRE(root.getObject("data").getNumber("value") == 512);
}

TEST_CASE("Loading: Json Array") {
    JsonLoader loader{};

    Json root = loader.getJson("{\"data\": [512, 128]}");

    REQUIRE(root.getArray("data").getNumber(0) == 512);
    REQUIRE(root.getArray("data").getNumber(1) == 128);
}

TEST_CASE("Loading: Json Array with Json") {
    JsonLoader loader{};

    Json root = loader.getJson("{\"data\": [{\"value\": 512}, {\"value\" : 128}]}");

    REQUIRE(root.getArray("data").getObject(0).getNumber("value") == 512);
    REQUIRE(root.getArray("data").getObject(1).getNumber("value") == 128);
}
