//
// Created by joost on 4/16/26.
//

#include "Texture/Texture.h"
#include "Asset/Asset.hpp"

#include <iostream>
#include <ostream>
#include <stdexcept>
#include <string>

Texture::Texture(std::string name)
    : Asset(name)
{
    glGenTextures(1, &mId);

    if (mId == 0) {
        throw std::runtime_error("Failed to create new texture!");
    }
}

Texture::Texture(std::string name, TextureData data)
    : Texture(name, data.texData, data.texWidth, data.texHeight, data.channelCount)
{

}

Texture::Texture(std::string name, const unsigned char *texData, int width, int height, int channelCount)
    : Texture(name)
{
    bind();

    setParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
    setParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
    setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    setPixelData(texData, width, height, channelCount);
}

Texture::Texture(std::string name, int width, int height, GLenum format, GLenum internalFormat, GLenum datatype)
    : Texture(name)
{
    mInternalFormat = internalFormat;
    mFormat = format;
    mDatatype = datatype;
    mHasData = true;

    bind();

    setParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
    setParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
    setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    mWidth = width;
    mHeight = height;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(mTextureType, 0, internalFormat, mWidth, mHeight, 0, format, datatype, nullptr);
    glGenerateMipmap(mTextureType);
}

void Texture::bind(int unit){
    glActiveTexture(GL_TEXTURE0 + unit);
    if(mTextureType != GL_TEXTURE_2D){
        std::cout << mTextureType << std::endl;
    }

    glBindTexture(mTextureType, mId);
}

Texture::Texture(Texture&& other) noexcept
    : Asset(other.getName()), mId(other.mId), mWidth(other.mWidth), mHeight(other.mHeight), mInternalFormat(other.mInternalFormat), mFormat(other.mFormat), mDatatype(other.mDatatype), mHasData(other.mHasData)
{
    other.mHasData = false;
    other.mId = 0;
}

// Move Assignment
Texture& Texture::operator=(Texture&& other) noexcept {
    if (this != &other) {
        glDeleteTextures(1, &mId);
        mId = other.mId;
        mWidth = other.mWidth;
        mHeight = other.mHeight;
        mDatatype = other.mDatatype;
        mFormat = other.mFormat;
        mInternalFormat = other.mInternalFormat;
        mHasData = other.mHasData;

        other.mHasData = false;
        other.mId = 0;
    }
    return *this;
}

Texture::~Texture() {
    if (mId != 0) {
        glDeleteTextures(1, &mId);
    }
}

void Texture::setParameter(GLenum name, GLenum param) {
    bind();
    glTexParameteri(mTextureType, name, param);
}

void Texture::attachToFramebuffer(GLenum attachment) {
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, mTextureType, mId, 0);
}

void Texture::resize(int widht, int height) {
    mWidth = widht;
    mHeight = height;
    if (mHasData){
        bind();
        glTexImage2D(mTextureType, 0, mInternalFormat, mWidth, mHeight, 0, mFormat, mDatatype, nullptr);
    }
}


GLenum Texture::getFormat() const {
    if (!mHasData){
        throw std::runtime_error("Tried to get format for a texture that has no attached data");
    }
    return mFormat;
}

GLenum Texture::getDatatype() const {
    if (!mHasData){
        throw std::runtime_error("Tried to get datatype for a texture that has no attached data");
    }
    return mDatatype;
}

int Texture::getWidth() const {
    if (!mHasData){
        throw std::runtime_error("Tried to get width for a texture that has no attached data");
    }
    return mWidth;
}

int Texture::getHeight() const {
    if (!mHasData){
        throw std::runtime_error("Tried to get height for a texture that has no attached data");
    }
    return mHeight;
}

void Texture::setPixelData(const unsigned char *texData, int width, int height, int channelCount){
    bind();

    mHasData = true;
    mWidth = width;
    mHeight = height;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    setupChannelCount(channelCount);


    if (channelCount == 4) {
        std::cout << "Texture uploaded in alpha mode" << std::endl;
    }else {
        std::cout << "Texture uploaded without alpha" << std::endl;
    }


    glTexImage2D(mTextureType, 0, mInternalFormat, mWidth, mHeight, 0, mFormat, mDatatype, texData);
    glGenerateMipmap(mTextureType);
}


void Texture::setupChannelCount(int channelCount){
    mInternalFormat = (channelCount == 4) ? GL_RGBA8 : GL_RGB8;
    mFormat = (channelCount == 4) ? GL_RGBA : GL_RGB;
    mDatatype = GL_UNSIGNED_BYTE;
}
