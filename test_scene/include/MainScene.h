//
// Created by joost on 6/8/26.
//

#ifndef TOONSHADER_TOONSCENE_H
#define TOONSHADER_TOONSCENE_H
#include "Defaults/Objects/Drawables/MeshObject.h"
#include "Mesh/MeshReference.hpp"
#include "Object/ObjectRepository.h"
#include "Scene/Scene.hpp"
#include "Texture/TextureReference.hpp"


class MainScene : public Scene {
private:
    InputManager* inputManager = nullptr;

    MeshReference mTigerMesh;
    TextureReference mTigerTexture;

    std::vector<ObjectReference<MeshObject>> mTigers;

public:
    MainScene();
    ~MainScene();

    void onLoad(Renderer &renderer, Window &window) override;
    void onUpdate(Renderer &renderer, Window &window, float deltaT) override;
private:
    void testUIRenderPass();
    void testRemoveAsset();
};

#endif //TOONSHADER_TOONSCENE_H
