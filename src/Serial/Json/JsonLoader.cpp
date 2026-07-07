#include "Serial/Json/JsonLoader.hpp"
#include "FileReader.h"



Json JsonLoader::readFile(const std::filesystem::path& path) {
    std::string contents = FileReader::readFile(path);


    return getJson(contents);
}

namespace {
    // Advances index past formatting characters (spaces, tabs, newlines)
    void skipWhitespace(std::string_view json, size_t& index) {
        while (index < json.size() && std::isspace(static_cast<unsigned char>(json[index]))) {
            index++;
        }
    }

    // Mutual recursion forward declarations
    JsonData parseValue(std::string_view json, size_t& index);
    Json parseObject(std::string_view json, size_t& index);
    JsonArray parseArray(std::string_view json, size_t& index);

    std::string parseString(std::string_view json, size_t& index) {
        if (index >= json.size() || json[index] != '"') {
            throw std::runtime_error("Expected opening quote for string");
        }
        index++; // Skip opening '"'

        size_t start = index;
        while (index < json.size() && json[index] != '"') {
            if (json[index] == '\\') { // Skip escaped string markers cleanly
                index++;
            }
            index++;
        }

        if (index >= json.size()) {
            throw std::runtime_error("Unterminated string inside JSON metadata");
        }

        std::string result(json.substr(start, index - start));
        index++; // Skip closing '"'
        return result;
    }

    double parseNumber(std::string_view json, size_t& index) {
        size_t start = index;
        if (index < json.size() && json[index] == '-') {
            index++;
        }
        while (index < json.size() && (std::isdigit(static_cast<unsigned char>(json[index])) ||
               json[index] == '.' || json[index] == 'e' || json[index] == 'E' ||
               json[index] == '+' || json[index] == '-')) {
            index++;
        }

        std::string numStr(json.substr(start, index - start));
        return std::stod(numStr);
    }

    bool parseBool(std::string_view json, size_t& index) {
        if (json.substr(index, 4) == "true") {
            index += 4;
            return true;
        } else if (json.substr(index, 5) == "false") {
            index += 5;
            return false;
        }
        throw std::runtime_error("Invalid literal variant conversion to boolean value");
    }

    std::monostate parseNull(std::string_view json, size_t& index) {
        if (json.substr(index, 4) == "null") {
            index += 4;
            return std::monostate{};
        }
        throw std::runtime_error("Invalid signature token during null check");
    }

    JsonArray parseArray(std::string_view json, size_t& index) {
        JsonArray arrayResult;
        index++; // Skip '['

        skipWhitespace(json, index);
        if (index < json.size() && json[index] == ']') {
            index++; // Handle completely empty arrays []
            return arrayResult;
        }

        while (index < json.size()) {
            // Evaluates structural objects, primitives, or nested arrays directly
            JsonData value = parseValue(json, index);
            arrayResult.addData(std::move(value));

            skipWhitespace(json, index);
            if (index < json.size() && json[index] == ']') {
                index++;
                return arrayResult;
            } else if (index < json.size() && json[index] == ',') {
                index++;
            } else {
                throw std::runtime_error("Expected a trailing ',' or ']' bracket marker in array sequence");
            }
        }
        throw std::runtime_error("Unterminated sequence layout inside nested array structure");
    }

    Json parseObject(std::string_view json, size_t& index) {
        Json objResult;
        index++; // Skip '{'

        while (index < json.size()) {
            skipWhitespace(json, index);
            if (json[index] == '}') {
                index++;
                return objResult;
            }

            if (json[index] != '"') {
                throw std::runtime_error("Expected a valid double-quoted string key sequence inside dictionary item");
            }

            std::string key = parseString(json, index);
            skipWhitespace(json, index);

            if (index >= json.size() || json[index] != ':') {
                throw std::runtime_error("Expected structural colon dividing target key from value block");
            }
            index++; // Skip ':'

            JsonData val = parseValue(json, index);
            objResult.addData(key, std::move(val));

            skipWhitespace(json, index);
            if (json[index] == '}') {
                index++;
                return objResult;
            } else if (json[index] == ',') {
                index++;
            } else {
                throw std::runtime_error("Expected element tracking separator or enclosing key terminal bracket '}'");
            }
        }
        throw std::runtime_error("Parsing error: reached file tail with unclosed terminal braces");
    }

    JsonData parseValue(std::string_view json, size_t& index) {
        skipWhitespace(json, index);
        if (index >= json.size()) {
            throw std::runtime_error("Unexpected end of streaming chunk data encountered prematurely");
        }

        char c = json[index];
        if (c == '{') {
            return parseObject(json, index);
        } else if (c == '[') {
            return parseArray(json, index);
        } else if (c == '"') {
            return parseString(json, index);
        } else if (c == 't' || c == 'f') {
            return parseBool(json, index);
        } else if (c == 'n') {
            return parseNull(json, index);
        } else if (std::isdigit(static_cast<unsigned char>(c)) || c == '-') {
            return parseNumber(json, index);
        }

        throw std::runtime_error(std::string("Encountered unexpected character token: ") + c);
    }
}

Json JsonLoader::getJson(std::string json) {
    size_t index = 0;
    std::string_view jsonView(json);

    skipWhitespace(jsonView, index);
    if (jsonView.empty() || jsonView[index] != '{') {
        throw std::runtime_error("Root element context layer must parse strictly down into a dictionary object '{}'");
    }

    return parseObject(jsonView, index);
}
