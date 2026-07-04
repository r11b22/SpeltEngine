//
// Created by joost on 6/8/26.
//

#include "MainScene.h"




#include "Asset/AssetLoader.hpp"
#include "Mesh/MeshLoader.hpp"
#include "Texture/Texture.h"
#include "Texture/TextureLoader.h"
#include "Texture/CubemapLoader.hpp"
#include "Defaults/Camera/FirstPersonCamera.h"
#include "Defaults/Objects/Drawables/MeshObject.h"
#include "Defaults/Objects/Lighting/AmbientLight.h"
#include "Defaults/Objects/Lighting/DirectionalLight.h"
#include "Material/Material.h"
#include "Object/ObjectRepository.h"
#include "Renderer/Renderer.h"
#include "Texture/CubemapAsset.hpp"
#include "Texture/CubemapTexture.hpp"
#include "glm/ext/vector_float3.hpp"
#include "Utilities/Random.h"
#include <memory>



MainScene::MainScene() {
    addAsset(AssetLoadInfo<Texture>{"tiger", "Models/Animals/tiger/Texture_1.png"});

    AssetLoadInfo<Mesh> tigerTest = {"tiger", "Models/Animals/tiger/tiger.gltf"};
    addAsset(tigerTest);
}

void MainScene::onLoad(Renderer &renderer, Window &window) {
    window.setVSYNC(false);





    inputManager = new InputManager(window);

    ObjectReference<FirstPersonCamera> camera = createObject<FirstPersonCamera>("main camera", inputManager, &window);
    camera->setPosition(glm::vec3{10.0f, 5.0f, 30.0f});


    Material tigerMaterial = {"tigerMaterial", getAssetManager().getAssetByName<Texture>("tiger")};


    // use get by name to test the system
    mTigerMesh = getAssetManager().getAssetByName<Mesh>("tiger");

    // Spawn 100 tigers using your utility functions
    for (int i = 0; i < 10000; ++i) {
        std::string name = "tiger_" + std::to_string(i);
        ObjectReference<MeshObject> tiger = createObject<MeshObject>(name, mTigerMesh, tigerMaterial);


        tiger->setPosition(getRandomVec3(glm::vec3{-50.0f, 0.0f, -100.0f}, glm::vec3{50.0f, 0.0f, 0.0f}));
        mTigers.push_back(tiger);
    }



    ObjectReference<AmbientLight> ambientLight = createObject<AmbientLight>("ambient light", glm::vec3{0.2f});

    ObjectReference<DirectionalLight> directionalLight = createObject<DirectionalLight>("directional light", glm::vec3{1.0f}, glm::vec3{1.0f});

    renderer.setClearColor({0.0f, 0.2f, 0.2f, 1.0f});


}

void MainScene::onUpdate(Renderer &renderer, Window &window, float deltaT) {
    // rotate the tiger
    int i = 0;
    for (auto& tiger : getObjectsOfType<MeshObject>()){
        tiger.rotate(glm::radians(10.0f)*deltaT, glm::vec3{0.0f, 1.0f, 0.0f});
    }
}

MainScene::~MainScene() {
    delete inputManager;
}
