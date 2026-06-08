//
// Created by joost on 5/1/26.
//

#include "../include/PickBuffer.h"

#include <stdexcept>

PickBuffer::PickBuffer(Window* window)
    : mWidth(window->getWidth()),
    mHeight(window->getHeight()),
    mFrameBuffer(window),
    mOutputTexture(mWidth, mHeight, GL_RED_INTEGER, GL_R32UI, GL_UNSIGNED_INT)
{
    mOutputTexture.setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    mOutputTexture.setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    mFrameBuffer.attachTexture(&mOutputTexture, GL_COLOR_ATTACHMENT0);
}


void PickBuffer::bind() {
    mFrameBuffer.bind();
}

void PickBuffer::unbind() {
    mFrameBuffer.unbind();
}

unsigned int PickBuffer::getAtPixel(int x, int y){
    return mFrameBuffer.getAtPixel(x, y);
}


