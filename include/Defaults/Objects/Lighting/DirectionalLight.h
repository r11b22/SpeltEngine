#pragma once


#include "Lighting/DirectionalLightData.h"
#include "Lighting/ILight.h"
#include "Scene/Object.h"
#include "glm/ext/vector_float3.hpp"
class DirectionalLight : public Object, public ILight {
  private:
      DirectionalLightData mLightData;
  public:
      DirectionalLight(const std::string& name, glm::vec3 direction, glm::vec3 color);
      ~DirectionalLight() override = default;

      LightData getLightData() const override;
  private:
};
