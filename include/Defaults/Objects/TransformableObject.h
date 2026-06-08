//
// Created by joost on 6/2/26.
//

#ifndef CGVCPROJECT_TRANSFORMABLEOBJECT_H
#define CGVCPROJECT_TRANSFORMABLEOBJECT_H
#include "Scene/Object.h"
#include "Transform/ITransformable.h"
#include "Transform/Transform.h"


class TransformableObject : public Object, public ITransformable {
private:
    Transform mTransform;
public:
    explicit TransformableObject(const std::string& name);
    ~TransformableObject() override = default;

    void setPosition(const glm::vec3& pos) override;

    void setScale(const glm::vec3& scale) override;

    void scale(const glm::vec3 &scalar) override;

    void rotate(float rad, const glm::vec3& axis) override;

    void lookAt(glm::vec3 forward) override;

    glm::mat4 getTransformationMatrix() const override;

    glm::vec3 getPosition() const override;

    glm::vec3 getRight() const override;
    glm::vec3 getLeft() const override;
    glm::vec3 getUp() const override;
    glm::vec3 getDown() const override;
    glm::vec3 getBackward() const override;
    glm::vec3 getForward() const override;

    glm::vec3 getScale() const override;
private:
    glm::mat4 getParentTransformationMatrix() const;
};

#endif //CGVCPROJECT_TRANSFORMABLEOBJECT_H