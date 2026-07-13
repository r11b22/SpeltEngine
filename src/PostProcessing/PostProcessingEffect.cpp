//
// Created by joost on 5/26/26.
//

#include "PostProcessing/PostProcessingEffect.h"
#include "Error/Result.hpp"
#include "FileReader.h"
#include "Strings/ShaderSource.h"


namespace Spelt {
    PostProcessingEffect PostProcessingEffect::fromFile(const std::filesystem::path& effectPath,
                                            int inputCount,
                                            int outputCount)
    {
        return PostProcessingEffect(FileReader::readFile(effectPath).value(), inputCount, outputCount);
    }
    PostProcessingEffect::PostProcessingEffect(const std::string& effectSource,
                                            int inputCount,
                                            int outputCount)
        : mInputCount(inputCount), mOutputCount(outputCount)
    {
        mShader = std::make_shared<ShaderProgram>();
        mShader->addShader(straightToScreenVertexShader,
                        GL_VERTEX_SHADER);
        mShader->addShader(effectSource.c_str(), GL_FRAGMENT_SHADER);
        mShader->link();
    }

    PostProcessingEffect PostProcessingEffect::fromSource(const std::string &effectSource, int inputCount, int outputCount) {
        return PostProcessingEffect(effectSource, inputCount, outputCount);
    }

    void PostProcessingEffect::addIntUniform(const std::string& name, int val) {
        mStagedPass.intUniforms.emplace_back(name, val);
    }

    void PostProcessingEffect::addFloatUniform(const std::string& name, float val) {
        mStagedPass.floatUniforms.emplace_back(name, val);
    }

    void PostProcessingEffect::addBoolUniform(const std::string& name, bool val) {
        mStagedPass.boolUniforms.emplace_back(name, val);
    }

    void PostProcessingEffect::commitPass() {
        mPasses.push_back(std::move(mStagedPass));
        mStagedPass = EffectPass{};
    }

    void PostProcessingEffect::useShader() {
        mShader->use();
    }

    Result<void, PostProcessingError> PostProcessingEffect::setupInputTextures(const std::vector<Texture*>& textures) {
        if (static_cast<int>(textures.size()) != mInputCount) {
            return Error(PostProcessingError::TextureCountMismatch);
        }

        for (int i = 0; i < mInputCount; ++i) {
            mShader->setUniformInt("uInputTexture" + std::to_string(i), i);
            textures[i]->bind(i);
        }

        for (int i = 0; i < mStaticTexture.size(); i++) {
            mShader->setUniformInt("uStaticTexture"+std::to_string(i), i+mInputCount);
            mStaticTexture[i].bind(i+mInputCount);
        }

        return Success{};
    }

    Result<void, PostProcessingError> PostProcessingEffect::applyPassUniforms(std::size_t passIndex) {
        if (passIndex >= mPasses.size()) {
            return Error(PostProcessingError::InvalidPassIndex);
        }

        const EffectPass& pass = mPasses[passIndex];

        for (const auto& [name, val] : pass.intUniforms) {
            mShader->setUniformInt(name, val);
        }
        for (const auto& [name, val] : pass.floatUniforms) {
            mShader->setUniformFloat(name, val);
        }
        for (const auto& [name, val] : pass.boolUniforms) {
            mShader->setUniformBool(name, val);
        }

        return Success{};
    }

    bool PostProcessingEffect::checkInputCompatibility(const PostProcessingEffect& previousEffect) const {
        return previousEffect.getOutputCount() == mInputCount;
    }

    std::shared_ptr<ShaderProgram> PostProcessingEffect::getShaderProgram() {
        return mShader;
    }

    int PostProcessingEffect::getInputCount() const {
        return mInputCount;
    }

    int PostProcessingEffect::getOutputCount() const {
        return mOutputCount;
    }

    std::size_t PostProcessingEffect::getPassCount() const {
        return mPasses.size();
    }

    void PostProcessingEffect::addStaticInputTexture(Texture texture) {
        mStaticTexture.push_back(std::move(texture));
    }
}
