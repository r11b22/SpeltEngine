//
// Created by joost on 6/8/26.
//

#include <App.h>

#include "ToonScene.h"

int main() {
    App app{"Toon Shader Experiment"};

    ToonScene scene{};

    app.loadScene(&scene);
    app.run();

    return 0;
}
