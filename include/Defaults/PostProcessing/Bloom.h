//
// Created by joost on 5/29/26.
//

#pragma once

#include "PostProcessing/PostProcessingGroup.h"
namespace Spelt::Defaults {
    class Bloom : public PostProcessingGroup {
        void setup() override;
    };
}
