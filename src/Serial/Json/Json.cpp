


#include "Serial/Json/Json.hpp"
#include "Error/Option.hpp"
#include <format>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

namespace Spelt {
    void JsonArray::addData(JsonData data) {
        mElements.push_back(std::move(data));
    }

    JsonData JsonArray::getData(size_t index) const {
        if (index >= mElements.size()) return std::monostate{};
        return mElements[index];
    }

    Option<bool> JsonArray::getBool(size_t index) const {
        auto variantData = getValidData(index);
        if(variantData.isNone()){
            return None{};
        }

        if (const auto* val = std::get_if<bool>(&variantData.value())) {
            return Value(*val);
        }
        return None{};
    }

    Option<double> JsonArray::getNumber(size_t index) const {
        auto variantData = getValidData(index);
        if(variantData.isNone()){
            return None{};
        }

        if (const auto* val = std::get_if<double>(&variantData.value())) {
            return Value(*val);
        }
        return None{};
    }

    Option<const std::string&> JsonArray::getString(size_t index) const {
        auto variantData = getValidData(index);
        if(variantData.isNone()){
            return None{};
        }

        if (const auto* val = std::get_if<std::string>(&variantData.value())) {
            return Option<const std::string&>::createValue(*val);
        }
        return None{};
    }

    Option<const JsonArray&> JsonArray::getArray(size_t index) const {
        Option<const JsonData&> variantData = getValidData(index);
        if (variantData.isNone()){
            return None{};
        }
        if (const auto* val = std::get_if<JsonArray>(&variantData.value())) {
            return Option<const JsonArray&>::createValue(*val);
        }
        return None{};
    }

    Option<const Json&> JsonArray::getObject(size_t index) const {
        Option<const JsonData&> variantData = getValidData(index);
        if (variantData.isNone()){
            return None{};
        }
        if (const auto* val = std::get_if<Json>(&variantData.value())) {
            return Option<const Json&>::createValue(*val);
        }
        return None{};
    }

    Option<const JsonData&> JsonArray::getValidData(size_t index) const {
        if (index >= mElements.size()) {
            throw None{};
        }
        return Option<const JsonData&>::createValue(mElements[index]);
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

    Option<bool> Json::getBool(const std::string& key) const {
        auto variantData = getValidData(key);
        if(variantData.isNone()){
            return None{};
        }

        if (const auto* val = std::get_if<bool>(&variantData.value())) {
            return Value(*val);
        }
        throw None{};
    }

    Option<double> Json::getNumber(const std::string& key) const {
        auto variantData = getValidData(key);
        if(variantData.isNone()){
            return None{};
        }

        if (const auto* val = std::get_if<double>(&variantData.value())) {
            return Value(*val);
        }
        return None{};
    }

    Option<const std::string&> Json::getString(const std::string& key) const {
        auto variantData = getValidData(key);
        if(variantData.isNone()){
            return None{};
        }

        if (const auto* val = std::get_if<std::string>(&variantData.value())) {
            return Option<const std::string&>::createValue(*val);
        }
        return None{};
    }

    Option<const JsonArray&> Json::getArray(const std::string& key) const {
        auto variantData = getValidData(key);
        if(variantData.isNone()){
            return None{};
        }

        if (const auto* val = std::get_if<JsonArray>(&variantData.value())) {
            return Option<const JsonArray&>::createValue(*val);
        }
        return None{};
    }

    Option<const Json&> Json::getObject(const std::string& key) const {
        auto variantData = getValidData(key);
        if(variantData.isNone()){
            return None{};
        }

        if (const auto* val = std::get_if<Json>(&variantData.value())) {
            return Option<const Json&>::createValue(*val);
        }
        return None{};
    }

    Option<const JsonData&> Json::getValidData(const std::string& key) const {
        auto it = mData.find(key);

        if(it == mData.end()){
            return None{};
        }

        return Option<const JsonData&>::createValue(it->second);
    }
}
