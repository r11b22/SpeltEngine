


#include "Serial/Json/Json.hpp"
#include <format>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>


void JsonArray::addData(JsonData data) {
    mElements.push_back(std::move(data));
}

JsonData JsonArray::getData(size_t index) const {
    if (index >= mElements.size()) return std::monostate{};
    return mElements[index];
}

bool JsonArray::getBool(size_t index) const {
    const JsonData& variantData = getValidData(index);
    if (const auto* val = std::get_if<bool>(&variantData)) {
        return *val;
    }
    throw std::out_of_range(std::format("Element at index {} exists but is not a boolean.", index));
}

double JsonArray::getNumber(size_t index) const {
    const JsonData& variantData = getValidData(index);
    if (const auto* val = std::get_if<double>(&variantData)) {
        return *val;
    }
    throw std::out_of_range(std::format("Element at index {} exists but is not a number.", index));
}

const std::string& JsonArray::getString(size_t index) const {
    const JsonData& variantData = getValidData(index);
    if (const auto* val = std::get_if<std::string>(&variantData)) {
        return *val;
    }
    throw std::out_of_range(std::format("Element at index {} exists but is not a string.", index));
}

const JsonArray& JsonArray::getArray(size_t index) const {
    const JsonData& variantData = getValidData(index);
    if (const auto* val = std::get_if<JsonArray>(&variantData)) {
        return *val;
    }
    throw std::out_of_range(std::format("Element at index {} exists but is not an array.", index));
}

const Json& JsonArray::getObject(size_t index) const {
    const JsonData& variantData = getValidData(index);
    if (const auto* val = std::get_if<Json>(&variantData)) {
        return *val;
    }
    throw std::out_of_range(std::format("Element at index {} exists but is not an object.", index));
}

std::optional<JsonData> JsonArray::tryGetData(size_t index) const {
    if (index < mElements.size()) {
        return mElements[index];
    }
    return std::nullopt;
}

std::optional<bool> JsonArray::tryGetBool(size_t index) const {
    return tryGet<bool>(index);
}

std::optional<double> JsonArray::tryGetNumber(size_t index) const {
    return tryGet<double>(index);
}

std::optional<std::string> JsonArray::tryGetString(size_t index) const {
    return tryGet<std::string>(index);
}

std::optional<JsonArray> JsonArray::tryGetArray(size_t index) const {
    return tryGet<JsonArray>(index);
}

std::optional<Json> JsonArray::tryGetObject(size_t index) const {
    return tryGet<Json>(index);
}

const JsonData& JsonArray::getValidData(size_t index) const {
    if (index >= mElements.size()) {
        throw std::out_of_range(std::format("Index {} is out of bounds for array size {}.", index, mElements.size()));
    }
    return mElements[index];
}

size_t JsonArray::size() const {
    return mElements.size();
}

void Json::addData(const std::string& key, JsonData data){
    mData.emplace(key, data);
}

JsonData Json::getData(const std::string& key) const{
    auto it = mData.find(key);
    if (it == mData.end()) return std::monostate{};
    return it->second;
}

bool Json::getBool(const std::string& key) const {
    const JsonData& variantData = getValidData(key);
    if (const auto* val = std::get_if<bool>(&variantData)) {
        return *val;
    }
    throw std::out_of_range(std::format("Key '{}' exists but is not a boolean.", key));
}

double Json::getNumber(const std::string& key) const {
    const JsonData& variantData = getValidData(key);
    if (const auto* val = std::get_if<double>(&variantData)) {
        return *val;
    }
    throw std::out_of_range(std::format("Key '{}' exists but is not a number.", key));
}

const std::string& Json::getString(const std::string& key) const {
    const JsonData& variantData = getValidData(key);
    if (const auto* val = std::get_if<std::string>(&variantData)) {
        return *val;
    }
    throw std::out_of_range(std::format("Key '{}' exists but is not a string.", key));
}

const JsonArray& Json::getArray(const std::string& key) const {
    const JsonData& variantData = getValidData(key);
    if (const auto* val = std::get_if<JsonArray>(&variantData)) {
        return *val;
    }
    throw std::out_of_range(std::format("Key '{}' exists but is not an array.", key));
}

const Json& Json::getObject(const std::string& key) const {
    const JsonData& variantData = getValidData(key);
    if (const auto* val = std::get_if<Json>(&variantData)) {
        return *val;
    }
    throw std::out_of_range(std::format("Key '{}' exists but is not an object.", key));
}


std::optional<JsonData> Json::tryGetData(const std::string& key) const {
    auto it = mData.find(key);
    if (it != mData.end()){
        return it->second;
    }
    return std::nullopt;
}

std::optional<bool> Json::tryGetBool(const std::string& key) const {
    return tryGet<bool>(key);
}

std::optional<double> Json::tryGetNumber(const std::string& key) const {
    return tryGet<double>(key);
}

std::optional<std::string> Json::tryGetString(const std::string& key) const {
    return tryGet<std::string>(key);
}

std::optional<JsonArray> Json::tryGetArray(const std::string& key) const {
    return tryGet<JsonArray>(key);
}

std::optional<Json> Json::tryGetObject(const std::string& key) const {
    return tryGet<Json>(key);
}


const JsonData& Json::getValidData(const std::string& key) const {
    auto it = mData.find(key);

    if(it == mData.end()){
        throw std::out_of_range(std::format("Could not find data with key: {}", key));
    }

    return it->second;
}
