//
// Created by joost on 5/25/26.
//

#ifndef CGVCPROJECT_POSTPROCESSINGPIPELINE_H
#define CGVCPROJECT_POSTPROCESSINGPIPELINE_H

#include <cstddef>
#include <optional>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include "PostProcessingComputeUnit.h"
#include "PostProcessingEffect.h"
#include "FrameBuffer.h"
#include "Mesh.h"
#include "Texture.h"
#include "Window.h"


/**
 * Opaque handle returned by addEffect() or addGroup().
 * Use it to enable, disable, or remove an effect (or a whole group) without
 * relying on a positional index that could shift after a removal.
 */
using EffectHandle = std::size_t;

static constexpr EffectHandle kInvalidEffectHandle = std::numeric_limits<EffectHandle>::max();


class PostProcessingPipeline {
public:
    explicit PostProcessingPipeline(Window* window);

    // ── Single-effect API ──────────────────────────────────────────────────

    /**
     * Appends an effect to the end of the chain and returns a stable handle to it.
     * The first effect must have exactly one input texture.
     * Each subsequent effect must accept as many inputs as the previous effect produces.
     * Marks the pipeline dirty; prepare() must be called again before process().
     */
    EffectHandle addEffect(PostProcessingEffect effect);

    /**
     * Removes the effect identified by handle from the chain.
     * The handle becomes invalid after this call.
     * If the handle belongs to a group, the entire group is removed.
     * Marks the pipeline dirty; prepare() must be called again before process().
     * Throws if the handle is invalid.
     */
    void removeEffect(EffectHandle handle);

    /**
     * Disables the effect identified by handle at runtime without modifying GPU resources.
     * A disabled effect is skipped during process(): its input textures are passed through
     * directly as its outputs. This is only valid when inputCount == outputCount for that
     * effect; throws otherwise, because skipping an asymmetric effect would break the chain.
     * If handle is a group handle, all member effects are disabled atomically.
     * Does not mark the pipeline dirty; no prepare() call is needed.
     * Throws if the handle is invalid.
     */
    void disableEffect(EffectHandle handle);

    /**
     * Re-enables an effect or group that was previously disabled.
     * Does not mark the pipeline dirty.
     * Throws if the handle is invalid.
     */
    void enableEffect(EffectHandle handle);

    /**
     * Returns whether the effect (or every member of a group) is currently enabled.
     * For a group handle this returns true only when ALL member effects are enabled.
     * Throws if the handle is invalid.
     */
    bool isEffectEnabled(EffectHandle handle) const;

    // ── Group API ──────────────────────────────────────────────────────────

    /**
     * Appends a sequence of effects to the end of the chain as a named group.
     * Returns a single group handle; passing that handle to enable/disableEffect
     * or removeEffect affects every member effect atomically.
     * The same chain-compatibility rules that apply to addEffect() are enforced
     * for every effect in the sequence.
     * Marks the pipeline dirty; prepare() must be called again before process().
     * Throws if the vector is empty.
     */
    EffectHandle addGroup(std::vector<PostProcessingEffect> effects);

    // ── Pipeline lifecycle ─────────────────────────────────────────────────

    /**
     * Validates that the pipeline is in a runnable state:
     * - At least one effect has been added.
     * - Every effect has at least one committed pass.
     * - The last effect produces exactly one output texture.
     */
    bool validate() const;

    /**
     * Allocates all GPU resources (framebuffers, textures) for the registered effects.
     * Must be called once after the effect chain has been fully built, and again whenever
     * addEffect(), addGroup(), or removeEffect() has been called.
     * enable/disableEffect() do not require a new prepare() call.
     */
    void prepare();

    /**
     * Executes every enabled effect in order and stores the final single-texture output.
     * Disabled effects forward their inputs unchanged.
     * Throws if the pipeline is dirty (i.e. prepare() has not been called since the last
     * structural change).
     */
    void process();

    /**
     * Returns a pointer to the texture produced by the last effect.
     * Valid after process() has been called at least once.
     */
    Texture* getOutput() const;

    /**
     * Bind the input texture for this pipeline.
     */
    void bind();

    /**
     * Unbind the input texture for this pipeline.
     */
    void unbind();

private:
    struct EffectEntry {
        EffectHandle              handle;
        PostProcessingEffect      effect;
        PostProcessingComputeUnit computeUnit;
        bool                      enabled = true;
        /// kInvalidEffectHandle when this effect does not belong to a group.
        EffectHandle              groupHandle = kInvalidEffectHandle;
    };

    // ── Internal helpers ───────────────────────────────────────────────────

    /**
     * Returns an iterator to the entry with the given effect handle, or throws.
     */
    std::vector<EffectEntry>::iterator       entryForHandle(EffectHandle handle);
    std::vector<EffectEntry>::const_iterator entryForHandle(EffectHandle handle) const;

    /**
     * Returns true when handle is a known group handle.
     */
    bool isGroupHandle(EffectHandle handle) const;

    /**
     * Returns iterators to all entries that belong to groupHandle. Never empty.
     */
    std::vector<EffectEntry*>       entriesForGroup(EffectHandle groupHandle);
    std::vector<const EffectEntry*> entriesForGroup(EffectHandle groupHandle) const;

    /**
     * Core addEffect implementation shared by addEffect() and addGroup().
     * Validates chain compatibility, appends the entry, marks dirty.
     */
    EffectHandle addEffectInternal(PostProcessingEffect effect, EffectHandle groupHandle);

    /**
     * Validates that a disabled effect can safely be skipped (inputCount == outputCount).
     */
    void validateSymmetryForDisable(const EffectEntry& entry) const;

    // ── Data members ───────────────────────────────────────────────────────

    Window* mWindow;
    Mesh    mQuadMesh;

    std::vector<EffectEntry> mEntries;

    EffectHandle mNextHandle = 0;
    bool         mDirty      = false;

    Texture* mOutputTexture = nullptr;

    FrameBuffer<float> mSceneFrameBuffer;
    Texture            mSceneInputTexture;
};

#endif //CGVCPROJECT_POSTPROCESSINGPIPELINE_H
