//
// Created by joost on 6/8/26.
//

#include "ToonScene.h"

#include "Defaults/Camera/FirstPersonCamera.h"
#include "Defaults/Objects/Drawables/MeshObject.h"

ToonScene::ToonScene() {

}

void ToonScene::onLoad(Renderer &renderer, Window &window) {

    inputManager = new InputManager(window);

    std::shared_ptr<FirstPersonCamera> camera = std::make_shared<FirstPersonCamera>("main camera", *inputManager, window);
    camera->setPosition(glm::vec3{10.0f, 5.0f, 10.0f});
    addObject(camera);

    ModelLoader tigerLoader= {};
    tigerLoader.readFile("Models/Animals/tiger/tiger.gltf");
    tigerLoader.loadTexture("Models/Animals/tiger/Texture_1.png");
    std::shared_ptr<Mesh> tigerMesh = tigerLoader.createMesh();
    mTiger = std::make_shared<MeshObject>("tiger", tigerMesh);
    addObject(mTiger);

    glm::vec3 diff = mTiger->getPosition() - camera->getPosition();
    camera->lookAt(glm::normalize(diff));

    std::shared_ptr<AmbientLight> ambientLight = std::make_shared<AmbientLight>("ambient light", glm::vec3{0.5f, 0.5f, 0.5f});
    addObject(ambientLight);
}

void ToonScene::onUpdate(Renderer &renderer, Window &window, float deltaT) {
    // rotate the tiger
    mTiger->rotate(glm::radians(10.0f)*deltaT, glm::vec3{0.0f, 1.0f, 0.0f});
}

ToonScene::~ToonScene() {
    delete inputManager;
}
