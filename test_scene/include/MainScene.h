//
// Created by joost on 6/8/26.
//

#pragma once

#include "Defaults/Objects/Drawables/MeshObject.h"
#include "Mesh/MeshReference.hpp"
#include "Object/ObjectRepository.h"
#include "Scene/Scene.hpp"
#include "Texture/TextureReference.hpp"


class MainScene : public Spelt::Scene {
private:
    Spelt::InputManager* inputManager = nullptr;

    Spelt::MeshReference mTigerMesh;
    Spelt::TextureReference mTigerTexture;

    std::vector<Spelt::ObjectReference<Spelt::Defaults::MeshObject>> mTigers;

public:
    MainScene();
    ~MainScene();

    void onLoad(Spelt::Renderer &renderer, Spelt::Window &window) override;
    void onUpdate(Spelt::Renderer &renderer, Spelt::Window &window, float deltaT) override;
private:
    void testUIRenderPass();
    void testRemoveAsset();
};
