//
// Created by joost on 4/16/26.
//

#include "Texture/Texture.h"
#include "Asset/Asset.hpp"
#include "Error/Panic.hpp"
#include "Error/Result.hpp"
#include "OpenGL/TextureBindTracker.hpp"

#include <iostream>
#include <ostream>
#include <stdexcept>
#include <string>


namespace Spelt {
    Texture::Texture(std::string name)
        : Asset(name)
    {
        glGenTextures(1, &mId);

        if (mId == 0) {
            fatalPanic("Failed to create new texture!");
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

        setPixelData(texData, width, height, channelCount).panicOnError(std::format("Unsupported channelcount: {}", channelCount));
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
        if(!TextureBindTracker::getInstance().isBound(unit, mId)){
            glActiveTexture(GL_TEXTURE0 + unit);
            glBindTexture(GL_TEXTURE_2D, mId);
            TextureBindTracker::getInstance().bind(unit, mId);
        }
    }

    void Texture::imageBind(int location, GLenum rwType){
        glBindImageTexture(
            location,
            mId,
            0,
            GL_FALSE,
            0,
            rwType,
            mInternalFormat
        );
    }

    Texture::Texture(Texture&& other) noexcept
        : Asset(other.getName()), mId(other.mId), mWidth(other.mWidth), mHeight(other.mHeight), mInternalFormat(other.mInternalFormat), mFormat(other.mFormat), mDatatype(other.mDatatype), mHasData(other.mHasData), mTextureType(other.mTextureType)
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
            mTextureType = other.mTextureType;

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


    Result<GLenum, TextureError> Texture::getFormat() const {
        if (!mHasData){
            return Error(TextureError::NoAttachedData);
        }
        return Value(mFormat);
    }

    Result<GLenum, TextureError> Texture::getDatatype() const {
        if (!mHasData){
            return Error(TextureError::NoAttachedData);
        }
        return Value(mDatatype);
    }

    Result<int, TextureError> Texture::getWidth() const {
        if (!mHasData){
            return Error(TextureError::NoAttachedData);
        }
        return Value(mWidth);
    }

    Result<int, TextureError> Texture::getHeight() const {
        if (!mHasData){
            return Error(TextureError::NoAttachedData);
        }
        return Value(mHeight);
    }

    Result<void, TextureError> Texture::setPixelData(const unsigned char *texData, int width, int height, int channelCount){
        bind();

        mHasData = true;
        mWidth = width;
        mHeight = height;

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        auto result = setupChannelCount(channelCount);
        if(result.isError()){
            return Error(result.error());
        }

        if (channelCount == 4) {
            std::cout << "Texture uploaded in alpha mode" << std::endl;
        }else {
            std::cout << "Texture uploaded without alpha" << std::endl;
        }


        glTexImage2D(mTextureType, 0, mInternalFormat, mWidth, mHeight, 0, mFormat, mDatatype, texData);
        glGenerateMipmap(mTextureType);
        return Success{};
    }


    Result<void, TextureError> Texture::setupChannelCount(int channelCount) {
        mDatatype = GL_UNSIGNED_BYTE;

        switch (channelCount) {
            case 1: // Grayscale (e.g., your planet depth maps)
                mInternalFormat = GL_R8;
                mFormat = GL_RED;
                break;

            case 2: // Grayscale + Alpha
                mInternalFormat = GL_RG8;
                mFormat = GL_RG;
                break;

            case 3: // Standard RGB
                mInternalFormat = GL_RGB8;
                mFormat = GL_RGB;
                break;

            case 4: // Standard RGBA
                mInternalFormat = GL_RGBA8;
                mFormat = GL_RGBA;
                break;

            default:
            return Error(TextureError::UnsupportedChannelCount);
        }
        return Success{};
    }
}
