#include "Serial/Json/JsonLoader.hpp"
#include "Asset/EmbeddedAsset.hpp"
#include "Error/Result.hpp"
#include "FileReader.h"
#include "Serial/Json/Json.hpp"


namespace Spelt {
    void JsonLoader::readFile(const std::filesystem::path& path) {
        mContents = FileReader::readFile(path).value();
    }

    void JsonLoader::readRaw(EmbeddedAsset asset){
        mContents = embeddedAssetToString(asset);
    }

    Result<Json, JsonLoaderError> JsonLoader::createJson(){
        return fromString(mContents);
    }

    namespace {
        // Advances index past formatting characters (spaces, tabs, newlines)
        void skipWhitespace(std::string_view json, size_t& index) {
            while (index < json.size() && std::isspace(static_cast<unsigned char>(json[index]))) {
                index++;
            }
        }

        // Mutual recursion forward declarations
        Result<JsonData, JsonLoaderError> parseValue(std::string_view json, size_t& index);
        Result<Json, JsonLoaderError> parseObject(std::string_view json, size_t& index);
        Result<JsonArray, JsonLoaderError> parseArray(std::string_view json, size_t& index);

        Result<std::string, JsonLoaderError> parseString(std::string_view json, size_t& index) {
            if (index >= json.size() || json[index] != '"') {
                return Error(JsonLoaderError::MissingOpeningQoute);
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
                return Error(JsonLoaderError::UnterminatedString);
            }

            std::string result(json.substr(start, index - start));
            index++; // Skip closing '"'
            return Value(result);
        }

        Result<double, JsonLoaderError> parseNumber(std::string_view json, size_t& index) {
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
            return Value(std::stod(numStr));
        }

        Result<bool, JsonLoaderError> parseBool(std::string_view json, size_t& index) {
            if (json.substr(index, 4) == "true") {
                index += 4;
                return Value(true);
            } else if (json.substr(index, 5) == "false") {
                index += 5;
                return Value(false);
            }
            return Error(JsonLoaderError::InvalidLiteralVariant);
        }

        Result<std::monostate, JsonLoaderError> parseNull(std::string_view json, size_t& index) {
            if (json.substr(index, 4) == "null") {
                index += 4;
                return Value(std::monostate{});
            }
            return Error(JsonLoaderError::InvalidLiteralVariant);
        }

        Result<JsonArray, JsonLoaderError> parseArray(std::string_view json, size_t& index) {
            JsonArray arrayResult;
            index++; // Skip '['

            skipWhitespace(json, index);
            if (index < json.size() && json[index] == ']') {
                index++; // Handle completely empty arrays []
                return Value(arrayResult);
            }

            while (index < json.size()) {
                // Evaluates structural objects, primitives, or nested arrays directly
                auto value = parseValue(json, index);
                if(value.isError()){
                    return Error(value.error());
                }

                arrayResult.addData(std::move(value.value()));

                skipWhitespace(json, index);
                if (index < json.size() && json[index] == ']') {
                    index++;
                    return Value(arrayResult);
                } else if (index < json.size() && json[index] == ',') {
                    index++;
                } else {
                    return Error(JsonLoaderError::UnexpectedCharacter);
                }
            }
            return Error(JsonLoaderError::UnterminatedArray);
        }

        Result<Json, JsonLoaderError> parseObject(std::string_view json, size_t& index) {
            Json objResult;
            index++; // Skip '{'

            while (index < json.size()) {
                skipWhitespace(json, index);
                if (json[index] == '}') {
                    index++;
                    return Value(objResult);
                }

                if (json[index] != '"') {
                    return Error(JsonLoaderError::MissingKey);
                }

                auto key = parseString(json, index);
                if(key.isError()){
                    return Error(key.error());
                }

                skipWhitespace(json, index);

                if (index >= json.size() || json[index] != ':') {
                    return Error(JsonLoaderError::MissingSeperator);
                }
                index++; // Skip ':'

                auto val = parseValue(json, index);
                if(val.isError()){
                    return Error(val.error());
                }

                objResult.addData(key.value(), std::move(val.value()));

                skipWhitespace(json, index);
                if (json[index] == '}') {
                    index++;
                    return Value(objResult);
                } else if (json[index] == ',') {
                    index++;
                } else {
                    return Error(JsonLoaderError::UnclosedBraces);
                }
            }
            return Error(JsonLoaderError::UnclosedBraces);
        }

        Result<JsonData, JsonLoaderError> parseValue(std::string_view json, size_t& index) {
            skipWhitespace(json, index);
            if (index >= json.size()) {
                return Error(JsonLoaderError::UnexpectedStreamEnd);
            }

            char c = json[index];
            if (c == '{') {
                auto obj = parseObject(json, index);
                if (obj.isError()){
                    return Error(obj.error());
                }
                return Value(obj.value());
            } else if (c == '[') {
                auto arr = parseArray(json, index);
                if (arr.isError()){
                    return Error(arr.error());
                }
                return Value(arr.value());
            } else if (c == '"') {
                auto str = parseString(json, index);
                if (str.isError()){
                    return Error(str.error());
                }
                return Value(str.value());
            } else if (c == 't' || c == 'f') {
                auto b = parseBool(json, index);
                if (b.isError()){
                    return Error(b.error());
                }
                return Value(b.value());
            } else if (c == 'n') {
                auto n = parseNull(json, index);
                if (n.isError()){
                    return Error(n.error());
                }
                return Value(n.value());
            } else if (std::isdigit(static_cast<unsigned char>(c)) || c == '-') {
                auto num = parseNumber(json, index);
                if (num.isError()){
                    return Error(num.error());
                }
                return Value(num.value());
            }

            return Error(JsonLoaderError::UnexpectedCharacter);
        }
    }

    Result<Json, JsonLoaderError> JsonLoader::fromString(std::string json) {
        size_t index = 0;
        std::string_view jsonView(json);

        skipWhitespace(jsonView, index);
        if (jsonView.empty() || jsonView[index] != '{') {
            return Error(JsonLoaderError::MissingRootBraces);
        }

        return parseObject(jsonView, index);
    }
}
