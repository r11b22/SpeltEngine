//
// Created by joost on 6/9/26.
//

#ifndef SPELTENGINE_BASICPERDRAWABLERENDERPASS_H
#define SPELTENGINE_BASICPERDRAWABLERENDERPASS_H
#include "IRenderPass.h"

class BasicRenderPass : public IRenderPass {
private:
public:
     ~BasicRenderPass() override = default;


     void drawObject(std::shared_ptr<IDrawable> drawable, ShaderProgram& currentProgram) override;
private:
};

#endif //SPELTENGINE_BASICPERDRAWABLERENDERPASS_H