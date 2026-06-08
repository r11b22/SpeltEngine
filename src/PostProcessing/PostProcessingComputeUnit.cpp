//
// Created by joost on 5/26/26.
//

#include "PostProcessing/PostProcessingComputeUnit.h"
#include "PostProcessing/PostProcessingEffect.h"

PostProcessingComputeUnit::PostProcessingComputeUnit(Window* window, int inputCount, int outputCount)
    : mInputCount(inputCount),
      mOutputCount(outputCount),
      mFrameBufferA(window),
      mFrameBufferB(window)
{
    if (inputCount < 1 || outputCount < 1) {
        throw std::runtime_error(
            "PostProcessingComputeUnit: inputCount and outputCount must both be >= 1.");
    }

    // Allocate two banks of outputCount textures.
    // Bank A: indices [0 .. outputCount-1]
    // Bank B: indices [outputCount .. 2*outputCount-1]
    mTextures.reserve(2 * outputCount);
    for (int i = 0; i < 2 * outputCount; ++i) {
        mTextures.emplace_back(window->getWidth(), window->getHeight(), GL_RGB, GL_RGB16F, GL_FLOAT);
        mTextures[i].setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        mTextures[i].setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        mTextures[i].setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        mTextures[i].setParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    // Build the attachment list once — both framebuffers use the same attachment points.
    std::vector<GLenum> attachments;
    attachments.reserve(outputCount);
    for (int i = 0; i < outputCount; ++i) {
        attachments.push_back(GL_COLOR_ATTACHMENT0 + i);
    }

    // Pre-attach bank A to mFrameBufferA.
    mFrameBufferA.setAttachments(attachments);
    for (int i = 0; i < outputCount; ++i) {
        mFrameBufferA.attachTexture(&mTextures[i], GL_COLOR_ATTACHMENT0 + i);
    }

    // Pre-attach bank B to mFrameBufferB.
    mFrameBufferB.setAttachments(attachments);
    for (int i = 0; i < outputCount; ++i) {
        mFrameBufferB.attachTexture(&mTextures[outputCount + i], GL_COLOR_ATTACHMENT0 + i);
    }
}

PostProcessingComputeUnit::PostProcessingComputeUnit(PostProcessingComputeUnit&& other) noexcept
    : mInputCount(other.mInputCount),
      mOutputCount(other.mOutputCount),
      mTextures(std::move(other.mTextures)),
      mFrameBufferA(std::move(other.mFrameBufferA)),
      mFrameBufferB(std::move(other.mFrameBufferB)) {}

PostProcessingComputeUnit& PostProcessingComputeUnit::operator=(PostProcessingComputeUnit&& other) noexcept {
    if (this != &other) {
        mInputCount   = other.mInputCount;
        mOutputCount  = other.mOutputCount;
        mTextures     = std::move(other.mTextures);
        mFrameBufferA = std::move(other.mFrameBufferA);
        mFrameBufferB = std::move(other.mFrameBufferB);
    }
    return *this;
}

std::vector<Texture*> PostProcessingComputeUnit::execute(PostProcessingEffect& effect,
                                                          const std::vector<Texture*>& inputTextures,
                                                          Mesh& quadMesh)
{
    glDisable(GL_DEPTH_TEST);

    if (static_cast<int>(inputTextures.size()) != mInputCount) {
        throw std::runtime_error(
            "PostProcessingComputeUnit: input texture count mismatch. "
            "Expected " + std::to_string(mInputCount) +
            ", got "    + std::to_string(inputTextures.size()));
    }

    const std::size_t passCount = effect.getPassCount();
    if (passCount == 0) {
        throw std::runtime_error(
            "PostProcessingComputeUnit: effect has no committed passes. "
            "Call commitPass() at least once before executing.");
    }

    effect.useShader();

    // writeToA == true  → current pass renders into bank A (mFrameBufferA)
    // writeToA == false → current pass renders into bank B (mFrameBufferB)
    // We start by writing to A so that after an odd number of passes the output
    // lives in A, and after an even number it lives in B.
    bool writeToA = true;

    for (std::size_t passIndex = 0; passIndex < passCount; ++passIndex) {
        FrameBuffer<float>& writeFB  = writeToA ? mFrameBufferA : mFrameBufferB;

        // On the first pass sample the external inputs supplied by the pipeline.
        // On every subsequent pass sample whichever bank was just written.
        const std::vector<Texture*>& sampledTextures =
            (passIndex == 0) ? inputTextures
                             : bankPointers(writeToA ? mOutputCount : 0);

        writeFB.bind();

        effect.setupInputTextures(sampledTextures);
        effect.applyPassUniforms(passIndex);
        quadMesh.draw(*effect.getShaderProgram());

        writeFB.unbind();

        writeToA = !writeToA;
    }

    glEnable(GL_DEPTH_TEST);


    // After the loop, writeToA has already been flipped, so the last written bank is
    // the opposite of the current value of writeToA.
    int lastWrittenBankOffset = writeToA ? mOutputCount : 0;
    return bankPointers(lastWrittenBankOffset);
}

std::vector<Texture*> PostProcessingComputeUnit::bankPointers(int bankOffset) {
    std::vector<Texture*> ptrs;
    ptrs.reserve(mOutputCount);
    for (int i = 0; i < mOutputCount; ++i) {
        ptrs.push_back(&mTextures[bankOffset + i]);
    }
    return ptrs;
}

int PostProcessingComputeUnit::getInputCount() const {
    return mInputCount;
}

int PostProcessingComputeUnit::getOutputCount() const {
    return mOutputCount;
}
