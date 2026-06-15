#pragma once


#include "Lighting/DirectionalLightData.h"
#include "Shaders/ShaderProgram.h"
#include "LightData.h"

struct LightVisitor {
    ShaderProgram& program;
    int pointLightCount = 0;
    int directionalLightCount = 0;

    LightVisitor(ShaderProgram& program) : program{program} {}

    void operator()(const PointLightData& light);
    void operator()(const AmbientLightData& light);
    void operator()(const DirectionalLightData& light);
};

class LightManager {
    private:
    public:
        void applyLightData(ShaderProgram& program, std::vector<LightData> lights);
    private:
};
