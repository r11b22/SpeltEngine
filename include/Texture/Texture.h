//
// Created by joost on 4/16/26.
//

#ifndef CGVCPROJECT_TEXTURE_H
#define CGVCPROJECT_TEXTURE_H
#include "Asset/Asset.hpp"
#include "Texture/TextureData.h"
#include "glad/glad.h"

class Texture : public Asset {
private:
    int mWidth, mHeight;
    GLuint mId;
    GLenum mInternalFormat, mFormat, mDatatype;
public:
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



private:
};

#endif //CGVCPROJECT_TEXTURE_H
