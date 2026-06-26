//
// Created by joost on 6/8/26.
//

#include <App.h>

#include "MainScene.h"

int main() {
    App app{"Toon Shader Experiment"};

    MainScene scene{};

    app.loadScene(&scene);
    app.run();

    return 0;
}
