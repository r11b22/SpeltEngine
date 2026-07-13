#pragma once

#include "FrameBuffer/FrameBuffer.h"
#include "Texture/MultisampledTexture.h"
#include "Texture/Texture.h"
#include "Window.h"

namespace Spelt {

    class MultisampledFrameBuffer {

        private:
            int mWidth, mHeight;

            uint32_t mFrameBuffer;
            uint32_t mRenderBuffer;
            Window* mWindow;
            std::vector<MultisampledTexture*> mAttachedTextures;

            int mCallbackId = -1;
            int mSamples;
        public:
            MultisampledFrameBuffer(Window *window, int samples);

            ~MultisampledFrameBuffer();
            // Disable copying
            MultisampledFrameBuffer(const MultisampledFrameBuffer&) = delete;
            MultisampledFrameBuffer& operator=(const MultisampledFrameBuffer&) = delete;

            // Move Constructor
            MultisampledFrameBuffer(MultisampledFrameBuffer&& other) noexcept;

            // Move Assignment
            MultisampledFrameBuffer& operator=(MultisampledFrameBuffer&& other) noexcept;

            void bind();

            void bindRead();
            void bindDraw();

            void unbind();
            void unbindRead();
            void unbindDraw();

            void attachTexture(MultisampledTexture *tex, GLenum attachment);

            void clearAttachments();

            void setAttachments(const std::vector<GLenum> &attachments);

            int getWidth() const;
            int getHeight() const;

        private:
            void cleanup();
            void resize(int width, int height);
            Result<void, FrameBufferError> checkCompleteness();

    };
}
