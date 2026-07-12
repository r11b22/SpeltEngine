#pragma once


#include "Lighting/DirectionalLightData.h"
#include "Lighting/ILight.h"
#include "Object/Object.h"
#include "glm/ext/vector_float3.hpp"

namespace Spelt::Defaults {
    class DirectionalLight : public Object, public ILight {
    private:
        DirectionalLightData mLightData;
    public:
        DirectionalLight(const std::string& name, glm::vec3 direction, glm::vec3 color);
        ~DirectionalLight() override = default;

        LightData getLightData() const override;
    private:
    };
}
