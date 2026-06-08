//
// Created by joost on 5/29/26.
//

#include "PostProcessing/PostProcessingGroup.h"

void PostProcessingGroup::addEffect(PostProcessingEffect effect) {
    mEffects.push_back(std::move(effect));
}

EffectHandle PostProcessingGroup::apply(PostProcessingPipeline& pipeline) {
    if (mApplied) {
        throw std::runtime_error("PostProcessingGroup::apply() called more than once.");
    }

    setup();

    if (mEffects.empty()) {
        throw std::runtime_error(
            "PostProcessingGroup::setup() did not add any effects.");
    }

    // Submit every effect and let the pipeline allocate a group handle for them.
    std::vector<PostProcessingEffect> effects = std::move(mEffects);
    EffectHandle groupHandle = pipeline.addGroup(std::move(effects));

    mApplied = true;
    return groupHandle;
}
