//
// Created by joost on 6/8/26.
//

#include "MainScene.h"




#include "Asset/AssetLoader.hpp"
#include "Defaults/Objects/TransformableObject.h"
#include "Error/Option.hpp"
#include "Error/Result.hpp"
#include "Mesh/MeshLoader.hpp"
#include "Mesh/MeshReference.hpp"
#include "Renderer/RenderPass.hpp"
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
#include <algorithm>
#include <memory>
#include "Error/Panic.hpp"

#include "Assets/Texture_1PNG.h"




MainScene::MainScene() {
    addAsset(Spelt::AssetLoadInfo<Spelt::Texture>::FromEmbedded("tiger", Texture_1_png));

    Spelt::AssetLoadInfo<Spelt::Mesh> tigerTest = {"tiger", "Models/Animals/tiger/tiger.gltf"};
    addAsset(tigerTest);

    createRenderPass();
    getRenderPass(1).value().setProjectionType(Spelt::ProjectionType::Orthographic);
    getRenderPass(1).value().setCameraType(Spelt::CameraType::UI);
}

void MainScene::onLoad(Spelt::Renderer &renderer, Spelt::Window &window) {
    window.setVSYNC(false);

    inputManager = new Spelt::InputManager(window);

    Spelt::ObjectReference<Spelt::Defaults::FirstPersonCamera> camera = createObject<Spelt::Defaults::FirstPersonCamera>("main camera", inputManager, &window);
    camera->setPosition(glm::vec3{10.0f, 5.0f, 30.0f});


    Spelt::Material tigerMaterial = {"tigerMaterial", getAssetManager().value().getAssetByName<Spelt::Texture>("tiger")};


    // use get by name to test the system
    mTigerMesh = getAssetManager().value().getAssetByName<Spelt::Mesh>("tiger");

    Spelt::ObjectReference<Spelt::Defaults::TransformableObject> testParent = createObject<Spelt::Defaults::TransformableObject>("parent");
    testParent->setPosition(glm::vec3{5.0f});
    std::cout << testParent->isLoaded() << std::endl;

    // Spawn 100 tigers using your utility functions
    for (int i = 0; i < 100; ++i) {
        std::string name = "tiger_" + std::to_string(i);
        Spelt::ObjectReference<Spelt::Defaults::MeshObject> tiger = createObject<Spelt::Defaults::MeshObject>(name, mTigerMesh, tigerMaterial);
        tiger->setParent(testParent);

        tiger->setPosition(Spelt::getRandomVec3(glm::vec3{-50.0f, 0.0f, -100.0f}, glm::vec3{50.0f, 0.0f, 0.0f}));
        mTigers.push_back(tiger);
    }



    Spelt::ObjectReference<Spelt::Defaults::AmbientLight> ambientLight = createObject<Spelt::Defaults::AmbientLight>("ambient light", glm::vec3{0.2f});

    Spelt::ObjectReference<Spelt::Defaults::DirectionalLight> directionalLight = createObject<Spelt::Defaults::DirectionalLight>("directional light", glm::vec3{1.0f}, glm::vec3{1.0f});

    renderer.setClearColor({0.0f, 0.2f, 0.2f, 1.0f});

    testUIRenderPass();
    testRemoveAsset();
}

void MainScene::onUpdate(Spelt::Renderer &renderer, Spelt::Window &window, float deltaT) {

    // rotate the tiger
    int i = 0;
    for (auto& tiger : mTigers){
        tiger->rotate(glm::radians(10.0f)*deltaT, glm::vec3{0.0f, 1.0f, 0.0f});
    }
}

void MainScene::testUIRenderPass(){
    std::vector<float> vertices = {
        -0.5f, -0.5f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.0f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.0f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f
    };

    std::vector<unsigned int> indices = {
        0, 1, 2,
        2, 3, 0
    };

    Spelt::Mesh quad{"name"};
    quad.setVertices(vertices);
    quad.setIndices(indices);

    Spelt::MeshReference quadRef = getAssetManager().value().addAsset(std::move(quad));

    Spelt::ObjectReference<Spelt::Defaults::MeshObject> obj = createObject<Spelt::Defaults::MeshObject>("testQuad", quadRef, Spelt::Material{"test", glm::vec3{1.0f}});
    obj->setScale(glm::vec3{100.0f});
    obj->setPosition(glm::vec3{100.0f, 100.0f, 0.0f});
    obj->setRenderPass(1);
}

void MainScene::testRemoveAsset(){
    std::vector<float> vertices = {
        -0.5f, -0.5f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.0f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.0f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f
    };

    std::vector<unsigned int> indices = {
        0, 1, 2,
        2, 3, 0
    };

    Spelt::Mesh quad{"name"};
    quad.setVertices(vertices);
    quad.setIndices(indices);

    Spelt::MeshReference quadRef = getAssetManager().value().addAsset(std::move(quad));

    getAssetManager().value().removeAsset(quadRef).panicOnError("Failed to delete asset due to missing asset");


    Spelt::Mesh* result = getAssetManager().value().getAsset(quadRef);
    if (result != nullptr){
        std::cout << "Asset not deleted" << std::endl;
    }else{
        std::cout << "Delete succes" << std::endl;
    }
}

MainScene::~MainScene() {
    delete inputManager;
}
