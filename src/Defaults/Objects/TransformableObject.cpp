//
// Created by joost on 6/2/26.
//

#include "../../../include/Defaults/Objects/TransformableObject.h"

#include "Object/Object.h"
#include "Object/ObjectRepository.h"
#include "Scene/Scene.hpp"
#include "Transform/ITransformable.h"
#include <Tracy/tracy/Tracy.hpp>


namespace Spelt::Defaults {
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
        ZoneScoped;
        return mTransform.getTransformationMatrix(getParentTransformationMatrix());
    }

    glm::mat4 TransformableObject::getParentTransformationMatrix() const {
        // find first ancestor with transform
        // FIXME: this only checks the immediate parent not all parents
        // TODO cache result
        while (true) {
            ObjectReference<ITransformable> current = getScene().getParentByID<ITransformable>(getID());
            if (current.isNoReference()) {
                return {1.0f};
            }else{
                return current->getTransformationMatrix();
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
}
