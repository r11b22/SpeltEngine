//
// Created by joost on 6/9/26.
//

#pragma once

#include <optional>


#include "Asset/Asset.hpp"
#include "Asset/AssetManager.hpp"
#include "Shader/ShaderProgram.h"
#include "Texture/TextureReference.hpp"
#include "glm/ext/vector_float3.hpp"
#include "Hashing/Hashing.hpp"

namespace Spelt {
    class TextureData;

    enum class MaterialType {
        Colored = 0,
        Textured = 1
    };

    class Material : public Asset{
    private:
        MaterialType mType;

        TextureReference mTexture;
        glm::vec3 mColor;

        float mDiffuse = 1.0f;
        float mSpecular = 1.0f;
        float mShininess = 32.0f;
    public:
        Material(std::string name, TextureReference texture);
        Material(std::string name, glm::vec3 color);

        bool operator==(const Material& other) const;

        void readyMaterial(ShaderProgram &shaderProgram, AssetManager& assetManager);

        void setDiffuse(float diffuse);
        void setSpecular(float specular);
        void setShininess(float shininess);

        friend struct std::hash<Material>;
    private:
        void readyTextured(ShaderProgram &shaderProgram, AssetManager& assetManager);
        void readyColored(ShaderProgram &shaderProgram, AssetManager& assetManager);
    };

}

namespace std {
    template<>
    struct hash<Spelt::Material> {
        std::size_t operator()(const Spelt::Material& mat) const {
            std::size_t seed = 0;

            // 1. Hash the base asset name (assuming Asset provides getName())
            Spelt::hash_combine(seed, mat.getName());

            // 2. Hash the material type enum (cast to underlying int type)
            Spelt::hash_combine(seed, static_cast<int>(mat.mType));

            // 3. Hash type-specific details
            if (mat.mType == Spelt::MaterialType::Textured) {
                Spelt::hash_combine(seed, mat.mTexture.getID()); // Using the .id field from your earlier example
            } else if (mat.mType == Spelt::MaterialType::Colored) {
                Spelt::hash_combine(seed, mat.mColor.x);
                Spelt::hash_combine(seed, mat.mColor.y);
                Spelt::hash_combine(seed, mat.mColor.z);
            }

            // 4. Hash the unique material parameters
            Spelt::hash_combine(seed, mat.mDiffuse);
            Spelt::hash_combine(seed, mat.mSpecular);
            Spelt::hash_combine(seed, mat.mShininess);

            return seed;
        }
    };


}
