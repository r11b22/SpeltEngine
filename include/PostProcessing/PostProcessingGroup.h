//
// Created by joost on 5/29/26.
//

#ifndef CGVCPROJECT_POSTPROCESSINGGROUP_H
#define CGVCPROJECT_POSTPROCESSINGGROUP_H

#include <stdexcept>
#include <vector>

#include "PostProcessingEffect.h"
#include "PostProcessingPipeline.h"

/**
 * Base class for a named collection of post-processing effects that are
 * submitted to the pipeline as a unit and can be toggled with a single handle.
 *
 * Subclasses override setup() to call addEffect() one or more times.
 * Call apply() exactly once to register all effects with the pipeline and
 * receive a single EffectHandle that controls the entire group.
 *
 * Example:
 *   class BloomGroup : public PostProcessingGroup {
 *   protected:
 *       void setup() override {
 *           addEffect(PostProcessingEffect{ ... }); // bright-pass
 *           addEffect(PostProcessingEffect{ ... }); // blur
 *           addEffect(PostProcessingEffect{ ... }); // composite
 *       }
 *   };
 *
 *   BloomGroup bloom;
 *   EffectHandle h = bloom.apply(pipeline);
 *   pipeline.disableEffect(h);   // turns off all three effects at once
 */
class PostProcessingGroup {
public:
    virtual ~PostProcessingGroup() = default;

    /**
     * Calls setup(), pushes every registered effect into the pipeline,
     * and returns a single EffectHandle that represents the whole group.
     * May only be called once; throws on a second call.
     */
    EffectHandle apply(PostProcessingPipeline& pipeline);

protected:
    /**
     * Override to register the effects that make up this group by calling
     * addEffect() one or more times. Called once by apply().
     */
    virtual void setup() = 0;

    /**
     * Queue an effect for submission. Call only from within setup().
     */
    void addEffect(PostProcessingEffect effect);

private:
    bool mApplied = false;
    std::vector<PostProcessingEffect> mEffects;
};

#endif //CGVCPROJECT_POSTPROCESSINGGROUP_H
