//
// Created by joost on 5/26/26.
//

#ifndef CGVCPROJECT_POSTPROCESSINGCOMPUTEUNIT_H
#define CGVCPROJECT_POSTPROCESSINGCOMPUTEUNIT_H

#include <vector>

#include "FrameBuffer.h"
#include "Texture.h"
#include "Mesh.h"

class PostProcessingEffect;

/**
 * Owns the GPU resources (framebuffers + textures) needed to execute one PostProcessingEffect,
 * including all of its passes.
 *
 * Texture layout (all owned by this unit):
 *   Bank A  [0 .. outputCount-1]            — pre-attached to mFrameBufferA
 *   Bank B  [outputCount .. 2*outputCount-1] — pre-attached to mFrameBufferB
 *
 * On each pass the unit alternates which bank it renders into, so the previous pass's
 * output is always available as a sampler input without any framebuffer attachment changes.
 * The external input textures (from the preceding pipeline stage) are only sampled on the
 * first pass; all subsequent passes sample from the last written bank.
 */
class PostProcessingComputeUnit {
public:
    PostProcessingComputeUnit(Window* window, int inputCount, int outputCount);

    // Non-copyable, movable
    PostProcessingComputeUnit(const PostProcessingComputeUnit&) = delete;
    PostProcessingComputeUnit& operator=(const PostProcessingComputeUnit&) = delete;
    PostProcessingComputeUnit(PostProcessingComputeUnit&& other) noexcept;
    PostProcessingComputeUnit& operator=(PostProcessingComputeUnit&& other) noexcept;

    /**
     * Executes all passes of the given effect using the provided external input textures.
     * On the first pass the external inputs are sampled; on subsequent passes the output
     * of the previous pass is sampled instead, using the same shader and framebuffers.
     * Returns pointers to the textures written by the final pass.
     * The returned pointers remain valid for the lifetime of this unit.
     */
    std::vector<Texture*> execute(PostProcessingEffect& effect,
                                  const std::vector<Texture*>& inputTextures,
                                  Mesh& quadMesh);

    int getInputCount() const;
    int getOutputCount() const;

private:
    /**
     * Builds the list of Texture* pointers for one bank starting at the given index offset.
     */
    std::vector<Texture*> bankPointers(int bankOffset);

    int mInputCount;
    int mOutputCount;

    // Two banks of outputCount textures each, ping-ponged across passes.
    std::vector<Texture> mTextures;

    FrameBuffer<float> mFrameBufferA;  // renders into bank A
    FrameBuffer<float> mFrameBufferB;  // renders into bank B
};

#endif //CGVCPROJECT_POSTPROCESSINGCOMPUTEUNIT_H
