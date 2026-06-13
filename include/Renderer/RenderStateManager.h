//
// Created by joost on 6/13/26.
//

#ifndef SPELTENGINE_RENDERSTATEMANAGER_H
#define SPELTENGINE_RENDERSTATEMANAGER_H
#include "RenderState.h"

class RenderStateManager {
private:
    RenderState mCurrentState;
public:
    void applyState(const RenderState& newState);
};
#endif //SPELTENGINE_RENDERSTATEMANAGER_H