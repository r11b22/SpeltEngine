//
// Created by joost on 4/16/26.
//

#ifndef CGVCPROJECT_TEXTURE_H
#define CGVCPROJECT_TEXTURE_H
#include "Asset/Asset.hpp"
#include "Texture/TextureData.h"
#include "glad/glad.h"

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

    GLenum getFormat() const;
    GLenum getDatatype() const;

    int getWidth() const;
    int getHeight() const;

    void setPixelData(const unsigned char *texData, int width, int height, int channelCount);

protected:
    void setupChannelCount(int channelCount);


private:
};

#endif //CGVCPROJECT_TEXTURE_H
