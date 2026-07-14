

#include "Error/Panic.hpp"
#include "Texture/MultisampledTexture.h"
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
        : mId(other.mId), mWidth(other.mWidth), mHeight(other.mHeight) {
        other.mId = 0;
    }

    // Move Assignment
    MultisampledTexture& MultisampledTexture::operator=(MultisampledTexture&& other) noexcept {
        if (this != &other) {
            glDeleteTextures(1, &mId);
            mId = other.mId;
            mWidth = other.mWidth;
            mHeight = other.mHeight;
            other.mId = 0;
        }
        return *this;
    }

    void MultisampledTexture::bind(int unit){
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, mId);
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
