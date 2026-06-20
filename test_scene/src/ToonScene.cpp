//
// Created by joost on 6/8/26.
//

#include "ToonScene.h"

#include "Defaults/Camera/FirstPersonCamera.h"
#include "Defaults/Objects/Drawables/MeshObject.h"
#include "Defaults/Objects/Lighting/AmbientLight.h"
#include "Defaults/Objects/Lighting/DirectionalLight.h"
#include "Object/ObjectRepository.h"
#include "Renderer/Renderer.h"
#include "glm/ext/vector_float3.hpp"
#include "Utilities/Random.h"
#include <memory>

ToonScene::ToonScene() {

}

void ToonScene::onLoad(Renderer &renderer, Window &window) {
    window.setVSYNC(false);

    inputManager = new InputManager(window);

    ObjectReference<FirstPersonCamera> camera = createObject<FirstPersonCamera>("main camera", inputManager, &window);
    camera->setPosition(glm::vec3{10.0f, 5.0f, 10.0f});

    ModelLoader tigerLoader= {};
    tigerLoader.readFile("Models/Animals/tiger/tiger.gltf");
    tigerLoader.loadTexture("Models/Animals/tiger/Texture_1.png");
    std::shared_ptr<Mesh> tigerMesh = tigerLoader.createMesh();
    std::shared_ptr<Material> tigerMaterial = tigerLoader.createMaterial();

    // Spawn 100 tigers using your utility functions
    for (int i = 0; i < 1000; ++i) {
        std::string name = "tiger_" + std::to_string(i);
        ObjectReference<MeshObject> tiger = createObject<MeshObject>(name, tigerMesh, tigerMaterial);

        // Using your basic uniform random distribution over a 100x100 area
        float x = getRandomFloat(-50.0f, 50.0f);
        float y = 0.0f; // Keep them on the ground
        float z = getRandomFloat(-50.0f, 50.0f);

        tiger->setPosition(glm::vec3{x, y, z});
        mTigers.push_back(tiger);
    }

    if (!mTigers.empty()) {
        glm::vec3 diff = mTigers[0]->getPosition() - camera->getPosition();
        camera->lookAt(glm::normalize(diff));
    }

    ObjectReference<AmbientLight> ambientLight = createObject<AmbientLight>("ambient light", glm::vec3{0.2f});

    /* std::shared_ptr<PointLight> pointLight = std::make_shared<PointLight>("point light", glm::vec3{3.0f});
        pointLight->setPosition(glm::vec3{2.0f, 0.0f, 0.0f});
        addObject(pointLight);*/

    ObjectReference<DirectionalLight> directionalLight = createObject<DirectionalLight>("directional light", glm::vec3{1.0f}, glm::vec3{1.0f});

    renderer.setClearColor({0.0f, 0.2f, 0.2f, 1.0f});
}

void ToonScene::onUpdate(Renderer &renderer, Window &window, float deltaT) {
    // rotate the tiger
    for (auto tiger : mTigers){
        tiger->rotate(glm::radians(10.0f)*deltaT, glm::vec3{0.0f, 1.0f, 0.0f});
    }
}

ToonScene::~ToonScene() {
    delete inputManager;
}
