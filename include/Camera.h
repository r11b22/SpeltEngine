#pragma once

#include "Window.h"
#include "Defaults/Objects/TransformableObject.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"


namespace Spelt {
    class Camera : public Defaults::TransformableObject {
    private:
        float mZoom;

        void updateCameraVectors();
    public:
        Camera(const std::string& name);
        ~Camera();

        glm::mat4 getViewMatrix() const;
        glm::mat4 getPerspectiveProjectionMatrix(const Window& window) const;
        glm::mat4 getOrthographicProjectionMatrix(const Window& window) const;
        void changeSpeed(float offset);

    };
}
