#pragma once

#include "Error/Option.hpp"
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace Spelt {
    struct JsonData;
    class Json;
    class JsonArray;

    using JsonVariantBase = std::variant<
        std::monostate,
        bool,
        double,
        std::string,
        JsonArray,
        Json
    >;

    class JsonArray {
    private:
        std::vector<JsonData> mElements;

    public:
        JsonArray() = default;

        void addData(JsonData data);
        JsonData getData(size_t index) const;

        /**
        * throws: out_of_range if index is invalid or element is not a bool
        */
        Option<bool> getBool(size_t index) const;
        /**
        * throws: out_of_range if index is invalid or element is not a number
        */
        Option<double> getNumber(size_t index) const;
        /**
        * throws: out_of_range if index is invalid or element is not a string
        */
        Option<const std::string&> getString(size_t index) const;
        /**
        * throws: out_of_range if index is invalid or element is not an array
        */
        Option<const JsonArray&> getArray(size_t index) const;
        /**
        * throws: out_of_range if index is invalid or element is not an object
        */
        Option<const Json&> getObject(size_t index) const;

        // Helper utility for array sizing
        size_t size() const;

    private:
        /**
        * throws: out_of_range if index is out of bounds
        */
        Option<const JsonData&> getValidData(size_t index) const;
    };

    // 4. Define Json second
    class Json {
    private:
        std::unordered_map<std::string, JsonData> mData;
    public:
        Json() = default;

        void addData(const std::string& key, JsonData data);
        JsonData getData(const std::string& key) const;

        /**
        * throws: out_of_range if bool with key is not found
        */
        Option<bool> getBool(const std::string& key) const;
        /**
        * throws: out_of_range if number with key is not found
        */
        Option<double> getNumber(const std::string& key) const;
        /**
        * throws: out_of_range if string with key is not found
        */
        Option<const std::string&> getString(const std::string& key) const;
        /**
        * throws: out_of_range if array with key is not found
        */
        Option<const JsonArray&> getArray(const std::string& key) const;
        /**
        * throws: out_of_range if object with key is not found
        */
        Option<const Json&> getObject(const std::string& key) const;

    private:
        /**
        * throws: out_of_range if value with key is not found
        */
        Option<const JsonData&> getValidData(const std::string& key) const;
    };

    // 5. Define JsonData variant wrapper here (where Json and JsonArray are complete types)
    struct JsonData : JsonVariantBase {
        using JsonVariantBase::JsonVariantBase; // Inherit variant constructors

        // Expose helpers to cast smoothly to the base variant for std::get_if compatibility
        const JsonVariantBase* asVariant() const { return static_cast<const JsonVariantBase*>(this); }
        JsonVariantBase* asVariant() { return static_cast<JsonVariantBase*>(this); }
    };
}
