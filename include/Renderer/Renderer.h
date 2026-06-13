//
// Created by joost on 4/12/26.
//

#ifndef GCVCPROJECT_RENDERER_H
#define GCVCPROJECT_RENDERER_H
#include <map>
#include <memory>
#include <vector>

#include "../Camera.h"
#include "IDrawable.h"
#include "RenderStateManager.h"
#include "../Mesh.h"
#include "../PostProcessing/PostProcessingPipeline.h"
#include "../Scene/Scene.h"
#include "../Window.h"
#include "../Lighting/PointLight.h"
#include "../PostProcessing/PostProcessingGroup.h"

class Renderer {
private:
    std::map<std::string, std::unique_ptr<ShaderProgram>> mShaderPrograms;

    ShaderProgram* mScreenShader;
    Mesh* mScreenQuad;

    Window* mTarget;

    PostProcessingPipeline* mPostProcessingPipeline;

    RenderStateManager mStateManager = {};

    GLbitfield mClearBitField = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
public:
    Renderer(Window* target);
    ~Renderer();

    void addShaderProgram(std::string name, std::unique_ptr<ShaderProgram> shaderPrograms);

    EffectHandle addPostProcessingEffect(PostProcessingEffect effect);
    EffectHandle addPostProcessingEffect(PostProcessingGroup& effect);
    void enableEffect(EffectHandle handle);
    void disableEffect(EffectHandle handle);

    void prepare();

    void draw(const RenderQueue& queue, const Camera& camera, const std::vector<std::shared_ptr<PointLight>>& pointLights, std::shared_ptr<AmbientLight> ambientLight);

    void setClearBits(GLbitfield bits);

private:
    void drawPass(const RenderQueue& queue, const Camera& camera, const std::vector<std::shared_ptr<PointLight>>& pointLights, std::shared_ptr<AmbientLight> ambientLight);
    void renderToScreen();
    void uploadStandardUniforms(ShaderProgram& program, const Camera& camera, const std::vector<std::shared_ptr<PointLight>>& pointLights, std::shared_ptr<AmbientLight> ambientLight);
    void uploadLightData(ShaderProgram& program, const std::vector<std::shared_ptr<PointLight>>& points, std::shared_ptr<AmbientLight> ambient);
};


#endif //GCVCPROJECT_RENDERER_H