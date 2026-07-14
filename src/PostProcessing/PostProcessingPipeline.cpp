//
// Created by joost on 5/25/26.
//

#include "PostProcessing/PostProcessingPipeline.h"
#include "Error/Panic.hpp"
#include "Error/Result.hpp"
#include "FrameBuffer/MultisampledFrameBuffer.h"
#include "PostProcessing/PostProcessingComputeUnit.h"
#include "PostProcessing/PostProcessingError.h"

#include <algorithm>
#include <limits>
#include <vector>


namespace Spelt {
    // ─── Constructor ──────────────────────────────────────────────────────────────

    PostProcessingPipeline::PostProcessingPipeline(Window* window)
        : mWindow(window),
        mQuadMesh("nonameneeded"),
        mSceneFrameBuffer(window),
        mSceneInputTexture("noname", window->getWidth(), window->getHeight(), GL_RGB, GL_RGB16F, GL_FLOAT)
    {
        const std::vector<float> vertices = {
            // Positions          // Normals           // UVs
            -1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
            1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f,
            1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f
        };
        const std::vector<unsigned int> indices = { 0, 1, 2, 0, 2, 3 };

        mQuadMesh.setVertices(vertices);
        mQuadMesh.setIndices(indices);

        mSceneInputTexture.setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        mSceneInputTexture.setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        mSceneInputTexture.setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        mSceneInputTexture.setParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        mSceneFrameBuffer.attachTexture(&mSceneInputTexture, GL_COLOR_ATTACHMENT0);
        mSceneFrameBuffer.setAttachments({GL_COLOR_ATTACHMENT0});
    }

    // ─── Binding ──────────────────────────────────────────────────────────────────

    void PostProcessingPipeline::bind() {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        mSceneFrameBuffer.bind();
    }

    void PostProcessingPipeline::blitToInput(MultisampledFrameBuffer& msaaFBO){
        msaaFBO.bindRead();
        mSceneFrameBuffer.bindDraw();

        glBlitFramebuffer(
            0, 0, msaaFBO.getWidth(), msaaFBO.getHeight(),
            0, 0, mSceneFrameBuffer.getWidth(), mSceneFrameBuffer.getHeight(),
            GL_COLOR_BUFFER_BIT,
            GL_NEAREST
        );

        msaaFBO.unbindRead();
        mSceneFrameBuffer.unbindDraw();
    }

    void PostProcessingPipeline::unbind() {
        mSceneFrameBuffer.unbind();
    }

    // ─── Handle lookup ────────────────────────────────────────────────────────────

    std::vector<PostProcessingPipeline::EffectEntry>::iterator
    PostProcessingPipeline::entryForHandle(EffectHandle handle) {
        auto it = std::find_if(mEntries.begin(), mEntries.end(),
            [handle](const EffectEntry& e) { return e.handle == handle; });

        if (it == mEntries.end()) {
            fatalPanic(
                "PostProcessingPipeline: invalid EffectHandle " + std::to_string(handle) + ".");
        }
        return it;
    }

    std::vector<PostProcessingPipeline::EffectEntry>::const_iterator
    PostProcessingPipeline::entryForHandle(EffectHandle handle) const {
        auto it = std::find_if(mEntries.begin(), mEntries.end(),
            [handle](const EffectEntry& e) { return e.handle == handle; });

        if (it == mEntries.end()) {
            fatalPanic(
                "PostProcessingPipeline: invalid EffectHandle " + std::to_string(handle) + ".");
        }
        return it;
    }

    bool PostProcessingPipeline::isGroupHandle(EffectHandle handle) const {
        // A handle is a group handle when at least one entry references it as its groupHandle.
        return std::any_of(mEntries.begin(), mEntries.end(),
            [handle](const EffectEntry& e) { return e.groupHandle == handle; });
    }

    std::vector<PostProcessingPipeline::EffectEntry*>
    PostProcessingPipeline::entriesForGroup(EffectHandle groupHandle) {
        std::vector<EffectEntry*> result;
        for (auto& entry : mEntries) {
            if (entry.groupHandle == groupHandle) {
                result.push_back(&entry);
            }
        }
        return result;
    }

    std::vector<const PostProcessingPipeline::EffectEntry*>
    PostProcessingPipeline::entriesForGroup(EffectHandle groupHandle) const {
        std::vector<const EffectEntry*> result;
        for (const auto& entry : mEntries) {
            if (entry.groupHandle == groupHandle) {
                result.push_back(&entry);
            }
        }
        return result;
    }

    // ─── Effect management ────────────────────────────────────────────────────────

