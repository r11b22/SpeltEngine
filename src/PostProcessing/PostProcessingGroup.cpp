//
// Created by joost on 5/29/26.
//

#include "PostProcessing/PostProcessingGroup.h"
#include "Error/Result.hpp"


namespace Spelt {
    void PostProcessingGroup::addEffect(PostProcessingEffect effect) {
        mEffects.push_back(std::move(effect));
    }

    Result<EffectHandle, PostProcessingError> PostProcessingGroup::apply(PostProcessingPipeline& pipeline) {
        if (mApplied) {
            return Error(PostProcessingError::MultipleApplies);
        }

        setup();

        if (mEffects.empty()) {
            return Error(PostProcessingError::NoEffect);
        }

        // Submit every effect and let the pipeline allocate a group handle for them.
        std::vector<PostProcessingEffect> effects = std::move(mEffects);
        auto groupHandleResult = pipeline.addGroup(std::move(effects));

        if(groupHandleResult.isError()){
            return Error(groupHandleResult.error());
        }

        EffectHandle groupHandle = groupHandleResult.value();
        mApplied = true;
        return Value(groupHandle);
    }
}
