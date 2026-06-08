//
// Created by joost on 4/9/26.
//

#include "Window.h"

#include <stdexcept>


Window::Window(const int width, const int height, const std::string& title)
    : mWindow(nullptr), mContext(), mWidth(width), mHeight(height)
{
    if (!glfwInit()) throw std::runtime_error("Failed to initialize glfw!");

    mWindow = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!mWindow) {
        glfwTerminate();
        throw std::runtime_error("Could not create window!");
    }
    mContext.window = this;

    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    glfwMakeContextCurrent(mWindow);
    glfwSetWindowUserPointer(mWindow, &mContext);

    glfwSetFramebufferSizeCallback(mWindow, [](GLFWwindow* window, int width, int height) {
        if (auto context = static_cast<WindowContext*>(glfwGetWindowUserPointer(window))) {
            if (Window* windowObj = context->window) {
                windowObj->mWidth = width;
                windowObj->mHeight = height;
                windowObj->resizeCallback(width, height);
            }
        }
        glViewport(0,0,width,height);

    });

}

Window::~Window() {
    if (mWindow) {
        glfwDestroyWindow(mWindow);
    }
    glfwTerminate();
}

void Window::pollEvents() const{
    glfwPollEvents();
}

void Window::swapBuffers() const {
    glfwSwapBuffers(mWindow);
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(mWindow);
}

bool Window::isKeyPressed(int key) const {
    return glfwGetKey(mWindow, key) == GLFW_PRESS;
}

void Window::setCursorCallBack(GLFWcursorposfun callback) const {
    glfwSetCursorPosCallback(mWindow, callback);
}

void Window::setInputMode(int mode, int value) const {
    glfwSetInputMode(mWindow, mode, value);
}

void Window::setInputManager(InputManager *inputManager) {
    mContext.inputManager = inputManager;
}

void Window::setKeyCallback(GLFWkeyfun function) const {
    glfwSetKeyCallback(mWindow, function);
}

void Window::setMouseButtonCallback(GLFWmousebuttonfun function) const {
    glfwSetMouseButtonCallback(mWindow, function);
}

int Window::getWidth() const {
    return mWidth;
}

int Window::getHeight() const {
    return mHeight;
}

void Window::resizeCallback(int width, int height) {
    for (const auto &entry : mResizeCallbacks) {
        entry.second(width, height);
    }
}

int Window::addResizeCallback(const std::function<void(int width, int height)> &callback) {
    int id = mResizeCallbackId++;
    mResizeCallbacks[id] = callback;
    return id;
}

void Window::removeResizeCallback(int id) {
    mResizeCallbacks.erase(id);
}

void Window::setVSYNC(bool enabled) {
    glfwSwapInterval(enabled ? 1 : 0);
}
