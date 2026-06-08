//
// Created by joost on 4/14/26.
//

#ifndef GCVCPROJECT_TRANSFORM_H
#define GCVCPROJECT_TRANSFORM_H

#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>

class Transform {
private:
    // used for getters
    glm::vec3 mPosition;

    glm::mat4 mPositionMatrix{1.0f};
    glm::mat4 mScaleMatrix{1.0f};
    glm::mat4 mRotationMatrix{1.0f};
public:

    /*
     * Sets local position (Does not take into account rotation)
     */
    void setPosition(const glm::vec3& pos);

    /*
     * Sets local scale (Does not take into account rotation)
     */
    void setScale(const glm::vec3& scale);

    /*
     * Scales the object by a factor
     */
    void scale(const glm::vec3& scalar);

    /*
     * Rotate the transform by radians arround the axis
     * This is done relative to the parents rotation
     */
    void rotate(float rad, const glm::vec3& axis);

    void lookAt(glm::vec3 forward);

    glm::mat4 getTransformationMatrix() const;

    glm::vec3 getPosition() const;

    glm::vec3 getRight() const;
    glm::vec3 getLeft() const;
    glm::vec3 getForward() const;
    glm::vec3 getBackward() const;
    glm::vec3 getUp() const;
    glm::vec3 getDown() const;

    glm::vec3 getScale() const;
private:
};

#endif //GCVCPROJECT_TRANSFORM_H