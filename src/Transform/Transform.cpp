//
// Created by joost on 4/14/26.
//



#include "../../include/Transform/Transform.h"

#include <stdexcept>


#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

void Transform::setPosition(const glm::vec3 &pos) {
    mPosition = pos;
    mPositionMatrix = glm::translate(glm::mat4(1.0f), pos);
}

void Transform::setScale(const glm::vec3 &scale) {
    mScaleMatrix = glm::scale(glm::mat4(1.0f), scale);
}

void Transform::rotate(float rad, const glm::vec3 &axis) {
    mRotationMatrix = glm::rotate(mRotationMatrix, rad, axis);
}


glm::mat4 Transform::getTransformationMatrix() const{
    return mPositionMatrix * mRotationMatrix * mScaleMatrix;
}

void Transform::lookAt(glm::vec3 forward) {
    glm::quat rotation = glm::quatLookAt(forward, glm::vec3(0, 1, 0));
    mRotationMatrix = glm::toMat4(rotation);
}

glm::vec3 Transform::getPosition() const {
    return mPosition;
}

glm::vec3 Transform::getScale() const {
    glm::vec3 scale;
    scale.x = mScaleMatrix[0][0];
    scale.y = mScaleMatrix[1][1];
    scale.z = mScaleMatrix[2][2];
    return scale;
}

glm::vec3 Transform::getRight() const {
    return glm::vec3(mRotationMatrix[0]);
}

glm::vec3 Transform::getLeft() const {
    return -getRight();
}

glm::vec3 Transform::getUp() const {
    return glm::vec3(mRotationMatrix[1]);
}

glm::vec3 Transform::getDown() const {
    return -getUp();
}

glm::vec3 Transform::getBackward() const {
    return glm::vec3(mRotationMatrix[2]);
}

glm::vec3 Transform::getForward() const {
    return -getBackward();
}

void Transform::scale(const glm::vec3 &scalar) {
    mScaleMatrix = glm::scale(mScaleMatrix, scalar);
}
