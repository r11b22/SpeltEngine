//
// Created by joost on 4/28/26.
//


#include "InputManager.h"

InputManager::InputManager(Window& window)
    :mWindow(window)
{
    window.setKeyCallback(keyCallback);
    window.setMouseButtonCallback(mouseButtonCallback);
    window.setCursorCallBack(mousePosCallback);
    window.setInputManager(this);
}

void InputManager::setKeyCallback(int key, std::function<void(int action)> func) {
    mKeyCallbacks.insert({key, func});
}

void InputManager::setMouseButtonCallback(int button, std::function<void(int action)> func) {
    mMouseButtonCallbacks.insert({button, func});
}

void InputManager::setMousePosCallback(std::function<void(double x, double y)> callback) {
    mMousePosCallback = callback;
}

void InputManager::removeMousePosCallback() {
    mMousePosCallback.reset();
}

bool InputManager::isKeyPressed(const int key) const{
    return mWindow.isKeyPressed(key);
}

void InputManager::handleKey(int key, int action) {
    auto range = mKeyCallbacks.equal_range(key);
    for (auto it = range.first; it != range.second; ++it) {
        it->second(action);
    }
}
void InputManager::handleMouseButton(int button, int action) {
    auto range = mMouseButtonCallbacks.equal_range(button);
    for (auto it = range.first; it != range.second; ++it) {
        it->second(action);
    }
}

void InputManager::handleMousePosChange(const double x, const double y) const {
    if (mMousePosCallback.has_value()) {
        mMousePosCallback.value()(x, y);
    }
}

void InputManager::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    static_cast<WindowContext*>(glfwGetWindowUserPointer(window))->inputManager->handleKey(key, action);
}

void InputManager::mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
    static_cast<WindowContext*>(glfwGetWindowUserPointer(window))->inputManager->handleMouseButton(button, action);
}

void InputManager::mousePosCallback(GLFWwindow *window, double x, double y) {
    static_cast<WindowContext*>(glfwGetWindowUserPointer(window))->inputManager->handleMousePosChange(x, y);
}
