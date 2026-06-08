//
// Created by joost on 4/9/26.
//

#ifndef GCVCPROJECT_WINDOW_H
#define GCVCPROJECT_WINDOW_H
#include <string>
#include <vector>

#include <glad/glad.h>
#include "GLFW/glfw3.h"
#include <functional>


class InputManager;

class Window;

struct WindowContext {
    InputManager* inputManager = nullptr;
    Window* window = nullptr;
};

class Window {

private:
    GLFWwindow* mWindow;
    WindowContext mContext;

    int mWidth, mHeight;

    std::unordered_map<int, std::function<void(int width, int height)>> mResizeCallbacks;
    int mResizeCallbackId = 0;
public:
    Window(int width, int height, const std::string& title);
    ~Window();
    void pollEvents() const;
    void swapBuffers() const;
    bool shouldClose() const;

    bool isKeyPressed(int key) const;
    void setCursorCallBack(GLFWcursorposfun callback) const;
    void setInputMode(int mode, int value) const;
    void setInputManager(InputManager* inputManager);
    void setKeyCallback(GLFWkeyfun function) const;
    void setMouseButtonCallback(GLFWmousebuttonfun function) const;

    int getWidth() const;
    int getHeight() const;

    int addResizeCallback(const std::function<void(int width, int height)> &callback);
    void removeResizeCallback(int id);

    void setVSYNC(bool enabled);
private:
    void resizeCallback(int width, int height);
};



#endif //GCVCPROJECT_WINDOW_H