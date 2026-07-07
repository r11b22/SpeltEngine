#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

// 1. Forward declarations
struct JsonData;
class Json;
class JsonArray;

// 2. Define standard variant base type to keep code clean
using JsonVariantBase = std::variant<
    std::monostate,
    bool,
    double,
    std::string,
    JsonArray,
    Json
>;

// 3. Define JsonArray first
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
    bool getBool(size_t index) const;
    /**
     * throws: out_of_range if index is invalid or element is not a number
     */
    double getNumber(size_t index) const;
    /**
     * throws: out_of_range if index is invalid or element is not a string
     */
    const std::string& getString(size_t index) const;
    /**
     * throws: out_of_range if index is invalid or element is not an array
     */
    const JsonArray& getArray(size_t index) const;
    /**
     * throws: out_of_range if index is invalid or element is not an object
     */
    const Json& getObject(size_t index) const;

    /**
     * Returns nullopt if index is out of bounds
     */
    std::optional<JsonData> tryGetData(size_t index) const;

    /**
     * Returns nullopt if element does not exist or isn't a bool
     */
    std::optional<bool> tryGetBool(size_t index) const;

    /**
     * Returns nullopt if element does not exist or isn't a number
     */
    std::optional<double> tryGetNumber(size_t index) const;

    /**
     * Returns nullopt if element does not exist or isn't a string
     * @warning: Due to optional, the data is copied fully, this can be very slow
     */
    std::optional<std::string> tryGetString(size_t index) const;

    /**
     * Returns nullopt if element does not exist or isn't an array
     * @warning: Due to optional, the data is copied fully, this can be very slow
     */
    std::optional<JsonArray> tryGetArray(size_t index) const;

    /**
     * Returns nullopt if element does not exist or isn't an object
     * @warning: Due to optional, the data is copied fully, this can be very slow
     */
    std::optional<Json> tryGetObject(size_t index) const;

    // Helper utility for array sizing
    size_t size() const;

private:
    /**
     * throws: out_of_range if index is out of bounds
     */
    const JsonData& getValidData(size_t index) const;

    /**
     * Returns nullopt if value with index does not exist or type mismatch
     */
    template<typename T>
    std::optional<T> tryGet(size_t index) const;
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
    bool getBool(const std::string& key) const;
    /**
     * throws: out_of_range if number with key is not found
     */
    double getNumber(const std::string& key) const;
    /**
     * throws: out_of_range if string with key is not found
     */
    const std::string& getString(const std::string& key) const;
    /**
     * throws: out_of_range if array with key is not found
     */
    const JsonArray& getArray(const std::string& key) const;
    /**
     * throws: out_of_range if object with key is not found
     */
    const Json& getObject(const std::string& key) const;

    /**
     * Returns nullopt if value with key does not exist
     */
    std::optional<JsonData> tryGetData(const std::string& key) const;

    /**
     * Returns nullopt if bool with key does not exist
     */
    std::optional<bool> tryGetBool(const std::string& key) const;

    /**
     * Returns nullopt if number with key does not exist
     */
    std::optional<double> tryGetNumber(const std::string& key) const;

    /**
     * Returns nullopt if string with key does not exist
     * @warning: Due to optional, the data is copied fully, this can be very slow
     */
    std::optional<std::string> tryGetString(const std::string& key) const;

    /**
     * Returns nullopt if array with key does not exist
     * @warning: Due to optional, the data is copied fully, this can be very slow
     */
    std::optional<JsonArray> tryGetArray(const std::string& key) const;

    /**
     * Returns nullopt if object with key does not exist
     * @warning: Due to optional, the data is copied fully, this can be very slow
     */
    std::optional<Json> tryGetObject(const std::string& key) const;
private:
    /**
     * throws: out_of_range if value with key is not found
     */
    const JsonData& getValidData(const std::string& key) const;

    /**
     * Returns nullopt if value with key does not exist
     */
    template<typename T>
    std::optional<T> tryGet(const std::string& key) const;
};

// 5. Define JsonData variant wrapper here (where Json and JsonArray are complete types)
struct JsonData : JsonVariantBase {
    using JsonVariantBase::JsonVariantBase; // Inherit variant constructors

    // Expose helpers to cast smoothly to the base variant for std::get_if compatibility
    const JsonVariantBase* asVariant() const { return static_cast<const JsonVariantBase*>(this); }
    JsonVariantBase* asVariant() { return static_cast<JsonVariantBase*>(this); }
};

// 6. Inline template method implementations (must be defined after JsonData is complete)
template<typename T>
std::optional<T> JsonArray::tryGet(size_t index) const {
    if (index < mElements.size()) {
        if (const T* val = std::get_if<T>(mElements[index].asVariant())) {
            return *val;
        }
    }
    return std::nullopt;
}

template<typename T>
std::optional<T> Json::tryGet(const std::string& key) const {
    auto it = mData.find(key);
    if (it != mData.end()) {
        if (const T* val = std::get_if<T>(it->second.asVariant())) {
            return *val;
        }
    }
    return std::nullopt;
}
