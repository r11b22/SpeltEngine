//
// Created by joost on 6/9/26.
//


#include "Material/Material.h"
#include "Asset/Asset.hpp"
#include "Texture/Texture.h"
#include "Texture/TextureReference.hpp"

Material::Material(std::string name, TextureReference texture)
    : Asset(name), mTexture(texture), mColor(), mType(MaterialType::Textured)
{

}

Material::Material(std::string name, glm::vec3 color)
    : Asset(name), mTexture(), mColor(color), mType(MaterialType::Colored)
{

}

bool Material::operator==(const Material& other) const {
    if (mType != other.mType)
        return false;

    switch (mType) {
        case MaterialType::Textured:
            if (mTexture != other.mTexture) return false;
        case MaterialType::Colored:
            if (mColor != other.mColor) return false;
    }

    return mDiffuse == other.mDiffuse
    && mSpecular == other.mSpecular
    && mShininess == other.mShininess;
}

void Material::readyMaterial(ShaderProgram &shaderProgram,  AssetManager& assetManager) {

    switch (mType) {
        case MaterialType::Colored:
            readyColored(shaderProgram, assetManager);
            break;
        case MaterialType::Textured:
            readyTextured(shaderProgram, assetManager);
            break;
    }

    shaderProgram.setUniformInt("uMaterialProperties.materialType", static_cast<int>(mType));
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


void Material::readyTextured(ShaderProgram &shaderProgram, AssetManager& assetManager){
    shaderProgram.setUniformInt("uTexture", 0);

    if (!mTexture.isNoReference()){
        Texture* tex = assetManager.getAsset<Texture>(mTexture);
        if(tex){
            tex->bind(0);
        }
    }
}

void Material::readyColored(ShaderProgram &shaderProgram, AssetManager& assetManager) {
    shaderProgram.setUniformVec3("uColor", mColor);
}
