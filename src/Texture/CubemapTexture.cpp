#include "Texture/CubemapTexture.hpp"
#include "Buffer/Buffer.h"
#include "Texture/Texture.h"


GLenum toGLEnum(CubeFace face){
    switch (face) {
        case CubeFace::Right:
        return GL_TEXTURE_CUBE_MAP_POSITIVE_X;
        case CubeFace::Left:
        return GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
        case CubeFace::Top:
        return GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
        case CubeFace::Bottom:
        return GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
        case CubeFace::Back:
        return GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
        case CubeFace::Front:
        return GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
    }
}


CubemapTexture::CubemapTexture(std::string name, int width, int height, int channelCount)
    : Texture(name)
{
    mWidth = width;
    mHeight = height;
    setupChannelCount(channelCount);

    mTextureType = GL_TEXTURE_CUBE_MAP;

    setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    setParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    setParameter(GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void CubemapTexture::setSidePixelData(CubeFace face, const unsigned char *texData){
    bind();
    GLenum sideEnum = toGLEnum(face);

    glTexImage2D(sideEnum, 0, mInternalFormat, mWidth, mHeight, 0, mFormat, mDatatype, texData);
}
