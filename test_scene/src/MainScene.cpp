//
// Created by joost on 6/8/26.
//

#include "MainScene.h"

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
    addMeshAsset({"tiger", "Models/Animals/tiger/tiger.gltf"});
    addTextureAsset({"tiger", "Models/Animals/tiger/Texture_1.png"});
    CubemapAsset cubemapTest = {"test"};

    cubemapTest.setPath(CubeFace::Front, "Models/Animals/tiger/Texture_1.png");
    cubemapTest.setPath(CubeFace::Back, "Models/Animals/tiger/Texture_1.png");
    cubemapTest.setPath(CubeFace::Bottom, "Models/Animals/tiger/Texture_1.png");
    cubemapTest.setPath(CubeFace::Top, "Models/Animals/tiger/Texture_1.png");
    cubemapTest.setPath(CubeFace::Left, "Models/Animals/tiger/Texture_1.png");
    cubemapTest.setPath(CubeFace::Right, "Models/Animals/tiger/Texture_1.png");

    addCubemapAsset(cubemapTest);
}

void MainScene::onLoad(Renderer &renderer, Window &window) {
    window.setVSYNC(false);

    inputManager = new InputManager(window);

    ObjectReference<FirstPersonCamera> camera = createObject<FirstPersonCamera>("main camera", inputManager, &window);
    camera->setPosition(glm::vec3{10.0f, 5.0f, 10.0f});


    Material tigerMaterial = {"tigerMaterial", getAssetManager().getTextureByName("tiger")};

    // use get by name to test the system
    mTigerMesh = getAssetManager().getMeshByName("tiger");

    // Spawn 100 tigers using your utility functions
    for (int i = 0; i < 1000; ++i) {
        std::string name = "tiger_" + std::to_string(i);
        ObjectReference<MeshObject> tiger = createObject<MeshObject>(name, mTigerMesh, tigerMaterial);

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


    testCreateMeshAtRuntime();
}

void MainScene::onUpdate(Renderer &renderer, Window &window, float deltaT) {
    // rotate the tiger
    int i = 0;
    for (auto& tiger : getObjectsOfType<MeshObject>()){
        tiger.rotate(glm::radians(10.0f)*deltaT, glm::vec3{0.0f, 1.0f, 0.0f});
    }
}

void MainScene::testCreateMeshAtRuntime() {

    std::vector<float> cubeVertices = {
        // Position          // Normals           // UVs
        // --- Front Face (Z+)
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,

        // --- Back Face (Z-)
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,

        // --- Left Face (X-)
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,

        // --- Right Face (X+)
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,

        // --- Top Face (Y+)
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,

        // --- Bottom Face (Y-)
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f
    };

    std::vector<unsigned int> cubeIndices = {
        0,  1,  2,    2,  3,  0,  // Front
        4,  5,  6,    6,  7,  4,  // Back
        8,  9,  10,   10, 11, 8,  // Left
        12, 13, 14,   14, 15, 12, // Right
        16, 17, 18,   18, 19, 16, // Top
        20, 21, 22,   22, 23, 20  // Bottom
    };

    Mesh mesh{"cube"};

    mesh.setVertices(cubeVertices);
    mesh.setIndices(cubeIndices);

    MeshReference ref = getAssetManager().addMesh(std::move(mesh));
    Material mat = {"cubeMaterial", glm::vec3{1.0f, 0.0f, 0.0f}};

    // draw the mesh
    ObjectReference<MeshObject> testCube = createObject<MeshObject>("cube", ref, mat);


}

MainScene::~MainScene() {
    delete inputManager;
}
