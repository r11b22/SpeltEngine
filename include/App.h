//
// Created by joost on 5/25/26.
//

#pragma once

#include "Asset/AssetManager.hpp"
#include "Renderer/Renderer.h"
#include "Window.h"

namespace Spelt {

    class App {
    private:
        Window mWindow;
        Renderer mRenderer;
        Scene* mCurrentScene;
        AssetManager mAssetManager;

        float mDeltaT, mLastT;

    public:
        explicit App(const std::string &title);
        void loadScene(Scene* scene);
        void run();
    private:

        void loadSceneAssets(const Scene& scene);
    };
}
