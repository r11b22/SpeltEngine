#pragma once

namespace Spelt {
    enum class PostProcessingError{
        TextureCountMismatch,
        WrongInitialEntry,
        MultipleApplies,
        NoEffect,
        InvalidPassIndex,
        NoPasses,
        Unprepared,
    };
};
