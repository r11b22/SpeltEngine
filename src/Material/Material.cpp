//
// Created by joost on 6/9/26.
//


#include "Material/Material.h"
#include "Texture/Texture.h"
#include "Texture/TextureReference.hpp"

Material::Material(std::string name, TextureReference texture)
    : Asset(name), mTexture(texture)
{

}

void Material::readyMaterial(ShaderProgram &shaderProgram,  AssetManager& assetManager) {

    shaderProgram.setUniformInt("uTexture", 0);

    if (!mTexture.isNoReference()){
        Texture* tex = assetManager.getTexture(mTexture);
        if(tex){
            tex->bind(0);
        }
    }

    shaderProgram.setUniformFloat("uMaterialProperties.diffuse", mDiffuse);
    shaderProgram.setUniformFloat("uMaterialProperties.specular", mSpecular);
    shaderProgram.setUniformFloat("uMaterialProperties.shininess", mShininess);
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
