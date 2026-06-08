//
// Created by joost on 6/3/26.
//


#include "Defaults/Camera/FirstPersonCamera.h"

FirstPersonCamera::FirstPersonCamera(const std::string& name, InputManager& inputManager, Window& window, float speed)
    : Camera(name), mInputManager(inputManager), mWindow(window), mSpeed(speed), mMouseFocused(false)
{

}

void FirstPersonCamera::onLoad() {

    glm::vec3 forward = getForward();
    mYaw   = glm::degrees(std::atan2(forward.z, forward.x));
    mPitch = glm::degrees(std::asin(glm::clamp(forward.y, -1.0f, 1.0f)));

    mInputManager.setMouseButtonCallback(GLFW_MOUSE_BUTTON_LEFT, [&](int action) {
        if (action == GLFW_PRESS){
            if (!mMouseFocused) {
                // hide mouse
                mWindow.setInputMode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                mInputManager.setMousePosCallback([&](double xposIn, double yposIn) {
                    mouseCallback(xposIn, yposIn);
                });
                mMouseFocused = true;
            }
        }
    });
    mInputManager.setKeyCallback(GLFW_KEY_ESCAPE, [&](int action) {
        if (action == GLFW_PRESS) {
            if (mMouseFocused) {
                mInputManager.removeMousePosCallback();
                mWindow.setInputMode(GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                mFirstMouse = true;
                mMouseFocused = false;
            }
        }
    });
}

void FirstPersonCamera::onUpdate(float deltaT) {
    const float speed = mSpeed * deltaT;

    // Flatten forward/backward movement onto the XZ plane (Minecraft-style)
    if (mInputManager.isKeyPressed(GLFW_KEY_W))
        setPosition(getPosition() + getForward() * speed);
    if (mInputManager.isKeyPressed(GLFW_KEY_S))
        setPosition(getPosition() + getBackward() * speed);
    if (mInputManager.isKeyPressed(GLFW_KEY_A))
        setPosition(getPosition() + getLeft() * speed);
    if (mInputManager.isKeyPressed(GLFW_KEY_D))
        setPosition(getPosition() + getRight() * speed);
    if (mInputManager.isKeyPressed(GLFW_KEY_SPACE))
        setPosition(getPosition() + getUp() * speed);
    if (mInputManager.isKeyPressed(GLFW_KEY_LEFT_SHIFT))
        setPosition(getPosition() + getDown() * speed);
}


void FirstPersonCamera::mouseCallback(double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (mFirstMouse) {
        mLastX = xpos;
        mLastY = ypos;
        mFirstMouse = false;
    }

    float xoffset = xpos - mLastX;
    float yoffset = mLastY - ypos; // Reversed since y-coordinates go from bottom to top

    mLastX = xpos;
    mLastY = ypos;

    // 1. Apply mouse sensitivity
    const float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    // 2. Update yaw and pitch, clamping pitch to prevent flipping upside down
    mYaw   += xoffset;
    mPitch += yoffset;
    if (mPitch >  89.0f) mPitch =  89.0f;
    if (mPitch < -89.0f) mPitch = -89.0f;

    // 3. Calculate forward vector from yaw and pitch
    const float yawRad   = glm::radians(mYaw);
    const float pitchRad = glm::radians(mPitch);

    glm::vec3 forward;
    forward.x = std::cos(pitchRad) * std::cos(yawRad);
    forward.y = std::sin(pitchRad);
    forward.z = std::cos(pitchRad) * std::sin(yawRad);

    // 4. Apply the forward vector
    lookAt(glm::normalize(forward));
}

