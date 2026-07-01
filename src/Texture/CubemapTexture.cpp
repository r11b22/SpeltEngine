#include "Texture/CubemapTexture.hpp"
#include "Buffer/Buffer.h"
#include "Texture/Texture.h"
#include "Window.h"
#include <iostream>


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
        return GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
        case CubeFace::Front:
        return GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
    }
}


CubemapTexture::CubemapTexture(std::string name, int width, int height, int channelCount)
    : Texture(name)
{
    mTextureType = GL_TEXTURE_CUBE_MAP;

    mWidth = width;
    mHeight = height;
    setupChannelCount(channelCount);


    bind();

    setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    setParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    setParameter(GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

void CubemapTexture::setSidePixelData(CubeFace face, const unsigned char *texData){
    bind();
    GLenum sideEnum = toGLEnum(face);

    glTexImage2D(sideEnum, 0, mInternalFormat, mWidth, mHeight, 0, mFormat, mDatatype, texData);
    mHasData = true;
}

void CubemapTexture::initEmpty(){
    bind();
    for(int i = 0; i < 6; i++){
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, mInternalFormat, mWidth, mHeight, 0, mFormat, mDatatype, nullptr);
    }
    mHasData = true;
}

void CubemapTexture::imageBind(int location, GLenum rwType){
    glBindImageTexture(
        location,
        mId,
        0,
        GL_TRUE,
        0,
        rwType,
        mInternalFormat
    );
}

void CubemapTexture::singleSideImageBind(int location, CubeFace face, GLenum rwType){
    glBindImageTexture(
        location,
        mId,
        0,
        GL_FALSE,
        static_cast<int>(face),
        rwType,
        mInternalFormat
    );
}
