//
// Created by Sofie on 4/27/26
//

#include "Camera.h"

#include <iostream>

#include "Window.h"

Camera::Camera(const std::string& name)
	:
	TransformableObject(name),
	mZoom(45.0f)
{}

glm::mat4 Camera::getViewMatrix() const {
	return glm::inverse(getTransformationMatrix());
}

glm::mat4 Camera::getProjectionMatrix(const Window& window) const {
	return glm::perspective(glm::radians(mZoom), (float)window.getWidth() / (float)window.getHeight(), 0.1f, 1000.0f);
}

Camera::~Camera() {

}