//
// Created by Sofie on 4/27/26
//

#include "Camera.h"

#include <iostream>

#include "Window.h"
#include "glm/ext/matrix_clip_space.hpp"
namespace Spelt {
    Camera::Camera(const std::string& name)
    	:
    	TransformableObject(name),
    	mZoom(45.0f)
    {}

    glm::mat4 Camera::getViewMatrix() const {
    	return glm::inverse(getTransformationMatrix());
    }

    glm::mat4 Camera::getPerspectiveProjectionMatrix(const Window& window) const {
    	return glm::perspective(glm::radians(mZoom), (float)window.getWidth() / (float)window.getHeight(), 0.1f, 1000.0f);
    }

    glm::mat4 Camera::getOrthographicProjectionMatrix(const Window& window) const {
        float halfWidth = (float)window.getWidth() / 2.0f;
        float halfHeight = (float)window.getHeight() / 2.0f;

        return glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, -1.0f, 1.0f);
    }

    Camera::~Camera() {

    }
}
