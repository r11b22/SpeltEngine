#include "Serial/Json/JsonLoader.hpp"
#include "catch2/catch_test_macros.hpp"
#include <stdexcept>
#include <variant>
#include <vector>
#include "Serial/Json/Json.hpp"

using namespace Spelt;

TEST_CASE("Create empty Json Object") {
    Json json{};
    REQUIRE(true);
}

TEST_CASE("Add and get a single data point") {
    Json json{};

    json.addData("key", 0.5);

    REQUIRE(json.getNumber("key").isValue());
    REQUIRE(json.getNumber("key").value() == 0.5);
    REQUIRE(json.getBool("key").isNone());
    REQUIRE_FALSE(json.getBool("key").isValue());
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

    REQUIRE(root.getObject  ("child1").isValue());
    REQUIRE(root.getObject("child1").value().getString("data").isValue());
    REQUIRE(root.getObject("child1").value().getString("data").value() == "value");

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

    REQUIRE(root.getArray("array").value().getObject(0).value().getNumber("data").value() == 1.0);
    REQUIRE(root.getArray("array").value().getObject(1).value().getNumber("data").value() == 2.0);
    REQUIRE(root.getArray("array").value().getObject(2).value().getNumber("data").value() == 3.0);
}


TEST_CASE("Loading: Empty Json Object"){
    JsonLoader loader{};

    Json root = loader.fromString("{}").value();

    REQUIRE(std::holds_alternative<std::monostate>(root.getData("")));
}

TEST_CASE("Loading: Single data Json Object") {
    JsonLoader loader{};

    Json root = loader.fromString("{\"data\": 512}").value();
    REQUIRE(root.getNumber("data").value() == 512);

    root = loader.fromString("{\"data\": true}").value();
    REQUIRE(root.getBool("data").value() == true);

    root = loader.fromString("{\"data\": \"string\"}").value();
    REQUIRE(root.getString("data").value() == "string");

    root = loader.fromString("{\"data\": null}").value();
    REQUIRE(std::holds_alternative<std::monostate>(root.getData("data")));
}

TEST_CASE("Loading: Nested Json") {
    JsonLoader loader{};

    Json root = loader.fromString("{\"data\": {\"value\" : 512}}").value();

    REQUIRE(root.getObject("data").value().getNumber("value").value() == 512);
}

TEST_CASE("Loading: Json Array") {
    JsonLoader loader{};

    Json root = loader.fromString("{\"data\": [512, 128]}").value();

    REQUIRE(root.getArray("data").value().getNumber(0).value() == 512);
    REQUIRE(root.getArray("data").value().getNumber(1).value() == 128);
}

TEST_CASE("Loading: Json Array with Json") {
    JsonLoader loader{};

    Json root = loader.fromString("{\"data\": [{\"value\": 512}, {\"value\" : 128}]}").value();

    REQUIRE(root.getArray("data").value().getObject(0).value().getNumber("value").value() == 512);
    REQUIRE(root.getArray("data").value().getObject(1).value().getNumber("value").value() == 128);
}
