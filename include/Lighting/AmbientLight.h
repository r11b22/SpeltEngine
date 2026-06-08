//
// Created by joost on 6/4/26.
//

#ifndef CGVCPROJECT_AMBIENTLIGHT_H
#define CGVCPROJECT_AMBIENTLIGHT_H
#include "../Defaults/Objects/TransformableObject.h"
#include "glm/vec3.hpp"

class AmbientLight : public TransformableObject {
private:
    glm::vec3 mAmbient;
public:
    AmbientLight(const std::string& name, glm::vec3 ambient = glm::vec3{0.2f, 0.2f, 0.2f});

    glm::vec3 getAmbient();
private:
};

#endif //CGVCPROJECT_AMBIENTLIGHT_H