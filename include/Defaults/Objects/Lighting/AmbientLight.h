//
// Created by joost on 6/4/26.
//

#ifndef CGVCPROJECT_AMBIENTLIGHT_H
#define CGVCPROJECT_AMBIENTLIGHT_H
#include "Lighting/AmbientLightData.h"
#include "Lighting/ILight.h"
#include "Lighting/LightData.h"
#include "Scene/Object.h"

class AmbientLight : public Object, public ILight {
private:
    AmbientLightData mLightData;

public:
    AmbientLight(const std::string& name, glm::vec3 ambient = glm::vec3{0.2f, 0.2f, 0.2f});


    LightData getLightData() const override;
private:
};

#endif //CGVCPROJECT_AMBIENTLIGHT_H
