//
// Created by joost on 6/9/26.
//

#ifndef SPELTENGINE_IPERDRAWABLERENDERPASS_H
#define SPELTENGINE_IPERDRAWABLERENDERPASS_H
#include "IDrawable.h"

class IPerDrawableRenderPass {
private:
public:
    virtual ~IPerDrawableRenderPass() = default;

    virtual void drawObject(std::shared_ptr<IDrawable> drawable, ShaderProgram& currentProgram) = 0;
private:
};

#endif //SPELTENGINE_IPERDRAWABLERENDERPASS_H