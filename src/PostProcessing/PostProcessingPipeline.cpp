//
// Created by joost on 5/25/26.
//

#include "PostProcessing/PostProcessingPipeline.h"

#include <algorithm>
#include <limits>

// ─── Constructor ──────────────────────────────────────────────────────────────

PostProcessingPipeline::PostProcessingPipeline(Window* window)
    : mWindow(window),
      mQuadMesh(),
      mSceneFrameBuffer(window),
      mSceneInputTexture(window->getWidth(), window->getHeight(), GL_RGB, GL_RGB16F, GL_FLOAT)
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

void PostProcessingPipeline::unbind() {
    mSceneFrameBuffer.unbind();
}

// ─── Handle lookup ────────────────────────────────────────────────────────────

std::vector<PostProcessingPipeline::EffectEntry>::iterator
PostProcessingPipeline::entryForHandle(EffectHandle handle) {
    auto it = std::find_if(mEntries.begin(), mEntries.end(),
        [handle](const EffectEntry& e) { return e.handle == handle; });

    if (it == mEntries.end()) {
        throw std::runtime_error(
            "PostProcessingPipeline: invalid EffectHandle " + std::to_string(handle) + ".");
    }
    return it;
}

std::vector<PostProcessingPipeline::EffectEntry>::const_iterator
PostProcessingPipeline::entryForHandle(EffectHandle handle) const {
    auto it = std::find_if(mEntries.begin(), mEntries.end(),
        [handle](const EffectEntry& e) { return e.handle == handle; });

    if (it == mEntries.end()) {
        throw std::runtime_error(
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

EffectHandle PostProcessingPipeline::addEffectInternal(PostProcessingEffect effect,
                                                        EffectHandle groupHandle) {
    if (!mEntries.empty()) {
        const PostProcessingEffect& prev = mEntries.back().effect;
        if (!effect.checkInputCompatibility(prev)) {
            throw std::runtime_error(
                "PostProcessingPipeline: effect input count (" +
                std::to_string(effect.getInputCount()) +
                ") does not match previous effect output count (" +
                std::to_string(prev.getOutputCount()) + ").");
        }
    } else if (effect.getInputCount() != 1) {
        throw std::runtime_error(
            "PostProcessingPipeline: the first effect must have exactly 1 input texture, "
            "but has " + std::to_string(effect.getInputCount()) + ".");
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
    return handle;
}

EffectHandle PostProcessingPipeline::addEffect(PostProcessingEffect effect) {
    return addEffectInternal(std::move(effect), kInvalidEffectHandle);
}

EffectHandle PostProcessingPipeline::addGroup(std::vector<PostProcessingEffect> effects) {
    if (effects.empty()) {
        throw std::runtime_error("PostProcessingPipeline::addGroup(): effect list is empty.");
    }

    // Reserve a dedicated group handle that is never used as an effect handle.
    EffectHandle groupHandle = mNextHandle++;

    for (auto& effect : effects) {
        addEffectInternal(std::move(effect), groupHandle);
    }

    return groupHandle;
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

void PostProcessingPipeline::validateSymmetryForDisable(const EffectEntry& entry) const {
    if (entry.effect.getInputCount() != entry.effect.getOutputCount()) {
        throw std::runtime_error(
            "PostProcessingPipeline: cannot disable effect with handle " +
            std::to_string(entry.handle) +
            " because its input count (" +
            std::to_string(entry.effect.getInputCount()) +
            ") != output count (" +
            std::to_string(entry.effect.getOutputCount()) +
            "). Disabling an asymmetric effect would break the chain.");
    }
}

void PostProcessingPipeline::disableEffect(EffectHandle handle) {
    if (isGroupHandle(handle)) {
        auto members = entriesForGroup(handle);
        // Validate symmetry for every member before touching any of them.
        for (const EffectEntry* e : members) {
            validateSymmetryForDisable(*e);
        }
        for (EffectEntry* e : members) {
            e->enabled = false;
        }
    } else {
        auto it = entryForHandle(handle);
        validateSymmetryForDisable(*it);
        it->enabled = false;
    }
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
            throw std::runtime_error(
                "PostProcessingPipeline: effect with handle " +
                std::to_string(entry.handle) +
                " has no committed passes. Call commitPass() at least once on every effect.");
        }
    }

    if (mEntries.back().effect.getOutputCount() != 1) {
        throw std::runtime_error(
            "PostProcessingPipeline: the last effect must produce exactly 1 output texture, "
            "but produces " + std::to_string(mEntries.back().effect.getOutputCount()) + ".");
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

void PostProcessingPipeline::process() {
    if (mDirty) {
        throw std::runtime_error(
            "PostProcessingPipeline: prepare() must be called after adding or removing effects.");
    }

    std::vector<Texture*> currentInputs = { &mSceneInputTexture };

    for (auto& entry : mEntries) {
        if (!entry.enabled) {
            continue;
        }
        currentInputs = entry.computeUnit.execute(entry.effect, currentInputs, mQuadMesh);
    }

    mOutputTexture = currentInputs[0];
}

Texture* PostProcessingPipeline::getOutput() const {
    return mOutputTexture;
}
