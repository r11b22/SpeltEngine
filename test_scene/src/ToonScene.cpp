//
// Created by joost on 6/8/26.
//

#include "ToonScene.h"

#include "Defaults/Camera/FirstPersonCamera.h"
#include "Defaults/Objects/Drawables/MeshObject.h"
#include "Defaults/Objects/Lighting/AmbientLight.h"
#include "Defaults/Objects/Lighting/DirectionalLight.h"
#include "Renderer/Renderer.h"
#include "glm/ext/vector_float3.hpp"
#include <memory>

ToonScene::ToonScene() {

}

void ToonScene::onLoad(Renderer &renderer, Window &window) {
    window.setVSYNC(false);


    inputManager = new InputManager(window);

    std::shared_ptr<FirstPersonCamera> camera = std::make_shared<FirstPersonCamera>("main camera", *inputManager, window);
    camera->setPosition(glm::vec3{10.0f, 5.0f, 10.0f});
    addObject(camera);

    ModelLoader tigerLoader= {};
    tigerLoader.readFile("Models/Animals/tiger/tiger.gltf");
    tigerLoader.loadTexture("Models/Animals/tiger/Texture_1.png");
    std::shared_ptr<Mesh> tigerMesh = tigerLoader.createMesh();
    std::shared_ptr<Material> tigerMaterial = tigerLoader.createMaterial();
    mTiger = std::make_shared<MeshObject>("tiger", tigerMesh, tigerMaterial);
    addObject(mTiger);

    glm::vec3 diff = mTiger->getPosition() - camera->getPosition();
    camera->lookAt(glm::normalize(diff));

    std::shared_ptr<AmbientLight> ambientLight = std::make_shared<AmbientLight>("ambient light", glm::vec3{0.2f});
    addObject(ambientLight);

   /*  std::shared_ptr<PointLight> pointLight = std::make_shared<PointLight>("point light", glm::vec3{3.0f});
    pointLight->setPosition(glm::vec3{2.0f, 0.0f, 0.0f});
    addObject(pointLight);*/

    std::shared_ptr<DirectionalLight> directionalLight = std::make_shared<DirectionalLight>("directional light", glm::vec3{1.0f}, glm::vec3{1.0f});
    addObject(directionalLight);

    renderer.setClearColor({0.0f, 0.2f, 0.2f, 1.0f});
}

void ToonScene::onUpdate(Renderer &renderer, Window &window, float deltaT) {
    // rotate the tiger
    mTiger->rotate(glm::radians(10.0f)*deltaT, glm::vec3{0.0f, 1.0f, 0.0f});
}

ToonScene::~ToonScene() {
    delete inputManager;
}
