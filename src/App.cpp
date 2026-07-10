//
// Created by joost on 5/25/26.
//


#include "App.h"

#include <iostream>

#include "glad/glad.h"
#include "Strings/ShaderSource.h"
#include "Tracy/tracy/Tracy.hpp"

App::App(const std::string &title) :
    mWindow(800, 500, title), mRenderer(&mWindow), mCurrentScene(nullptr)
{
    std::unique_ptr<ShaderProgram> litProgram = std::make_unique<ShaderProgram>();

    litProgram->addShader(standardVertexShader, GL_VERTEX_SHADER);
    litProgram->addShader(litFragmentShader, GL_FRAGMENT_SHADER);
    litProgram->link();

    mRenderer.addShaderProgram("litShader", std::move(litProgram));
    mRenderer.setAssetManager(&mAssetManager);
}

void App::loadScene(Scene* scene) {
    if (mCurrentScene != nullptr) {
        delete mCurrentScene;
    }

    loadSceneAssets(*scene);

    mCurrentScene = scene;
    mCurrentScene->setAssetManager(&mAssetManager);
    mCurrentScene->onLoad(mRenderer, mWindow);

    mDeltaT = 0;
    mLastT = 0;
}

void App::run() {

    mCurrentScene->markRunning();
    mCurrentScene->loadObjects();
    mRenderer.prepare();
    mCurrentScene->onStart();

    while (!mWindow.shouldClose()) {
        try {
            float currentT = glfwGetTime();
            mDeltaT = currentT - mLastT;
            mLastT = currentT;

            mCurrentScene->onUpdate(mRenderer, mWindow, mDeltaT);
            mCurrentScene->updateObjects(mDeltaT);
            mRenderer.draw(mCurrentScene->getRenderPasses(), *mCurrentScene->getCamera(), mCurrentScene->getLightData());

            mWindow.swapBuffers();
            mWindow.pollEvents();
        } catch (const std::exception& e) {
            std::cout << "Runtime error: " << e.what() << std::endl;
        }
        FrameMark;
    }
}

void App::loadSceneAssets(const Scene& scene){
    mAssetManager.clear();
    scene.loadAssets(mAssetManager);
}
