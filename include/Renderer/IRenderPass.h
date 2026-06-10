//
// Created by joost on 6/9/26.
//

#ifndef SPELTENGINE_IPERDRAWABLERENDERPASS_H
#define SPELTENGINE_IPERDRAWABLERENDERPASS_H
#include "IDrawable.h"

class IRenderPass {
private:
public:
    virtual ~IRenderPass() = default;

    virtual std::optional<std::string> getCustomShaderName() {return std::nullopt;}
    virtual void drawObject(std::shared_ptr<IDrawable> drawable, ShaderProgram& currentProgram) = 0;
private:
};

#endif //SPELTENGINE_IPERDRAWABLERENDERPASS_H