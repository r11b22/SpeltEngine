


#include "Texture/CubemapTexture.hpp"
#include "Texture/CubemapTextureReference.hpp"
#include "Texture/Texture.h"
#include "Texture/TextureReference.hpp"
#include <variant>

using UniformTextureData = std::variant<
    TextureReference,
    CubemapTextureReference
>;

struct TextureUniform {
    std::string name;
    UniformTextureData data;

    bool operator==(const TextureUniform& other) const {
        if (name != other.name) return false;

        return data == other.data;
    }
};
