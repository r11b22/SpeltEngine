#pragma once

#include "glad/glad.h"


namespace Spelt {
    class MultisampledTexture {
        private:
            int mWidth, mHeight;
            GLuint mId;
            GLenum mInternalFormat;
            int mSamples;
        public:
            MultisampledTexture(int width, int height, GLenum internalFormat, int samples);
            ~MultisampledTexture();

            // Disable copying (prevents accidental deletions)
            MultisampledTexture(const MultisampledTexture&) = delete;
            MultisampledTexture& operator=(const MultisampledTexture&) = delete;

            // Enable moving (allows the optional to take ownership)
            MultisampledTexture(MultisampledTexture&& other) noexcept;
            MultisampledTexture& operator=(MultisampledTexture&& other) noexcept;

            void bind(int unit = 0);
            void imageBind(int location, GLenum rwType);

            /**
            * Make sure the applicable framebuffer is bound
            * @param attachment
            */
            void attachToFramebuffer(GLenum attachment);

            void resize(int widht, int height);

            int getWidth() const;
            int getHeight() const;
            int getSamples() const;



        private:
    };
}
