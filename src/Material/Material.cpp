//
// Created by joost on 6/9/26.
//


#include "Material/Material.h"
#include "ModelLoader.h"

void Material::readyMaterial(ShaderProgram &shaderProgram) {

    shaderProgram.setUniformInt("uTexture", 0);
    if (mTexture.has_value()) {
        mTexture.value().bind(0);
    }

    shaderProgram.setUniformFloat("uMaterialProperties.diffuse", mDiffuse);
    shaderProgram.setUniformFloat("uMaterialProperties.specular", mSpecular);
    shaderProgram.setUniformFloat("uMaterialProperties.shininess", mShininess);
}

void Material::setTexture(const TextureData &texData) {
    mTexture = Texture(texData.texData, texData.texWidth, texData.texHeight, texData.channelCount);
}


void Material::setDiffuse(float diffuse){
    mDiffuse = diffuse;
}

void Material::setSpecular(float specular){
    mSpecular = specular;
}

void Material::setShininess(float shininess){
    mShininess = shininess;
}
