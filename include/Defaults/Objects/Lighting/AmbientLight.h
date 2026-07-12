//
// Created by joost on 6/4/26.
//

#pragma once

#include "Lighting/AmbientLightData.h"
#include "Lighting/ILight.h"
#include "Lighting/LightData.h"
#include "Object/Object.h"
namespace Spelt::Defaults {
    class AmbientLight : public Object, public ILight {
    private:
        AmbientLightData mLightData;

    public:
        AmbientLight(const std::string& name, glm::vec3 ambient = glm::vec3{0.2f, 0.2f, 0.2f});


        LightData getLightData() const override;
    private:
    };
}
