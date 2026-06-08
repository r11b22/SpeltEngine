//
// Created by joost on 6/2/26.
//

#ifndef CGVCPROJECT_TRANSFORMABLE_H
#define CGVCPROJECT_TRANSFORMABLE_H

#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>

class ITransformable {

public:
    virtual ~ITransformable() = default;

    /*
    * Sets local position (Does not take into account rotation)
    */
    virtual void setPosition(const glm::vec3& pos) = 0;

    /*
     * Sets local scale (Does not take into account rotation)
     */
    virtual void setScale(const glm::vec3& scale) = 0;

    /*
     *
     */
    virtual void scale(const glm::vec3& scalar) = 0;

    /*
     * Rotate the transform by radians around the axis
     * The rotation axis is relative to the parents rotation
     */
    virtual void rotate(float rad, const glm::vec3& axis) = 0;

    virtual void lookAt(glm::vec3 forward) = 0;

    [[nodiscard]] virtual glm::mat4 getTransformationMatrix() const = 0;

    [[nodiscard]] virtual glm::vec3 getPosition() const = 0;

    [[nodiscard]] virtual glm::vec3 getScale() const = 0;

    [[nodiscard]] virtual glm::vec3 getRight() const = 0;
    [[nodiscard]] virtual glm::vec3 getLeft() const = 0;
    [[nodiscard]] virtual glm::vec3 getForward() const = 0;
    [[nodiscard]] virtual glm::vec3 getBackward() const = 0;
    [[nodiscard]] virtual glm::vec3 getUp() const = 0;
    [[nodiscard]] virtual glm::vec3 getDown() const = 0;
};

#endif //CGVCPROJECT_TRANSFORMABLE_H