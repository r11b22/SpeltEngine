//
// Created by joost on 6/3/26.
//

#pragma once

#include "Camera.h"
#include "InputManager.h"

namespace Spelt::Defaults {
    class FirstPersonCamera : public Camera {
    private:
        bool mMouseFocused;
        bool mFirstMouse = true;

        float mLastX = 0;
        float mLastY = 0;

        float mPitch = 0;
        float mYaw = 0;

        float mSpeed;

        InputManager* mInputManager;
        Window* mWindow;
    public:
        FirstPersonCamera(const std::string& name, InputManager* inputManager, Window* window, float speed = 5.0f);
        ~FirstPersonCamera() = default;

        void onLoad() override;
        void onUpdate(float deltaT) override;
    private:
        void mouseCallback(double xposIn, double yposIn);
    };

}
