//
// Created by joost on 5/1/26.
//

#ifndef CGVCPROJECT_PICKBUFFER_H
#define CGVCPROJECT_PICKBUFFER_H

#include "FrameBuffer.h"
#include "Texture.h"
#include "Window.h"

class PickBuffer {
private:
    int mWidth, mHeight;

    FrameBuffer<unsigned int> mFrameBuffer;
    Texture mOutputTexture;

public:
    PickBuffer(Window* window);

    void bind();
    void unbind();

    unsigned int getAtPixel(int x, int y);
private:
    void resize(int width, int height);
};


#endif //CGVCPROJECT_PICKBUFFER_H