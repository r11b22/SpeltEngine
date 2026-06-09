//
// Created by joost on 6/9/26.
//

#ifndef SPELTENGINE_MATERIAL_H
#define SPELTENGINE_MATERIAL_H
#include <optional>

#include "ModelLoader.h"
#include "Texture.h"
#include "Shaders/ShaderProgram.h"

class Material {
private:
    std::optional<Texture> mTexture;
public:
    void readyMaterial(ShaderProgram &shaderProgram);

    void setTexture(const TextureData& texData);
private:
};

#endif //SPELTENGINE_MATERIAL_H