//
// Created by Sofie on 4/27/26
//

#include "Window.h"
#include "Defaults/Objects/TransformableObject.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#ifndef GCVCPROJECT_CAMERA_H
#define GCVCPROJECT_CAMERA_H


class Camera : public TransformableObject {
private:
    float mZoom;

    void updateCameraVectors();
public:
    Camera(const std::string& name);
    ~Camera();

    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix(const Window& window) const;
    void changeSpeed(float offset);

};

#endif // !GCVCPROJECT_CAMERA_H
