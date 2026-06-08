//
// Created by joost on 4/28/26.
//

#ifndef CGVCPROJECT_INPUTMANAGER_H
#define CGVCPROJECT_INPUTMANAGER_H
#include <functional>
#include <optional>

#include "Window.h"

class InputManager {
private:
    std::unordered_multimap<int, std::function<void(int action)>> mKeyCallbacks;
    std::unordered_multimap<int, std::function<void(int action)>> mMouseButtonCallbacks;
    std::optional<std::function<void(double x, double y)>> mMousePosCallback;

    // non owning
    Window& mWindow;
public:
    InputManager(Window& window);

    void setKeyCallback(int key, std::function<void(int action)> func);
    void setMouseButtonCallback(int button, std::function<void(int action)> func);
    void setMousePosCallback(std::function<void(double x, double y)> callback);
    void removeMousePosCallback();

    bool isKeyPressed(int key) const;
private:
    void handleKey(int key, int action);
    void handleMouseButton(int button, int action);
    void handleMousePosChange(double x, double y) const;
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void mousePosCallback(GLFWwindow* window, double x, double y);
};


#endif //CGVCPROJECT_INPUTMANAGER_H