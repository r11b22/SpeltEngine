//
// Created by joost on 6/9/26.
//

#ifndef SPELTENGINE_MATERIAL_H
#define SPELTENGINE_MATERIAL_H
#include <optional>


#include "Texture/Texture.h"
#include "Shader/ShaderProgram.h"

class TextureData;

class Material {
private:
    std::optional<Texture> mTexture;
    float mDiffuse = 1.0f;
    float mSpecular = 1.0f;
    float mShininess = 32.0f;
public:
    void readyMaterial(ShaderProgram &shaderProgram);

    void setTexture(const TextureData& texData);

    void setDiffuse(float diffuse);
    void setSpecular(float specular);
    void setShininess(float shininess);
private:
};

#endif //SPELTENGINE_MATERIAL_H
