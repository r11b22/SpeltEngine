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
#include "../Mesh.h"
#include "../PostProcessing/PostProcessingPipeline.h"
#include "../Scene/Scene.h"
#include "../PickBuffer.h"
#include "../Window.h"
#include "../Lighting/PointLight.h"
#include "../PostProcessing/PostProcessingGroup.h"

class Renderer {
private:
    std::map<std::string, std::unique_ptr<ShaderProgram>> mShaderPrograms;

    ShaderProgram* mScreenShader;
    Mesh* mScreenQuad;

    PickBuffer* mPickBuffer;
    ShaderProgram* mPickShader;

    Window* mTarget;

    PostProcessingPipeline* mPostProcessingPipeline;


public:
    Renderer(Window* target);
    ~Renderer();

    void addShaderProgram(std::string name, std::unique_ptr<ShaderProgram> shaderPrograms);

    EffectHandle addPostProcessingEffect(PostProcessingEffect effect);
    EffectHandle addPostProcessingEffect(PostProcessingGroup& effect);
    void enableEffect(EffectHandle handle);
    void disableEffect(EffectHandle handle);

    void prepare();

    void drawScene(const Scene& scene);

    std::shared_ptr<IDrawable> getObjectAtPixel(const Scene& scene, int x, int y) const;
private:
    void drawPass(const Scene& scene, const Camera& camera);
    void pickingPass(const Scene& scene, const Camera& camera);
    void renderToScreen();
    void uploadLightDataToShader(ShaderProgram& program, const std::vector<std::shared_ptr<PointLight>>& lights);
};


#endif //GCVCPROJECT_RENDERER_H