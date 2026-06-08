//
// Created by joost on 4/16/26.
//

#include "Texture.h"

#include <iostream>
#include <ostream>
#include <stdexcept>

Texture::Texture(const unsigned char *texData, int width, int height, int channelCount) {
    glGenTextures(1, &mId);

    if (mId == 0) {
        throw std::runtime_error("Failed to create new texture!");
    }

    bind();

    mWidth = width;
    mHeight = height;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    mInternalFormat = (channelCount == 4) ? GL_RGBA8 : GL_RGB8;
    mFormat = (channelCount == 4) ? GL_RGBA : GL_RGB;
    mDatatype = GL_UNSIGNED_BYTE;


    if (channelCount == 4) {
        std::cout << "Texture uploaded in alpha mode" << std::endl;
    }else {
        std::cout << "Texture uploaded without alpha" << std::endl;
    }


    glTexImage2D(GL_TEXTURE_2D, 0, mInternalFormat, mWidth, mHeight, 0, mFormat, mDatatype, texData);
    glGenerateMipmap(GL_TEXTURE_2D);
}

Texture::Texture(int width, int height, GLenum format, GLenum internalFormat, GLenum datatype)
    : mInternalFormat(internalFormat), mFormat(format), mDatatype(datatype)
{

    glGenTextures(1, &mId);

    if (mId == 0) {
        throw std::runtime_error("Failed to create new texture!");
    }

    bind();

    setParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
    setParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
    setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    mWidth = width;
    mHeight = height;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, mWidth, mHeight, 0, format, datatype, nullptr);
    glGenerateMipmap(GL_TEXTURE_2D);
}

void Texture::bind(int unit){
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, mId);
}

Texture::Texture(Texture&& other) noexcept
    : mId(other.mId), mWidth(other.mWidth), mHeight(other.mHeight) {
    other.mId = 0;
}

// Move Assignment
Texture& Texture::operator=(Texture&& other) noexcept {
    if (this != &other) {
        glDeleteTextures(1, &mId);
        mId = other.mId;
        mWidth = other.mWidth;
        mHeight = other.mHeight;
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
    glTexParameteri(GL_TEXTURE_2D, name, param);
}

void Texture::attachToFramebuffer(GLenum attachment) {
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, mId, 0);
}

void Texture::resize(int widht, int height) {
    mWidth = widht;
    mHeight = height;
    bind();
    glTexImage2D(GL_TEXTURE_2D, 0, mInternalFormat, mWidth, mHeight, 0, mFormat, mDatatype, nullptr);
}


GLenum Texture::getFormat() const {
    return mFormat;
}

GLenum Texture::getDatatype() const {
    return mDatatype;
}

int Texture::getWidth() const {
    return mWidth;
}

int Texture::getHeight() const {
    return mHeight;
}
