#pragma once

#include "Texture/Texture.h"

enum class CubeFace : int {
    Right = 0,
    Left = 1,
    Top = 2,
    Bottom = 3,
    Front = 4,
    Back = 5
};

GLenum toGLEnum(CubeFace face);

class CubemapTexture : public Texture {
    private:
    public:
        CubemapTexture(std::string name, int width, int height, int channelCount);

        void setSidePixelData(CubeFace face, const unsigned char *texData);

        void initEmpty();

        /**
         * Binds the multiple sides of the cubemap as a layered 3D ImageTexture
         * Layers:
         * 0 = +X = Right
         * 1 = -X = Left
         * 2 = +Y = Top
         * 3 = -Y = Bottom
         * 4 = +Z = Front
         * 5 = -Z = Back
         */
        void imageBind(int location, GLenum rwType);

        /**
         * Binds a single side of the cubemap as a 2D ImageTexture
         */
        void singleSideImageBind(int location, CubeFace face, GLenum rwType);
    private:
};
