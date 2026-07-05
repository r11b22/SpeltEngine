//
// Created by joost on 4/14/26.
//



#include "../../include/Transform/Transform.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/quaternion_geometric.hpp"
#include "glm/ext/quaternion_trigonometric.hpp"
#include "glm/ext/vector_float3.hpp"

#include <cmath>
#include <stdexcept>


#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

void Transform::setPosition(const glm::vec3 &pos) {
    mCacheInvalid = true;
    mPosition = pos;
}

void Transform::setScale(const glm::vec3 &scale) {
    mCacheInvalid = true;
    mScale = scale;
}

void Transform::rotate(float rad, const glm::vec3 &axis) {
    mCacheInvalid = true;
    mRotation = glm::angleAxis(rad, glm::normalize(axis)) * mRotation;
}


glm::mat4 Transform::getTransformationMatrix() const{
    if (mCacheInvalid) {
        const glm::mat3 rot = glm::mat3_cast(mRotation);

        mTransformationMatrixCache[0] = glm::vec4(rot[0] * mScale.x, 0.0f);
        mTransformationMatrixCache[1] = glm::vec4(rot[1] * mScale.y, 0.0f);
        mTransformationMatrixCache[2] = glm::vec4(rot[2] * mScale.z, 0.0f);
        mTransformationMatrixCache[3] = glm::vec4(mPosition, 1.0f);

        mCacheInvalid = false;
    }

    return mTransformationMatrixCache;
}

inline glm::mat4 multiplyAffine(const glm::mat4 &a, const glm::mat4 &b) {
    const glm::mat3 aRot(a); // extract a's 3x3 upper-left once

    glm::mat4 result;
    result[0] = glm::vec4(aRot * glm::vec3(b[0]), 0.0f);
    result[1] = glm::vec4(aRot * glm::vec3(b[1]), 0.0f);
    result[2] = glm::vec4(aRot * glm::vec3(b[2]), 0.0f);
    result[3] = glm::vec4(aRot * glm::vec3(b[3]) + glm::vec3(a[3]), 1.0f);
    return result;
}

glm::mat4 Transform::getTransformationMatrix(glm::mat4 parent) const{
    return multiplyAffine(parent, getTransformationMatrix());
}

void Transform::lookAt(glm::vec3 forward) {
    mCacheInvalid = true;
    mRotation = glm::quatLookAt(glm::normalize(forward), glm::vec3(0, 1, 0));
}

glm::vec3 Transform::getPosition() const {
    return mPosition;
}

glm::vec3 Transform::getScale() const {
    return mScale;
}

glm::vec3 Transform::getRight() const {
    return mRotation * glm::vec3(1.0f, 0.0f, 0.0f);
}

glm::vec3 Transform::getLeft() const {
    return -getRight();
}

glm::vec3 Transform::getUp() const {
    return mRotation * glm::vec3(0.0f, 1.0f, 0.0f);
}

glm::vec3 Transform::getDown() const {
    return -getUp();
}

glm::vec3 Transform::getBackward() const {
    return mRotation * glm::vec3(0.0f, 0.0f, 1.0f);
}

glm::vec3 Transform::getForward() const {
    return -getBackward();
}

void Transform::scale(const glm::vec3 &scalar) {
    mScale *= scalar;
    mCacheInvalid = true;
}
