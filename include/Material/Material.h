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

class TextureData;

class Material : public Asset{
private:
    TextureReference mTexture;
    float mDiffuse = 1.0f;
    float mSpecular = 1.0f;
    float mShininess = 32.0f;
public:
    Material(std::string name, TextureReference texture = {});

    void readyMaterial(ShaderProgram &shaderProgram, AssetManager& assetManager);

    void setDiffuse(float diffuse);
    void setSpecular(float specular);
    void setShininess(float shininess);
private:
};

#endif //SPELTENGINE_MATERIAL_H
