//
// Created by joost on 6/9/26.
//


#include "Material/Material.h"
#include "ModelLoader.h"

void Material::readyMaterial(ShaderProgram &shaderProgram) {
    shaderProgram.setUniformFloat("uSpecularStrength", 0.5f);

    shaderProgram.setUniformInt("uTexture", 0);
    if (mTexture.has_value()) {
        mTexture.value().bind(0);
    }
}

void Material::setTexture(const TextureData &texData) {
    mTexture = Texture(texData.texData, texData.texWidth, texData.texHeight, texData.channelCount);
}
