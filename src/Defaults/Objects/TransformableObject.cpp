//
// Created by joost on 6/2/26.
//

#include "../../../include/Defaults/Objects/TransformableObject.h"

#include "Scene/Scene.h"

TransformableObject::TransformableObject(const std::string &name)
    : Object(name), mTransform()
{

}

void TransformableObject::setPosition(const glm::vec3 &pos) {
    mTransform.setPosition(pos);
}

void TransformableObject::setScale(const glm::vec3 &scale) {
    mTransform.setScale(scale);
}

void TransformableObject::rotate(float rad, const glm::vec3 &axis) {
    mTransform.rotate(rad, axis);
}

void TransformableObject::lookAt(glm::vec3 forward) {
    mTransform.lookAt(forward);
}


glm::mat4 TransformableObject::getTransformationMatrix() const {
    return getParentTransformationMatrix() * mTransform.getTransformationMatrix();
}

glm::mat4 TransformableObject::getParentTransformationMatrix() const {
    // find first ancestor with transform

    while (true) {
        std::shared_ptr<Object> current = getScene().getParent(shared_from_this());
        if (current == nullptr) {
            return {1.0f};
        }


        if ( std::shared_ptr<ITransformable> transformable =
                std::dynamic_pointer_cast<ITransformable>(current)) {
            return transformable->getTransformationMatrix();
        }
    }


}

glm::vec3 TransformableObject::getPosition() const {
    return mTransform.getPosition();
}

glm::vec3 TransformableObject::getRight() const {
    return mTransform.getRight();
}

glm::vec3 TransformableObject::getLeft() const {
    return mTransform.getLeft();
}

glm::vec3 TransformableObject::getForward() const {
    return mTransform.getForward();
}

glm::vec3 TransformableObject::getBackward() const {
    return mTransform.getBackward();
}

glm::vec3 TransformableObject::getUp() const {
    return mTransform.getUp();
}

glm::vec3 TransformableObject::getDown() const {
    return mTransform.getDown();
}

void TransformableObject::scale(const glm::vec3 &scalar) {
    mTransform.scale(scalar);
}

glm::vec3 TransformableObject::getScale() const {
    return mTransform.getScale();
}