    Result<EffectHandle, PostProcessingError> PostProcessingPipeline::addEffectInternal(PostProcessingEffect effect,
                                                            EffectHandle groupHandle) {
        if (!mEntries.empty()) {
            const PostProcessingEffect& prev = mEntries.back().effect;
            if (!effect.checkInputCompatibility(prev)) {
                return Error(PostProcessingError::TextureCountMismatch);
            }
        } else if (effect.getInputCount() != 1) {
            return Error(PostProcessingError::WrongInitialEntry);
        }

        EffectHandle handle      = mNextHandle++;
        int          inputCount  = effect.getInputCount();
        int          outputCount = effect.getOutputCount();

        mEntries.push_back(EffectEntry{
            handle,
            std::move(effect),
            PostProcessingComputeUnit{mWindow, inputCount, outputCount},
            true,
            groupHandle
        });

        mDirty = true;
        return Value(handle);
    }

    Result<EffectHandle, PostProcessingError> PostProcessingPipeline::addEffect(PostProcessingEffect effect) {
        return addEffectInternal(std::move(effect), kInvalidEffectHandle);
    }

    Result<EffectHandle, PostProcessingError> PostProcessingPipeline::addGroup(std::vector<PostProcessingEffect> effects) {
        if (effects.empty()) {
            return Error(PostProcessingError::NoEffect);
        }

        // Reserve a dedicated group handle that is never used as an effect handle.
        EffectHandle groupHandle = mNextHandle++;

        for (auto& effect : effects) {
            auto result = addEffectInternal(std::move(effect), groupHandle);
            if (result.isError()){
                return Error(result.error());
            }
        }

        return Value(groupHandle);
    }

    void PostProcessingPipeline::removeEffect(EffectHandle handle) {
        if (isGroupHandle(handle)) {
            // Remove every effect that belongs to the group.
            mEntries.erase(
                std::remove_if(mEntries.begin(), mEntries.end(),
                    [handle](const EffectEntry& e) { return e.groupHandle == handle; }),
                mEntries.end());
        } else {
            auto it = entryForHandle(handle);
            mEntries.erase(it);
        }
        mDirty = true;
    }

    Result<void, PostProcessingError> PostProcessingPipeline::validateSymmetryForDisable(const EffectEntry& entry) const {
        if (entry.effect.getInputCount() != entry.effect.getOutputCount()) {
            return Error(PostProcessingError::TextureCountMismatch);
        }

        return Success{};
    }

    Result<void, PostProcessingError> PostProcessingPipeline::disableEffect(EffectHandle handle) {
        if (isGroupHandle(handle)) {
            auto members = entriesForGroup(handle);
            // Validate symmetry for every member before touching any of them.
            for (const EffectEntry* e : members) {

                auto result = validateSymmetryForDisable(*e);
                if (result.isError()){
                    return Error(result.error());
                }

            }
            for (EffectEntry* e : members) {
                e->enabled = false;
            }
        } else {
            auto it = entryForHandle(handle);

            auto result = validateSymmetryForDisable(*it);
            if (result.isError()){
                return Error(result.error());
            }

            it->enabled = false;
        }
        return Success{};
    }

    void PostProcessingPipeline::enableEffect(EffectHandle handle) {
        if (isGroupHandle(handle)) {
            for (EffectEntry* e : entriesForGroup(handle)) {
                e->enabled = true;
            }
        } else {
            entryForHandle(handle)->enabled = true;
        }
    }

    bool PostProcessingPipeline::isEffectEnabled(EffectHandle handle) const {
        if (isGroupHandle(handle)) {
            // Returns true only when every member is enabled.
            auto members = entriesForGroup(handle);
            return std::all_of(members.begin(), members.end(),
                [](const EffectEntry* e) { return e->enabled; });
        }
        return entryForHandle(handle)->enabled;
    }

    // ─── Pipeline lifecycle ───────────────────────────────────────────────────────

    bool PostProcessingPipeline::validate() const {
        if (mEntries.empty()) {
            return true;
        }

        for (const auto& entry : mEntries) {
            if (entry.effect.getPassCount() == 0) {
                return false;
            }
        }

        if (mEntries.back().effect.getOutputCount() != 1) {
            return false;
        }

        return true;
    }

    void PostProcessingPipeline::prepare() {
        validate();


        for (auto& entry : mEntries) {
            entry.computeUnit = PostProcessingComputeUnit{
                mWindow,
                entry.effect.getInputCount(),
                entry.effect.getOutputCount()
            };
        }

        mDirty = false;
    }

    Result<void, PostProcessingError> PostProcessingPipeline::process() {
        if (mDirty) {
            return Error(PostProcessingError::Unprepared);
        }

        std::vector<Texture*> currentInputs = { &mSceneInputTexture };

        for (auto& entry : mEntries) {
            if (!entry.enabled) {
                continue;
            }

            auto result = entry.computeUnit.execute(entry.effect, currentInputs, mQuadMesh);

            if (result.isError()) {
                return Error(result.error());
            }

            currentInputs = result.value();
        }

        if (currentInputs.empty()) {
            return Error(PostProcessingError::NoEffect);
        }

        mOutputTexture = currentInputs[0];
        return Success{};
    }

    Texture* PostProcessingPipeline::getOutput() const {
        return mOutputTexture;
    }
}
