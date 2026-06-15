

#include "Lighting/LightManager.h"
#include "Shaders/ShaderProgram.h"
#include <variant>
#include <vector>

void LightVisitor::operator()(const PointLightData& light){
    pointLightCount++;

    program.setUniformInt("numPointLights", pointLightCount);


    std::string base = "uPointLights[" + std::to_string(pointLightCount-1) + "].";
    program.setUniformVec3(base + "position", light.position);
    program.setUniformVec3(base + "diffuse", light.difuse);
    program.setUniformVec3(base + "specular", light.specular);

    program.setUniformFloat(base + "constant", light.constant);
    program.setUniformFloat(base + "lineair", light.linear);
    program.setUniformFloat(base + "quadratic", light.quadratic);

}

void LightVisitor::operator()(const AmbientLightData& light){
    program.setUniformVec3("uAmbient", light.ambient);

}

void LightManager::applyLightData(ShaderProgram& program, std::vector<LightData> lights)
{
    auto lightVisitor = LightVisitor(program);
    for (const auto& light : lights) {
        std::visit(lightVisitor, light);
    }
}
