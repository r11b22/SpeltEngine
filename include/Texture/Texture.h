//
// Created by joost on 4/16/26.
//

#pragma once


#include "Asset/Asset.hpp"
#include "Error/Result.hpp"
#include "Texture/TextureData.h"
#include "glad/glad.h"

namespace Spelt {
    enum class TextureError{
        NoAttachedData,
        UnsupportedChannelCount
    };

    class Texture : public Asset {
    protected:
        int mWidth, mHeight;
        GLuint mId;
        bool mHasData = false;
        GLenum mInternalFormat, mFormat, mDatatype;
        GLenum mTextureType = GL_TEXTURE_2D;
    public:
        Texture(std::string name);
        Texture(std::string name, TextureData data);
        Texture(std::string name, int width, int height, GLenum format, GLenum internalFormat, GLenum datatype);
        Texture(std::string name, const unsigned char *texData, int width, int height, int channelCount);
        ~Texture();

        // Disable copying (prevents accidental deletions)
        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        // Enable moving (allows the optional to take ownership)
        Texture(Texture&& other) noexcept;
        Texture& operator=(Texture&& other) noexcept;

        void bind(int unit = 0);
        void imageBind(int location, GLenum rwType);

        void setParameter(GLenum name, GLenum param);

        /**
        * Make sure the applicable framebuffer is bound
        * @param attachment
        */
        void attachToFramebuffer(GLenum attachment);

        void resize(int widht, int height);

        Result<GLenum, TextureError> getFormat() const;
        Result<GLenum, TextureError> getDatatype() const;

        Result<int, TextureError> getWidth() const;
        Result<int, TextureError> getHeight() const;

        Result<void, TextureError> setPixelData(const unsigned char *texData, int width, int height, int channelCount);

    protected:
        Result<void, TextureError> setupChannelCount(int channelCount);


    private:
    };
}
