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

        /**
         * Binds the multiple sides of the cubemap as a layered 3D Texture
         * Layers:
         * 0 = +X = Right
         * 1 = -X = Left
         * 2 = +Y = Top
         * 3 = -Y = Bottom
         * 4 = +Z = Front
         * 5 = -Z = Back
         */
        void imageBind(int location, GLenum rwType);
    private:
};
