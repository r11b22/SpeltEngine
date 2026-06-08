//
// Created by joost on 5/9/26.
//

#ifndef CGVCPROJECT_FRAMEBUFFER_H
#define CGVCPROJECT_FRAMEBUFFER_H
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <vector>

#include "Texture.h"
#include "Window.h"
#include "glad/glad.h"

template <typename T>
class FrameBuffer {
private:
    int mWidth, mHeight;

    uint32_t mFrameBuffer;
    uint32_t mRenderBuffer;
    Window* mWindow;
    std::vector<Texture*> mAttachedTextures;

    int mCallbackId = -1;
public:
    FrameBuffer(Window *window)
        : mWidth(window->getWidth()),
        mHeight(window->getHeight()),
        mFrameBuffer(0),
        mRenderBuffer(0),
        mWindow(window)
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
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, mWidth, mHeight);

        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, mRenderBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        mCallbackId = window->addResizeCallback([this](int width, int height) {
            this->resize(width, height);
        });
    }

    ~FrameBuffer() {
        cleanup(); // Fixed: Utilizing the cleanup function here
    }

    // Disable copying
    FrameBuffer(const FrameBuffer&) = delete;
    FrameBuffer& operator=(const FrameBuffer&) = delete;

    // Move Constructor
    FrameBuffer(FrameBuffer&& other) noexcept
        : mWidth(other.mWidth),
          mHeight(other.mHeight),
          mFrameBuffer(other.mFrameBuffer),
          mRenderBuffer(other.mRenderBuffer),
          mWindow(other.mWindow),
          mAttachedTextures(std::move(other.mAttachedTextures)),
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

    // Move Assignment
    FrameBuffer& operator=(FrameBuffer&& other) noexcept {
        if (this != &other) {
            cleanup(); // Clean up current FBO resources first

            mWidth = other.mWidth;
            mHeight = other.mHeight;
            mFrameBuffer = other.mFrameBuffer;
            mRenderBuffer = other.mRenderBuffer;
            mWindow = other.mWindow;
            mAttachedTextures = std::move(other.mAttachedTextures);

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

    void bind() {
        glBindFramebuffer(GL_FRAMEBUFFER, mFrameBuffer);
    }
    void unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    void attachTexture(Texture *tex, GLenum attachment) {
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

    void clearAttachments() {
        bind();
        for (int i = 0; i < 4; ++i) {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, 0, 0);
        }
        unbind();
        mAttachedTextures.clear();
    }

    void setAttachments(const std::vector<GLenum> &attachments) {
        bind();
        glDrawBuffers(static_cast<GLsizei>(attachments.size()), attachments.data());

        checkCompleteness();
        unbind();
    }

    T getAtPixel(int x, int y) {
        if (!mAttachedTextures.empty() && mAttachedTextures[0]) {
            bind();
            T result = 0;
            glReadPixels(x, y, 1, 1, mAttachedTextures[0]->getFormat(), mAttachedTextures[0]->getDatatype(), &result);
            unbind();

            return result;
        }
        throw std::runtime_error("Can not get pixel data from incomplete FrameBuffer, attach a texture before calling getPixelAt!");
    }

private:
    void cleanup() {
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
    void resize(int width, int height) {
        mWidth = width;
        mHeight = height;

        for (const auto& tex : mAttachedTextures) {
            if (tex) {
                tex->resize(mWidth, mHeight);
            }
        }

        glBindRenderbuffer(GL_RENDERBUFFER, mRenderBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, mWidth, mHeight);
    }
    void checkCompleteness() {
        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            std::string errorMsg = "Framebuffer did not complete! Error code: 0x";
            std::cerr << errorMsg << std::hex << status << std::dec << std::endl;
            throw std::runtime_error("Framebuffer completeness check failed.");
        }
    }
};

#endif //CGVCPROJECT_FRAMEBUFFER_H