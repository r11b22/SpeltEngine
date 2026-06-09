//
// Created by joost on 5/25/26.
//

#ifndef CGVCPROJECT_APP_H
#define CGVCPROJECT_APP_H

#include "Renderer/Renderer.h"
#include "Window.h"

class App {
private:
    Window mWindow;
    Renderer mRenderer;
    Scene* mCurrentScene;

    float mDeltaT, mLastT;

public:
    explicit App(const std::string &title);
    void loadScene(Scene* scene);
    void run();
private:
};

#endif //CGVCPROJECT_APP_H