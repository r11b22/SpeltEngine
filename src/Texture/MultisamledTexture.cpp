

#include "Error/Panic.hpp"
#include "OpenGL/TextureBindTracker.hpp"
#include "Texture/MultisampledTexture.h"
#include "Window.h"
#include <iostream>
#include <stdexcept>


namespace Spelt {
    MultisampledTexture::MultisampledTexture(int width, int height, GLenum internalFormat, int samples)
        : mInternalFormat(internalFormat), mWidth(width), mHeight(height), mSamples(samples)
    {
        glGenTextures(1, &mId);
        if (mId == 0) {
            fatalPanic("Failed to create new texture!");
        }

        bind();



        glTexImage2DMultisample(
            GL_TEXTURE_2D_MULTISAMPLE,
            mSamples,
            mInternalFormat,
            mWidth,
            mHeight,
            GL_TRUE
        );
    }

    MultisampledTexture::~MultisampledTexture() {
        if (mId != 0) {
            glDeleteTextures(1, &mId);
        }
    }

    MultisampledTexture::MultisampledTexture(MultisampledTexture&& other) noexcept
        : mId(other.mId),
          mWidth(other.mWidth),
          mHeight(other.mHeight),
          mInternalFormat(other.mInternalFormat),
          mSamples(other.mSamples)
    {
        other.mId = 0;
    }
    // Move Assignment
    MultisampledTexture& MultisampledTexture::operator=(MultisampledTexture&& other) noexcept {
        if (this != &other) {
            if (mId != 0) {
                glDeleteTextures(1, &mId);
            }
            mId = other.mId;
            mWidth = other.mWidth;
            mHeight = other.mHeight;
            mInternalFormat = other.mInternalFormat;
            mSamples = other.mSamples;
            other.mId = 0;
        }
        return *this;
    }

    void MultisampledTexture::bind(int unit){
        if(!TextureBindTracker::getInstance().isBound(unit, mId)){
            glActiveTexture(GL_TEXTURE0 + unit);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, mId);
            TextureBindTracker::getInstance().bind(unit, mId);
        }
    }

    void MultisampledTexture::imageBind(int location, GLenum rwType){
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

    void MultisampledTexture::attachToFramebuffer(GLenum attachment) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D_MULTISAMPLE, mId, 0);
    }

    void MultisampledTexture::resize(int widht, int height) {
        mWidth = widht;
        mHeight = height;
        bind();

        glTexImage2DMultisample(
            GL_TEXTURE_2D_MULTISAMPLE,
            mSamples,
            mInternalFormat,
            mWidth,
            mHeight,
            GL_TRUE
        );
    }

    int MultisampledTexture::getWidth() const {
        return mWidth;
    }

    int MultisampledTexture::getHeight() const {
        return mHeight;
    }

    int MultisampledTexture::getSamples() const {
        return mSamples;
    }
}
