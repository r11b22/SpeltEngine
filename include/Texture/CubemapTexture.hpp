#pragma once

#include "Texture/Texture.h"

enum class CubeFace {
    Right,
    Left,
    Top,
    Bottom,
    Back,
    Front
};

GLenum toGLEnum(CubeFace face);

class CubemapTexture : public Texture {
    private:
    public:
        CubemapTexture(std::string name, int width, int height, int channelCount);

        void setSidePixelData(CubeFace face, const unsigned char *texData);
    private:
};
