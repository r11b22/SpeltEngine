//
// Created by joost on 6/8/26.
//

#ifndef TOONSHADER_TOONSCENE_H
#define TOONSHADER_TOONSCENE_H
#include "Defaults/Objects/Drawables/MeshObject.h"
#include "Scene/Scene.h"

class ToonScene : public Scene {
private:
    InputManager* inputManager = nullptr;

    std::shared_ptr<MeshObject> mTiger = nullptr;

public:
    ToonScene();
    ~ToonScene();

    void onLoad(Renderer &renderer, Window &window) override;
    void onUpdate(Renderer &renderer, Window &window, float deltaT) override;
private:
};

#endif //TOONSHADER_TOONSCENE_H