#include "FrameBuffer/MultisampledFrameBuffer.h"
#include "Texture/MultisampledTexture.h"
#include <algorithm>
#include <iostream>
#include <stdexcept>


namespace Spelt {
    MultisampledFrameBuffer::MultisampledFrameBuffer(Window *window, int samples)
        : mWidth(window->getWidth()),
        mHeight(window->getHeight()),
        mFrameBuffer(0),
        mRenderBuffer(0),
        mWindow(window),
        mSamples(samples)
    {
        glGenFramebuffers(1, &mFrameBuffer);
        if (mFrameBuffer == 0) {
            throw std::runtime_error("Failed to create FrameBuffer!");
        }

        glGenRenderbuffers(1, &mRenderBuffer);
        if (mRenderBuffer == 0) {
            glDeleteFramebuffers(1, &mFrameBuffer);
            mFrameBuffer = 0;
            throw std::runtime_error("Failed to create FrameBuffer depth buffer!");
        }

        glBindFramebuffer(GL_FRAMEBUFFER, mFrameBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, mRenderBuffer);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH24_STENCIL8, mWidth, mHeight);

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mRenderBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        mCallbackId = window->addResizeCallback([this](int width, int height) {
            this->resize(width, height);
        });
    }

    MultisampledFrameBuffer::~MultisampledFrameBuffer(){
        cleanup();
    }


    MultisampledFrameBuffer::MultisampledFrameBuffer(MultisampledFrameBuffer&& other) noexcept
        : mWidth(other.mWidth),
        mHeight(other.mHeight),
        mFrameBuffer(other.mFrameBuffer),
        mRenderBuffer(other.mRenderBuffer),
        mWindow(other.mWindow),
        mAttachedTextures(std::move(other.mAttachedTextures)),
        mSamples(other.mSamples),
        mCallbackId(-1) // Start at -1, we will register our own shortly
    {
        // 1. Unregister the old object's callback so it stops firing at dead memory
        if (other.mCallbackId != -1 && other.mWindow != nullptr) {
            other.mWindow->removeResizeCallback(other.mCallbackId);
            other.mCallbackId = -1;
        }

        // 2. Register a completely fresh callback bound to OUR 'this' pointer
        if (mWindow != nullptr) {
            mCallbackId = mWindow->addResizeCallback([this](int width, int height) {
                this->resize(width, height);
            });
        }

        // 3. Prevent the old object from touching OpenGL state
        other.mFrameBuffer = 0;
        other.mRenderBuffer = 0;
        other.mWindow = nullptr;
    }

    MultisampledFrameBuffer& MultisampledFrameBuffer::operator=(MultisampledFrameBuffer&& other) noexcept {
        if (this != &other) {
            cleanup(); // Clean up current FBO resources first

            mWidth = other.mWidth;
            mHeight = other.mHeight;
            mFrameBuffer = other.mFrameBuffer;
            mRenderBuffer = other.mRenderBuffer;
            mWindow = other.mWindow;
            mAttachedTextures = std::move(other.mAttachedTextures);
            mSamples = other.mSamples;

            // Deregister the old object's callback
            if (other.mCallbackId != -1 && other.mWindow != nullptr) {
                other.mWindow->removeResizeCallback(other.mCallbackId);
                other.mCallbackId = -1;
            }

            // Register our new callback tracking the updated memory address
            if (mWindow != nullptr) {
                mCallbackId = mWindow->addResizeCallback([this](int width, int height) {
                    this->resize(width, height);
                });
            } else {
                mCallbackId = -1;
            }

            other.mFrameBuffer = 0;
            other.mRenderBuffer = 0;
            other.mWindow = nullptr;
        }
        return *this;
    }

    void MultisampledFrameBuffer::bind() {
        glBindFramebuffer(GL_FRAMEBUFFER, mFrameBuffer);
    }

    void MultisampledFrameBuffer::bindRead() {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, mFrameBuffer);
    }

    void MultisampledFrameBuffer::bindDraw() {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mFrameBuffer);
    }


    void MultisampledFrameBuffer::unbindRead() {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    }

    void MultisampledFrameBuffer::unbindDraw() {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    }

    void MultisampledFrameBuffer::unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void MultisampledFrameBuffer::attachTexture(MultisampledTexture *tex, GLenum attachment){
        bind();
        if (tex) {
            tex->resize(mWidth, mHeight);
            tex->attachToFramebuffer(attachment);

            if (std::find(mAttachedTextures.begin(), mAttachedTextures.end(), tex) == mAttachedTextures.end()) {
                mAttachedTextures.push_back(tex);
            }
        }
        unbind();
    }

    void MultisampledFrameBuffer::clearAttachments() {
        bind();
        for (int i = 0; i < 4; ++i) {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, 0, 0, 0);
        }
        unbind();
        mAttachedTextures.clear();
    }

    void MultisampledFrameBuffer::setAttachments(const std::vector<GLenum> &attachments){
        bind();
        glDrawBuffers(static_cast<GLsizei>(attachments.size()), attachments.data());

        checkCompleteness();
        unbind();
    }

    int MultisampledFrameBuffer::getWidth() const {
        return mWidth;
    }

    int MultisampledFrameBuffer::getHeight() const{
        return mHeight;
    }


    void MultisampledFrameBuffer::cleanup() {
        if (mCallbackId != -1 && mWindow != nullptr) {
            mWindow->removeResizeCallback(mCallbackId);
            mCallbackId = -1;
        }
        if (mFrameBuffer != 0) {
            glDeleteFramebuffers(1, &mFrameBuffer);
            mFrameBuffer = 0;
        }
        if (mRenderBuffer != 0) {
            glDeleteRenderbuffers(1, &mRenderBuffer);
            mRenderBuffer = 0;
        }
    }
    void MultisampledFrameBuffer::resize(int width, int height) {
        mWidth = width;
        mHeight = height;

        for (const auto& tex : mAttachedTextures) {
            if (tex) {
                tex->resize(mWidth, mHeight);
            }
        }

        glBindRenderbuffer(GL_RENDERBUFFER, mRenderBuffer);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, mSamples, GL_DEPTH24_STENCIL8, mWidth, mHeight);
    }
    void MultisampledFrameBuffer::checkCompleteness() {
        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            std::string errorMsg = "Framebuffer did not complete! Error code: 0x";
            std::cerr << errorMsg << std::hex << status << std::dec << std::endl;
            throw std::runtime_error("Framebuffer completeness check failed.");
        }
    }
}
