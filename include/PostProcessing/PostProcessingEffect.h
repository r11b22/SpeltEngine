//
// Created by joost on 5/26/26.
//

#ifndef CGVCPROJECT_POSTPROCESSINGEFFECT_H
#define CGVCPROJECT_POSTPROCESSINGEFFECT_H

#include <filesystem>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "../Shaders/ShaderProgram.h"
#include "Texture.h"


/**
 * A snapshot of uniform values to upload for one execution pass of a PostProcessingEffect.
 * Effects with multiple passes share the same shader and framebuffers; only the uniforms differ.
 */
struct EffectPass {
    std::vector<std::pair<std::string, int>>   intUniforms;
    std::vector<std::pair<std::string, float>> floatUniforms;
    std::vector<std::pair<std::string, bool>>  boolUniforms;
};


class PostProcessingEffect {
private:
    PostProcessingEffect(const std::string& source, int inputCount, int outputCount);
public:
    static PostProcessingEffect fromFile(const std::filesystem::path& effectPath, int inputCount, int outputCount);
    static PostProcessingEffect fromSource(const std::string& effectSource, int inputCount, int outputCount);

    /**
     * Stages an int uniform onto the pass currently being built.
     * Has no effect until commitPass() is called.
     */
    void addIntUniform(const std::string& name, int val);

    /**
     * Stages a float uniform onto the pass currently being built.
     * Has no effect until commitPass() is called.
     */
    void addFloatUniform(const std::string& name, float val);

    /**
     * Stages a bool uniform onto the pass currently being built.
     * Has no effect until commitPass() is called.
     */
    void addBoolUniform(const std::string& name, bool val);

    /**
     * Finalises the currently staged uniforms into a new EffectPass and clears the staging area.
     * Must be called at least once before the effect is executed.
     * Call multiple times to register multiple sequential passes (e.g. horizontal + vertical blur).
     */
    void commitPass();

    void useShader();

    /**
     * Binds each texture to its sampler slot and sets the corresponding uniform on the shader.
     */
    void setupInputTextures(const std::vector<Texture*>& textures);

    /**
     * Uploads the uniforms of the given pass index to the currently active shader.
     */
    void applyPassUniforms(std::size_t passIndex);

    /**
     * Returns true when the output count of `previousEffect` matches this effect's input count.
     */
    bool checkInputCompatibility(const PostProcessingEffect& previousEffect) const;

    std::shared_ptr<ShaderProgram> getShaderProgram();

    int         getInputCount()  const;
    int         getOutputCount() const;
    std::size_t getPassCount()   const;

    void addStaticInputTexture(Texture texture);

private:
    std::vector<Texture> mStaticTexture;

    std::shared_ptr<ShaderProgram> mShader;

    // Committed passes, executed in order at runtime.
    std::vector<EffectPass> mPasses;

    // Staging area for the pass currently being built.
    EffectPass mStagedPass;

    int mInputCount;
    int mOutputCount;
};

#endif //CGVCPROJECT_POSTPROCESSINGEFFECT_H
