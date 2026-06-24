//
// Created by joost on 6/9/26.
//

#ifndef SPELTENGINE_MATERIAL_H
#define SPELTENGINE_MATERIAL_H
#include <optional>


#include "Asset/Asset.hpp"
#include "Asset/AssetManager.hpp"
#include "Shader/ShaderProgram.h"
#include "Texture/TextureReference.hpp"
#include "glm/ext/vector_float3.hpp"

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

    void readyMaterial(ShaderProgram &shaderProgram, AssetManager& assetManager);

    void setDiffuse(float diffuse);
    void setSpecular(float specular);
    void setShininess(float shininess);
private:
    void readyTextured(ShaderProgram &shaderProgram, AssetManager& assetManager);
    void readyColored(ShaderProgram &shaderProgram, AssetManager& assetManager);
};

#endif //SPELTENGINE_MATERIAL_H
